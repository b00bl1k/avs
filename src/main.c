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

#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define U_DISABLE_CURL
#define U_DISABLE_WEBSOCKET
#include <ulfius.h>

#include <sqlite3.h>

int callback_get_test (const struct _u_request * request,
    struct _u_response * response, void * user_data);

int callback_default (const struct _u_request * request,
    struct _u_response * response, void * user_data);

int main(int argc, char **argv)
{
    int ret;

    struct _u_instance instance;

    y_init_logs("simple_example", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG,
        NULL, "Starting simple_example");

    if (ulfius_init_instance(&instance, 5000, NULL, NULL) != U_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
        return 1;
    }

    u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");

    instance.max_post_body_size = 1024;

    ulfius_add_endpoint_by_val(&instance, "GET", "/", NULL, 0, &callback_get_test, NULL);
    ulfius_set_default_endpoint(&instance, &callback_default, NULL);

    ret = ulfius_start_framework(&instance);

    if (ret == U_OK) {
        y_log_message(Y_LOG_LEVEL_DEBUG, "Start framework on port %d",
            instance.port);
        getchar();
    }
    else {
        y_log_message(Y_LOG_LEVEL_DEBUG, "Error starting framework");
    }

    y_log_message(Y_LOG_LEVEL_DEBUG, "End framework");
    y_close_logs();

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}

int callback_get_test(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    ulfius_set_string_body_response(response, 200, "Hello World!");

    return U_CALLBACK_CONTINUE;
}

int callback_default(const struct _u_request * request,
    struct _u_response * response, void * user_data)
{
    ulfius_set_string_body_response(response, 404, "Page not found");

    return U_CALLBACK_CONTINUE;
}
