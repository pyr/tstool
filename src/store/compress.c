/*
 * Copyright (c) 2016 Pierre-Yves Ritschard <pyr@spootnik.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Here we implement the storage layout described in the Facebook Gorilla
 * paper.
 *
 * A chunk is a contiguous space of memory where we pack timeseries data.
 * A chunk always start with a 64-bit timestamp, a 14-bit initial time-delta
 * from the start timestamp and a 64-bit first metric value.
 *
 * Subsequent values are stored as time-deltas and metric-deltas which makes
 * chunks optimized for series with data coming at regular intervals with small
 * differences between metric values.
 *
 * Chun
 */

#include <sys/types.h>
#include <sys/queue.h>

#include "ts.h"
#include "buf.h"
#include "compress.h"
#include "delta.h"

static  int chunk_write_header(struct chunk *, struct point *);
static  int chunk_write_time_delta(struct chunk *, struct point *);
static  int chunk_write_value(struct chunk *, struct point *);

int
chunk_init(struct chunk *chunk, timestamp_t t)
{
    buf_clear(&chunk->buf);
    chunk->t_prev = t;
    chunk->state = CHUNK_HEADER;
    return buf_push(&chunk->buf, (u_int64_t)t, 64);
}

int
chunk_write_header(struct chunk *chunk, struct point *point)
{
    int         res;
    int         diff = time_delta_init(chunk->t_prev, point->timestamp);
    u_int64_t   w = 0L | ((u_int64_t)diff) << 50;

    res = buf_push(&chunk->buf, w, 14);
    if (res != 0)
        return res;
    chunk->t_delta = diff;
    chunk->t_prev = point->timestamp;
    res = buf_push(&chunk->buf, point->metric, 64);
    if (res != 0)
        return res;
    chunk->m_prev = point->metric;
    chunk->m_delta = 0;
    chunk->state = CHUNK_VALUE;
    return 0;
}

int
chunk_write_time_delta(struct chunk *chunk, struct point *point)
{
    int         res;
    int         width;
    int         td;

    width = time_delta(chunk->t_delta, chunk->t_prev, point->timestamp, &td);
    if (width == 36) {
        res = buf_push(&chunk->buf, (u_int64_t)0xff, 4);
        if (res != 0)
            return res;
        return buf_push(&chunk->buf, (u_int64_t)td, 32);
    }
    return buf_push(&chunk->buf, (u_int64_t)td, width);
}

int
chunk_write_value(struct chunk *chunk, struct point *point)
{
    int         res;
    int         width;
    u_int16_t   hdr;
    u_int64_t   md;

    width = double_delta(chunk->m_prev, point->metric, &hdr, &md);
    if (hdr == 0) {
        return buf_push(&chunk->buf, (u_int64_t)0L, 1);
    } else if (hdr == chunk->m_prev_header) {
       res = buf_push(&chunk->buf, (u_int64_t)(1L<<63), 2);
    } else {
        res = buf_push(&chunk->buf, (u_int64_t)hdr, (64 - width));
    }
    if (res != 0)
        return res;
    return buf_push(&chunk->buf, (u_int64_t)md, width);
}


int
chunk_write(struct chunk *chunk, struct point *point)
{
    int res;

    res = chunk_write_time_delta(chunk, point);
    if (res != 0)
        return res;
    return chunk_write_value(chunk, point);
}

int
chunk_augment(struct chunk *chunk, struct point *point)
{
    int res;

    switch (chunk->state) {
    case CHUNK_INIT:
        buf_clear(&chunk->buf);
        res = chunk_init(chunk, point->timestamp);
        if (res != 0)
            return res;
        chunk->state = CHUNK_HEADER;
        /*
         * Fall through
         */
    case CHUNK_HEADER:
        return chunk_write_header(chunk, point);
    case CHUNK_VALUE:
        return chunk_write(chunk, point);
    }
    return -1;
}

int
chunk_compress(struct chunk *chunk, struct point *point, size_t len)
{
    int     res;
    size_t  i;

    for (i = 0; i < len; i++) {
        res = chunk_augment(chunk, &(point[i]));
        if (res != 0)
            break;
    }
    return res;
}
