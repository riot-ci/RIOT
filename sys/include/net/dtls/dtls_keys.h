/*
 * Copyright (c) 2016, Raul Fuentes <raul.fuentes_samaniego@inria.fr>
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
 * @file
 * @author      Raul Fuentes <raul.fuentes_samaniego@inria.fr>
 *
 */


#ifndef _RIOT_DTLS_KEYS_PARAMS_H
#define _RIOT_DTLS_KEYS_PARAMS_H

/*
 * TODO: They must be unique by node.
 *       Default values are those for the dtls-server original example.
 */

/* Used by the PSK client side */
# ifdef DTLS_PSK
#   define PSK_DEFAULT_IDENTITY "Client_identity"
#   define PSK_DEFAULT_KEY      "secretPSK"
#   define PSK_OPTIONS          "i:k:"
    /* Max size for PSK lowered for embedded devices */
#   define PSK_ID_MAXLEN 32
#   define PSK_MAXLEN 32
#endif /* DTLS_PSK */

# ifdef DTLS_ECC
  static const unsigned char ecdsa_priv_key[] = {
      0xD9, 0xE2, 0x70, 0x7A, 0x72, 0xDA, 0x6A, 0x05,
      0x04, 0x99, 0x5C, 0x86, 0xED, 0xDB, 0xE3, 0xEF,
      0xC7, 0xF1, 0xCD, 0x74, 0x83, 0x8F, 0x75, 0x70,
      0xC8, 0x07, 0x2D, 0x0A, 0x76, 0x26, 0x1B, 0xD4
  };
  static const unsigned char ecdsa_pub_key_x[] = {
      0xD0, 0x55, 0xEE, 0x14, 0x08, 0x4D, 0x6E, 0x06,
      0x15, 0x59, 0x9D, 0xB5, 0x83, 0x91, 0x3E, 0x4A,
      0x3E, 0x45, 0x26, 0xA2, 0x70, 0x4D, 0x61, 0xF2,
      0x7A, 0x4C, 0xCF, 0xBA, 0x97, 0x58, 0xEF, 0x9A
  };
  static const unsigned char ecdsa_pub_key_y[] = {
      0xB4, 0x18, 0xB6, 0x4A, 0xFE, 0x80, 0x30, 0xDA,
      0x1D, 0xDC, 0xF4, 0xF4, 0x2E, 0x2F, 0x26, 0x31,
      0xD0, 0x43, 0xB1, 0xFB, 0x03, 0xE2, 0x2F, 0x4D,
      0x17, 0xDE, 0x43, 0xF9, 0xF9, 0xAD, 0xEE, 0x70
  };
# endif /* DTLS_ECC */

  /**
   * @   mainpage
   * @author Raul Fuentes, Inria
   *
   */
#endif /*_RIOT_DTLS_KEYS_PARAMS_H */
