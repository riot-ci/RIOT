/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rail
 * @{
 *
 * @file
 * @brief       low level radio configuration for IEEE 802.15.4 mode on 868 MHz
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 */

#ifndef RAIL_802154_CONFIG_H
#define RAIL_802154_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RAIL Configuration
 *
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 *
 * Adapted form mbed
 * https://github.com/ARMmbed/mbed-os/blob/master/targets/TARGET_Silicon_Labs/TARGET_SL_RAIL/efr32-rf-driver/rail/TOOLCHAIN_GCC_ARM/TARGET_EFR32MG1/ieee802154_subg_efr32xg1_configurator_out.c
 *
 * @{
 */
#define RAIL_IEEE802154_CONFIG_868MHZ { \
        0x01010FF4UL, 0x00000000UL, \
        0x01010FF8UL, 0x0003C000UL, \
        0x01010FFCUL, 0x0003C008UL, \
        0x00010004UL, 0x00157001UL, \
        0x00010008UL, 0x0000007FUL, \
        0x00010018UL, 0x00000000UL, \
        0x0001001CUL, 0x00000000UL, \
        0x00010028UL, 0x00000000UL, \
        0x0001002CUL, 0x00000000UL, \
        0x00010030UL, 0x00000000UL, \
        0x00010034UL, 0x00000000UL, \
        0x0001003CUL, 0x00000000UL, \
        0x00010040UL, 0x000007A0UL, \
        0x00010048UL, 0x00000000UL, \
        0x00010054UL, 0x00000000UL, \
        0x00010058UL, 0x00000000UL, \
        0x000100A0UL, 0x00004000UL, \
        0x000100A4UL, 0x00004CFFUL, \
        0x000100A8UL, 0x00004100UL, \
        0x000100ACUL, 0x00004DFFUL, \
        0x00012000UL, 0x00000704UL, \
        0x00012010UL, 0x00000000UL, \
        0x00012018UL, 0x00008408UL, \
        0x00013008UL, 0x0000AC3FUL, \
        0x0001302CUL, 0x021EB000UL, \
        0x00013030UL, 0x00108000UL, \
        0x00013034UL, 0x00000003UL, \
        0x0001303CUL, 0x00014000UL, \
        0x00013040UL, 0x00000000UL, \
        0x000140A0UL, 0x0F00277AUL, \
        0x000140F4UL, 0x00001020UL, \
        0x00014134UL, 0x00000880UL, \
        0x00014138UL, 0x000087F6UL, \
        0x00014140UL, 0x00880048UL, \
        0x00014144UL, 0x1153E6C0UL, \
        0x00016014UL, 0x00000010UL, \
        0x00016018UL, 0x04127920UL, \
        0x0001601CUL, 0x0051C007UL, \
        0x00016020UL, 0x000000C2UL, \
        0x00016024UL, 0x00000000UL, \
        0x00016028UL, 0x03000000UL, \
        0x0001602CUL, 0x00000000UL, \
        0x00016030UL, 0x00FF0BF4UL, \
        0x00016034UL, 0x00000C20UL, \
        0x00016038UL, 0x0102000AUL, \
        0x0001603CUL, 0x00080430UL, \
        0x00016040UL, 0x000000A7UL, \
        0x00016044UL, 0x00000000UL, \
        0x00016048UL, 0x04602123UL, \
        0x0001604CUL, 0x0000A47CUL, \
        0x00016050UL, 0x00000018UL, \
        0x00016054UL, 0x00000000UL, \
        0x00016058UL, 0x00000000UL, \
        0x0001605CUL, 0x30100101UL, \
        0x00016060UL, 0x7F7F7050UL, \
        0x00016064UL, 0x00000000UL, \
        0x00017014UL, 0x000270F1UL, \
        0x00017018UL, 0x00001700UL, \
        0x0001701CUL, 0x82840000UL, \
        0x00017028UL, 0x00000000UL, \
        0x00017048UL, 0x0000383EUL, \
        0x0001704CUL, 0x000025BCUL, \
        0x00017070UL, 0x00010103UL, \
        0x00017074UL, 0x00000442UL, \
        0x00017078UL, 0x006D8480UL, \
        0xFFFFFFFFUL, \
}

#define RAIL_IEEE802154_CONFIG_915MHZ { \
        0x01010FF4UL, 0x00000000UL, \
        0x01010FF8UL, 0x0003C000UL, \
        0x01010FFCUL, 0x0003C008UL, \
        0x00010004UL, 0x00157001UL, \
        0x00010008UL, 0x0000007FUL, \
        0x00010018UL, 0x00000000UL, \
        0x0001001CUL, 0x00000000UL, \
        0x00010028UL, 0x00000000UL, \
        0x0001002CUL, 0x00000000UL, \
        0x00010030UL, 0x00000000UL, \
        0x00010034UL, 0x00000000UL, \
        0x0001003CUL, 0x00000000UL, \
        0x00010040UL, 0x000007A0UL, \
        0x00010048UL, 0x00000000UL, \
        0x00010054UL, 0x00000000UL, \
        0x00010058UL, 0x00000000UL, \
        0x000100A0UL, 0x00004000UL, \
        0x000100A4UL, 0x00004CFFUL, \
        0x000100A8UL, 0x00004100UL, \
        0x000100ACUL, 0x00004DFFUL, \
        0x00012000UL, 0x00000704UL, \
        0x00012010UL, 0x00000000UL, \
        0x00012018UL, 0x00008408UL, \
        0x00013008UL, 0x0000AC3FUL, \
        0x0001302CUL, 0x02364000UL, \
        0x00013030UL, 0x00108000UL, \
        0x00013034UL, 0x00000003UL, \
        0x0001303CUL, 0x00014000UL, \
        0x00013040UL, 0x00000000UL, \
        0x000140A0UL, 0x0F00277AUL, \
        0x000140F4UL, 0x00001020UL, \
        0x00014134UL, 0x00000880UL, \
        0x00014138UL, 0x000087F6UL, \
        0x00014140UL, 0x00880048UL, \
        0x00014144UL, 0x1153E6C0UL, \
        0x00016014UL, 0x00000010UL, \
        0x00016018UL, 0x04127920UL, \
        0x0001601CUL, 0x0051C007UL, \
        0x00016020UL, 0x000000C2UL, \
        0x00016024UL, 0x00000000UL, \
        0x00016028UL, 0x03000000UL, \
        0x0001602CUL, 0x00000000UL, \
        0x00016030UL, 0x00FF04C8UL, \
        0x00016034UL, 0x000008A2UL, \
        0x00016038UL, 0x0100000AUL, \
        0x0001603CUL, 0x00080430UL, \
        0x00016040UL, 0x000000A7UL, \
        0x00016044UL, 0x00000000UL, \
        0x00016048UL, 0x0AC02123UL, \
        0x0001604CUL, 0x0000A47CUL, \
        0x00016050UL, 0x00000018UL, \
        0x00016054UL, 0x00000000UL, \
        0x00016058UL, 0x00000000UL, \
        0x0001605CUL, 0x30100101UL, \
        0x00016060UL, 0x7F7F7050UL, \
        0x00016064UL, 0x00000000UL, \
        0x00017014UL, 0x000270F1UL, \
        0x00017018UL, 0x00001700UL, \
        0x0001701CUL, 0x82840000UL, \
        0x00017028UL, 0x00000000UL, \
        0x00017048UL, 0x0000383EUL, \
        0x0001704CUL, 0x000025BCUL, \
        0x00017070UL, 0x00010103UL, \
        0x00017074UL, 0x00000442UL, \
        0x00017078UL, 0x006D8480UL, \
        0xFFFFFFFFUL, \
}

/** @} */


#ifdef __cplusplus
}
#endif


#endif /* RAIL_802154_CONFIG_H */
/** @} */
