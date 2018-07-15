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

#include "db.h"
#include "endpoints.h"

static int ep_api_users(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    int count;
    json_t * json_body;
    json_t * users;

    if (db_get_users_count(&count) == false) {
        ulfius_set_empty_body_response(response, 500);
        return U_CALLBACK_CONTINUE;
    }

    users = db_get_users(0, 20);
    if (users == NULL) {
        ulfius_set_empty_body_response(response, 500);
        return U_CALLBACK_CONTINUE;
    }

    json_body = json_object();

    json_object_set_new(json_body, "total", json_integer(count));
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

static int ep_api_users_id_stat(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    ulfius_set_empty_body_response(response, 501);

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
    ulfius_add_endpoint_by_val(inst, "GET", "/api/users", NULL, 0,
        &ep_api_users, NULL);
    ulfius_add_endpoint_by_val(inst, "GET", "/api/users/:id", NULL, 0,
        &ep_api_users_id, NULL);
    ulfius_add_endpoint_by_val(inst, "GET", "/api/users/:id/stat", NULL, 0,
        &ep_api_users_id_stat, NULL);
    ulfius_set_default_endpoint(inst, &ep_default, NULL);
}
