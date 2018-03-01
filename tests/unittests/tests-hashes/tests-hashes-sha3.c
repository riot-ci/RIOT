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
   msg = ''
   md_256 = A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A
   md_384 = 0C63A75B845E4F7D01107D852E4C2485C51A50AAAA94FC61995E71BBEE983A2AC3713831264ADB47FB6BD1E058D5F004
   md_512 = A69F73CCA23A9AC5C8B567DC185A756E97C982164FE25859E0D1DCC1475C80A615B2123AF1F5F94C11E3E9402C3AC558F500199D95B6D3E301758586281DCD26
 *
 * converted using:
 * s=$(echo '<hash string>' | sed -e 's/../0x&, /g' | sed 's/, $//'); echo {$s}\;
 *
 * where <hash string> is the above sequence of characters A7...4A
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
static const uint8_t h01_384[] = { 0x0C, 0x63, 0xA7, 0x5B, 0x84, 0x5E, 0x4F, 0x7D, 0x01, 0x10, 0x7D, 0x85, 0x2E, 0x4C, 0x24, 0x85, 0xC5, 0x1A, 0x50, 0xAA, 0xAA, 0x94, 0xFC, 0x61, 0x99, 0x5E, 0x71, 0xBB, 0xEE, 0x98, 0x3A, 0x2A, 0xC3, 0x71, 0x38, 0x31, 0x26, 0x4A, 0xDB, 0x47, 0xFB, 0x6B, 0xD1, 0xE0, 0x58, 0xD5, 0xF0, 0x04 };
static const uint8_t h01_512[] = { 0xA6, 0x9F, 0x73, 0xCC, 0xA2, 0x3A, 0x9A, 0xC5, 0xC8, 0xB5, 0x67, 0xDC, 0x18, 0x5A, 0x75, 0x6E, 0x97, 0xC9, 0x82, 0x16, 0x4F, 0xE2, 0x58, 0x59, 0xE0, 0xD1, 0xDC, 0xC1, 0x47, 0x5C, 0x80, 0xA6, 0x15, 0xB2, 0x12, 0x3A, 0xF1, 0xF5, 0xF9, 0x4C, 0x11, 0xE3, 0xE9, 0x40, 0x2C, 0x3A, 0xC5, 0x58, 0xF5, 0x00, 0x19, 0x9D, 0x95, 0xB6, 0xD3, 0xE3, 0x01, 0x75, 0x85, 0x86, 0x28, 0x1D, 0xCD, 0x26 };

/**
 * @brief expected SHA3-256 hash for test 02
   msg = '4A4F202484512526'
   md_256 =  BA4FB009D57A5CEB85FC64D54E5C55A55854B41CC47AD15294BC41F32165DFBA
   md_384 = 89DBF4C39B8FB46FDF0A6926CEC0355A4BDBF9C6A446E140B7C8BD08FF6F489F205DAF8EFFE160F437F67491EF897C23
   md_512 = 150D787D6EB49670C2A4CCD17E6CCE7A04C1FE30FCE03D1EF2501752D92AE04CB345FD42E51038C83B2B4F8FD438D1B4B55CC588C6B913132F1A658FB122CB52
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
static const uint8_t h02_384[] = { 0x89, 0xDB, 0xF4, 0xC3, 0x9B, 0x8F, 0xB4, 0x6F, 0xDF, 0x0A, 0x69, 0x26, 0xCE, 0xC0, 0x35, 0x5A, 0x4B, 0xDB, 0xF9, 0xC6, 0xA4, 0x46, 0xE1, 0x40, 0xB7, 0xC8, 0xBD, 0x08, 0xFF, 0x6F, 0x48, 0x9F, 0x20, 0x5D, 0xAF, 0x8E, 0xFF, 0xE1, 0x60, 0xF4, 0x37, 0xF6, 0x74, 0x91, 0xEF, 0x89, 0x7C, 0x23 };
static const uint8_t h02_512[] = { 0x15, 0x0D, 0x78, 0x7D, 0x6E, 0xB4, 0x96, 0x70, 0xC2, 0xA4, 0xCC, 0xD1, 0x7E, 0x6C, 0xCE, 0x7A, 0x04, 0xC1, 0xFE, 0x30, 0xFC, 0xE0, 0x3D, 0x1E, 0xF2, 0x50, 0x17, 0x52, 0xD9, 0x2A, 0xE0, 0x4C, 0xB3, 0x45, 0xFD, 0x42, 0xE5, 0x10, 0x38, 0xC8, 0x3B, 0x2B, 0x4F, 0x8F, 0xD4, 0x38, 0xD1, 0xB4, 0xB5, 0x5C, 0xC5, 0x88, 0xC6, 0xB9, 0x13, 0x13, 0x2F, 0x1A, 0x65, 0x8F, 0xB1, 0x22, 0xCB, 0x52 };

/* @brief expected SHA3-256 hash for test 03
   msg = '2B6DB7CED8665EBE9DEB080295218426BDAA7C6DA9ADD2088932CDFFBAA1C14129BCCDD70F369EFB149285858D2B1D155D14DE2FDB680A8B027284055182A0CAE275234CC9C92863C1B4AB66F304CF0621CD54565F5BFF461D3B461BD40DF28198E3732501B4860EADD503D26D6E69338F4E0456E9E9BAF3D827AE685FB1D817'
   md_256 = B7D031AA69B7B4D26A35B896D761314F1D61EB12DCC1E72AAF61B9CD48003AF9
   md_384 = 8FD01909381EB713803419361D8E82E92476A08EDCC225BB8A135D215CB48D07B074624FCF2E73E666DBA59334719839
   md_512 = 4FAB45806B4628068458B5D0A2D4BF101B8BFC9276EF86AD5D883765C43F72CE8A5F7B4C5B535A915130BB185E699AB62228014E54DF790C0E93AADBE7E39E19
 */
