#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect(u'START')
    for i in range(5):
        child.expect_exact(u'<SCOPE {}{}>'
                           .format(i + 1, ' /' if i == 4 else ''))

    child.expect_exact(u'Cleanup: <5>')
    child.expect_exact(u'</SCOPE 4>')
    child.expect_exact(u'</SCOPE 3>')
    for i in (3, 2, 1):
        child.expect_exact(u'Cleanup: <{}>'.format(i))
    child.expect_exact(u'Result: 1234')
    child.expect(u'SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
