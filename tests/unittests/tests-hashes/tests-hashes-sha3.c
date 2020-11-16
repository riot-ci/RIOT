/*
 * Copyright (C) 2017-2018 Mathias Tausig <mathias.tausig@fh-campsuwien.ac.at>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test cases for the SHA-3 hash implementation
 *
 * @author      Mathias Tausig <mathias.tausig@fh-campsuwien.ac.at>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "hashes/sha3.h"
#include "embUnit/embUnit.h"

/**
 * @brief expected SHA3-256 hash for test 01
 *
 * All test values taken from the Keccak code package:
 * https://github.com/gvanas/KeccakCodePackage
 * Files: TestVectors/ShortMsgKAT_SHA3-256.txt, TestVectors/ShortMsgKAT_SHA3-384.txt,
 *        TestVectors/ShortMsgKAT_SHA3-512.txt
 *
 * * converted using:
 * s=$(echo '<hash string>' | sed -e 's/../0x&, /g' | sed 's/, $//'); echo {$s}\;
 *
 * where <hash string> is the above sequence of characters A7...4A
 *
 *  msg = ''
 *  md_256 = A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A
 *  md_384 = 0C63A75B845E4F7D01107D852E4C2485C51A50AAAA94FC61995E71BBEE983A2A
 *           C3713831264ADB47FB6BD1E058D5F004
 *  md_512 = A69F73CCA23A9AC5C8B567DC185A756E97C982164FE25859E0D1DCC1475C80A6
 *           15B2123AF1F5F94C11E3E9402C3AC558F500199D95B6D3E301758586281DCD26
 *
 */
static const uint8_t m01[1];
size_t m01_len = 0;
static const uint8_t m01_1[1];
size_t m01_1_len = 0;
static const uint8_t m01_2[1];
size_t m01_2_len = 0;
static const uint8_t h01_256[] = { 0xA7, 0xFF, 0xC6, 0xF8, 0xBF, 0x1E, 0xD7, 0x66,
                                   0x51, 0xC1, 0x47, 0x56, 0xA0, 0x61, 0xD6, 0x62,
                                   0xF5, 0x80, 0xFF, 0x4D, 0xE4, 0x3B, 0x49, 0xFA,
                                   0x82, 0xD8, 0x0A, 0x4B, 0x80, 0xF8, 0x43, 0x4A };
static const uint8_t h01_384[] = { 0x0C, 0x63, 0xA7, 0x5B, 0x84, 0x5E, 0x4F, 0x7D,
                                   0x01, 0x10, 0x7D, 0x85, 0x2E, 0x4C, 0x24, 0x85,
                                   0xC5, 0x1A, 0x50, 0xAA, 0xAA, 0x94, 0xFC, 0x61,
                                   0x99, 0x5E, 0x71, 0xBB, 0xEE, 0x98, 0x3A, 0x2A,
                                   0xC3, 0x71, 0x38, 0x31, 0x26, 0x4A, 0xDB, 0x47,
                                   0xFB, 0x6B, 0xD1, 0xE0, 0x58, 0xD5, 0xF0, 0x04 };
static const uint8_t h01_512[] = { 0xA6, 0x9F, 0x73, 0xCC, 0xA2, 0x3A, 0x9A, 0xC5,
                                   0xC8, 0xB5, 0x67, 0xDC, 0x18, 0x5A, 0x75, 0x6E,
                                   0x97, 0xC9, 0x82, 0x16, 0x4F, 0xE2, 0x58, 0x59,
                                   0xE0, 0xD1, 0xDC, 0xC1, 0x47, 0x5C, 0x80, 0xA6,
                                   0x15, 0xB2, 0x12, 0x3A, 0xF1, 0xF5, 0xF9, 0x4C,
                                   0x11, 0xE3, 0xE9, 0x40, 0x2C, 0x3A, 0xC5, 0x58,
                                   0xF5, 0x00, 0x19, 0x9D, 0x95, 0xB6, 0xD3, 0xE3,
                                   0x01, 0x75, 0x85, 0x86, 0x28, 0x1D, 0xCD, 0x26 };

