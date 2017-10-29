#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def _check_test_output(child):
    child.expect(u'show tls values:')
    for i in range(20):
        if i != 5:
            child.expect(u'key\[%d\]: [0-9]{10}, val: %d'
                         % (i, i + 1 if i != 3 else 42))


def testfunc(child):
    child.expect(u'START')

    child.expect(u'-= TEST 1 - create 20 tls with sequencial values 0...19 =-')
    _check_test_output(child)
    child.expect(u'-= TEST 2 - '
                 'delete deliberate key \(key\[5\]:[0-9]{10}\) =-')
    _check_test_output(child)
    child.expect(u'-= TEST 3 - create new tls =-')
    _check_test_output(child)
    child.expect(u'-= TEST 4 - delete all keys =-')
    child.expect(u'show tls values:')
    child.expect(u'-= TEST 5 - try delete non-existing key =-')
    child.expect(u'try to delete returns: 0')
    child.expect(u'-= TEST 6 - add key and delete without a tls =-')
    child.expect(u'created key: [0-9]{10}')
    child.expect(u'try to delete returns: 0')
    child.expect(u'-= TEST 7 - add key without tls =-')
    child.expect(u'created key: [0-9]{10}')
    child.expect(u'test_7_val: \(nil\)')
    child.expect(u'tls tests finished.')

    child.expect(u'SUCCESS')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
