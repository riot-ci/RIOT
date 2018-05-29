/*
 * Copyright (C) 2016 Andrew Moon (dedicated to the public domain)
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "crypto/poly1305.h"

static void poly1305_blocks(poly1305_ctx_t *ctx, const uint8_t *msg, size_t msglen);
static uint32_t u8to32(const uint8_t *p)
{
    return
        (((uint32_t)(p[0] & 0xff)) |
         ((uint32_t)(p[1] & 0xff) <<  8) |
         ((uint32_t)(p[2] & 0xff) << 16) |
         ((uint32_t)(p[3] & 0xff) << 24));
}

static void u32to8(uint8_t *p, uint32_t v)
{
    p[0] = (v) & 0xff;
    p[1] = (v >>  8) & 0xff;
    p[2] = (v >> 16) & 0xff;
    p[3] = (v >> 24) & 0xff;
}

void poly1305_update(poly1305_ctx_t *ctx, const uint8_t *data, size_t len)
{
    size_t i;

    /* handle leftover */
    if (ctx->leftover) {
        size_t want = (poly1305_block_size - ctx->leftover);
        if (want > len) {
            want = len;
        }
        for (i = 0; i < want; i++) {
            ctx->buffer[ctx->leftover + i] = data[i];
        }
        len -= want;
        data += want;
        ctx->leftover += want;
        if (ctx->leftover < poly1305_block_size) {
            return;
        }
        poly1305_blocks(ctx, ctx->buffer, poly1305_block_size);
        ctx->leftover = 0;
    }

    /* process full blocks */
    if (len >= poly1305_block_size) {
        size_t want = (len & ~(poly1305_block_size - 1));
        poly1305_blocks(ctx, data, want);
        data += want;
        len -= want;
    }

    /* store leftover */
    if (len) {
        for (i = 0; i < len; i++)
            ctx->buffer[ctx->leftover + i] = data[i];
        ctx->leftover += len;
    }
}

void poly1305_init(poly1305_ctx_t *ctx, const uint8_t *key)
{
    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    ctx->r[0] = (u8to32(&key[ 0])) & 0x3ffffff;
    ctx->r[1] = (u8to32(&key[ 3]) >> 2) & 0x3ffff03;
    ctx->r[2] = (u8to32(&key[ 6]) >> 4) & 0x3ffc0ff;
    ctx->r[3] = (u8to32(&key[ 9]) >> 6) & 0x3f03fff;
    ctx->r[4] = (u8to32(&key[12]) >> 8) & 0x00fffff;

    /* h = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;

    /* save pad for later */
    ctx->pad[0] = u8to32(&key[16]);
    ctx->pad[1] = u8to32(&key[20]);
    ctx->pad[2] = u8to32(&key[24]);
    ctx->pad[3] = u8to32(&key[28]);

    ctx->leftover = 0;
    ctx->final = 0;
}