/**
 * @brief expected SHA3-256 hash for test 02
   msg = '4A4F202484512526'
   md_256 = BA4FB009D57A5CEB85FC64D54E5C55A55854B41CC47AD15294BC41F32165DFBA
   md_384 = 89DBF4C39B8FB46FDF0A6926CEC0355A4BDBF9C6A446E140B7C8BD08FF6F489F
            205DAF8EFFE160F437F67491EF897C23
   md_512 = 150D787D6EB49670C2A4CCD17E6CCE7A04C1FE30FCE03D1EF2501752D92AE04C
            B345FD42E51038C83B2B4F8FD438D1B4B55CC588C6B913132F1A658FB122CB52
 */
static const uint8_t m02[] = { 0x4A, 0x4F, 0x20, 0x24, 0x84, 0x51, 0x25, 0x26 };
size_t m02_len = sizeof(m02);
static const uint8_t m02_1[] = { 0x4A, 0x4F };
size_t m02_1_len = sizeof(m02_1);
static const uint8_t m02_2[] = { 0x20, 0x24, 0x84, 0x51, 0x25, 0x26 };
size_t m02_2_len = sizeof(m02_2);
static const uint8_t h02_256[] = { 0xBA, 0x4F, 0xB0, 0x09, 0xD5, 0x7A, 0x5C, 0xEB,
                                   0x85, 0xFC, 0x64, 0xD5, 0x4E, 0x5C, 0x55, 0xA5,
                                   0x58, 0x54, 0xB4, 0x1C, 0xC4, 0x7A, 0xD1, 0x52,
                                   0x94, 0xBC, 0x41, 0xF3, 0x21, 0x65, 0xDF, 0xBA };
static const uint8_t h02_384[] = { 0x89, 0xDB, 0xF4, 0xC3, 0x9B, 0x8F, 0xB4, 0x6F,
                                   0xDF, 0x0A, 0x69, 0x26, 0xCE, 0xC0, 0x35, 0x5A,
                                   0x4B, 0xDB, 0xF9, 0xC6, 0xA4, 0x46, 0xE1, 0x40,
                                   0xB7, 0xC8, 0xBD, 0x08, 0xFF, 0x6F, 0x48, 0x9F,
                                   0x20, 0x5D, 0xAF, 0x8E, 0xFF, 0xE1, 0x60, 0xF4,
                                   0x37, 0xF6, 0x74, 0x91, 0xEF, 0x89, 0x7C, 0x23 };
static const uint8_t h02_512[] = { 0x15, 0x0D, 0x78, 0x7D, 0x6E, 0xB4, 0x96, 0x70,
                                   0xC2, 0xA4, 0xCC, 0xD1, 0x7E, 0x6C, 0xCE, 0x7A,
                                   0x04, 0xC1, 0xFE, 0x30, 0xFC, 0xE0, 0x3D, 0x1E,
                                   0xF2, 0x50, 0x17, 0x52, 0xD9, 0x2A, 0xE0, 0x4C,
                                   0xB3, 0x45, 0xFD, 0x42, 0xE5, 0x10, 0x38, 0xC8,
                                   0x3B, 0x2B, 0x4F, 0x8F, 0xD4, 0x38, 0xD1, 0xB4,
                                   0xB5, 0x5C, 0xC5, 0x88, 0xC6, 0xB9, 0x13, 0x13,
                                   0x2F, 0x1A, 0x65, 0x8F, 0xB1, 0x22, 0xCB, 0x52 };

/* @brief expected SHA3-256 hash for test 03
   msg = '2B6DB7CED8665EBE9DEB080295218426BDAA7C6DA9ADD2088932CDFFBAA1C14129
          BCCDD70F369EFB149285858D2B1D155D14DE2FDB680A8B027284055182A0CAE275
          234CC9C92863C1B4AB66F304CF0621CD54565F5BFF461D3B461BD40DF28198E373
          2501B4860EADD503D26D6E69338F4E0456E9E9BAF3D827AE685FB1D817'
   md_256 = B7D031AA69B7B4D26A35B896D761314F1D61EB12DCC1E72AAF61B9CD48003AF9
   md_384 = 8FD01909381EB713803419361D8E82E92476A08EDCC225BB8A135D215CB48D07
            B074624FCF2E73E666DBA59334719839
   md_512 = 4FAB45806B4628068458B5D0A2D4BF101B8BFC9276EF86AD5D883765C43F72CE
            8A5F7B4C5B535A915130BB185E699AB62228014E54DF790C0E93AADBE7E39E19
 */
