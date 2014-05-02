/* MODIFIED 4-25-14 */

/* This table has one slot per process.  It contains scheduling information
 * for each process.
 */
#include <limits.h>

/* EXTERN should be extern except in main.c, where we want to keep the struct */
#ifdef _MAIN
#undef EXTERN
#define EXTERN
#endif

/**
 * We might later want to add more information to this table, such as the
 * process owner, process group or cpumask.
 */

EXTERN struct schedproc {
    endpoint_t endpoint;    /* process endpoint id */
    endpoint_t parent;  /* parent endpoint id */
    unsigned flags;     /* flag bits */

    /* User space scheduling */
    unsigned max_priority;  /* this process' highest allowed priority */
    unsigned priority;      /* the process' current priority */
    unsigned time_slice;        /* this process's time slice */
/* CHANGE START */
    unsigned tickets;         /* the number of tickets for this process, changed by nice */
    unsigned blocking;         /* number of times that another process has end of
                                 quantum while this process is in the winner queue */
/* CHANGE END */
} schedproc[NR_PROCS];

/* Flag values */
#define IN_USE          0x00001 /* set when 'schedproc' slot in use */
/* CHANGE START */
#define USER_PROCESS    0x0002  /* set when used by user lottery scheduler */
/* CHANGE END */
