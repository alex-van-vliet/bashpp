#include "start_visitor.hpp"

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
}// namespace

namespace bashpp {
    std::vector<const char *> StartVisitor::constructArguments(const Command &command) {
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

    std::vector<const char *> StartVisitor::constructEnvironment(const Env &env) {
        std::vector<const char *> result;
        result.reserve(env.entries().size() + 1);

        for (const auto &[_, value]: env.entries()) {
            result.push_back(value.c_str());
        }

        result.push_back(nullptr);

        return result;
    }

    void StartVisitor::visit(Command &command) {
        if (command.process()) {
            throw std::logic_error{"Command was already started"};
        }
        command.setupProcess();
        for (const auto &redirection: command.redirections()) {
            std::visit(RedirectionSetupVisitor{redirection.fd, command}, redirection.redirection);
        }
        command.process()->pid(wrappers::fork());
        if (command.process()->pid() == 0) {
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

    void StartVisitor::visit(Pipeline &pipeline) {
        (void) pipeline;
    }
}// namespace bashpp
