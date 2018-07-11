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

#include <sqlite3.h>

#include "endpoints.h"
#include "opts.h"

int main(int argc, char **argv)
{
    int ret;
    struct _u_instance instance;

    opts_parse_args(argc, argv);

    y_init_logs("avs", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG,
        NULL, NULL);

    if (ulfius_init_instance(&instance, opts_get_port(), NULL, NULL) != U_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
        return EXIT_FAILURE;
    }

    u_map_put(instance.default_headers, "Access-Control-Allow-Origin", "*");

    instance.max_post_body_size = 1024;

    endpoints_register(&instance);

    ret = ulfius_start_framework(&instance);

    if (ret == U_OK) {
        y_log_message(Y_LOG_LEVEL_DEBUG, "Start avs on port %d",
            instance.port);
        getchar();
    }
    else {
        y_log_message(Y_LOG_LEVEL_DEBUG, "Error starting");
    }

    y_close_logs();

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return EXIT_SUCCESS;
}
