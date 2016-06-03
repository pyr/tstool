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
 * This is a standalone file. If you need a simple bit array, you can
 * use this file and use the bits up to the first function in your include
 * file.
 */
#include <sys/types.h>
#include <stdlib.h>

#include "buf.h"

/*
 * Initialize a bit array structure to the expected capacity.
 */
int
buf_init(struct buf *buf, size_t capacity)
{
    if ((buf->data = calloc(capacity, sizeof(*buf->data))) == NULL)
        return -1;
    buf->pos = 0;
    buf->size = capacity;
    return 0;
}

void
buf_clear(struct buf *buf)
{
    size_t i;

    for (i = 0; i < buf->size; i++)
        buf->data[i] = 0L;
    buf->pos = 0L;
}

/*
 * Create a new bit array, with the specified underlying capacity.
 */
struct buf  *
buf_new(size_t capacity)
{
    struct buf  *buf;

    if ((buf = calloc(1, sizeof(*buf))))
        return NULL;

    if (buf_init(buf, capacity) != 0) {
        free(buf);
        return NULL;
    }
    return buf;
}

/*
 * Set bits in the array to the values in the word.
 */
int
buf_set(struct buf *buf, u_int64_t index, u_int64_t w, int width)
{
    int     spills_over;
    size_t  idx;
    int     off;

    idx = BUF_IDX(index);
    off = BUF_OFF(index);
    spills_over = ((width + off) > BUF_WORD_WIDTH);

    if (spills_over && ((idx + 1) >= buf->size))
        return -1;

    if (off == 0) {
        buf->data[idx] = w;
    } else {
        buf->data[idx] = (w << off) | (buf->data[idx] & BUF_MASK(off));
        idx++;
        if (spills_over) {
            buf->data[idx] = (((w >> (BUF_WORD_WIDTH - off))) |
                              (buf->data[idx] & (BUF_WORD_FULL << off)));
        }
    }
    buf->pos += width;
    return 0;
}

/*
 * Push a word to the array.
 */
int
buf_push(struct buf *buf, u_int64_t w, int width)
{
    return buf_set(buf, buf->pos, w, width);
}