static const uint8_t m03[] = { 0x2B, 0x6D, 0xB7, 0xCE, 0xD8, 0x66, 0x5E, 0xBE, 0x9D,
                               0xEB, 0x08, 0x02, 0x95, 0x21, 0x84, 0x26, 0xBD, 0xAA,
                               0x7C, 0x6D, 0xA9, 0xAD, 0xD2, 0x08, 0x89, 0x32, 0xCD,
                               0xFF, 0xBA, 0xA1, 0xC1, 0x41, 0x29, 0xBC, 0xCD, 0xD7,
                               0x0F, 0x36, 0x9E, 0xFB, 0x14, 0x92, 0x85, 0x85, 0x8D,
                               0x2B, 0x1D, 0x15, 0x5D, 0x14, 0xDE, 0x2F, 0xDB, 0x68,
                               0x0A, 0x8B, 0x02, 0x72, 0x84, 0x05, 0x51, 0x82, 0xA0,
                               0xCA, 0xE2, 0x75, 0x23, 0x4C, 0xC9, 0xC9, 0x28, 0x63,
                               0xC1, 0xB4, 0xAB, 0x66, 0xF3, 0x04, 0xCF, 0x06, 0x21,
                               0xCD, 0x54, 0x56, 0x5F, 0x5B, 0xFF, 0x46, 0x1D, 0x3B,
                               0x46, 0x1B, 0xD4, 0x0D, 0xF2, 0x81, 0x98, 0xE3, 0x73,
                               0x25, 0x01, 0xB4, 0x86, 0x0E, 0xAD, 0xD5, 0x03, 0xD2,
                               0x6D, 0x6E, 0x69, 0x33, 0x8F, 0x4E, 0x04, 0x56, 0xE9,
                               0xE9, 0xBA, 0xF3, 0xD8, 0x27, 0xAE, 0x68, 0x5F, 0xB1,
                               0xD8, 0x17 };
const size_t m03_len = sizeof(m03);
static const uint8_t m03_1[] = { 0x2B, 0x6D, 0xB7, 0xCE, 0xD8, 0x66, 0x5E, 0xBE, 0x9D,
                                 0xEB, 0x08, 0x02, 0x95, 0x21, 0x84, 0x26, 0xBD };
size_t m03_1_len = sizeof(m03_1);
static const uint8_t m03_2[] = { 0xAA, 0x7C, 0x6D, 0xA9, 0xAD, 0xD2, 0x08, 0x89, 0x32,
                                 0xCD, 0xFF, 0xBA, 0xA1, 0xC1, 0x41, 0x29, 0xBC, 0xCD,
                                 0xD7, 0x0F, 0x36, 0x9E, 0xFB, 0x14, 0x92, 0x85, 0x85,
                                 0x8D, 0x2B, 0x1D, 0x15, 0x5D, 0x14, 0xDE, 0x2F, 0xDB,
                                 0x68, 0x0A, 0x8B, 0x02, 0x72, 0x84, 0x05, 0x51, 0x82,
                                 0xA0, 0xCA, 0xE2, 0x75, 0x23, 0x4C, 0xC9, 0xC9, 0x28,
                                 0x63, 0xC1, 0xB4, 0xAB, 0x66, 0xF3, 0x04, 0xCF, 0x06,
                                 0x21, 0xCD, 0x54, 0x56, 0x5F, 0x5B, 0xFF, 0x46, 0x1D,
                                 0x3B, 0x46, 0x1B, 0xD4, 0x0D, 0xF2, 0x81, 0x98, 0xE3,
                                 0x73, 0x25, 0x01, 0xB4, 0x86, 0x0E, 0xAD, 0xD5, 0x03,
                                 0xD2, 0x6D, 0x6E, 0x69, 0x33, 0x8F, 0x4E, 0x04, 0x56,
                                 0xE9, 0xE9, 0xBA, 0xF3, 0xD8, 0x27, 0xAE, 0x68, 0x5F,
                                 0xB1, 0xD8, 0x17 };
size_t m03_2_len = sizeof(m03_2);


static const uint8_t h03_256[] = { 0xB7, 0xD0, 0x31, 0xAA, 0x69, 0xB7, 0xB4, 0xD2,
                                   0x6A, 0x35, 0xB8, 0x96, 0xD7, 0x61, 0x31, 0x4F,
                                   0x1D, 0x61, 0xEB, 0x12, 0xDC, 0xC1, 0xE7, 0x2A,
                                   0xAF, 0x61, 0xB9, 0xCD, 0x48, 0x00, 0x3A, 0xF9 };
