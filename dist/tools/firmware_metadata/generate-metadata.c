/*
 * Copyright (c) 2016, Mark Solters <msolters@gmail.com>.
 *               2016, Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @ingroup     FW
 * @file
 * @brief       Meta-data generation for FW images
 *
 * @author      Mark Solters <msolters@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

#include "hashes/sha256.h"
#include "fw_slots.h"

/* Input firmware .bin file */
FILE *firmware_bin;

/* Output metadata .bin file */
FILE *metadata_bin;

int32_t firmware_size = 0;

static void print_metadata(firmware_metadata_t *fw_metadata)
{
    printf("Firmware Size: %d\n", fw_metadata->size);
    printf("Firmware Version: %#x\n", fw_metadata->version);
    printf("Firmware APPID: %#x\n", fw_metadata->appid);
    printf("Firmware HASH: ");
    for (unsigned long i = 0; i < sizeof(fw_metadata->hash); i++) {
        printf("%02x ", fw_metadata->hash[i]);
    }
    printf("\n");
    printf("Firmware signed HASH: ");
    for (unsigned long i = 0; i < sizeof(fw_metadata->shash); i++) {
        printf("%02x ", fw_metadata->shash[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    firmware_metadata_t metadata;
    sha256_context_t firmware_sha256;
    uint8_t output_buffer[sizeof(firmware_metadata_t)];
    int bytes_read = 0;
    uint8_t firmware_buffer[1024];
    char firmware_metadata_path[128];

    if (argc < 4) {
        puts("Usage: generate-metadata <BINFILE> <VERSION> <APPID> [output path]");
        puts("Options:");
        puts("--with-metadata    The sha256 hash will also include previous metadata");
        return -1;
    }

    if (argv[4]) {
        strcpy(firmware_metadata_path, argv[4]);
    }
    else {
        strcpy(firmware_metadata_path, "firmware-metadata.bin");
    }

    /* Open the firmware .bin file */
    if (!(firmware_bin = fopen(argv[1], "r+"))) {
        printf("Error: No binary file found!\n");
        return -1;
    }

    /*
     * Firmware might need to be re-hashed to increase security (e.g. avoid
     * non-matching version and/or signature), by also including the metadata.
     */
    if (argv[5] != NULL && (strcmp(argv[5], "--with-metadata") == 0)) {
        firmware_metadata_t current_metadata;
        bytes_read = fread(&current_metadata, 1, sizeof(current_metadata), firmware_bin);
        rewind(firmware_bin);
        if (bytes_read == sizeof(current_metadata)) {
            /*
             * Remove previous hashes and signatures, which will be added later.
             */
            for (unsigned long i = 0; i < sizeof(current_metadata.shash); i++) {
                current_metadata.shash[i] = 0xFF;
            }
            for (unsigned long i = 0; i < sizeof(current_metadata.hash); i++) {
                current_metadata.hash[i] = 0xFF;
            }

            /*
             * Reduce the size of the firmware since metadata is taking
             * FW_METADATA_SPACE more bytes.
             */
            firmware_size -= FW_METADATA_SPACE;

            fwrite(&current_metadata, sizeof(current_metadata), 1, firmware_bin);
            rewind(firmware_bin);
            puts("New firmware metadata:");
        }
    }

    sha256_init(&firmware_sha256);

    while((bytes_read = fread(firmware_buffer, 1, sizeof(firmware_buffer), firmware_bin))) {
        sha256_update(&firmware_sha256, firmware_buffer, bytes_read);
        firmware_size += bytes_read;
    }
    sha256_final(&firmware_sha256, metadata.hash);

    /* Close the firmware .bin file. */
    fclose(firmware_bin);

    /*
     * TODO Sign hash
     */
    for (unsigned long i = 0; i < sizeof(metadata.shash); i++) {
        metadata.shash[i] = 0;
    }

    /* Generate FW image metadata */
    metadata.size = firmware_size;
    sscanf(argv[2], "%xu", (unsigned int *)&(metadata.version));
    sscanf(argv[3], "%xu", &(metadata.appid));
    memcpy(output_buffer, (uint8_t*)&metadata, sizeof(firmware_metadata_t));
    print_metadata(&metadata);

    /* Open the output firmware .bin file */
    metadata_bin = fopen(firmware_metadata_path, "w");

    /* Write the metadata */
    printf("Metadata size: %lu\n", sizeof(firmware_metadata_t));
    fwrite(output_buffer, sizeof(output_buffer), 1, metadata_bin);

    /* 0xff spacing until firmware binary starts */
    uint8_t blank_buffer[FW_METADATA_SPACE - sizeof(firmware_metadata_t)];

    for (unsigned long b = 0; b < sizeof(blank_buffer); b++) {
        blank_buffer[b] = 0xff;
    }

    fwrite(blank_buffer, sizeof(blank_buffer), 1, metadata_bin);

    /* Close the metadata file */
    fclose(metadata_bin);

    return 0;
}
