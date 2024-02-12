#!/usr/bin/env python3
import os
import sys
import argparse
import typing
import signal


class Print(typing.NamedTuple):
    content: str

    @classmethod
    def build(cls, content: str):
        return cls(content)

    def execute(self):
        print(self.content, end='')


class Error(typing.NamedTuple):
    content: str

    @classmethod
    def build(cls, content: str):
        return cls(content)

    def execute(self):
        print(self.content, end='', file=sys.stderr)


class Exit(typing.NamedTuple):
    code: int

    @classmethod
    def build(cls, code: str):
        return cls(int(code))

    def execute(self):
        sys.exit(self.code)


class Signal(typing.NamedTuple):
    number: int

    @classmethod
    def build(cls, number: str):
        return cls(int(number))

    def execute(self):
        signal.raise_signal(self.number)


class TestStdout(typing.NamedTuple):
    def execute(self):
        if sys.stdout is None:
            print('Stdout is closed', file=sys.stderr)
        else:
            print('Stdout is open')
            print('Stdout is open', file=sys.stderr)


class Read(typing.NamedTuple):
    stream: typing.IO

    @classmethod
    def build(cls, number: str):
        return cls({
                       '0': sys.stdin,
                       '1': os.fdopen(os.dup(1), 'w+'),
                   }[number])

    def execute(self):
        print(self.stream.read(), end='', file=sys.stderr)


def main(prog, *args):
    parser = argparse.ArgumentParser(prog=prog, description='Script to run various scenarios')
    parser.add_argument('--print', dest='instructions', action='append', type=Print.build,
                        help='Prints the argument to stdout')
    parser.add_argument('--error', dest='instructions', action='append', type=Error.build,
                        help='Prints the argument to stderr')
    parser.add_argument('--exit', dest='instructions', action='append', type=Exit.build,
                        help='Exits with the given code')
    parser.add_argument('--signal', dest='instructions', action='append', type=Signal.build,
                        help='Sends the signal with the given number to itself')
    parser.add_argument('--test-stdout', dest='instructions', action='append_const', const=TestStdout(),
                        help='Prints whether stdout is open/closed to stderr')
    parser.add_argument('--read', dest='instructions', action='append', type=Read.build,
                        help='Read from stdin or stdout and prints it on stderr')

    args = parser.parse_args(args)

    for instruction in args.instructions:
        instruction.execute()


if __name__ == '__main__':
    sys.exit(main(*sys.argv))
