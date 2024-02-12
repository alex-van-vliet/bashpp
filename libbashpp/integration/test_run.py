import os
import subprocess
import tempfile


def no_action():
    pass


def run_test(test_name: str, stdin: str = '', stdout: str = '', stderr: str = '', exit=0, setup=no_action,
             teardown=no_action, variables=None):
    with tempfile.TemporaryDirectory() as test_dir:
        os.chdir(test_dir)
        setup()
        process = subprocess.Popen([os.getenv('BASHPP_INTEGRATION_EXE'), test_name],
                                   executable=os.getenv('BASHPP_INTEGRATION_EXE'), stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        out, err = process.communicate(stdin.encode())
        assert process.returncode == 0
        program_stderr = err.decode().removeprefix('========== STDERR START ==========\n').removesuffix(
            '========== STDERR STOP ==========\n')
        assert program_stderr == stderr
        program_stdout, captured = out.decode().split('========== CAPTURED ==========\n')
        program_stdout = program_stdout.removeprefix('========== STDOUT START ==========\n').removesuffix(
            '========== STDOUT STOP ==========\n')
        assert program_stdout == stdout

        captured_exit, *captured = captured.split('\n')
        captured = '\n'.join(captured)
        captured_exit = int(captured_exit.removeprefix('EXIT CODE: '))
        assert captured_exit == exit
        teardown()

        captured_fds = {}
        while captured:
            fd = int(captured[len('---------- '):captured.find(' START')])
            end = captured.find(f'---------- {fd} STOP ----------\n')
            captured_fds[fd] = captured[len(f'---------- {fd} START ----------\n'):end]
            captured = captured[end+len(f'---------- {fd} STOP ----------\n'):]

        assert captured_fds == (variables or {})


def test_simple_echo():
    run_test('test_simple_echo', stdout='Hello, world!\n')


def test_simple_error():
    run_test('test_simple_error', stderr='Bye, world!\n')


def test_exit():
    run_test('test_exit', exit=10)


def test_signal():
    run_test('test_signal', exit=137)


def test_redirect_out_to_err():
    run_test('test_redirect_out_to_err', stderr='Hello, world!\n')


def test_redirect_err_to_out():
    run_test('test_redirect_err_to_out', stdout='Hello, world!\n')


def test_close_stdout():
    run_test('test_close_stdout', stderr='Stdout is closed\n')


def test_redirect_stdin_from_file():
    def setup():
        with open('./file.txt', 'w') as f:
            print('Hello from file', file=f)

    run_test('test_redirect_stdin_from_file', setup=setup, stderr='Hello from file\n')


def test_redirect_stdout_to_file():
    def setup():
        with open('./file.txt', 'w') as f:
            print('Original file', file=f)

    def teardown():
        with open('./file.txt', 'r') as f:
            assert f.read() == 'Printed from test\n'

    run_test('test_redirect_stdout_to_file', setup=setup, teardown=teardown)


def test_redirect_stdout_to_file_and_append():
    def setup():
        with open('./file.txt', 'w') as f:
            print('Original file', file=f)

    def teardown():
        with open('./file.txt', 'r') as f:
            assert f.read() == 'Original file\nPrinted from test\n'

    run_test('test_redirect_stdout_to_file_and_append', setup=setup, teardown=teardown)


def test_redirect_stdout_to_read_write():
    def setup():
        with open('./file.txt', 'w') as f:
            print('Original file', file=f)

    def teardown():
        with open('./file.txt', 'r') as f:
            assert f.read() == 'Original file\nPrinted from test\n'

    run_test('test_redirect_stdout_to_read_write', setup=setup, teardown=teardown, stderr='Original file\n')


def test_redirect_stdin_from_variable():
    run_test('test_redirect_stdin_from_variable', stderr='Printed from test\n')


def test_redirect_stdout_to_variable():
    run_test('test_redirect_stdout_to_variable', variables={1: 'Printed from test\n'})
