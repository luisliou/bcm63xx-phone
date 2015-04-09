/*
 * Copyright (C) 2015
 * Gilles Mazoyer <mazoyer.gilles@gilande.com>
 *
 * This is free software, licensed under the GNU General Public License v2.
 * See /LICENSE for more information.
 */
#ifndef __WAIT_QUEUE_H__
#define __WAIT_QUEUE_H__

#include "config.h"

#ifdef __KERNEL__
#include <linux/sched.h>
#include <linux/wait.h>
#else // !__KERNEL__
#include <fake_kernel.h>
#endif // !__KERNEL__

#include <bcm63xx_log.h>
#include <mutex.h>

typedef struct {
   // Kernel wait queue used to block processes
   wait_queue_head_t wq;
   // Counter that tells processes blocked in wait queue that something has changed
   atomic_t counter;
} bcm_wait_queue_t;

static inline void bcm_wait_queue_init(bcm_wait_queue_t *t)
{
   bcm_pr_debug("bcm_wait_queue_init()\n");
   init_waitqueue_head(&(t->wq));
   atomic_set(&(t->counter), 0);
}

static inline void bcm_wait_queue_deinit(bcm_wait_queue_t *t)
{
   bcm_pr_debug("bcm_wait_queue_deinit()\n");
}

static inline void bcm_wait_queue_wake_up(bcm_wait_queue_t *t)
{
   dd_bcm_pr_debug("bcm_wait_queue_wake_up()\n");
   if (waitqueue_active(&(t->wq))) {
      atomic_inc(&(t->counter));
      wake_up_interruptible_all(&(t->wq));
   }
}

static inline int bcm_wait_queue_get_counter(const bcm_wait_queue_t *t)
{
   return (atomic_read(&(t->counter)));
}

extern int bcm_wait_queue_wait_event_counter(bcm_wait_queue_t *t, int counter,
   bcmph_mutex_t *lock);

static inline int bcm_wait_queue_wait_event(bcm_wait_queue_t *t,
   bcmph_mutex_t *lock)
{
   return (bcm_wait_queue_wait_event_counter(t,
      bcm_wait_queue_get_counter(t), lock));
}

extern int bcm_wait_queue_wait_event_counter_timeout(bcm_wait_queue_t *t,
   int counter, long timeout_in_jiffies, bcmph_mutex_t *lock);

static inline int bcm_wait_queue_wait_event_timeout(bcm_wait_queue_t *t,
   long timeout_in_jiffies, bcmph_mutex_t *lock)
{
   return (bcm_wait_queue_wait_event_counter_timeout(t,
      bcm_wait_queue_get_counter(t), timeout_in_jiffies,
      lock));
}

#endif // __WAIT_QUEUE_H__