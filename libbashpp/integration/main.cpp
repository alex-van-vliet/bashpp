#include <bashpp/command.hpp>
#include <bashpp/context.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
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

int main(int argc, const char *argv[]) {
    std::map<std::string_view, void (*)(Context &)> tests{
            {"test_simple_echo", test_simple_echo},
            {"test_simple_error", test_simple_error},
            {"test_exit", test_exit},
            {"test_signal", test_signal},
            {"test_redirect_out_to_err", test_redirect_out_to_err},
            {"test_redirect_err_to_out", test_redirect_err_to_out},
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
