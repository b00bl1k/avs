FROM python:3.9

RUN groupadd -r app \
    && useradd -u 1000 -r -g app app

COPY Pipfile* /tmp/
RUN cd /tmp && pip install pipenv==2021.5.29 \
    && pipenv lock --requirements > requirements.txt \
    && pip install -r /tmp/requirements.txt

ADD . /app

WORKDIR /app
RUN mkdir -p data && chown -R app:app /app
USER app

CMD ["./server.py"]
