#include "run_visitor.hpp"

#include <bashpp/command.hpp>
#include <bashpp/pipeline.hpp>
#include <bashpp/wrappers.hpp>
#include <iostream>
#include <ranges>

namespace {
    using namespace bashpp;

    FileDescriptor openTemporaryFile() {
        char filename[] = "bashpp.XXXXXXXXX";
        FileDescriptor fd{wrappers::mkstemp(filename)};
        wrappers::unlink(filename);
        wrappers::fcntlSetFD(fd.fd(), wrappers::fcntlGetFD(fd.fd()) | FD_CLOEXEC);
        return fd;
    }

    class RedirectionSetupVisitor {
        int fd_;
        Command &command_;

    public:
        RedirectionSetupVisitor(int fd, Command &command) : fd_{fd}, command_{command} {}

        void operator()(const OutputVariableRedirection &) {
            command_.process()->addRedirection(fd_, openTemporaryFile());
        }
        void operator()(const auto &) {
        }
    };

    class RedirectionApplyVisitor {
        int fd_;
        Command &command_;

    public:
        explicit RedirectionApplyVisitor(int fd, Command &command) : fd_{fd}, command_{command} {}

        void operator()(const FDRedirection &r) const {
            wrappers::dup2(r.fd, fd_);
        }
        void operator()(const CloseFDRedirection &) const {
            wrappers::close(fd_);
        }

        void operator()(const InputPathRedirection &r) const {
            (*this)(FDRedirection{wrappers::open(r.path.c_str(), O_RDONLY | O_CLOEXEC, 0)});
        }
        void operator()(const OutputPathRedirection &r) const {
            (*this)(FDRedirection{wrappers::open(r.path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC,
                                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)});
        }
        void operator()(const OutputPathAppendRedirection &r) const {
            (*this)(FDRedirection{wrappers::open(r.path.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC,
                                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)});
        }
        void operator()(const InputOutputPathRedirection &r) const {
            (*this)(FDRedirection{wrappers::open(r.path.c_str(), O_RDWR | O_CREAT | O_CLOEXEC,
                                                 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)});
        }

        void operator()(const InputVariableRedirection &r) const {
            auto tmp = openTemporaryFile();
            std::span<const std::byte> bytes = r.bytes;
            while (!bytes.empty()) {
                std::size_t written = wrappers::write(tmp.fd(), bytes.data(), bytes.size());
                bytes = bytes.subspan(written);
            }
            wrappers::lseek(tmp.fd(), 0, SEEK_SET);
            (*this)(FDRedirection{tmp.fd()});
        }
        void operator()(const OutputVariableRedirection &) const {
            (*this)(FDRedirection{command_.process()->getRedirectionTempFD(fd_)});
        }
    };

    class RedirectionTeardownVisitor {
        int fd_;
        Command &command_;

    public:
        RedirectionTeardownVisitor(int fd, Command &command) : fd_{fd}, command_{command} {}

        void operator()(const OutputVariableRedirection &) {
            int fd = command_.process()->getRedirectionTempFD(fd_);

            wrappers::lseek(fd, SEEK_SET, 0);

            constexpr std::size_t BUFFER_SIZE = 2048;

            std::vector<std::byte> buffer;

            buffer.resize(BUFFER_SIZE);
            std::size_t read = wrappers::read(fd, buffer.data(), BUFFER_SIZE);
            while (read == BUFFER_SIZE) {
                std::size_t current_size = buffer.size();
                buffer.resize(current_size + BUFFER_SIZE);
                read = wrappers::read(fd, buffer.data() + current_size, BUFFER_SIZE);
            }
            buffer.resize(buffer.size() + read - BUFFER_SIZE);

            command_.process()->finishRedirection(fd_, std::move(buffer));
        }
        void operator()(const auto &) {
        }
    };
}// namespace

namespace bashpp {
    std::vector<const char *> RunVisitor::constructArguments(const Command &command) {
        std::vector<const char *> result;
        result.reserve(1 + command.arguments().size() + 1);

        std::string_view program{command.program()};
        auto sep = program.rfind('/');
        if (sep != std::string_view::npos) {
            program = program.substr(sep + 1);
        }
        result.push_back(program.data());

        for (const auto &argument: command.arguments()) {
            result.push_back(argument.c_str());
        }

        result.push_back(nullptr);

        return result;
    }

