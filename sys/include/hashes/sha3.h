#ifndef __SHA_3_H_
#define __SHA_3_H_

#include <stdlib.h>

#define SHA3_256_DIGEST_LENGTH 32
#define SHA3_384_DIGEST_LENGTH 48
#define SHA3_512_DIGEST_LENGTH 64

typedef struct {
  unsigned char state[200];
  unsigned int i;
  unsigned char delimitedSuffix;
  unsigned int rate;
  unsigned int capacity;
  unsigned int rateInBytes;
  unsigned int blockSize;
} keccak_state_t;

void Keccak_init(keccak_state_t *ctx, unsigned int rate, unsigned int capacity, unsigned char delimitedSuffix);
void Keccak_update(keccak_state_t *ctx, const unsigned char* input, unsigned long long int inputByteLen);
void Keccak_final(keccak_state_t *ctx, unsigned char *output, unsigned long long int outputByteLen);

void sha3_256_init(keccak_state_t *ctx);
void sha3_update(keccak_state_t *ctx, const void *data, size_t len);
void sha3_256_final(keccak_state_t *ctx, void *digest);

void sha3_256(void *digest, const void *data, size_t len);
void sha3_384(void *digest, const void *data, size_t len);
void sha3_512(void *digest, const void *data, size_t len);

#endif
