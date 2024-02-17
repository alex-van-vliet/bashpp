import os
import subprocess
import tempfile
import json


def no_action():
    pass


def run_test(test_name: str, stdin: str = '', stdout: str = '', stderr: str = '', exit=0, setup=no_action,
             teardown=no_action, variables=None, timeout=10):
    with tempfile.TemporaryDirectory() as test_dir:
        os.chdir(test_dir)
        setup()
        process = subprocess.Popen([os.getenv('BASHPP_INTEGRATION_EXE'), test_name],
                                   executable=os.getenv('BASHPP_INTEGRATION_EXE'), stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE)
        try:
            out, err = process.communicate(stdin.encode(), timeout=timeout)
        except subprocess.TimeoutExpired:
            process.kill()
            assert False, f'Process timed out'
        assert process.returncode == 0

        out = out.decode()
        err = err.decode()

        out, captured = out.split('========== CAPTURED ==========\n')
        captured = json.loads(captured)

        assert err == stderr
        assert out == stdout

        assert captured['exit_code'] == exit
        assert captured['redirections'] == (variables or {})
        teardown()

        assert captured['fd_leaks'] == []


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
    run_test('test_redirect_stdout_to_variable', variables={'1': 'Printed from test\n'})


def test_pipe_two_processes():
    run_test('test_pipe_two_processes', stdout='First line\nSecond line\n')


def test_pipe_three_processes():
    run_test('test_pipe_two_processes', stdout='First line\nSecond line\n')


def test_complex_1():
    run_test('test_complex_1', variables={'1': 'This is\nA test\nWhere\nNumbers are removed!\n'})
