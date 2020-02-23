/*
 * EAP peer state machine functions (RFC 4137)
 * Copyright (c) 2004-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef EAP_H
#define EAP_H

#include "wpa/defs.h"
#include "wpa2/eap_peer/eap_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct eap_sm;

struct eap_method_type {
	int vendor;
	EapType method;
};

extern u8 *g_wpa_anonymous_identity;
extern int g_wpa_anonymous_identity_len;
extern u8 *g_wpa_username;
extern int g_wpa_username_len;
extern const u8 *g_wpa_client_cert;
extern int g_wpa_client_cert_len;
extern const u8 *g_wpa_private_key;
extern int g_wpa_private_key_len;
extern const u8 *g_wpa_private_key_passwd;
extern int g_wpa_private_key_passwd_len;

extern const u8 *g_wpa_ca_cert;
extern int g_wpa_ca_cert_len;

extern u8 *g_wpa_password;
extern int g_wpa_password_len;

extern u8 *g_wpa_new_password;
extern int g_wpa_new_password_len;

const u8 * eap_get_eapKeyData(struct eap_sm *sm, size_t *len);
void eap_deinit_prev_method(struct eap_sm *sm, const char *txt);
struct wpabuf * eap_sm_build_nak(struct eap_sm *sm, EapType type, u8 id);
int eap_peer_blob_init(struct eap_sm *sm);
void eap_peer_blob_deinit(struct eap_sm *sm);
int eap_peer_config_init(
	struct eap_sm *sm, u8 *private_key_passwd,
	int private_key_passwd_len);
void eap_peer_config_deinit(struct eap_sm *sm);
void eap_sm_abort(struct eap_sm *sm);
int eap_peer_register_methods(void);

#ifdef __cplusplus
}
#endif

#endif /* EAP_H */
