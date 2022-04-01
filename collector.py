#!/usr/bin/env python

import argparse
import schedule
import re
import time
import sqlite3
import datetime
import cfscrape
import logging
from requests.adapters import HTTPAdapter
from urllib3.util import Retry
from random import randint

URL = "https://platesmania.com/aktivuserall?galak=0&start={}"
TOP_PAGES = 15

FORMAT = "%(asctime)s:%(levelname)s:%(module)s:%(message)s"
logging.basicConfig(level=logging.INFO, format=FORMAT)
logger = logging.getLogger("avstat")
retry_strategy = Retry(
    total=10,
    backoff_factor=2,
    status_forcelist=[429, 500, 502, 503, 504],
)


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
    retry_adapter = HTTPAdapter(max_retries=retry_strategy)

    start_time = time.time()
    db = sqlite3.connect("data/stat.db")
    scraper = cfscrape.create_scraper()
    scraper.mount("https://", retry_adapter)
    scraper.mount("http://", retry_adapter)

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


def serve():
    schedule.every().day.at("04:00").do(collect_stats)
    migrate()
    logger.info("started")

    while True:
        schedule.run_pending()
        time.sleep(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("command", choices=["runonce", "serve"],
                        default="serve", const="serve", nargs="?")
    args = parser.parse_args()
    if args.command == "runonce":
        collect_stats()
    elif args.command == "serve":
        serve()
