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
 * The different states a chunk can be in.
 */
enum chunk_state {
    CHUNK_INIT = 0,
    CHUNK_HEADER = 1,
    CHUNK_VALUE = 2
};

/*
 * A chunk of timeseries in memory.
 * This is the equivalent of a Cassandra memtable
 * and follows the memory layout described in the
 * gorilla paper.
 *
 * All this does is hold on to a byte array.
 */
struct chunk {
    TAILQ_ENTRY(ts_chunk)    entry;
    struct buf               buf;
    enum chunk_state         state;
    timestamp_t              t_prev;
    double                   m_prev;
    u_int16_t                m_prev_header;
    int                      t_delta;
    u_int16_t                m_delta;
};
/*
 * A list of chunks
 */
TAILQ_HEAD(chunk_list, chunk);

/* compress.c */
int     chunk_init(struct chunk *, timestamp_t);
int     chunk_augment(struct chunk *, struct point *);
int     chunk_compress(struct chunk *, struct point *, size_t);
