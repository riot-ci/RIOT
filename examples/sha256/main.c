#include <stdlib.h>
#include "hashes/sha256.h"

static void sha256_buf(uint8_t *buf, size_t len)
{
    static sha256_context_t sha256;
    uint8_t digest[SHA256_DIGEST_LENGTH];
    sha256_init(&sha256);

    sha256_update(&sha256, buf, len);

    sha256_final(&sha256, digest);
}

int main(void)
{
    uint8_t buf[8];
    sha256_buf(buf, 8);
    return 0;
}
