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

#define BUF_IDX(pos)    ((pos) >> 6)
#define BUF_OFF(pos)    ((pos) & 63)
#define BUF_WORD_FULL   (~(u_int64_t)0)
#define BUF_WORD_WIDTH  64
#define BUF_IDX(pos)    ((pos) >> 6)
#define BUF_OFF(pos)    ((pos) & 63)
#define BUF_MASK(off)   ((off)? ~(u_int64_t)0 >> (64-(off)) : (u_int64_t)0)

struct buf {
    u_int64_t   *data;   /* holds the bit array */
    u_int64_t    pos;    /* bit position */
    size_t       size;   /* length of buffer */
};

int          buf_init(struct buf *, size_t);
void         buf_clear(struct buf *);
struct buf  *buf_new(size_t);
int          buf_set(struct buf *, u_int64_t, u_int64_t, int);
int          buf_push(struct buf *, u_int64_t, int);
