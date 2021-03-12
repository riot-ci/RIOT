/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @{
 * @ingroup     lwm2m_objects_common
 *
 * @file
 * @brief       Common functionalities to interact with LwM2M objects
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include "liblwm2m.h"
#include "lwm2m_client.h"
#include "objects/common.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Auxiliary structure to hold pointers that form an object link data type.
 */
typedef struct {
    uint16_t *object_id;    /**< pointer to object ID */
    uint16_t *instance_id;  /**< pointer to instance ID */
} _objlink_t;

/**
 * @brief Get data of a given type from an object's instance.
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  uri               Initialized URI structure specifying the resource to get.
 * @param[in]  expected_type     Type of data that is expected from the specified resource.
 * @param[out] out               Pointer where to store the data.
 * @param[in]  out_len           Length of @p out.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
static int _get_resource_data(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                              lwm2m_data_type_t expected_type, void *out, size_t out_len)
{
    assert(client_data);
    assert(uri);
    assert(out);

    int result = 0;
    int data_num = 1;
    lwm2m_data_t *data = NULL;

    /* try to find the requested object */
    lwm2m_object_t *object = lwm2m_get_object_by_id(client_data, uri->objectId);
    if (!object || !object->readFunc) {
        DEBUG("[lwm2m:get_data] could not find object with ID %d\n", uri->objectId);
        result = -1;
        goto out;
    }

    /* prepare a new data structure */
    data = lwm2m_data_new(1);
    data->id = uri->resourceId;

    /* read the resource from the specified instance */
    uint8_t res = object->readFunc(uri->instanceId, &data_num, &data, object);
    if (res != COAP_205_CONTENT || data->type != expected_type) {
        result = -1;
        goto free_out;
    }

    /* process the data according to the type */
    switch (expected_type) {
    case LWM2M_TYPE_INTEGER:
        *(int64_t *)out = data->value.asInteger;
        break;

    case LWM2M_TYPE_FLOAT:
        *(double *)out = data->value.asFloat;
        break;

    case LWM2M_TYPE_BOOLEAN:
        *(bool *)out = data->value.asBoolean;
        break;

    case LWM2M_TYPE_OBJECT_LINK:
    {
        _objlink_t *_out = (_objlink_t *)out;
        *_out->object_id = data->value.asObjLink.objectId;
        *_out->instance_id = data->value.asObjLink.objectInstanceId;
        break;
    }

    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
        if (data->value.asBuffer.length > out_len) {
            DEBUG("[lwm2m:get_data] not enough space in buffer\n");
            result = -1;
        }
        else {
            memcpy(out, data->value.asBuffer.buffer, data->value.asBuffer.length);
        }
        break;

    default:
        DEBUG("[lwm2m:get_data] not supported type\n");
        result = -1;
        break;
    }

free_out:
    lwm2m_data_free(1, data);
out:
    return result;
}