static const uint8_t h03_384[] = { 0x8F, 0xD0, 0x19, 0x09, 0x38, 0x1E, 0xB7, 0x13,
                                   0x80, 0x34, 0x19, 0x36, 0x1D, 0x8E, 0x82, 0xE9,
                                   0x24, 0x76, 0xA0, 0x8E, 0xDC, 0xC2, 0x25, 0xBB,
                                   0x8A, 0x13, 0x5D, 0x21, 0x5C, 0xB4, 0x8D, 0x07,
                                   0xB0, 0x74, 0x62, 0x4F, 0xCF, 0x2E, 0x73, 0xE6,
                                   0x66, 0xDB, 0xA5, 0x93, 0x34, 0x71, 0x98, 0x39 };
static const uint8_t h03_512[] = { 0x4F, 0xAB, 0x45, 0x80, 0x6B, 0x46, 0x28, 0x06,
                                   0x84, 0x58, 0xB5, 0xD0, 0xA2, 0xD4, 0xBF, 0x10,
                                   0x1B, 0x8B, 0xFC, 0x92, 0x76, 0xEF, 0x86, 0xAD,
                                   0x5D, 0x88, 0x37, 0x65, 0xC4, 0x3F, 0x72, 0xCE,
                                   0x8A, 0x5F, 0x7B, 0x4C, 0x5B, 0x53, 0x5A, 0x91,
                                   0x51, 0x30, 0xBB, 0x18, 0x5E, 0x69, 0x9A, 0xB6,
                                   0x22, 0x28, 0x01, 0x4E, 0x54, 0xDF, 0x79, 0x0C,
                                   0x0E, 0x93, 0xAA, 0xDB, 0xE7, 0xE3, 0x9E, 0x19 };

/* @brief expected SHA3-256 hash for test 04
   msg = '3A3A819C48EFDE2AD914FBF00E18AB6BC4F14513AB27D0C178A188B61431E7F562
          3CB66B23346775D386B50E982C493ADBBFC54B9A3CD383382336A1A0B2150A1535
          8F336D03AE18F666C7573D55C4FD181C29E6CCFDE63EA35F0ADF5885CFC0A3D84A
          2B2E4DD24496DB789E663170CEF74798AA1BBCD4574EA0BBA40489D764B2F83AAD
          C66B148B4A0CD95246C127D5871C4F11418690A5DDF01246A0C80A43C70088B618
          3639DCFDA4125BD113A8F49EE23ED306FAAC576C3FB0C1E256671D817FC2534A52
          F5B439F72E424DE376F4C565CCA82307DD9EF76DA5B7C4EB7E085172E328807C02
          D011FFBF33785378D79DC266F6A5BE6BB0E4A92ECEEBAEB1'
   md_256 = C11F3522A8FB7B3532D80B6D40023A92B489ADDAD93BF5D64B23F35E9663521C
   md_384 = 128DC611762BE9B135B3739484CFAADCA7481D68514F3DFD6F5D78BB1863AE68
            130835CDC7061A7ED964B32F1DB75EE1
   md_512 = 6E8B8BD195BDD560689AF2348BDC74AB7CD05ED8B9A57711E9BE71E9726FDA45
            91FEE12205EDACAF82FFBBAF16DFF9E702A708862080166C2FF6BA379BC7FFC2
 */
