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
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PORT 65535

static int bind_port = 5000;
static const char *db_path;

static void usage(void)
{
    printf("Usage: avs [-p port | -h] <database>\n");
}

void opts_parse_args(int argc, char **argv)
{
    struct stat path_stat;
    char *endp;
    char c;

    while ((c = getopt(argc, argv, "hp:")) != -1) {
        switch (c) {
        case 'p':
            errno = 0;
            bind_port = strtoul(optarg, &endp, 10);

            if (errno != 0 || *endp != '\0' || bind_port > MAX_PORT) {
                error(0, errno, "invalid value of the argument '%c'", c);
                exit(EXIT_FAILURE);
            }
            break;

        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;

        case '?':
        default:
            exit(EXIT_FAILURE);
        }
    }

    if (argc < 2 || optind == argc) {
        usage();
        exit(EXIT_FAILURE);
    }

    db_path = argv[optind];
    if (access(db_path, R_OK) != 0) {
        error(0, errno, "%s", db_path);
        exit(EXIT_FAILURE);
    }

    stat(db_path, &path_stat);
    if (S_ISREG(path_stat.st_mode) == 0) {
        error(0, 0, "'%s' is not a file", db_path);
        exit(EXIT_FAILURE);
    }
}

int opts_get_port()
{
    return bind_port;
}

const char * opts_get_db_path()
{
    return db_path;
}