    std::vector<const char *> RunVisitor::constructEnvironment(const Env &env) {
        std::vector<const char *> result;
        result.reserve(env.entries().size() + 1);

        for (const auto &[_, value]: env.entries()) {
            result.push_back(value.c_str());
        }

        result.push_back(nullptr);

        return result;
    }

    void RunVisitor::start(Command &command, int inPipe, int outPipe) {
        if (command.process()) {
            throw std::logic_error{"Command was already started"};
        }
        command.setupProcess();
        if (inPipe >= 0) {
            RedirectionSetupVisitor{in, command}(FDRedirection{inPipe});
        }
        if (outPipe >= 0) {
            RedirectionSetupVisitor{out, command}(FDRedirection{outPipe});
        }
        for (const auto &redirection: command.redirections()) {
            std::visit(RedirectionSetupVisitor{redirection.fd, command}, redirection.redirection);
        }
        command.process()->pid(wrappers::fork());
        if (command.process()->pid() == 0) {
            if (inPipe >= 0) {
                RedirectionApplyVisitor{in, command}(FDRedirection{inPipe});
            }
            if (outPipe >= 0) {
                RedirectionApplyVisitor{out, command}(FDRedirection{outPipe});
            }
            for (const auto &redirection: command.redirections()) {
                std::visit(RedirectionApplyVisitor{redirection.fd, command}, redirection.redirection);
            }
            auto args = constructArguments(command);
            auto env = constructEnvironment(context_.env());

            if (command.program().contains('/')) {
                try {
                    wrappers::execve(command.program().c_str(),
                                     const_cast<char **>(args.data()),
                                     const_cast<char **>(env.data()));
                } catch (const std::system_error &e) {
                    std::cerr << e.what() << ": " << command.program() << std::endl;
                    exit(e.code().value() == ENOENT ? 127 : 126);
                }
            } else {
                for (auto entry: std::views::split(context_.env().get("PATH"), ':')) {
                    try {
                        std::string executable{std::string_view{entry}};
                        executable += '/';
                        executable += command.program();
                        wrappers::execve(executable.c_str(),
                                         const_cast<char **>(args.data()),
                                         const_cast<char **>(env.data()));
                    } catch (const std::system_error &e) {
                    }
                    // if execute works, program is replaced and does not continue
                }
                std::cerr << "Command not found: " << command.program() << std::endl;
                exit(127);
            }
        }
    }

    void RunVisitor::wait(Command &command) {
        if (!command.process()) {
            throw std::logic_error{"Command was not started"};
        }
        if (command.process()->pid() == 0) {
            throw std::logic_error{"Command was already waited"};
        }

        int status;
        wrappers::waitpid(command.process()->pid(), &status, 0);
        command.process()->pid(0);

        if (WIFEXITED(status)) {
            command.process()->exit(WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            command.process()->exit(128 + WTERMSIG(status));
        } else {
            // Should not happen, leave 0
        }
        for (const auto &redirection: command.redirections()) {
            std::visit(RedirectionTeardownVisitor{redirection.fd, command}, redirection.redirection);
        }
    }

    void RunVisitor::visit(Command &command) {
        start(command);
        wait(command);
    }

    void RunVisitor::visit(Pipeline &pipeline) {
        FileDescriptor previousOutput;
        for (std::size_t i = pipeline.commands().size() - 1; i > 0; --i) {
            int fds[2];
            wrappers::pipe(fds);

            FileDescriptor input{fds[0]};
            FileDescriptor output{fds[1]};

            wrappers::fcntlSetFD(input.fd(), wrappers::fcntlGetFD(fds[0]) | FD_CLOEXEC);
            wrappers::fcntlSetFD(output.fd(), wrappers::fcntlGetFD(fds[1]) | FD_CLOEXEC);

            start(pipeline.commands()[i], input.fd(), previousOutput.fd());

            // Keep output fd open until previous has run
            previousOutput = std::move(output);
        }
        start(pipeline.commands()[0], -1, previousOutput.fd());
        previousOutput.close();
        for (size_t i = pipeline.commands().size(); i > 0; --i) {
            wait(pipeline.commands()[i - 1]);
        }
    }
}// namespace bashpp