static const uint8_t m04[] = { 0x3A, 0x3A, 0x81, 0x9C, 0x48, 0xEF, 0xDE, 0x2A,
                               0xD9, 0x14, 0xFB, 0xF0, 0x0E, 0x18, 0xAB, 0x6B,
                               0xC4, 0xF1, 0x45, 0x13, 0xAB, 0x27, 0xD0, 0xC1,
                               0x78, 0xA1, 0x88, 0xB6, 0x14, 0x31, 0xE7, 0xF5,
                               0x62, 0x3C, 0xB6, 0x6B, 0x23, 0x34, 0x67, 0x75,
                               0xD3, 0x86, 0xB5, 0x0E, 0x98, 0x2C, 0x49, 0x3A,
                               0xDB, 0xBF, 0xC5, 0x4B, 0x9A, 0x3C, 0xD3, 0x83,
                               0x38, 0x23, 0x36, 0xA1, 0xA0, 0xB2, 0x15, 0x0A,
                               0x15, 0x35, 0x8F, 0x33, 0x6D, 0x03, 0xAE, 0x18,
                               0xF6, 0x66, 0xC7, 0x57, 0x3D, 0x55, 0xC4, 0xFD,
                               0x18, 0x1C, 0x29, 0xE6, 0xCC, 0xFD, 0xE6, 0x3E,
                               0xA3, 0x5F, 0x0A, 0xDF, 0x58, 0x85, 0xCF, 0xC0,
                               0xA3, 0xD8, 0x4A, 0x2B, 0x2E, 0x4D, 0xD2, 0x44,
                               0x96, 0xDB, 0x78, 0x9E, 0x66, 0x31, 0x70, 0xCE,
                               0xF7, 0x47, 0x98, 0xAA, 0x1B, 0xBC, 0xD4, 0x57,
                               0x4E, 0xA0, 0xBB, 0xA4, 0x04, 0x89, 0xD7, 0x64,
                               0xB2, 0xF8, 0x3A, 0xAD, 0xC6, 0x6B, 0x14, 0x8B,
                               0x4A, 0x0C, 0xD9, 0x52, 0x46, 0xC1, 0x27, 0xD5,
                               0x87, 0x1C, 0x4F, 0x11, 0x41, 0x86, 0x90, 0xA5,
                               0xDD, 0xF0, 0x12, 0x46, 0xA0, 0xC8, 0x0A, 0x43,
                               0xC7, 0x00, 0x88, 0xB6, 0x18, 0x36, 0x39, 0xDC,
                               0xFD, 0xA4, 0x12, 0x5B, 0xD1, 0x13, 0xA8, 0xF4,
                               0x9E, 0xE2, 0x3E, 0xD3, 0x06, 0xFA, 0xAC, 0x57,
                               0x6C, 0x3F, 0xB0, 0xC1, 0xE2, 0x56, 0x67, 0x1D,
                               0x81, 0x7F, 0xC2, 0x53, 0x4A, 0x52, 0xF5, 0xB4,
                               0x39, 0xF7, 0x2E, 0x42, 0x4D, 0xE3, 0x76, 0xF4,
                               0xC5, 0x65, 0xCC, 0xA8, 0x23, 0x07, 0xDD, 0x9E,
                               0xF7, 0x6D, 0xA5, 0xB7, 0xC4, 0xEB, 0x7E, 0x08,
                               0x51, 0x72, 0xE3, 0x28, 0x80, 0x7C, 0x02, 0xD0,
                               0x11, 0xFF, 0xBF, 0x33, 0x78, 0x53, 0x78, 0xD7,
                               0x9D, 0xC2, 0x66, 0xF6, 0xA5, 0xBE, 0x6B, 0xB0,
                               0xE4, 0xA9, 0x2E, 0xCE, 0xEB, 0xAE, 0xB1 };