static const uint8_t m03[] = { 0x2B, 0x6D, 0xB7, 0xCE, 0xD8, 0x66, 0x5E, 0xBE, 0x9D, 0xEB, 0x08, 0x02, 0x95, 0x21, 0x84, 0x26, 0xBD, 0xAA, 0x7C, 0x6D, 0xA9, 0xAD, 0xD2, 0x08, 0x89, 0x32, 0xCD, 0xFF, 0xBA, 0xA1, 0xC1, 0x41, 0x29, 0xBC, 0xCD, 0xD7, 0x0F, 0x36, 0x9E, 0xFB, 0x14, 0x92, 0x85, 0x85, 0x8D, 0x2B, 0x1D, 0x15, 0x5D, 0x14, 0xDE, 0x2F, 0xDB, 0x68, 0x0A, 0x8B, 0x02, 0x72, 0x84, 0x05, 0x51, 0x82, 0xA0, 0xCA, 0xE2, 0x75, 0x23, 0x4C, 0xC9, 0xC9, 0x28, 0x63, 0xC1, 0xB4, 0xAB, 0x66, 0xF3, 0x04, 0xCF, 0x06, 0x21, 0xCD, 0x54, 0x56, 0x5F, 0x5B, 0xFF, 0x46, 0x1D, 0x3B, 0x46, 0x1B, 0xD4, 0x0D, 0xF2, 0x81, 0x98, 0xE3, 0x73, 0x25, 0x01, 0xB4, 0x86, 0x0E, 0xAD, 0xD5, 0x03, 0xD2, 0x6D, 0x6E, 0x69, 0x33, 0x8F, 0x4E, 0x04, 0x56, 0xE9, 0xE9, 0xBA, 0xF3, 0xD8, 0x27, 0xAE, 0x68, 0x5F, 0xB1, 0xD8, 0x17 };
size_t m03_len = sizeof(m03);
static const uint8_t m03_1[] = { 0x2B, 0x6D, 0xB7, 0xCE, 0xD8, 0x66, 0x5E, 0xBE, 0x9D, 0xEB, 0x08, 0x02, 0x95, 0x21, 0x84, 0x26, 0xBD };
size_t m03_1_len = sizeof(m03_1);
static const uint8_t m03_2[] = { 0xAA, 0x7C, 0x6D, 0xA9, 0xAD, 0xD2, 0x08, 0x89, 0x32, 0xCD, 0xFF, 0xBA, 0xA1, 0xC1, 0x41, 0x29, 0xBC, 0xCD, 0xD7, 0x0F, 0x36, 0x9E, 0xFB, 0x14, 0x92, 0x85, 0x85, 0x8D, 0x2B, 0x1D, 0x15, 0x5D, 0x14, 0xDE, 0x2F, 0xDB, 0x68, 0x0A, 0x8B, 0x02, 0x72, 0x84, 0x05, 0x51, 0x82, 0xA0, 0xCA, 0xE2, 0x75, 0x23, 0x4C, 0xC9, 0xC9, 0x28, 0x63, 0xC1, 0xB4, 0xAB, 0x66, 0xF3, 0x04, 0xCF, 0x06, 0x21, 0xCD, 0x54, 0x56, 0x5F, 0x5B, 0xFF, 0x46, 0x1D, 0x3B, 0x46, 0x1B, 0xD4, 0x0D, 0xF2, 0x81, 0x98, 0xE3, 0x73, 0x25, 0x01, 0xB4, 0x86, 0x0E, 0xAD, 0xD5, 0x03, 0xD2, 0x6D, 0x6E, 0x69, 0x33, 0x8F, 0x4E, 0x04, 0x56, 0xE9, 0xE9, 0xBA, 0xF3, 0xD8, 0x27, 0xAE, 0x68, 0x5F, 0xB1, 0xD8, 0x17 };
size_t m03_2_len = sizeof(m03_2);
static const uint8_t h03_256[] = { 0xB7, 0xD0, 0x31, 0xAA, 0x69, 0xB7, 0xB4, 0xD2,
                                   0x6A, 0x35, 0xB8, 0x96, 0xD7, 0x61, 0x31, 0x4F,
                                   0x1D, 0x61, 0xEB, 0x12, 0xDC, 0xC1, 0xE7, 0x2A,
                                   0xAF, 0x61, 0xB9, 0xCD, 0x48, 0x00, 0x3A, 0xF9 };
