#!/usr/bin/env python3

import os
import sys
import matplotlib.pyplot as plt
import numpy as np
import re


def testfunc(child):
    child.expect(r'------------- BEGIN STATISTICS --------------')
    child.expect(r'=== (.+) ===')
    title = child.match.group(1)
    xlabels = []
    minlst = []
    maxlst = []
    meanlst = []
    variancelst = []
    while True:
        child.expect([r'(TOTAL)', r'(\w[-\w\s]+):\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)'])
        print("Match: %r" % (child.match.groups(), ))
        if child.match.group(1) == 'TOTAL':
            break
        label, count, thesum, sum_sq, minimum, maximum, mean, variance = child.match.groups()
        xlabels.append(re.sub(' +',' ', label))
        minlst.append(minimum)
        maxlst.append(maximum)
        meanlst.append(mean)
        variancelst.append(variance)

    print("xlabels: %r" % (xlabels, ))
    print("ymin: %r" % (minlst, ))
    print("ymax: %r" % (maxlst, ))
    print("means: %r" % (meanlst, ))
    print("variances: %r" % (variancelst, ))
    mins = np.array([int(x) for x in minlst])
    maxes = np.array([int(x) for x in maxlst])
    means = np.array([int(x) for x in meanlst])
    variances = np.array([int(x) for x in variancelst])
    std = np.sqrt(variances)
    print("std: %r" % (std, ))
    # plot standard deviation, min and max as error bars
    plt.errorbar(xlabels, means, std, fmt='ok', lw=3)
    plt.errorbar(xlabels, means, [means - mins, maxes - means],
                 fmt='.k', ecolor='gray', lw=1)
    plt.title(title)
    plt.ylabel("expected - actual [ticks]")
    plt.xlabel("target duration [ticks]")
    plt.xticks(rotation=90)
    plt.show()

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc, timeout=None))
