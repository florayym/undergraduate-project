#pragma once

/*
 * Improving the interactive performance of the O(1) scheduler. 
 * The most notable of these was the Rotating Staircase Deadline 
 * scheduler, which introduced the concept of fair scheduling, 
 * borrowed from queuing theory, to Linux's process scheduler. 
 * This concept was the inspiration for the O(1) scheduler's 
 * eventual replacement in kernel version 2.6.23, 
 * the Completely Fair Scheduler (CFS).
 *
 * The scheduling policy in a system must attempt to satisfy 
 * two conflicting goals:
 * Fast process response time (low latency)
 * Maximal system utilization (high throughput)
 */

#include <stdint.h>
#include "../process/proc.h"
#include "../job/job.h"

/* block_q */
/* block_suspend_q */

/* CPU principle timter */
struct cpu_timer_t {
	unsigned int normal_running;
};

#define le2timer(le, member) to_struct((le), struct timer_t, member)

// The introduction of scheduling classes is borrrowed from Linux, and makes the 
// core scheduler quite extensible. These classes (the scheduler modules) encapsulate 
// the scheduling policies. 
struct sched_class {
	// the name of sched_class
	const char *name;

	// Init the run/job queue
	void(*init)(struct run_queue *rq);
	
	// put the proc into runqueue, and this function must be called with rq_lock
	void(*enqueue)(struct run_queue *rq, struct proc_struct *proc);
	
	// get the proc out runqueue, and this function must be called with rq_lock
	void(*dequeue)(struct run_queue *rq, struct proc_struct *proc);
	
	// choose the next runnable task
	struct proc_struct *(*pick_next)(struct run_queue *rq);

	// dealer of the time-tick for process
	void(*time_tick)(struct run_queue *rq, struct proc_struct *proc);
};

/* ���������ж������ݽṹ����Ϊ���̾������� */
struct run_queue {
	/* ���ж��е��ڱ��ṹ�����Կ����Ƕ���ͷ��β������� */
	struct list_ele_t run_list;
	/* ������Ԫ������ */
	unsigned int proc_num;
	/* RR special��ÿ��Ԫ��һ��ռ�õ����ʱ��Ƭ */
	int max_time_slice;
	/* MLFQ special������� */
	struct list_ele_t rq_link;
};

#define le2rq(le, member) to_struct((le), struct run_queue, member)

/* ��ҵ�󱸶��е����ݽṹ */
struct job_queue {
	struct list_ele_t job_list;
	unsigned int job_num;
};

/* �������е����ݽṹ */
struct suspended_queue {
	struct list_ele_t suspended_list;
	unsigned int suspended_num;
};

enum sched_level {
	LONG_TERM_SCHED,
	INTERMEDIATE_LEVEL_SCHED,
	LOW_LEVEL_SCHED
};

static struct cpu_timer_t *cpu_timer;

static struct run_queue *rq;

static struct job_queue *jq;

static struct suspended_queue *sq;

static struct sched_class *job_sched_class, *proc_sched_class, *intermediate_sched_class;

void job_create(void);
void proc_suspend(const int pid);
void proc_active(const int pid);
void sched_init(void);
void high_level_schedule(void);
void low_level_schedule(void);

unsigned int update_cpu();
void task_manager(int *);
void proc_menu(char *, const char);

//void wakeup_proc(struct proc_struct *proc);