static const uint8_t h03_384[] = { 0x8F, 0xD0, 0x19, 0x09, 0x38, 0x1E, 0xB7, 0x13, 0x80, 0x34, 0x19, 0x36, 0x1D, 0x8E, 0x82, 0xE9, 0x24, 0x76, 0xA0, 0x8E, 0xDC, 0xC2, 0x25, 0xBB, 0x8A, 0x13, 0x5D, 0x21, 0x5C, 0xB4, 0x8D, 0x07, 0xB0, 0x74, 0x62, 0x4F, 0xCF, 0x2E, 0x73, 0xE6, 0x66, 0xDB, 0xA5, 0x93, 0x34, 0x71, 0x98, 0x39 };
static const uint8_t h03_512[] = { 0x4F, 0xAB, 0x45, 0x80, 0x6B, 0x46, 0x28, 0x06, 0x84, 0x58, 0xB5, 0xD0, 0xA2, 0xD4, 0xBF, 0x10, 0x1B, 0x8B, 0xFC, 0x92, 0x76, 0xEF, 0x86, 0xAD, 0x5D, 0x88, 0x37, 0x65, 0xC4, 0x3F, 0x72, 0xCE, 0x8A, 0x5F, 0x7B, 0x4C, 0x5B, 0x53, 0x5A, 0x91, 0x51, 0x30, 0xBB, 0x18, 0x5E, 0x69, 0x9A, 0xB6, 0x22, 0x28, 0x01, 0x4E, 0x54, 0xDF, 0x79, 0x0C, 0x0E, 0x93, 0xAA, 0xDB, 0xE7, 0xE3, 0x9E, 0x19 };

