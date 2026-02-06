/* @file: ecc.h
 * #desc:
 *    The definitions of elliptic curve cryptography.
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#ifndef _DEMOZ_LIB_ECC_H
#define _DEMOZ_LIB_ECC_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define ECDH_X25519_LEN 32
#define ECDH_X25519_PRI_LEN ECDH_X25519_LEN
#define ECDH_X25519_PUB_LEN ECDH_X25519_LEN
#define ECDH_X25519_KEY_LEN ECDH_X25519_LEN
#define EDDSA_ED25519_LEN 32
#define EDDSA_ED25519_PRI_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_RAN_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_PUB_LEN EDDSA_ED25519_LEN
#define EDDSA_ED25519_SIGN_LEN 64

#define ECDH_X448_LEN 56
#define ECDH_X448_PRI_LEN ECDH_X448_LEN
#define ECDH_X448_PUB_LEN ECDH_X448_LEN
#define ECDH_X448_KEY_LEN ECDH_X448_LEN
#define EDDSA_ED448_LEN 57
#define EDDSA_ED448_PRI_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_RAN_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_PUB_LEN EDDSA_ED448_LEN
#define EDDSA_ED448_SIGN_LEN 114

#define ECDH_P256_LEN 32
#define ECDH_P256_PRI_LEN ECDH_P256_LEN
#define ECDH_P256_PUB_LEN 64
#define ECDH_P256_KEY_LEN ECDH_P256_LEN
#define ECDSA_P256_LEN 32
#define ECDSA_P256_PRI_LEN ECDSA_P256_LEN
#define ECDSA_P256_RAN_LEN ECDSA_P256_LEN
#define ECDSA_P256_PUB_LEN 64
#define ECDSA_P256_SIGN_LEN 64

#define ECDH_P384_LEN 48
#define ECDH_P384_PRI_LEN ECDH_P384_LEN
#define ECDH_P384_PUB_LEN 96
#define ECDH_P384_KEY_LEN ECDH_P384_LEN
#define ECDSA_P384_LEN 48
#define ECDSA_P384_PRI_LEN ECDSA_P384_LEN
#define ECDSA_P384_RAN_LEN ECDSA_P384_LEN
#define ECDSA_P384_PUB_LEN 96
#define ECDSA_P384_SIGN_LEN 96

#define ECDH_P521_LEN 66
#define ECDH_P521_PRI_LEN ECDH_P521_LEN
#define ECDH_P521_PUB_LEN 132
#define ECDH_P521_KEY_LEN ECDH_P521_LEN
#define ECDSA_P521_LEN 66
#define ECDSA_P521_PRI_LEN ECDSA_P521_LEN
#define ECDSA_P521_RAN_LEN ECDSA_P521_LEN
#define ECDSA_P521_PUB_LEN 132
#define ECDSA_P521_SIGN_LEN 132
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/curve25519.c */

extern
void F_SYMBOL(ecdh_x25519_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdh_x25519_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
;

extern
void F_SYMBOL(eddsa_ed25519_nonce_key)(const uint8_t *inpri, uint8_t *pri,
		uint8_t *ran)
;

extern
void F_SYMBOL(eddsa_ed25519_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
void F_SYMBOL(eddsa_ed25519_sign)(const uint8_t *pri,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
;

extern
int32_t F_SYMBOL(eddsa_ed25519_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
;

/* lib/curve448.c */

extern
void F_SYMBOL(ecdh_x448_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdh_x448_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
;

extern
void F_SYMBOL(eddsa_ed448_nonce_key)(const uint8_t *inpri, uint8_t *pri,
		uint8_t *ran)
;

extern
void F_SYMBOL(eddsa_ed448_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
void F_SYMBOL(eddsa_ed448_sign)(const uint8_t *pri,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
;

extern
int32_t F_SYMBOL(eddsa_ed448_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
;

/* lib/nist-p256.c */

extern
int32_t F_SYMBOL(ecdh_p256_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdh_p256_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
;

extern
void F_SYMBOL(ecdsa_p256_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdsa_p256_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
;

extern
int32_t F_SYMBOL(ecdsa_p256_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
;

/* lib/nist-p384.c */

extern
int32_t F_SYMBOL(ecdh_p384_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdh_p384_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
;

extern
void F_SYMBOL(ecdsa_p384_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdsa_p384_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
;

extern
int32_t F_SYMBOL(ecdsa_p384_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
;

/* lib/nist-p521.c */

extern
int32_t F_SYMBOL(ecdh_p521_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdh_p521_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
;

extern
void F_SYMBOL(ecdsa_p521_public_key)(const uint8_t *pri, uint8_t *pub)
;

extern
int32_t F_SYMBOL(ecdsa_p521_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
;

extern
int32_t F_SYMBOL(ecdsa_p521_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
;

#ifdef __cplusplus
}
#endif


#endif
