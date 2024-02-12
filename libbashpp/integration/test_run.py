import os
import subprocess


def run_test(test_name: str, stdin: str = '', stdout: str = '', stderr: str = '', exit=0):
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

    captured_exit = int(captured.removeprefix('EXIT CODE: ').removesuffix('\n'))
    assert captured_exit == exit


def test_simple_echo():
    run_test('test_simple_echo', stdout='Hello, world!\n')


def test_simple_error():
    run_test('test_simple_error', stderr='Bye, world!\n')


def test_exit():
    run_test('test_exit', exit=10)


def test_signal():
    run_test('test_signal', exit=137)