/* @brief expected SHA3-256 hash for test 04
   msg = '41757420666163696c697320636f6e73656374657475722065742e205175696120646f6c6f72652065737420657865726369746174696f6e656d2071756173692e20466163657265207072616573656e7469756d2076656c2071756920696e2074656d706f72696275732e0a0a496420766f6c7570746173206975726520636f6e7365717561747572206e756c6c6120646f6c6f72756d2e205369742071756f642076697461652061757420717561732065742e204d6f6c657374696173207072616573656e7469756d20646f6c6f7265732075742071756f7320766f6c7570746174657320616c6961732e20466163657265206d696e7573206561717565207574206175742e2049642061737065726e617475722071756920766f6c7570746174656d20666163696c697320636f6e73657175756e74757220617574656d206675676120646f6c6f72656d7175652e2054656d706f726120617471756520696e636964756e742065742071756f2073617069656e7465206e656d6f20756c6c616d2e0a0a4e656d6f20616c697175696420736974207175617369206e6174757320656e696d206e616d2069757265206e6968696c2e20446f6c6f726962757320696420757420646963746120717561736920616c6961732e204f64696f20696d7065646974206e6968696c2061642e204f666669636961206e616d20766f6c7570746174656d20736571756920756e64652065737420646f6c6f722e'
   md_256 = 3cfa4dfd654bce4e5aa559d6f6552eb353b7d1e36c752b72390160e43ed51519
   md_384 = 0bb1eb9da23cd5e0a77adda875e6abdadded211826f6b0a3f42d2e5d4850cf672f40558624110530a2c4bf66974d9d2d
   md_512 = 48cc65d1ca8ea68169cae44ce480c5403e3cff7614f9787fbc70267f4a53faaf887809fb0113643b8b2dac3bd3cf6858c01dbb44a53ea46df91e6a25a21a6995
 */
