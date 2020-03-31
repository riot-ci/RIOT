/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_uri_parser
 * @{
 *
 * @file
 * @brief       A minimal, non-destructive URI parser.
 *              @see https://tools.ietf.org/html/rfc3986
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * @}
 */

#include "uri_parser.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static char *_consume_scheme(uri_parser_result_t *result, char *uri,
                             bool *has_authority)
{
    assert(uri);
    assert(!has_authority);

    /* cannot have empty scheme */
    if (uri[0] == ':') {
        return NULL;
    }

    char *p = strchr(uri, ':');

    result->scheme = uri;
    result->scheme_len = p - uri;

    /* check if authority part exists '://' */
    if ((p[1] != '\0') && (p[2] != '\0') && (p[1] == '/') && (p[2] == '/')) {
        *has_authority = true;
        /* skip '://' */
        return p + 3;
    }

    /* skip ':' */
    return p + 1;
}

static char *_consume_authority(uri_parser_result_t *result, char *uri)
{
    assert(uri);

    /* search until first '/' */
    char *authority_end = strchr(uri, '/');
    if (!authority_end) {
        authority_end = uri + strlen(uri);
    }
    result->host = uri;
    result->host_len = authority_end - uri;

    /* check for userinfo within authority */
    char *userinfo_end = strchr(uri, '@');

    /* check if match is within authority */
    if (userinfo_end && (userinfo_end < authority_end)) {
        result->userinfo = uri;
        result->userinfo_len = userinfo_end - uri;
        /* shift host part beyond userinfo and '@' */
        result->host += result->userinfo_len + 1;
        result->host_len -= result->userinfo_len + 1;
    }

    char *ipv6_end = NULL;
    /* validate IPv6 form */
    if (result->host[0] == '[') {
        ipv6_end = strchr(result->host, ']');
        /* found end marker of IPv6 form beyond authority part */
        if (ipv6_end >= authority_end) {
            return NULL;
        }
    }

    /* check for port after host part */
    char *port_begin = NULL;
    /* repeat until last ':' in authority section */
    /* if ipv6 address, check after ipv6 end marker */
    char *p = (ipv6_end ? ipv6_end : result->host);
    while ((p != NULL) && (p < authority_end)) {
        port_begin = p;
        p = strchr(p + 1, ':');
    }

    /* check if match is within authority */
    if (port_begin && (port_begin[0] == ':') &&
        (port_begin != result->host)) {
        /* port should be at least one character, => + 1 */
        if (port_begin + 1 == authority_end) {
            return NULL;
        }
        result->port = port_begin + 1;
        result->port_len = authority_end - result->port;
        /* cut host part before port and ':' */
        result->host_len -= result->port_len + 1;
    }

    /* do not allow empty host if userinfo or port are set */
    if ((result->host_len == 0) &&
        (result->userinfo || result->port)) {
        return NULL;
    }
    /* this includes the '/' */
    return authority_end;
}

static char *_consume_path(uri_parser_result_t *result, char *uri)
{
    assert(uri);

    result->path = uri;
    result->path_len = strlen(uri);

    /* check for query start '?' */
    char *path_end = strchr(uri, '?');

    /* no query string found, return! */
    if (!path_end) {
        return (result->path + result->path_len);
    }

    /* there is a query string */
    result->query = path_end + 1;
    /* do not count '?' */
    result->query_len = result->path_len - (path_end - uri) - 1;
    /* cut path part before query and '?' */
    result->path_len -= result->query_len + 1;

    return (result->query + result->query_len);
}

static int _parse_relative(uri_parser_result_t *result, char *uri)
{
    /* we expect '\0', i.e., end of string */
    uri = _consume_path(result, uri);
    if (uri[0] != '\0') {
        return -1;
    }

    return 0;
}

static int _parse_absolute(uri_parser_result_t *result, char *uri)
{
    bool has_authority = false;

    uri = _consume_scheme(result, uri, &has_authority);
    if (uri == NULL) {
        return -1;
    }

    if (has_authority) {
        uri = _consume_authority(result, uri);
        if (uri == NULL) {
            return -1;
        }
    }

    /* parsing the path, starting with '/' */
    return _parse_relative(result, uri);
}

bool uri_parser_is_absolute(const char *uri)
{
    char *colon = strchr(uri, ':');

    /* potentially absolute, if ':' exists */
    if (colon) {
        /* first character should be ALPHA */
        if (!(((uri[0] >= 'A') && (uri[0] <= 'Z')) ||
              ((uri[0] >= 'a') && (uri[0] <= 'z')))) {
            /* relative */
            return false;
        }

        /* absolute */
        return true;
    }

    /* relative */
    return false;
}

int uri_parser_process(uri_parser_result_t *result, const char *uri)
{
    /* uri cannot be empty */
    if ((NULL == uri) || (uri[0] == '\0')) {
        return -1;
    }

    memset(result, 0, sizeof(*result));

    if (uri_parser_is_absolute(uri)) {
        return _parse_absolute(result, (char *)uri);
    }
    else {
        return _parse_relative(result, (char *)uri);
    }

    return 0;
}
