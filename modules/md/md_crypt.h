/* Copyright 2017 greenbytes GmbH (https://www.greenbytes.de)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef mod_md_md_crypt_h
#define mod_md_md_crypt_h

#include <apr_file_io.h>

struct apr_array_header_t;
struct md_t;
struct md_http_response_t;
struct md_cert_t;
struct md_pkey_t;

/**************************************************************************************************/
/* random */

apr_status_t md_rand_bytes(unsigned char *buf, apr_size_t len, apr_pool_t *p);

/**************************************************************************************************/
/* digests */
apr_status_t md_crypt_sha256_digest64(const char **pdigest64, apr_pool_t *p, 
                                      const char *d, size_t dlen);
apr_status_t md_crypt_sha256_digest_hex(const char **pdigesthex, apr_pool_t *p, 
                                        const char *d, size_t dlen);

/**************************************************************************************************/
/* private keys */

typedef struct md_pkey_t md_pkey_t;

apr_status_t md_crypt_init(apr_pool_t *pool);

apr_status_t md_pkey_gen_rsa(md_pkey_t **ppkey, apr_pool_t *p, int bits);
void md_pkey_free(md_pkey_t *pkey);

const char *md_pkey_get_rsa_e64(md_pkey_t *pkey, apr_pool_t *p);
const char *md_pkey_get_rsa_n64(md_pkey_t *pkey, apr_pool_t *p);

apr_status_t md_pkey_fload(md_pkey_t **ppkey, apr_pool_t *p, 
                           const char *pass_phrase, apr_size_t pass_len,
                           const char *fname);
apr_status_t md_pkey_fsave(md_pkey_t *pkey, apr_pool_t *p, 
                           const char *pass_phrase, apr_size_t pass_len, 
                           const char *fname, apr_fileperms_t perms);

apr_status_t md_crypt_sign64(const char **psign64, md_pkey_t *pkey, apr_pool_t *p, 
                             const char *d, size_t dlen);

void *md_cert_get_X509(struct md_cert_t *cert);
void *md_pkey_get_EVP_PKEY(struct md_pkey_t *pkey);

/**************************************************************************************************/
/* X509 certificates */

typedef struct md_cert_t md_cert_t;

typedef enum {
    MD_CERT_UNKNOWN,
    MD_CERT_VALID,
    MD_CERT_EXPIRED
} md_cert_state_t;

void md_cert_free(md_cert_t *cert);

apr_status_t md_cert_fload(md_cert_t **pcert, apr_pool_t *p, const char *fname);
apr_status_t md_cert_fsave(md_cert_t *cert, apr_pool_t *p, 
                           const char *fname, apr_fileperms_t perms);

apr_status_t md_cert_read_http(md_cert_t **pcert, apr_pool_t *pool, 
                               const struct md_http_response_t *res);

md_cert_state_t md_cert_state_get(md_cert_t *cert);
int md_cert_is_valid_now(const md_cert_t *cert);
int md_cert_has_expired(const md_cert_t *cert);
int md_cert_covers_domain(md_cert_t *cert, const char *domain_name);
int md_cert_covers_md(md_cert_t *cert, const struct md_t *md);
apr_time_t md_cert_get_not_after(md_cert_t *cert);
apr_time_t md_cert_get_not_before(md_cert_t *cert);

apr_status_t md_cert_get_issuers_uri(const char **puri, md_cert_t *cert, apr_pool_t *p);
apr_status_t md_cert_get_alt_names(apr_array_header_t **pnames, md_cert_t *cert, apr_pool_t *p);

apr_status_t md_cert_to_base64url(const char **ps64, md_cert_t *cert, apr_pool_t *p);
apr_status_t md_cert_from_base64url(md_cert_t **pcert, const char *s64, apr_pool_t *p);

apr_status_t md_chain_fload(struct apr_array_header_t **pcerts, 
                            apr_pool_t *p, const char *fname);
apr_status_t md_chain_fsave(struct apr_array_header_t *certs, 
                            apr_pool_t *p, const char *fname, apr_fileperms_t perms);

apr_status_t md_cert_req_create(const char **pcsr_der_64, const struct md_t *md, 
                                md_pkey_t *pkey, apr_pool_t *p);

apr_status_t md_cert_self_sign(md_cert_t **pcert, const char *cn, 
                               const char *domain, md_pkey_t *pkey,
                               apr_interval_time_t valid_for, apr_pool_t *p);

#endif /* md_crypt_h */