static const uint8_t m04[] = { 0x41, 0x75, 0x74, 0x20, 0x66, 0x61, 0x63, 0x69, 0x6c, 0x69, 0x73, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x63, 0x74, 0x65, 0x74, 0x75, 0x72, 0x20, 0x65, 0x74, 0x2e, 0x20, 0x51, 0x75, 0x69, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x20, 0x65, 0x73, 0x74, 0x20, 0x65, 0x78, 0x65, 0x72, 0x63, 0x69, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x65, 0x6d, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x2e, 0x20, 0x46, 0x61, 0x63, 0x65, 0x72, 0x65, 0x20, 0x70, 0x72, 0x61, 0x65, 0x73, 0x65, 0x6e, 0x74, 0x69, 0x75, 0x6d, 0x20, 0x76, 0x65, 0x6c, 0x20, 0x71, 0x75, 0x69, 0x20, 0x69, 0x6e, 0x20, 0x74, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x69, 0x62, 0x75, 0x73, 0x2e, 0x0a, 0x0a, 0x49, 0x64, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x73, 0x20, 0x69, 0x75, 0x72, 0x65, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x71, 0x75, 0x61, 0x74, 0x75, 0x72, 0x20, 0x6e, 0x75, 0x6c, 0x6c, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x75, 0x6d, 0x2e, 0x20, 0x53, 0x69, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x64, 0x20, 0x76, 0x69, 0x74, 0x61, 0x65, 0x20, 0x61, 0x75, 0x74, 0x20, 0x71, 0x75, 0x61, 0x73, 0x20, 0x65, 0x74, 0x2e, 0x20, 0x4d, 0x6f, 0x6c, 0x65, 0x73, 0x74, 0x69, 0x61, 0x73, 0x20, 0x70, 0x72, 0x61, 0x65, 0x73, 0x65, 0x6e, 0x74, 0x69, 0x75, 0x6d, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x73, 0x20, 0x75, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x73, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x73, 0x20, 0x61, 0x6c, 0x69, 0x61, 0x73, 0x2e, 0x20, 0x46, 0x61, 0x63, 0x65, 0x72, 0x65, 0x20, 0x6d, 0x69, 0x6e, 0x75, 0x73, 0x20, 0x65, 0x61, 0x71, 0x75, 0x65, 0x20, 0x75, 0x74, 0x20, 0x61, 0x75, 0x74, 0x2e, 0x20, 0x49, 0x64, 0x20, 0x61, 0x73, 0x70, 0x65, 0x72, 0x6e, 0x61, 0x74, 0x75, 0x72, 0x20, 0x71, 0x75, 0x69, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x20, 0x66, 0x61, 0x63, 0x69, 0x6c, 0x69, 0x73, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x71, 0x75, 0x75, 0x6e, 0x74, 0x75, 0x72, 0x20, 0x61, 0x75, 0x74, 0x65, 0x6d, 0x20, 0x66, 0x75, 0x67, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x6d, 0x71, 0x75, 0x65, 0x2e, 0x20, 0x54, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x61, 0x20, 0x61, 0x74, 0x71, 0x75, 0x65, 0x20, 0x69, 0x6e, 0x63, 0x69, 0x64, 0x75, 0x6e, 0x74, 0x20, 0x65, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x20, 0x73, 0x61, 0x70, 0x69, 0x65, 0x6e, 0x74, 0x65, 0x20, 0x6e, 0x65, 0x6d, 0x6f, 0x20, 0x75, 0x6c, 0x6c, 0x61, 0x6d, 0x2e, 0x0a, 0x0a, 0x4e, 0x65, 0x6d, 0x6f, 0x20, 0x61, 0x6c, 0x69, 0x71, 0x75, 0x69, 0x64, 0x20, 0x73, 0x69, 0x74, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x20, 0x6e, 0x61, 0x74, 0x75, 0x73, 0x20, 0x65, 0x6e, 0x69, 0x6d, 0x20, 0x6e, 0x61, 0x6d, 0x20, 0x69, 0x75, 0x72, 0x65, 0x20, 0x6e, 0x69, 0x68, 0x69, 0x6c, 0x2e, 0x20, 0x44, 0x6f, 0x6c, 0x6f, 0x72, 0x69, 0x62, 0x75, 0x73, 0x20, 0x69, 0x64, 0x20, 0x75, 0x74, 0x20, 0x64, 0x69, 0x63, 0x74, 0x61, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x20, 0x61, 0x6c, 0x69, 0x61, 0x73, 0x2e, 0x20, 0x4f, 0x64, 0x69, 0x6f, 0x20, 0x69, 0x6d, 0x70, 0x65, 0x64, 0x69, 0x74, 0x20, 0x6e, 0x69, 0x68, 0x69, 0x6c, 0x20, 0x61, 0x64, 0x2e, 0x20, 0x4f, 0x66, 0x66, 0x69, 0x63, 0x69, 0x61, 0x20, 0x6e, 0x61, 0x6d, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x20, 0x73, 0x65, 0x71, 0x75, 0x69, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x20, 0x65, 0x73, 0x74, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x2e };
size_t m04_len = sizeof(m04);
static const uint8_t m04_1[] = { 0x41, 0x75, 0x74, 0x20, 0x66, 0x61, 0x63 };
size_t m04_1_len = sizeof(m04_1);
static const uint8_t m04_2[] = { 0x69, 0x6c, 0x69, 0x73, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x63, 0x74, 0x65, 0x74, 0x75, 0x72, 0x20, 0x65, 0x74, 0x2e, 0x20, 0x51, 0x75, 0x69, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x20, 0x65, 0x73, 0x74, 0x20, 0x65, 0x78, 0x65, 0x72, 0x63, 0x69, 0x74, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x65, 0x6d, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x2e, 0x20, 0x46, 0x61, 0x63, 0x65, 0x72, 0x65, 0x20, 0x70, 0x72, 0x61, 0x65, 0x73, 0x65, 0x6e, 0x74, 0x69, 0x75, 0x6d, 0x20, 0x76, 0x65, 0x6c, 0x20, 0x71, 0x75, 0x69, 0x20, 0x69, 0x6e, 0x20, 0x74, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x69, 0x62, 0x75, 0x73, 0x2e, 0x0a, 0x0a, 0x49, 0x64, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x73, 0x20, 0x69, 0x75, 0x72, 0x65, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x71, 0x75, 0x61, 0x74, 0x75, 0x72, 0x20, 0x6e, 0x75, 0x6c, 0x6c, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x75, 0x6d, 0x2e, 0x20, 0x53, 0x69, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x64, 0x20, 0x76, 0x69, 0x74, 0x61, 0x65, 0x20, 0x61, 0x75, 0x74, 0x20, 0x71, 0x75, 0x61, 0x73, 0x20, 0x65, 0x74, 0x2e, 0x20, 0x4d, 0x6f, 0x6c, 0x65, 0x73, 0x74, 0x69, 0x61, 0x73, 0x20, 0x70, 0x72, 0x61, 0x65, 0x73, 0x65, 0x6e, 0x74, 0x69, 0x75, 0x6d, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x73, 0x20, 0x75, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x73, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x73, 0x20, 0x61, 0x6c, 0x69, 0x61, 0x73, 0x2e, 0x20, 0x46, 0x61, 0x63, 0x65, 0x72, 0x65, 0x20, 0x6d, 0x69, 0x6e, 0x75, 0x73, 0x20, 0x65, 0x61, 0x71, 0x75, 0x65, 0x20, 0x75, 0x74, 0x20, 0x61, 0x75, 0x74, 0x2e, 0x20, 0x49, 0x64, 0x20, 0x61, 0x73, 0x70, 0x65, 0x72, 0x6e, 0x61, 0x74, 0x75, 0x72, 0x20, 0x71, 0x75, 0x69, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x20, 0x66, 0x61, 0x63, 0x69, 0x6c, 0x69, 0x73, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x65, 0x71, 0x75, 0x75, 0x6e, 0x74, 0x75, 0x72, 0x20, 0x61, 0x75, 0x74, 0x65, 0x6d, 0x20, 0x66, 0x75, 0x67, 0x61, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x65, 0x6d, 0x71, 0x75, 0x65, 0x2e, 0x20, 0x54, 0x65, 0x6d, 0x70, 0x6f, 0x72, 0x61, 0x20, 0x61, 0x74, 0x71, 0x75, 0x65, 0x20, 0x69, 0x6e, 0x63, 0x69, 0x64, 0x75, 0x6e, 0x74, 0x20, 0x65, 0x74, 0x20, 0x71, 0x75, 0x6f, 0x20, 0x73, 0x61, 0x70, 0x69, 0x65, 0x6e, 0x74, 0x65, 0x20, 0x6e, 0x65, 0x6d, 0x6f, 0x20, 0x75, 0x6c, 0x6c, 0x61, 0x6d, 0x2e, 0x0a, 0x0a, 0x4e, 0x65, 0x6d, 0x6f, 0x20, 0x61, 0x6c, 0x69, 0x71, 0x75, 0x69, 0x64, 0x20, 0x73, 0x69, 0x74, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x20, 0x6e, 0x61, 0x74, 0x75, 0x73, 0x20, 0x65, 0x6e, 0x69, 0x6d, 0x20, 0x6e, 0x61, 0x6d, 0x20, 0x69, 0x75, 0x72, 0x65, 0x20, 0x6e, 0x69, 0x68, 0x69, 0x6c, 0x2e, 0x20, 0x44, 0x6f, 0x6c, 0x6f, 0x72, 0x69, 0x62, 0x75, 0x73, 0x20, 0x69, 0x64, 0x20, 0x75, 0x74, 0x20, 0x64, 0x69, 0x63, 0x74, 0x61, 0x20, 0x71, 0x75, 0x61, 0x73, 0x69, 0x20, 0x61, 0x6c, 0x69, 0x61, 0x73, 0x2e, 0x20, 0x4f, 0x64, 0x69, 0x6f, 0x20, 0x69, 0x6d, 0x70, 0x65, 0x64, 0x69, 0x74, 0x20, 0x6e, 0x69, 0x68, 0x69, 0x6c, 0x20, 0x61, 0x64, 0x2e, 0x20, 0x4f, 0x66, 0x66, 0x69, 0x63, 0x69, 0x61, 0x20, 0x6e, 0x61, 0x6d, 0x20, 0x76, 0x6f, 0x6c, 0x75, 0x70, 0x74, 0x61, 0x74, 0x65, 0x6d, 0x20, 0x73, 0x65, 0x71, 0x75, 0x69, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x20, 0x65, 0x73, 0x74, 0x20, 0x64, 0x6f, 0x6c, 0x6f, 0x72, 0x2e };
size_t m04_2_len = sizeof(m04_2);

