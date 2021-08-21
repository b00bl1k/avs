#!/usr/bin/env python

from datetime import datetime, timedelta
from sanic import Sanic
from sanic import response
import aiosqlite

app = Sanic(__name__)


@app.middleware("request")
async def db_middleware(request):
    app.ctx.db = await aiosqlite.connect("data/stat.db")

app.register_middleware(db_middleware, "db")


@app.route("/api/users/<user_id:int>/stat")
async def users_stat(request, user_id: int):
    from_date = request.args.get("from")
    if from_date:
        try:
            from_date = datetime.strptime(from_date, "%Y-%m-%d")
        except ValueError as e:
            return response.json({"error": str(e)}, status=400)
    else:
        from_date = datetime.now() - timedelta(days=53 * 7)

    db = app.ctx.db
    cursor = await db.execute(
        "SELECT user_id, created, added FROM stat "
        "WHERE user_id = ? AND created >= ?",
        (user_id, from_date.strftime("%Y-%m-%d"))
    )
    rows = await cursor.fetchall()
    await cursor.close()
    await db.close()
    if rows:
        result = [{"date": row[1], "count": row[2]} for row in rows]
        return response.json({"data": result})
    return response.json({"error": "not found"}, status=404)


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8000)
