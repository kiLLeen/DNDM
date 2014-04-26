/* This file contains the scheduling policy for SCHED
 *
 * The entry points are:
 *   do_noquantum:        Called on behalf of process' that run out of quantum
 *   do_start_scheduling  Request to start scheduling a proc
 *   do_stop_scheduling   Request to stop scheduling a proc
 *   do_nice              Request to change the nice level on a proc
 *   init_scheduling      Called from main.c to set up/prepare scheduling
 */
#include "sched.h"
#include "schedproc.h"
#include <assert.h>
#include <minix/com.h>
#include <machine/archtypes.h>
#include "kernel/proc.h" /* for queue constants */

/* CHANGE START */
#define HOLDING_Q       (NR_SCHED_QUEUES - 1)
                        /* this should be 16, the queue in which processes are
                           when they have not won the lottery */
#define WINNING_Q       (MIN_USER_Q - 1)
                        /* this should be 14, the queue in which processes are
                           when they HAVE won the lottery */
#define STARTING_TICKETS 20 /* the number of tickets each process starts with */

#define MAX_TICKETS     100 /* the max number of tickets a process can have */

#define MIN_TICKETS     1   /* the min number of tickets a process can have */
#define STARTING_QUANTUM 200 /* the starting quantum for a process */
/* CHANGE END */

FORWARD _PROTOTYPE( int schedule_process, (struct schedproc * rmp)  );

/* CHANGE START */
   /* removed prototype for balance_queues() since it
      has been deleted */
/* CHANGE END */

/* CHANGE START */
/*===========================================================================*
*              do_lottery                     *
*===========================================================================*/

PUBLIC int do_lottery() {
    register struct schedproc *rmp;
    int rv, proc_nr_n, winning_proc_nr;
    int total_tickets = 0;
    u64_t tsc;
    int random;

    /* count the total number of tickets */
    for (proc_nr_n = 0; proc_nr_n < NR_PROCS; ++proc_nr_n) { /* go through all processes */
        rmp = &schedproc[proc_nr_n];
        if (rmp->priority == HOLDING_Q) /* is this process eligible to win? */
            total_tickets += rmp->tickets;
    }
    /* generate a "random" winning ticket */
    read_tsc_64(&tsc);
    random = tsc.lo % total_tickets;

    /* now random is the winning ticket */
    /* find the winning process index */
    for (proc_nr_n = 0; proc_nr_n < NR_PROCS; ++proc_nr_n) { /* go through all processes */
        rmp = &schedproc[proc_nr_n];
        if (rmp->priority == HOLDING_Q) /* is this process eligible to win? */
            random -= rmp->tickets;
        if (random <= 0) {
            winning_proc_nr = proc_nr_n;
            break;
        }
    }

    /* schedule new winning process */
    rmp->priority = WINNING_Q;
    rmp->time_slice = STARTING_QUANTUM;
    if ((rv = schedule_process(rmp)) != OK) /* move process to winner queue */
        return rv;

    return OK;
}
/* CHANGE END */

/*===========================================================================*
 *              do_noquantum                     *
 *===========================================================================*/

 PUBLIC int do_noquantum(message *m_ptr) {
    register struct schedproc *rmp;
    int rv, proc_nr_n;

    if (sched_isokendpt(m_ptr->m_source, &proc_nr_n) != OK) {
        printf("SCHED: WARNING: got an invalid endpoint in OOQ msg %u.\n",
        m_ptr->m_source);
        return EBADEPT;
    }

    rmp = &schedproc[proc_nr_n];
/* CHANGE START */
    rmp->priority = HOLDING_Q;

    if ((rv = schedule_process(rmp)) != OK) /* move process to holding */
        return rv;

    if (rv = do_lottery() != OK) /* schedule a new winner */
        return rv;
/* CHANGE END */

    return OK;
}

/*===========================================================================*
 *              do_stop_scheduling               *
 *===========================================================================*/
PUBLIC int do_stop_scheduling(message *m_ptr)
{
    register struct schedproc *rmp;
    int rv, proc_nr_n;

    /* check who can send you requests */
    if (!accept_message(m_ptr))
        return EPERM;

    if (sched_isokendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n) != OK) {
        printf("SCHED: WARNING: got an invalid endpoint in OOQ msg "
        "%ld\n", m_ptr->SCHEDULING_ENDPOINT);
        return EBADEPT;
    }

    rmp = &schedproc[proc_nr_n];
    rmp->flags = 0; /*&= ~IN_USE;*/

    return OK;
}

/*===========================================================================*
 *              do_start_scheduling              *
 *===========================================================================*/
