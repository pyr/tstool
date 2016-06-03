typedef u_int64_t   timestamp_t;

/*
 * A point in a timeseries
 */
struct point {
    timestamp_t timestamp;
    double      metric;
};

struct point_cell {
    TAILQ_ENTRY(point_cell) entry;
    struct point            point;
};
TAILQ_HEAD(point_list, point);
