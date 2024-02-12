#include <bashpp/command.hpp>
#include <bashpp/context.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <span>
#include <string_view>

using namespace bashpp;

void test(Context &context, Node &&command) {
    std::cout << "========== STDOUT START ==========" << std::endl;
    std::cerr << "========== STDERR START ==========" << std::endl;
    command.start(context);
    command.wait();
    std::cout << "========== STDOUT STOP ==========" << std::endl;
    std::cerr << "========== STDERR STOP ==========" << std::endl;

    std::cout << "========== CAPTURED ==========" << std::endl;
    auto &p = *dynamic_cast<Command &>(command).process();
    std::cout << "EXIT CODE: " << +p.exit() << std::endl;
    for (const auto &redirection: p.redirections()) {
        std::cout << "---------- " << redirection.first << " START ----------" << std::endl;
        std::cout << std::string_view(reinterpret_cast<const char *>(redirection.second.second.data()),
                                      redirection.second.second.size());
        std::cout << "---------- " << redirection.first << " STOP ----------" << std::endl;
    }

    std::cout << "========== FD LEAKS ==========" << std::endl;
    for (int i = 3; i < FD_SETSIZE; ++i) {
        if (wrappers::fcntlGetFL(wrappers::Allow{{EBADF}}, i) != -1) {
            std::cout << i << std::endl;
        }
    }
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
    const char *data = "Printed from test\n";
    std::span<const std::byte> span{reinterpret_cast<const std::byte *>(data), strlen(data)};
    test(context, Command{"script.py", {"--read", "0"}, {{in, InputVariableRedirection{{span.begin(), span.end()}}}}});
}

void test_redirect_stdout_to_variable(Context &context) {
    test(context, Command{"script.py", {"--print", "Printed from test\n"}, {{out, OutputVariableRedirection{}}}});
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