PUBLIC int do_start_scheduling(message *m_ptr)
{
    register struct schedproc *rmp;
    int rv, proc_nr_n, parent_nr_n, nice;
    
    /* we can handle two kinds of messages here */
    assert(m_ptr->m_type == SCHEDULING_START || 
        m_ptr->m_type == SCHEDULING_INHERIT);

    /* check who can send you requests */
    if (!accept_message(m_ptr))
        return EPERM;

    /* Resolve endpoint to proc slot. */
    if ((rv = sched_isemtyendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n))
            != OK) {
        return rv;
    }
    rmp = &schedproc[proc_nr_n];

    /* Populate process slot */
    rmp->endpoint     = m_ptr->SCHEDULING_ENDPOINT;
    rmp->parent       = m_ptr->SCHEDULING_PARENT;
    rmp->max_priority = (unsigned) m_ptr->SCHEDULING_MAXPRIO;
/* CHANGE START */
    rmp->tickets      = STARTING_TICKETS;
    rmp->max_tickets  = STARTING_TICKETS;
/* CHANGE END */

    if (rmp->max_priority >= NR_SCHED_QUEUES) {
        return EINVAL;
    }
    
    switch (m_ptr->m_type) {

    case SCHEDULING_START:
        /* We have a special case here for system processes, for which
         * quanum and priority are set explicitly rather than inherited 
         * from the parent */
        rmp->priority   = rmp->max_priority;
        rmp->time_slice = (unsigned) m_ptr->SCHEDULING_QUANTUM;
        break;
        
    case SCHEDULING_INHERIT:
        /* Inherit current priority and time slice from parent. Since there
         * is currently only one scheduler scheduling the whole system, this
         * value is local and we assert that the parent endpoint is valid */
        if ((rv = sched_isokendpt(m_ptr->SCHEDULING_PARENT,
                &parent_nr_n)) != OK)
            return rv;

/* CHANGE START */
        rmp->priority = HOLDING_Q;
        rmp->time_slice = STARTING_QUANTUM;
/* CHANGE END */
        break;
        
    default: 
        /* not reachable */
        assert(0);
    }

    /* Take over scheduling the process. The kernel reply message populates
     * the processes current priority and its time slice */
    if ((rv = sys_schedctl(0, rmp->endpoint, 0, 0)) != OK) {
        printf("Sched: Error taking over scheduling for %d, kernel said %d\n",
            rmp->endpoint, rv);
        return rv;
    }
    rmp->flags = IN_USE;

    /* Schedule the process, giving it some quantum */
    if ((rv = schedule_process(rmp)) != OK) {
        printf("Sched: Error while scheduling process, kernel replied %d\n",
            rv);
        return rv;
    }

    /* Mark ourselves as the new scheduler.
     * By default, processes are scheduled by the parents scheduler. In case
     * this scheduler would want to delegate scheduling to another
     * scheduler, it could do so and then write the endpoint of that
     * scheduler into SCHEDULING_SCHEDULER
     */

    m_ptr->SCHEDULING_SCHEDULER = SCHED_PROC_NR;

    return OK;
}

/*===========================================================================*
 *              do_nice                      *
 *===========================================================================*/
PUBLIC int do_nice(message *m_ptr)
{
    struct schedproc *rmp;
    int rv;
    int proc_nr_n;
    unsigned new_q, old_q, old_max_q;

    /* check who can send you requests */
    if (!accept_message(m_ptr))
        return EPERM;

    if (sched_isokendpt(m_ptr->SCHEDULING_ENDPOINT, &proc_nr_n) != OK) {
        printf("SCHED: WARNING: got an invalid endpoint in OOQ msg "
        "%ld\n", m_ptr->SCHEDULING_ENDPOINT);
        return EBADEPT;
    }

    rmp = &schedproc[proc_nr_n];
    new_q = (unsigned) m_ptr->SCHEDULING_MAXPRIO;
    if (new_q >= NR_SCHED_QUEUES) {
        return EINVAL;
    }

    /* Store old values, in case we need to roll back the changes */
    old_q     = rmp->priority;
    old_max_q = rmp->max_priority;

    /* Update the proc entry and reschedule the process */
    rmp->max_priority = rmp->priority = new_q;

    if ((rv = schedule_process(rmp)) != OK) {
        /* Something went wrong when rescheduling the process, roll
         * back the changes to proc struct */
        rmp->priority     = old_q;
        rmp->max_priority = old_max_q;
    }

    return rv;
}

/*===========================================================================*
 *              schedule_process                 *
 *===========================================================================*/
PRIVATE int schedule_process(struct schedproc * rmp)
{
    int rv;

    if ((rv = sys_schedule(rmp->endpoint, rmp->priority,
            rmp->time_slice)) != OK) {
        printf("SCHED: An error occurred when trying to schedule %d: %d\n",
        rmp->endpoint, rv);
    }

    return rv;
}


/*===========================================================================*
 *              start_scheduling                 *
 *===========================================================================*/

PUBLIC void init_scheduling(void)
{
/* CHANGE START */
    /* deleted code - we dont need balance_queues, 
       so we don't need to set up timers
       Also deleted balance_queues() */
}
/* CHANGE END */
