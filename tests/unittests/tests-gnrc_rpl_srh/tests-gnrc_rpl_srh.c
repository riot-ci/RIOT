/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Cenk Gündoğan <mail@cgundogan.de>
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */
#include <stdlib.h>
#include <string.h>
#include "embUnit.h"

#include "net/ipv6/addr.h"
#include "net/ipv6/ext.h"
#include "net/ipv6/hdr.h"
#include "net/gnrc/rpl/srh.h"
#include "net/gnrc/ipv6/ext/rh.h"

#include "unittests-constants.h"
#include "tests-gnrc_rpl_srh.h"


Test *tests_rpl_srh_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_rpl_srh_dst_multicast),
        new_TestFixture(test_rpl_srh_route_multicast),
        new_TestFixture(test_rpl_srh_too_many_seg_left),
        new_TestFixture(test_rpl_srh_nexthop_no_prefix_elided),
        new_TestFixture(test_rpl_srh_nexthop_prefix_elided),
    };

    EMB_UNIT_TESTCALLER(rpl_srh_tests, set_up, NULL, fixtures);

    return (Test *)&rpl_srh_tests;
}

void tests_gnrc_rpl_srh(void)
{
    TESTS_RUN(tests_rpl_srh_tests());
}
/** @} */
