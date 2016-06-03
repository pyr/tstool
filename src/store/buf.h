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