size_t m04_len = sizeof(m04);
static const uint8_t m04_1[] = { 0x3A, 0x3A, 0x81, 0x9C, 0x48, 0xEF, 0xDE, 0x2A,
                                 0xD9, 0x14, 0xFB, 0xF0, 0x0E, 0x18, 0xAB, 0x6B,
                                 0xC4, 0xF1, 0x45, 0x13, 0xAB, 0x27, 0xD0, 0xC1,
                                 0x78, 0xA1, 0x88, 0xB6, 0x14, 0x31, 0xE7, 0xF5,
                                 0x62, 0x3C, 0xB6, 0x6B, 0x23, 0x34, 0x67, 0x75,
                                 0xD3, 0x86, 0xB5, 0x0E, 0x98, 0x2C, 0x49, 0x3A,
                                 0xDB, 0xBF, 0xC5, 0x4B, 0x9A, 0x3C, 0xD3, 0x83,
                                 0x38, 0x23, 0x36, 0xA1, 0xA0, 0xB2, 0x15, 0x0A,
                                 0x15, 0x35, 0x8F, 0x33, 0x6D, 0x03, 0xAE, 0x18,
                                 0xF6, 0x66, 0xC7, 0x57, 0x3D, 0x55, 0xC4, 0xFD,
                                 0x18, 0x1C, 0x29, 0xE6, 0xCC, 0xFD, 0xE6, 0x3E,
                                 0xA3, 0x5F, 0x0A, 0xDF, 0x58, 0x85, 0xCF, 0xC0,
                                 0xA3, 0xD8, 0x4A, 0x2B, 0x2E, 0x4D, 0xD2, 0x44,
                                 0x96, 0xDB, 0x78, 0x9E, 0x66, 0x31, 0x70, 0xCE,
                                 0xF7, 0x47, 0x98, 0xAA, 0x1B, 0xBC, 0xD4, 0x57,
                                 0x4E, 0xA0, 0xBB, 0xA4, 0x04, 0x89, 0xD7, 0x64,
                                 0xB2, 0xF8, 0x3A, 0xAD, 0xC6, 0x6B, 0x14, 0x8B,
                                 0x4A, 0x0C, 0xD9, 0x52, 0x46, 0xC1, 0x27, 0xD5,
                                 0x87, 0x1C, 0x4F, 0x11, 0x41, 0x86, 0x90, 0xA5,
                                 0xDD, 0xF0, 0x12, 0x46, 0xA0, 0xC8, 0x0A, 0x43,
                                 0xC7, 0x00, 0x88, 0xB6, 0x18, 0x36, 0x39, 0xDC,
                                 0xFD, 0xA4, 0x12, 0x5B, 0xD1, 0x13, 0xA8, 0xF4,
                                 0x9E, 0xE2, 0x3E, 0xD3, 0x06, 0xFA, 0xAC, 0x57,
                                 0x6C, 0x3F, 0xB0, 0xC1, 0xE2, 0x56, 0x67, 0x1D,
                                 0x81, 0x7F, 0xC2, 0x53, 0x4A, 0x52, 0xF5, 0xB4,
                                 0x39, 0xF7, 0x2E, 0x42, 0x4D, 0xE3, 0x76, 0xF4,
                                 0xC5, 0x65, 0xCC, 0xA8, 0x23, 0x07, 0xDD, 0x9E,
                                 0xF7, 0x6D, 0xA5, 0xB7, 0xC4, 0xEB, 0x7E, 0x08,
                                 0x51, 0x72, 0xE3, 0x28, 0x80, 0x7C, 0x02, 0xD0,
                                 0x11, 0xFF, 0xBF, 0x33, 0x78, 0x53, 0x78, 0xD7,
                                 0x9D, 0xC2, 0x66, 0xF6, 0xA5, 0xBE, 0x6B, 0xB0 };

size_t m04_1_len = sizeof(m04_1);
static const uint8_t m04_2[] = { 0xE4, 0xA9, 0x2E, 0xCE, 0xEB, 0xAE, 0xB1 };
size_t m04_2_len = sizeof(m04_2);

static const uint8_t h04_256[] = { 0xC1, 0x1F, 0x35, 0x22, 0xA8, 0xFB, 0x7B, 0x35,
                                   0x32, 0xD8, 0x0B, 0x6D, 0x40, 0x02, 0x3A, 0x92,
                                   0xB4, 0x89, 0xAD, 0xDA, 0xD9, 0x3B, 0xF5, 0xD6,
                                   0x4B, 0x23, 0xF3, 0x5E, 0x96, 0x63, 0x52, 0x1C };
static const uint8_t h04_384[] = { 0x12, 0x8D, 0xC6, 0x11, 0x76, 0x2B, 0xE9, 0xB1,
                                   0x35, 0xB3, 0x73, 0x94, 0x84, 0xCF, 0xAA, 0xDC,
                                   0xA7, 0x48, 0x1D, 0x68, 0x51, 0x4F, 0x3D, 0xFD,
                                   0x6F, 0x5D, 0x78, 0xBB, 0x18, 0x63, 0xAE, 0x68,
                                   0x13, 0x08, 0x35, 0xCD, 0xC7, 0x06, 0x1A, 0x7E,
                                   0xD9, 0x64, 0xB3, 0x2F, 0x1D, 0xB7, 0x5E, 0xE1 };