/**
 * @brief Get the value of a string-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref _get_resource_data with a string representing the resource's
 * path.
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  expected_type     Type of data that is expected from the specified resource.
 * @param[out] out               Pointer where to store the data.
 * @param[in]  out_len           Length of @p out.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
static int _get_resource_data_by_path(lwm2m_client_data_t *client_data, const char *path,
                                      size_t path_len, lwm2m_data_type_t expected_type, void *out,
                                      size_t out_len)
{
    assert(path);

    lwm2m_uri_t uri;
    if (!lwm2m_stringToUri(path, path_len, &uri)) {
        DEBUG("[lwm2m:get_resource] malformed path\n");
        return -1;
    }

    return _get_resource_data(client_data, &uri, expected_type, out, out_len);
}

static int _set_resource_data(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                              lwm2m_data_type_t type, void *in, size_t in_len)
{
    assert(client_data);
    assert(uri);
    assert(in);

    int result = 0;
    lwm2m_data_t *data = NULL;

    /* try to find the requested object */
    lwm2m_object_t *object = lwm2m_get_object_by_id(client_data, uri->objectId);
    if (!object || !object->writeFunc) {
        DEBUG("[lwm2m:get_data] could not find object with ID %d\n", uri->objectId);
        result = -1;
        goto out;
    }

    /* prepare a new data structure */
    data = lwm2m_data_new(1);
    data->id = uri->resourceId;
    data->type = type;

    /* prepare the data according to the type */
    switch (type) {
    case LWM2M_TYPE_INTEGER:
        data->value.asInteger = *(int64_t *)in;
        break;

    case LWM2M_TYPE_FLOAT:
        data->value.asFloat = *(double *)in;
        break;

    case LWM2M_TYPE_BOOLEAN:
        data->value.asBoolean = *(bool *)in;
        break;

    case LWM2M_TYPE_OBJECT_LINK:
    {
        _objlink_t *_in = (_objlink_t *)in;
        data->value.asObjLink.objectId = *_in->object_id;
        data->value.asObjLink.objectInstanceId = *_in->instance_id;
        break;
    }

    case LWM2M_TYPE_STRING:
    case LWM2M_TYPE_OPAQUE:
        data->value.asBuffer.length = in_len;
        data->value.asBuffer.buffer = in;
        break;

    default:
        DEBUG("[lwm2m:get_data] not supported type\n");
        result = -1;
        goto free_out;
        break;
    }

    /* write the resource of the specified instance */
    uint8_t res = object->writeFunc(uri->instanceId, 1, data, object);
    if (res != COAP_204_CHANGED) {
        result = -1;
    }

free_out:
    /* NOTE: lwm2m_data_free will try to free strings and opaques if set, assuming they were
       allocated, so empty the pointer first */
    if (type == LWM2M_TYPE_STRING || type == LWM2M_TYPE_OPAQUE) {
        data->value.asBuffer.buffer = NULL;
    }
    lwm2m_data_free(1, data);
out:
    return result;
}

/**
 * @brief Set the value of a string-type resource, specified by a path @p path.
 *
 * Convenience function to call @ref _set_resource_data with a string representing the resource's
 * path.
 *
 * @param[in]  client_data       Pointer to the LwM2M client data.
 * @param[in]  path              Array containing the path to the resource to get.
 * @param[in]  path_len          Length of @p path.
 * @param[in]  expected_type     Type of data that is expected from the specified resource.
 * @param[in]  in               Pointer where to store the data.
 * @param[in]  in_len           Length of @p out.
 *
 * @return 0 on success
 * @return <0 otherwise
 */
static int _set_resource_data_by_path(lwm2m_client_data_t *client_data, const char *path,
                                      size_t path_len, lwm2m_data_type_t type, void *in,
                                      size_t in_len)
{
    assert(path);

    lwm2m_uri_t uri;
    if (!lwm2m_stringToUri(path, path_len, &uri)) {
        DEBUG("[lwm2m:set_resource] malformed path\n");
        return -1;
    }

    return _set_resource_data(client_data, &uri, type, in, in_len);
}

int lwm2m_get_string(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, char *out,
                     size_t out_len)
{
    return _get_resource_data(client_data, uri, LWM2M_TYPE_STRING, out, out_len);
}

int lwm2m_get_opaque(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, uint8_t *out,
                     size_t out_len)
{
    return _get_resource_data(client_data, uri, LWM2M_TYPE_OPAQUE, out, out_len);
}

int lwm2m_get_int(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, int64_t *out)
{
    return _get_resource_data(client_data, uri, LWM2M_TYPE_INTEGER, out, sizeof(int64_t));
}

int lwm2m_get_float(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, double *out)
{
    return _get_resource_data(client_data, uri, LWM2M_TYPE_FLOAT, out, sizeof(double));
}

int lwm2m_get_bool(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, bool *out)
{
    return _get_resource_data(client_data, uri, LWM2M_TYPE_BOOLEAN, out, sizeof(bool));
}

