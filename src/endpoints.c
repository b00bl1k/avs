/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Alexey Ryabov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <errno.h>
#include <stdlib.h>

#include "db.h"
#include "endpoints.h"

static int ep_api_users(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    int limit = 20;
    char *endp;
    const char *limit_val;
    json_t * json_body;
    json_t * users;

    limit_val = u_map_get(request->map_url, "limit");

    if (limit_val != NULL) {
        errno = 0;
        limit = strtoul(limit_val, &endp, 10);
        if (errno != 0 || *endp != '\0') {
            ulfius_set_empty_body_response(response, 400);
            return U_CALLBACK_CONTINUE;
        }
    }

    users = db_get_users(0, limit);
    if (users == NULL) {
        ulfius_set_empty_body_response(response, 500);
        return U_CALLBACK_CONTINUE;
    }

    json_body = json_object();

    json_object_set(json_body, "result", users);

    ulfius_set_json_body_response(response, 200, json_body);

    json_decref(users);
    json_decref(json_body);

    return U_CALLBACK_CONTINUE;
}

static int ep_api_users_id(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    ulfius_set_empty_body_response(response, 501);

    return U_CALLBACK_CONTINUE;
}

static int ep_api_users_id_stat(const struct _u_request *request,
    struct _u_response *response, void *user_data)
{
    int user_id;
    char *endp;
    const char *id_val;
    json_t *json_body;

    id_val = u_map_get(request->map_url, "id");

    if (id_val == NULL) {
        ulfius_set_empty_body_response(response, 400);
        return U_CALLBACK_CONTINUE;
    }

    errno = 0;
    user_id = strtoul(id_val, &endp, 10);

    if (errno != 0 || *endp != '\0') {
        ulfius_set_empty_body_response(response, 400);
        return U_CALLBACK_CONTINUE;
    }

    json_body = db_get_users_stat(user_id);

    ulfius_set_json_body_response(response, 200, json_body);

    json_decref(json_body);

    return U_CALLBACK_CONTINUE;
}

static int ep_default(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    ulfius_set_empty_body_response(response, 404);

    return U_CALLBACK_CONTINUE;
}

void endpoints_register(struct _u_instance * inst)
{
    ulfius_add_endpoint_by_val(inst, "GET", NULL, "/api/users", 0,
        &ep_api_users, NULL);
    ulfius_add_endpoint_by_val(inst, "GET", NULL, "/api/users/:id", 0,
        &ep_api_users_id, NULL);
    ulfius_add_endpoint_by_val(inst, "GET", NULL, "/api/users/:id/stat", 0,
        &ep_api_users_id_stat, NULL);
    ulfius_set_default_endpoint(inst, &ep_default, NULL);
}