static const uint8_t h04_512[] = { 0x6E, 0x8B, 0x8B, 0xD1, 0x95, 0xBD, 0xD5, 0x60,
                                   0x68, 0x9A, 0xF2, 0x34, 0x8B, 0xDC, 0x74, 0xAB,
                                   0x7C, 0xD0, 0x5E, 0xD8, 0xB9, 0xA5, 0x77, 0x11,
                                   0xE9, 0xBE, 0x71, 0xE9, 0x72, 0x6F, 0xDA, 0x45,
                                   0x91, 0xFE, 0xE1, 0x22, 0x05, 0xED, 0xAC, 0xAF,
                                   0x82, 0xFF, 0xBB, 0xAF, 0x16, 0xDF, 0xF9, 0xE7,
                                   0x02, 0xA7, 0x08, 0x86, 0x20, 0x80, 0x16, 0x6C,
                                   0x2F, 0xF6, 0xBA, 0x37, 0x9B, 0xC7, 0xFF, 0xC2 };

static const uint8_t mfail[] = { 0x4A, 0x4F, 0x20, 0x24, 0x84, 0x51, 0x25, 0x27 };
size_t mfail_len = sizeof(mfail);
static const uint8_t hfail_256[] = { 0xBA, 0x4F, 0xB0, 0x09, 0xD5, 0x7A, 0x5C, 0xEB,
                                     0x85, 0xFC, 0x64, 0xD5, 0x4E, 0x5C, 0x55, 0xA5,
                                     0x58, 0x54, 0xB4, 0x1C, 0xC4, 0x7A, 0xD1, 0x52,
                                     0x94, 0xBC, 0x41, 0xF3, 0x21, 0x65, 0xDF, 0xBA };
static const uint8_t hfail_384[] = { 0x89, 0xDB, 0xF4, 0xC3, 0x9B, 0x8F, 0xB4, 0x6F,
                                     0xDF, 0x0A, 0x69, 0x26, 0xCE, 0xC0, 0x35, 0x5A,
                                     0x4B, 0xDB, 0xF9, 0xC6, 0xA4, 0x46, 0xE1, 0x40,
                                     0xB7, 0xC8, 0xBD, 0x08, 0xFF, 0x6F, 0x48, 0x9F,
                                     0x20, 0x5D, 0xAF, 0x8E, 0xFF, 0xE1, 0x60, 0xF4,
                                     0x37, 0xF6, 0x74, 0x91, 0xEF, 0x89, 0x7C, 0x23 };
static const uint8_t hfail_512[] = { 0x15, 0x0D, 0x78, 0x7D, 0x6E, 0xB4, 0x96, 0x70,
                                     0xC2, 0xA4, 0xCC, 0xD1, 0x7E, 0x6C, 0xCE, 0x7A,
                                     0x04, 0xC1, 0xFE, 0x30, 0xFC, 0xE0, 0x3D, 0x1E,
                                     0xF2, 0x50, 0x17, 0x52, 0xD9, 0x2A, 0xE0, 0x4C,
                                     0xB3, 0x45, 0xFD, 0x42, 0xE5, 0x10, 0x38, 0xC8,
                                     0x3B, 0x2B, 0x4F, 0x8F, 0xD4, 0x38, 0xD1, 0xB4,
                                     0xB5, 0x5C, 0xC5, 0x88, 0xC6, 0xB9, 0x13, 0x13,
                                     0x2F, 0x1A, 0x65, 0x8F, 0xB1, 0x22, 0xCB, 0x52 };


