#!/usr/bin/env python3

import sys
from testrunner import run, test_utils_interactive_sync


def expect_unary(child):
    child.expect(r'RANGE: (\d+)')
    iterations = int(child.match.group(1))
    for _ in range(iterations):
        for op_name in ('abs', 'sq', 'atan', 'exp'):
            child.expect(r'{}\(-?\d+\.\d+\) = -?\d+\.\d+'.format(op_name))

    child.expect(r'RANGE: (\d+)')
    iterations = int(child.match.group(1))
    for _ in range(iterations):
        for op_name in ('sin', 'cos', 'tan'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    child.expect(r'RANGE: (\d+)')
    iterations = int(child.match.group(1))
    for _ in range(iterations):
        for op_name in ('asin', 'acos'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    child.expect(r'RANGE: (\d+)')
    iterations = int(child.match.group(1))
    for _ in range(iterations):
        for op_name in ('sqrt', 'log', 'log2', 'slog2'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))


def expect_binary(child):
    child.expect(r'RANGE: (\d+)')
    iterations = int(child.match.group(1))
    for _ in range(iterations):
        for op_name in ('add', 'sub', 'mul', 'div', 'mod', 'sadd', 'ssub',
                        'smul', 'sdiv', 'min', 'max'):
            child.expect(r'{}\(-?\d+.\d+\, -?\d+.\d+\) = -?\d+.\d+'
                         .format(op_name))


def testfunc(child):
    test_utils_interactive_sync(child)
    child.expect_exact('Unary.')
    expect_unary(child)
    child.expect_exact('Binary.')
    expect_binary(child)
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))