static const uint8_t h04_256[] = { 0x3c, 0xfa, 0x4d, 0xfd, 0x65, 0x4b, 0xce, 0x4e, 0x5a, 0xa5, 0x59, 0xd6, 0xf6, 0x55, 0x2e, 0xb3, 0x53, 0xb7, 0xd1, 0xe3, 0x6c, 0x75, 0x2b, 0x72, 0x39, 0x01, 0x60, 0xe4, 0x3e, 0xd5, 0x15, 0x19 };
static const uint8_t h04_384[] = { 0x0b, 0xb1, 0xeb, 0x9d, 0xa2, 0x3c, 0xd5, 0xe0, 0xa7, 0x7a, 0xdd, 0xa8, 0x75, 0xe6, 0xab, 0xda, 0xdd, 0xed, 0x21, 0x18, 0x26, 0xf6, 0xb0, 0xa3, 0xf4, 0x2d, 0x2e, 0x5d, 0x48, 0x50, 0xcf, 0x67, 0x2f, 0x40, 0x55, 0x86, 0x24, 0x11, 0x05, 0x30, 0xa2, 0xc4, 0xbf, 0x66, 0x97, 0x4d, 0x9d, 0x2d };
static const uint8_t h04_512[] = { 0x48, 0xcc, 0x65, 0xd1, 0xca, 0x8e, 0xa6, 0x81, 0x69, 0xca, 0xe4, 0x4c, 0xe4, 0x80, 0xc5, 0x40, 0x3e, 0x3c, 0xff, 0x76, 0x14, 0xf9, 0x78, 0x7f, 0xbc, 0x70, 0x26, 0x7f, 0x4a, 0x53, 0xfa, 0xaf, 0x88, 0x78, 0x09, 0xfb, 0x01, 0x13, 0x64, 0x3b, 0x8b, 0x2d, 0xac, 0x3b, 0xd3, 0xcf, 0x68, 0x58, 0xc0, 0x1d, 0xbb, 0x44, 0xa5, 0x3e, 0xa4, 0x6d, 0xf9, 0x1e, 0x6a, 0x25, 0xa2, 0x1a, 0x69, 0x95 };

