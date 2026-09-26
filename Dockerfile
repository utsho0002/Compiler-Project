# ------------------------------------------------------------
# Stage 1: generate the lexer/parser and build the native compiler
# ------------------------------------------------------------
FROM gcc:14 AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends flex bison \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY compiler/ ./

# lex.yy.c, parser.tab.c and parser.tab.h are GENERATED here.
# They are intentionally not stored in the source project.
RUN bison -d -o parser.tab.c parser.y \
    && flex -o lex.yy.c lexer.l \
    && g++ -std=c++17 -O2 -pipe lex.yy.c parser.tab.c main.cpp -o /usr/local/bin/bangla_compiler

# ------------------------------------------------------------
# Stage 2: small Python web runtime
# ------------------------------------------------------------
FROM python:3.12-slim
WORKDIR /app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY app.py ./
COPY templates/ ./templates/
COPY examples/ ./examples/
COPY --from=build /usr/local/bin/bangla_compiler /app/bin/bangla_compiler

RUN useradd --create-home --uid 10001 suborno \
    && chown -R suborno:suborno /app
USER suborno

ENV PORT=10000 \
    COMPILER_PATH=/app/bin/bangla_compiler \
    PYTHONUNBUFFERED=1

EXPOSE 10000

HEALTHCHECK --interval=30s --timeout=3s --start-period=10s --retries=3 \
    CMD python -c "import urllib.request; urllib.request.urlopen('http://127.0.0.1:' + __import__('os').environ.get('PORT','10000') + '/health', timeout=2)"

CMD ["sh", "-c", "exec gunicorn --bind 0.0.0.0:${PORT:-10000} --workers 2 --threads 2 --timeout 20 app:app"]
