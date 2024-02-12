#!/usr/bin/env python3

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

    args = parser.parse_args(args)

    for instruction in args.instructions:
        instruction.execute()


if __name__ == '__main__':
    sys.exit(main(*sys.argv))
