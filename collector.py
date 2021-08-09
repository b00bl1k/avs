#!/usr/bin/env python

import schedule
import re
import time
import sqlite3
import datetime
import cfscrape
import logging
from random import randint

URL = "http://avto-nomer.ru/aktivuserall?galak=0&start={}"
TOP_PAGES = 15

FORMAT = "%(asctime)s:%(levelname)s:%(module)s:%(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)
logger = logging.getLogger("avstat")


def get_date():
    date = datetime.datetime.now() + datetime.timedelta(days=-1)
    date_str = datetime.datetime.strftime(date, "%Y-%m-%d")
    return date_str


def parse_stat(page):
    matches = re.findall(r"""
        <tr>
            <td>
                <span>\d+</span>
            </td>
            <td>
                <span><a\s+href=\'/user(\d+)\'>(.*?)</a></span>
            </td>
            <td>
                <span>
                    <a\s+href=\'/gallery\.php\?usr=\d+\'>
                        ([0-9\s]+)
                    </a>
                </span>\s+(.*?)
            </td>
        </tr>
        """, page, re.X)

    retval = []
    for match in matches:
        badge = match[3]
        added = 0
        if badge:
            result = re.search(r""">\+(\d+)<""", badge)
            if result:
                added = int(result.groups()[0])
        retval.append({
            "id": int(match[0]),
            "name": match[1],
            "total": int(match[2].replace(" ", "")),
            "added": added
            })

    return retval


def add_record(db, user_id, name, total, added, date):
    result = db.execute(
        "SELECT id FROM users WHERE id = ?",
        (user_id, ),
    ).fetchone()
    if result is None:
        db.execute("INSERT INTO users(id, name) VALUES(?,?)", (user_id, name))

    result = db.execute(
        "SELECT id FROM stat WHERE user_id = ? AND created = ? LIMIT 1",
        (user_id, date),
    ).fetchone()

    if not result:
        db.execute(
            "INSERT INTO stat(user_id, created, total, added) VALUES(?,?,?,?)",
            (user_id, date, total, added),
        )
    else:
        logger.warning(f"user_id={user_id} stat already exist")


def collect_stats():
    start_time = time.time()
    db = sqlite3.connect("data/stat.db")
    scraper = cfscrape.create_scraper()
    date = get_date()
    logger.info("previous date %s" % date)

    for page in range(TOP_PAGES):
        if page != 0:
            time.sleep(randint(2, 10))
        logger.info(f"loading page {page+1}")
        response = scraper.get(URL.format(page)).text
        stat = parse_stat(response)
        for item in stat:
            add_record(db, item["id"], item["name"], item["total"],
                       item["added"], date)
        db.commit()

    db.close()
    logger.info("completed in %f seconds" % (time.time() - start_time))


def migrate():
    db = sqlite3.connect("data/stat.db")
    logger.info("apply migrations")
    with open("schema.sql") as f:
        db.executescript(f.read())
    db.close()


schedule.every().day.at("04:00").do(collect_stats)
migrate()
logger.info("started")

while True:
    schedule.run_pending()
    time.sleep(1)
