/* CHANGED - 4/26/14
 * This file contains the scheduling policy for SCHED
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

PRIVATE timer_t sched_timer;
PRIVATE unsigned balance_timeout;

#define BALANCE_TIMEOUT 5 /* how often to balance queues in seconds */
/* CHANGE START */
#define STARTING_TICKETS 20 /* the number of tickets each process starts with */
#define MAX_TICKETS 100 /* the max number of tickets a process can have (nice can change) */
#define MIN_TICKETS 1   /* the min number of tickets a process can have */
/* CHANGE END */

FORWARD _PROTOTYPE( int schedule_process, (struct schedproc * rmp)  );
FORWARD _PROTOTYPE( void balance_queues, (struct timer *tp)     );

#define DEFAULT_USER_TIME_SLICE 200

/* CHANGE START */
/*===========================================================================*
 * name: ticket_count                                                         *
 * parameters: none                                                          *
 * retuns: the number of tickets that have been distributed to the processes *
 * intended action: see "returns"                                            *
 * assumptions: none                                                         *
 *===========================================================================*/
PUBLIC int ticket_count()
{
    register struct schedproc *rmp;
    int total = 0; /* the total number of tickets */
    int i = 0;
  
    for (; i < NR_PROCS ; ++i) {
        rmp = &schedproc[proc_nr_n];
        /* Confirm the process exists. If it doesn't the tickets should 
           already be 0 but just in case. */
        if (rmp->flags & IN_USE) {
            total += rmp -> tickets;
        }
    }

    return total;
}

/*=============================================================================*
 * name: lottery                                                               *
 * parameters: none                                                            *
 * retuns: the index of the process in schedproc who won the lotter            *
 * intended action: to select a process to schedule probabaliticly via lottery *
 * assumptions: every process has at least one ticket                          *
 *=============================================================================*/
PRIVATE size_t lottery()
{
    int selection = 42 % ticket_count(); /* TODO replace 42 with the random number generated via dave's method */ /* ticket# 0 to X-1 where X is total number of tickets. */
    size_t winning_process = 0; /* index of the winner in the process table */
    register struct schedproc *rmp;

    /* subtract off the number of tickects the process' have until 
       the selection is found */
    do {
        rmp = &schedproc[winning_process++];
        if (rmp->flags & IN_USE) {
            selection -= rmp -> tickets; 
        }
    } while (selection >= 0);

    return (winning_process - 1);
}
/* CHANGE END */

/*===========================================================================*
 *              do_noquantum                     *TODO Call lottery() 
 *===========================================================================*/

PUBLIC int do_noquantum(message *m_ptr)
{
    register struct schedproc *rmp;
    int rv, proc_nr_n;

    if (sched_isokendpt(m_ptr->m_source, &proc_nr_n) != OK) {
        printf("SCHED: WARNING: got an invalid endpoint in OOQ msg %u.\n",
        m_ptr->m_source);
        return EBADEPT;
    }

    rmp = &schedproc[proc_nr_n];
    if (rmp->priority < MIN_USER_Q) {
        rmp->priority += 1; /* lower priority */
    }

    if ((rv = schedule_process(rmp)) != OK) {
        return rv;
    }
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
/* CHANGE START */
    rmp->tickets = 0; /* free up the tickets so they aren't miscounted */
/* CHANGE END */

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
    rmp->max_tickets  = MAX_TICKETS;
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

        rmp->priority = schedproc[parent_nr_n].priority;
        rmp->time_slice = schedproc[parent_nr_n].time_slice;
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
 *              do_nice                      *TODO: ticket stuff
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
    balance_timeout = BALANCE_TIMEOUT * sys_hz();
    init_timer(&sched_timer);
    set_timer(&sched_timer, balance_timeout, balance_queues, 0);
}

/*===========================================================================*
 *              balance_queues                   *
 *===========================================================================*/

/* This function in called every 100 ticks to rebalance the queues. The current
 * scheduler bumps processes down one priority when ever they run out of
 * quantum. This function will find all proccesses that have been bumped down,
 * and pulls them back up. This default policy will soon be changed.
 */
PRIVATE void balance_queues(struct timer *tp)
{
    struct schedproc *rmp;
    int proc_nr;
    int rv;

    for (proc_nr=0, rmp=schedproc; proc_nr < NR_PROCS; proc_nr++, rmp++) {
        if (rmp->flags & IN_USE) {
            if (rmp->priority > rmp->max_priority) {
                rmp->priority -= 1; /* increase priority */
                schedule_process(rmp);
            }
        }
    }

    set_timer(&sched_timer, balance_timeout, balance_queues, 0);
}
