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


#include <sys/types.h>

typedef u_int64_t   timestamp_t;
int                 double_delta(double x, double y, u_int16_t *, u_int64_t *);
int                 time_delta_init(timestamp_t, timestamp_t);
int                 time_delta(int, timestamp_t, timestamp_t, int *);

int
double_delta(double x, double y, u_int16_t *header, u_int64_t *delta)
{
    u_int64_t ix;
    u_int64_t iy;
    u_int64_t diff;
    u_int16_t leading;
    u_int16_t trailing;
    int       width;
    int       i;

    /*
     * Convert our values to u_int64_t to allow bitwise operations
     */
    ix = *((u_int64_t *)&x);
    iy = *((u_int64_t *)&y);

    diff = (ix ^ iy) | 0x0000000000000000L;
    if (diff == 0) {
        *header = *delta = 0;
        return 0;
    }

    for (i = 63, leading = 0; i > 0; i--, leading++) {
        if ((1L << i) & diff) {
            break;
        }
    }

    for (trailing = 0; trailing < 63; trailing++) {
        if ((1L << trailing) & diff) {
            break;
        }
    }
    width = 64 - leading - trailing;

    /*
     * Now store our
     */
    *delta = diff << leading;
    *header = (1 << 15) | (1 << 14) | (leading << 9) | (width << 3);
    return width;
}

int
time_delta_init(timestamp_t t1, timestamp_t t2)
{
    int diff = (int)(t2 - t1);

    return (diff << 18);
}

int
time_delta(int prev, timestamp_t t1, timestamp_t t2, int *delta)
{
    int diff = (int)(t2 - t1);
    int ddiff = diff - prev;

    if (ddiff == 0) {
        *delta = 0;
        return 1;
    } else if (ddiff >= -63 && ddiff <= 64) {
        *delta = (1 << 31) | (ddiff << 23);
        return 9;
    } else if (ddiff >= -255 && ddiff <= 256) {
        *delta = (1 << 31) | (1 << 30) | (ddiff << 20);
        return 12;
    } else if (ddiff >= -2047 && ddiff <= 2048) {
        *delta = (1 << 31) | (1 << 30) | (1 << 29) | (ddiff << 16);
        return 16;
    } else {
        *delta = ddiff;
        return 36;
    }
}
