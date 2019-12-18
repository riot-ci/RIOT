#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "hashes/sha256.h"
#include "board.h"
#include "xtimer.h"
#include "atca.h"
#include "atca_params.h"


#include "periph/gpio.h"

#define SHA256_HASH_SIZE (32)

int test_riot_sha256(uint8_t* teststring, uint16_t len, uint8_t* expected, uint8_t* result)
{
    LED0_ON;
    sha256_context_t ctx;    
    sha256_init(&ctx);
    sha256_update(&ctx, (void*)teststring, len);
    sha256_final(&ctx, result);
    LED0_OFF;
    return memcmp(expected, result, SHA256_HASH_SIZE);
}

int test_atca_sha(uint8_t* teststring, uint16_t len, uint8_t* expected, uint8_t* result)
{
    LED0_ON;
    atcab_sha_start();
    atcab_sha_end(result, len, teststring);
    LED0_OFF;
    return memcmp(expected, result, SHA256_HASH_SIZE);
}

int test_riot_aes(uint8_t* teststring, uint16_t len, uint8_t* expected, uint8_t* result)
{
    LED0_ON;
    sha256_context_t ctx;    
    sha256_init(&ctx);
    sha256_update(&ctx, (void*)teststring, len);
    sha256_final(&ctx, result);
    LED0_OFF;
    return memcmp(expected, result, SHA256_HASH_SIZE);
}

int test_atca_aes(uint8_t* teststring, uint16_t len, uint8_t* expected, uint8_t* result)
{
    LED0_ON;
    atcab_sha_start();
    atcab_sha_end(result, len, teststring);
    LED0_OFF;
    return memcmp(expected, result, SHA256_HASH_SIZE);
}

int main(void)
{
    // ATCA_STATUS status;

    // uint8_t revision[4] = { 0x00, 0x00, 0x00, 0x00 };
    gpio_init(GPIO_PIN(0,22), GPIO_OUT);

    uint8_t teststring[] = "chili cheese fries";
    uint8_t expected[] = {0x36, 0x46, 0xEF, 0xD6, 0x27, 0x6C, 0x0D, 0xCB, 0x4B, 0x07, 0x73, 0x41, 0x88, 0xF4, 
                            0x17, 0xB4, 0x38, 0xAA, 0xCF, 0xC6, 0xAE, 0xEF, 0xFA, 0xBE, 0xF3, 0xA8, 0x5D, 0x67, 0x42, 0x0D, 0xFE, 0xE5};

    uint8_t result[SHA256_HASH_SIZE]; // +3 to fit 1 byte length and 2 bytes checksum
    memset(result, 0, SHA256_HASH_SIZE); // alles in result auf 0 setzen

    uint16_t test_string_size = (sizeof(teststring)-1); // -1 to ignore \0
// 
    // status = atcab_random(result);
    // printf("Random status: %x\n", status);
    // printf("Number: %x %x %x %x %x %x %x %x\n", 
    // result[0], result[1], result[2],result[3], result[4],result[5],result[6],result[7]);
 
    // status = atcab_info(revision);
    // printf("info status= %x\n", status);
    // printf("Revision= %x %x %x %x\n", revision[0], revision[1], revision[2], revision[3]);    

    if (test_riot_sha256(teststring, test_string_size, expected, result) == 0)
    {
        printf("Riot: Success\n");
    }
    else
    {
        printf("Riot: Not a success.\n");
    }
    xtimer_usleep(10);
    memset(result, 0, SHA256_HASH_SIZE); // alles in result auf 0 setzen

    if (test_atca_sha(teststring, test_string_size, expected, result) == 0)
    {
        printf("ATCA: Success\n");
    }
    else
    {
        printf("ATCA: Not a success.\n");
    }

    return 0;
}