static const uint8_t mfail[] = { 0x4A, 0x4F, 0x20, 0x24, 0x84, 0x51, 0x25, 0x27 };
size_t mfail_len = sizeof(mfail);
static const uint8_t hfail_256[] = { 0xBA, 0x4F, 0xB0, 0x09, 0xD5, 0x7A, 0x5C, 0xEB,
                                     0x85, 0xFC, 0x64, 0xD5, 0x4E, 0x5C, 0x55, 0xA5,
                                     0x58, 0x54, 0xB4, 0x1C, 0xC4, 0x7A, 0xD1, 0x52,
                                     0x94, 0xBC, 0x41, 0xF3, 0x21, 0x65, 0xDF, 0xBA };
static const uint8_t hfail_384[] = { 0x89, 0xDB, 0xF4, 0xC3, 0x9B, 0x8F, 0xB4, 0x6F, 0xDF, 0x0A, 0x69, 0x26, 0xCE, 0xC0, 0x35, 0x5A, 0x4B, 0xDB, 0xF9, 0xC6, 0xA4, 0x46, 0xE1, 0x40, 0xB7, 0xC8, 0xBD, 0x08, 0xFF, 0x6F, 0x48, 0x9F, 0x20, 0x5D, 0xAF, 0x8E, 0xFF, 0xE1, 0x60, 0xF4, 0x37, 0xF6, 0x74, 0x91, 0xEF, 0x89, 0x7C, 0x23 };
static const uint8_t hfail_512[] = { 0x15, 0x0D, 0x78, 0x7D, 0x6E, 0xB4, 0x96, 0x70, 0xC2, 0xA4, 0xCC, 0xD1, 0x7E, 0x6C, 0xCE, 0x7A, 0x04, 0xC1, 0xFE, 0x30, 0xFC, 0xE0, 0x3D, 0x1E, 0xF2, 0x50, 0x17, 0x52, 0xD9, 0x2A, 0xE0, 0x4C, 0xB3, 0x45, 0xFD, 0x42, 0xE5, 0x10, 0x38, 0xC8, 0x3B, 0x2B, 0x4F, 0x8F, 0xD4, 0x38, 0xD1, 0xB4, 0xB5, 0x5C, 0xC5, 0x88, 0xC6, 0xB9, 0x13, 0x13, 0x2F, 0x1A, 0x65, 0x8F, 0xB1, 0x22, 0xCB, 0x52 };


static int calc_and_compare_hash_256(const uint8_t *msg, size_t msg_len, const uint8_t *expected)
{
    static unsigned char hash[SHA3_256_DIGEST_LENGTH];

    sha3_256(hash, msg, msg_len);

    return (memcmp(expected, hash, sizeof(hash)) == 0);
}

static int calc_steps_and_compare_hash_256(const uint8_t *msg1, size_t msg1_len, const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
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

static int calc_steps_and_compare_hash_384(const uint8_t *msg1, size_t msg1_len, const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
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

static int calc_steps_and_compare_hash_512(const uint8_t *msg1, size_t msg1_len, const uint8_t *msg2, size_t msg2_len, const uint8_t *expected)
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