static int calc_and_compare_hash_256(const uint8_t *msg, size_t msg_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_256_DIGEST_LENGTH];

    sha3_256(hash, msg, msg_len);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_steps_and_compare_hash_256(const uint8_t *msg1, size_t msg1_len,
                                           const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_256_DIGEST_LENGTH];
    keccak_state_t state;

    sha3_256_init(&state);
    sha3_update(&state, msg1, msg1_len);
    sha3_update(&state, msg2, msg2_len);
    sha3_256_final(&state, hash);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_and_compare_hash_384(const uint8_t *msg, size_t msg_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_384_DIGEST_LENGTH];

    sha3_384(hash, msg, msg_len);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_steps_and_compare_hash_384(const uint8_t *msg1, size_t msg1_len,
                                           const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_384_DIGEST_LENGTH];
    keccak_state_t state;

    sha3_384_init(&state);
    sha3_update(&state, msg1, msg1_len);
    sha3_update(&state, msg2, msg2_len);
    sha3_384_final(&state, hash);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_and_compare_hash_512(const uint8_t *msg, size_t msg_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_512_DIGEST_LENGTH];

    sha3_512(hash, msg, msg_len);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_steps_and_compare_hash_512(const uint8_t *msg1, size_t msg1_len,
                                           const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_512_DIGEST_LENGTH];
    keccak_state_t state;

    sha3_512_init(&state);
    sha3_update(&state, msg1, msg1_len);
    sha3_update(&state, msg2, msg2_len);
    sha3_512_final(&state, hash);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static void test_hashes_sha3_hash_sequence_01(void)
{
    TEST_ASSERT(calc_and_compare_hash_256(m01, m01_len, h01_256));
    TEST_ASSERT(calc_steps_and_compare_hash_256(m01_1, m01_1_len, m01_2, m01_2_len, h01_256));
    TEST_ASSERT(calc_and_compare_hash_384(m01, m01_len, h01_384));
    TEST_ASSERT(calc_steps_and_compare_hash_384(m01_1, m01_1_len, m01_2, m01_2_len, h01_384));
    TEST_ASSERT(calc_and_compare_hash_512(m01, m01_len, h01_512));
    TEST_ASSERT(calc_steps_and_compare_hash_512(m01_1, m01_1_len, m01_2, m01_2_len, h01_512));
}
static void test_hashes_sha3_hash_sequence_02(void)
{
    TEST_ASSERT(calc_and_compare_hash_256(m02, m02_len, h02_256));
    TEST_ASSERT(calc_steps_and_compare_hash_256(m02_1, m02_1_len, m02_2, m02_2_len, h02_256));
    TEST_ASSERT(calc_and_compare_hash_384(m02, m02_len, h02_384));
    TEST_ASSERT(calc_steps_and_compare_hash_384(m02_1, m02_1_len, m02_2, m02_2_len, h02_384));
    TEST_ASSERT(calc_and_compare_hash_512(m02, m02_len, h02_512));
    TEST_ASSERT(calc_steps_and_compare_hash_512(m02_1, m02_1_len, m02_2, m02_2_len, h02_512));
}

static void test_hashes_sha3_hash_sequence_03(void)
{
    TEST_ASSERT(calc_and_compare_hash_256(m03, m03_len, h03_256));
    TEST_ASSERT(calc_steps_and_compare_hash_256(m03_1, m03_1_len, m03_2, m03_2_len, h03_256));
    TEST_ASSERT(calc_and_compare_hash_384(m03, m03_len, h03_384));
    TEST_ASSERT(calc_steps_and_compare_hash_384(m03_1, m03_1_len, m03_2, m03_2_len, h03_384));
    TEST_ASSERT(calc_and_compare_hash_512(m03, m03_len, h03_512));
    TEST_ASSERT(calc_steps_and_compare_hash_512(m03_1, m03_1_len, m03_2, m03_2_len, h03_512));
}

static void test_hashes_sha3_hash_sequence_04(void)
{
    TEST_ASSERT(calc_and_compare_hash_256(m04, m04_len, h04_256));
    TEST_ASSERT(calc_steps_and_compare_hash_256(m04_1, m04_1_len, m04_2, m04_2_len, h04_256));
    TEST_ASSERT(calc_and_compare_hash_384(m04, m04_len, h04_384));
    TEST_ASSERT(calc_steps_and_compare_hash_384(m04_1, m04_1_len, m04_2, m04_2_len, h04_384));
    TEST_ASSERT(calc_and_compare_hash_512(m04, m04_len, h04_512));
    TEST_ASSERT(calc_steps_and_compare_hash_512(m04_1, m04_1_len, m04_2, m04_2_len, h04_512));
}

static void test_hashes_sha3_hash_sequence_failing_compare(void)
{
    /* failing compare (message from testcase 02 alterered slightly) */
    TEST_ASSERT(!calc_and_compare_hash_256(mfail, mfail_len, hfail_256));
    TEST_ASSERT(!calc_and_compare_hash_384(mfail, mfail_len, hfail_384));
    TEST_ASSERT(!calc_and_compare_hash_512(mfail, mfail_len, hfail_512));
}


Test *tests_hashes_sha3_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hashes_sha3_hash_sequence_01),
        new_TestFixture(test_hashes_sha3_hash_sequence_02),
        new_TestFixture(test_hashes_sha3_hash_sequence_03),
        new_TestFixture(test_hashes_sha3_hash_sequence_04),
        new_TestFixture(test_hashes_sha3_hash_sequence_failing_compare),
    };

    EMB_UNIT_TESTCALLER(hashes_sha3_tests, NULL, NULL,
                        fixtures);

    return (Test *)&hashes_sha3_tests;
}