static void poly1305_blocks(poly1305_ctx_t *ctx, const uint8_t *data, size_t len)
{
    const uint32_t hibit = (ctx->final) ? 0 : (1UL << 24); /* 1 << 128 */
    uint32_t r0, r1, r2, r3, r4;
    uint32_t s1, s2, s3, s4;
    uint32_t h0, h1, h2, h3, h4;
    uint64_t d0, d1, d2, d3, d4;
    uint32_t c;

    r0 = ctx->r[0];
    r1 = ctx->r[1];
    r2 = ctx->r[2];
    r3 = ctx->r[3];
    r4 = ctx->r[4];

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

    while (len >= poly1305_block_size) {
        /* h += m[i] */
        h0 += (u8to32(data + 0)) & 0x3ffffff;
        h1 += (u8to32(data + 3) >> 2) & 0x3ffffff;
        h2 += (u8to32(data + 6) >> 4) & 0x3ffffff;
        h3 += (u8to32(data + 9) >> 6) & 0x3ffffff;
        h4 += (u8to32(data + 12) >> 8) | hibit;

        /* h *= r */
        d0 = ((uint64_t)h0 * r0) + ((uint64_t)h1 * s4) + ((uint64_t)h2 * s3) + ((uint64_t)h3 * s2) + ((uint64_t)h4 * s1);
        d1 = ((uint64_t)h0 * r1) + ((uint64_t)h1 * r0) + ((uint64_t)h2 * s4) + ((uint64_t)h3 * s3) + ((uint64_t)h4 * s2);
        d2 = ((uint64_t)h0 * r2) + ((uint64_t)h1 * r1) + ((uint64_t)h2 * r0) + ((uint64_t)h3 * s4) + ((uint64_t)h4 * s3);
        d3 = ((uint64_t)h0 * r3) + ((uint64_t)h1 * r2) + ((uint64_t)h2 * r1) + ((uint64_t)h3 * r0) + ((uint64_t)h4 * s4);
        d4 = ((uint64_t)h0 * r4) + ((uint64_t)h1 * r3) + ((uint64_t)h2 * r2) + ((uint64_t)h3 * r1) + ((uint64_t)h4 * r0);

        /* (partial) h %= p */
        c = (uint32_t)(d0 >> 26); h0 = (uint32_t)d0 & 0x3ffffff;
        d1 += c;      c = (uint32_t)(d1 >> 26); h1 = (uint32_t)d1 & 0x3ffffff;
        d2 += c;      c = (uint32_t)(d2 >> 26); h2 = (uint32_t)d2 & 0x3ffffff;
        d3 += c;      c = (uint32_t)(d3 >> 26); h3 = (uint32_t)d3 & 0x3ffffff;
        d4 += c;      c = (uint32_t)(d4 >> 26); h4 = (uint32_t)d4 & 0x3ffffff;
        h0 += c * 5;  c =                (h0 >> 26); h0 =                h0 & 0x3ffffff;
        h1 += c;

        data += poly1305_block_size;
        len -= poly1305_block_size;
    }

    ctx->h[0] = h0;
    ctx->h[1] = h1;
    ctx->h[2] = h2;
    ctx->h[3] = h3;
    ctx->h[4] = h4;
}

void poly1305_finish(poly1305_ctx_t *ctx, uint8_t *mac)
{
    uint32_t h0, h1, h2, h3, h4, c;
    uint32_t g0, g1, g2, g3, g4;
    uint64_t f;
    uint32_t mask;

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i++] = 1;
        for (; i < poly1305_block_size; i++)
            ctx->buffer[i] = 0;
        ctx->final = 1;
        poly1305_blocks(ctx, ctx->buffer, poly1305_block_size);
    }

    /* fully carry h */
    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

    c = h1 >> 26; h1 = h1 & 0x3ffffff;
    h2 +=     c; c = h2 >> 26; h2 = h2 & 0x3ffffff;
    h3 +=     c; c = h3 >> 26; h3 = h3 & 0x3ffffff;
    h4 +=     c; c = h4 >> 26; h4 = h4 & 0x3ffffff;
    h0 += c * 5; c = h0 >> 26; h0 = h0 & 0x3ffffff;
    h1 +=     c;

    /* compute h + -p */
    g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
    g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
    g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
    g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
    g4 = h4 + c - (1UL << 26);

    /* select h if h < p, or h + -p if h >= p */
    mask = (g4 >> ((sizeof(uint32_t) * 8) - 1)) - 1;
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    g3 &= mask;
    g4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;
    h2 = (h2 & mask) | g2;
    h3 = (h3 & mask) | g3;
    h4 = (h4 & mask) | g4;

    /* h = h % (2^128) */
    h0 = ((h0) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

    /* mac = (h + pad) % (2^128) */
    f = (uint64_t)h0 + ctx->pad[0]; h0 = (uint32_t)f;
    f = (uint64_t)h1 + ctx->pad[1] + (f >> 32); h1 = (uint32_t)f;
    f = (uint64_t)h2 + ctx->pad[2] + (f >> 32); h2 = (uint32_t)f;
    f = (uint64_t)h3 + ctx->pad[3] + (f >> 32); h3 = (uint32_t)f;

    u32to8(mac +  0, h0);
    u32to8(mac +  4, h1);
    u32to8(mac +  8, h2);
    u32to8(mac + 12, h3);

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->r[3] = 0;
    ctx->r[4] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;
    ctx->pad[2] = 0;
    ctx->pad[3] = 0;
}

void poly1305_auth(uint8_t *mac, const uint8_t *data, size_t len, const uint8_t *key)
{
    poly1305_ctx_t ctx;

    poly1305_init(&ctx, key);
    poly1305_update(&ctx, data, len);
    poly1305_finish(&ctx, mac);
}
