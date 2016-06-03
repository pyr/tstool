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