int lwm2m_get_objlink(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                      uint16_t *object_id_out, uint16_t *instance_id_out)
{
    assert(object_id_out);
    assert(instance_id_out);

    _objlink_t link = { .object_id = object_id_out, .instance_id = instance_id_out };
    return _get_resource_data(client_data, uri, LWM2M_TYPE_OBJECT_LINK, &link, sizeof(_objlink_t));
}

int lwm2m_get_string_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             char *out, size_t out_len)
{
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_STRING, out, out_len);
}

int lwm2m_get_opaque_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             uint8_t *out, size_t out_len)
{
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_OPAQUE, out, out_len);
}

int lwm2m_get_int_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                          int64_t *out)
{
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_INTEGER, out,
                                      sizeof(int64_t));
}

int lwm2m_get_float_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                            double *out)
{
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_FLOAT, out,
                                      sizeof(double));
}

int lwm2m_get_bool_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                           bool *out)
{
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_BOOLEAN, out,
                                      sizeof(bool));
}

int lwm2m_get_objlink_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                              uint16_t *object_id_out, uint16_t *instance_id_out)
{
    assert(object_id_out);
    assert(instance_id_out);

    _objlink_t link = { .object_id = object_id_out, .instance_id = instance_id_out };
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_OBJECT_LINK, &link,
                                      sizeof(_objlink_t));
}

int lwm2m_set_string(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, char *val,
                     size_t val_len)
{
    return _set_resource_data(client_data, uri, LWM2M_TYPE_STRING, val, val_len);
}

int lwm2m_set_opaque(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, uint8_t *val,
                     size_t val_len)
{
    return _set_resource_data(client_data, uri, LWM2M_TYPE_OPAQUE, val, val_len);
}

int lwm2m_set_int(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, int64_t val)
{
    return _set_resource_data(client_data, uri, LWM2M_TYPE_INTEGER, &val, sizeof(int64_t));
}

int lwm2m_set_float(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, double val)
{
    return _set_resource_data(client_data, uri, LWM2M_TYPE_FLOAT, &val, sizeof(double));
}

int lwm2m_set_bool(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri, bool val)
{
    return _set_resource_data(client_data, uri, LWM2M_TYPE_BOOLEAN, &val, sizeof(bool));
}

int lwm2m_set_objlink(lwm2m_client_data_t *client_data, const lwm2m_uri_t *uri,
                      uint16_t object_id_in, uint16_t instance_id_in)
{
    _objlink_t link = { .object_id = &object_id_in, .instance_id = &instance_id_in };
    return _set_resource_data(client_data, uri, LWM2M_TYPE_OBJECT_LINK, &link, sizeof(_objlink_t));
}

int lwm2m_set_string_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             char *val, size_t val_len)
{
    return _set_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_STRING, val, val_len);
}

int lwm2m_set_opaque_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                             uint8_t *val, size_t val_len)
{
    return _set_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_OPAQUE, val, val_len);
}

int lwm2m_set_int_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                          int64_t val)
{
    return _set_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_INTEGER, &val,
                                      sizeof(int64_t));
}

int lwm2m_set_float_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                            double val)
{
    return _set_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_FLOAT, &val,
                                      sizeof(double));
}

int lwm2m_set_bool_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                           bool val)
{
    return _set_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_BOOLEAN, &val,
                                      sizeof(bool));
}

int lwm2m_set_objlink_by_path(lwm2m_client_data_t *client_data, const char *path, size_t path_len,
                              uint16_t object_id_in, uint16_t instance_id_in)
{

    _objlink_t link = { .object_id = &object_id_in, .instance_id = &instance_id_in };
    return _get_resource_data_by_path(client_data, path, path_len, LWM2M_TYPE_OBJECT_LINK, &link,
                                      sizeof(_objlink_t));
}
