#include "sched_RR.h"
#include "sched_MLFQ.h"

static struct sched_class *sched_class;

static void MLFQ_init(struct run_queue *rq)
{
	sched_class = &RR_sched_class;
	struct list_ele_t *list = &(rq->rq_link), *le = list;
	/* init n parallel run queue */
	do {
		sched_class->init(le2rq(le, rq_link));
		le = list_next(le);
	} while (le != list);
}

/*
 * 如果判断进程proc的rq不为空且time_silce为0,则需要降rq队列，
 * 从rq[i]调整到rq[i+1]，如果是最后一个rq，即rq[3]，则继续
 * 保持在rq[3]中，然后调用RR_enqueue把proc插入到rq[i+1]中。
 */
static void MLFQ_enqueue(struct run_queue *rq, 
			 struct proc_struct *proc)
{
	struct run_queue *nrq = rq;
	/* proc->rq != NULL是针对再次入队的进程 */
	if (proc->rq != NULL && proc->time_slice == 0) {
		nrq = le2rq(list_next(&(proc->rq->rq_link)), rq_link);
		/* 若已经是最后一级队列，则不改变所在队列，变成FCFS调度 */
		if (nrq == rq) {
			nrq = proc->rq;
		}
	}
	sched_class->enqueue(nrq, proc);
}

static void MLFQ_dequeue(struct run_queue *rq, struct proc_struct *proc)
{
	sched_class->dequeue(proc->rq, proc);
}

static struct proc_struct *MLFQ_pick_next(struct run_queue *rq)
{
	struct proc_struct *next;
	struct list_ele_t *list = &(rq->rq_link), *le = list;
	do {
		if ((next = sched_class->pick_next(le2rq(le, rq_link))) != NULL) {
			break;
		}
		le = list_next(le);
	} while (le != list);
	return next;
}

static void MLFQ_proc_tick(struct run_queue *rq, 
			   struct proc_struct *proc)
{
	sched_class->time_tick(proc->rq, proc);
}

struct sched_class MLFQ_sched_class = {
    .name = "MLFQ_scheduler",
    .init = MLFQ_init,
    .enqueue = MLFQ_enqueue,
    .dequeue = MLFQ_dequeue,
    .pick_next = MLFQ_pick_next,
    .time_tick = MLFQ_proc_tick,
};