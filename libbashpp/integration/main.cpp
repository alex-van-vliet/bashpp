#include <bashpp/command.hpp>
#include <bashpp/context.hpp>
#include <bashpp/helpers.hpp>
#include <bashpp/pipeline.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <span>
#include <string_view>

using namespace bashpp;
using namespace nlohmann;

void test(Context &context, Node &&node) {
    node.run(context);
    std::cout << "========== CAPTURED ==========" << std::endl;

    json result;
    result["exit_code"] = +node.exit();
    result["processes"] = json::array();
    result["redirections"] = json::object();
    if (auto command = dynamic_cast<Command *>(&node)) {
        result["processes"].push_back(json::object());
        result["processes"].back()["exit_code"] = +command->process()->exit();
        result["processes"].back()["redirections"] = json::object();
        auto &process = command->process();
        for (const auto &redirection: process->redirections()) {
            result["processes"].back()["redirections"][std::to_string(redirection.first)] = byteSpanToSv(redirection.second.second);
            result["redirections"][std::to_string(redirection.first)] = byteSpanToSv(command->redirection(redirection.first));
        }
    } else if (auto pipeline = dynamic_cast<Pipeline *>(&node)) {
        for (auto &command: pipeline->commands()) {
            result["processes"].push_back(json::object());
            result["processes"].back()["exit_code"] = +command.process()->exit();
            result["processes"].back()["redirections"] = json::object();
            auto &process = command.process();
            for (const auto &redirection: process->redirections()) {
                result["processes"].back()["redirections"][std::to_string(redirection.first)] = byteSpanToSv(redirection.second.second);
            }
        }
        for (const auto &redirection: pipeline->commands().back().process()->redirections()) {
            result["redirections"][std::to_string(redirection.first)] = byteSpanToSv(pipeline->redirection(redirection.first));
        }
    } else {
        throw std::logic_error{"Invalid node"};
    }
    result["fd_leaks"] = json::array();
#ifdef DEBUG
    int startFd = 4;
#else
    int startFd = 3;
#endif
    for (int i = startFd; i < FD_SETSIZE; ++i) {
        if (wrappers::fcntlGetFL(wrappers::Allow{{EBADF}}, i) != -1) {
            result["fd_leaks"].push_back(i);
        }
    }
    std::cout << result.dump(4) << std::endl;
}

void test_simple_echo(Context &context) {
    test(context, Command{"script.py", {"--print", "Hello, world!\n"}});
}

void test_simple_error(Context &context) {
    test(context, Command{"script.py", {"--error", "Bye, world!\n"}});
}

void test_exit(Context &context) {
    test(context, Command{"script.py", {"--exit", "10"}});
}

void test_signal(Context &context) {
    test(context, Command{"script.py", {"--signal", "9"}});
}

void test_redirect_out_to_err(Context &context) {
    test(context, Command{"script.py", {"--print", "Hello, world!\n"}, {{out, FDRedirection{err}}}});
}

void test_redirect_err_to_out(Context &context) {
    test(context, Command{"script.py", {"--error", "Hello, world!\n"}, {{err, FDRedirection{out}}}});
}

void test_close_stdout(Context &context) {
    test(context, Command{"script.py", {"--test-stdout"}, {{out, CloseFDRedirection{}}}});
}

void test_redirect_stdin_from_file(Context &context) {
    test(context, Command{"script.py", {"--read", "0"}, {{in, InputPathRedirection{"./file.txt"}}}});
}

void test_redirect_stdout_to_file(Context &context) {
    test(context, Command{"script.py", {"--print", "Printed from test\n"}, {{out, OutputPathRedirection{"./file.txt"}}}});
}

void test_redirect_stdout_to_file_and_append(Context &context) {
    test(context, Command{"script.py", {"--print", "Printed from test\n"}, {{out, OutputPathAppendRedirection{"./file.txt"}}}});
}

void test_redirect_stdout_to_read_write(Context &context) {
    test(context, Command{"script.py", {"--read", "1", "--print", "Printed from test\n"}, {{out, InputOutputPathRedirection{"./file.txt"}}}});
}

void test_redirect_stdin_from_variable(Context &context) {
    auto data = svToByteSpan("Printed from test\n");
    test(context, Command{"script.py", {"--read", "0"}, {{in, InputVariableRedirection{{data.begin(), data.end()}}}}});
}

void test_redirect_stdout_to_variable(Context &context) {
    test(context, Command{"script.py", {"--print", "Printed from test\n"}, {{out, OutputVariableRedirection{}}}});
}

void test_pipe_two_processes(Context &context) {
    test(context,
         Command{"script.py", {"--print", "First line\nInvalid line\nSecond line\n"}} |
                 Command{"script.py", {"--filter", "Invalid"}});
}

void test_pipe_three_processes(Context &context) {
    test(context,
         Command{"script.py", {"--print", "First line\nInvalid line\nSecond line\nErroneous line\n"}} |
                 Command{"script.py", {"--filter", "Invalid"}} |
                 Command{"script.py", {"--filter", "Erroneous"}});
}

void test_complex_1(Context &context) {
    auto data = svToByteSpan("This is\nA complica1ed\nA test\nT3st\nWhere\nNumbers are removed!\n");
    test(context, Command{"script.py", {"--read", "0"}, {{in, InputVariableRedirection{{data.begin(), data.end()}}}, {err, FDRedirection{1}}}} |
                          Command{"script.py", {"--filter", "1"}} |
                          Command{"script.py", {"--filter", "3"}, {{out, OutputVariableRedirection{}}}});
}

int main(int argc, const char *argv[]) {
    std::map<std::string_view, void (*)(Context &)> tests{
            {"test_simple_echo", test_simple_echo},
            {"test_simple_error", test_simple_error},
            {"test_exit", test_exit},
            {"test_signal", test_signal},
            {"test_redirect_out_to_err", test_redirect_out_to_err},
            {"test_redirect_err_to_out", test_redirect_err_to_out},
            {"test_close_stdout", test_close_stdout},
            {"test_redirect_stdin_from_file", test_redirect_stdin_from_file},
            {"test_redirect_stdout_to_file", test_redirect_stdout_to_file},
            {"test_redirect_stdout_to_file_and_append", test_redirect_stdout_to_file_and_append},
            {"test_redirect_stdout_to_read_write", test_redirect_stdout_to_read_write},
            {"test_redirect_stdin_from_variable", test_redirect_stdin_from_variable},
            {"test_redirect_stdout_to_variable", test_redirect_stdout_to_variable},
            {"test_pipe_two_processes", test_pipe_two_processes},
            {"test_pipe_three_processes", test_pipe_three_processes},
            {"test_complex_1", test_complex_1},
    };

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [test name]" << std::endl;
        return 1;
    }

    auto test = tests.find(argv[1]);
    if (test == tests.end()) {
        std::cerr << "Test not found: " << argv[1] << std::endl;
        std::cerr << "Available tests: " << std::endl;
        for (auto &[test_name, _]: tests) {
            std::cerr << "- " << test_name << std::endl;
        }
        return 2;
    }

    Context context;
    std::string oldPath{context.env().get("PATH")};
    context.env().set("PATH", std::filesystem::absolute(__FILE__).parent_path().string() + ':' + oldPath);
    test->second(context);
}
