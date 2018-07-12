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
#include <stdlib.h>
#include <sqlite3.h>

#include <yder.h>

#include "db.h"
#include "opts.h"

static bool db_open(sqlite3 ** db)
{
    int err = sqlite3_open_v2(opts_get_db_path(), db,
        SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX, NULL);

    if (err != SQLITE_OK)
        y_log_message(Y_LOG_LEVEL_ERROR, "sqlite3_open_v2: error %d", err);

    return err == SQLITE_OK;
}

static bool db_close(sqlite3 * db)
{
    int err = sqlite3_close(db);

    if (err != SQLITE_OK)
        y_log_message(Y_LOG_LEVEL_ERROR, "sqlite3_close: error %d", err);

    return err == SQLITE_OK;
}

void db_init()
{
    sqlite3 * db;

    if (sqlite3_threadsafe() == 0) {
        error(0, 0, "The libsqlite version is not threadsafe!\n");
        exit(EXIT_FAILURE);
    }

    if (db_open(&db) == false) {
        exit(EXIT_FAILURE);
    }

    db_close(db);
}

bool db_users_count(int * count)
{
    int err;
    sqlite3 * db;
    sqlite3_stmt * stmt;
    bool result = false;

    if (db_open(&db) == false)
        return false;

    err = sqlite3_prepare_v2(db, "SELECT COUNT(id) FROM users", -1, &stmt, 0);

    if (err == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            if (count != NULL) {
                *count = sqlite3_column_int(stmt, 0);
                result = true;
            }
        }
        else {
            y_log_message(Y_LOG_LEVEL_ERROR, "sqlite3_step: error %d", err);
        }
    }
    else {
        y_log_message(Y_LOG_LEVEL_ERROR, "sqlite3_prepare_v2: error %d", err);
    }

    sqlite3_finalize(stmt);
    db_close(db);

    return result;
}
