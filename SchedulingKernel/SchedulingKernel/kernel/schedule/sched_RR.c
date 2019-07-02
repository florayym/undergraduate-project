#include "sched_RR.h"

static void RR_init(struct run_queue *rq)
{
	list_init(&(rq->run_list));
	rq->proc_num = 0;
}

static void RR_enqueue(struct run_queue *rq, struct proc_struct *proc)
{
	// At the beginning of a list, add before means add from tail
	list_add_before(&(rq->run_list), &(proc->run_link));
	proc->time_slice = 
		(proc->run_next_slice > rq->max_time_slice) ? 
		rq->max_time_slice : proc->run_next_slice;
	proc->run_next_slice -= proc->time_slice;
	proc->rq = rq;
	rq->proc_num++;
}

static void RR_dequeue(struct run_queue *rq, struct proc_struct *proc)
{
	list_del_init(&(proc->run_link));
	rq->proc_num--;
}

static struct proc_struct *RR_pick_next(struct run_queue *rq)
{
	struct list_ele_t *le = list_next(&(rq->run_list));
	if (le != &(rq->run_list)) {
		return le2proc(le, run_link);
	}
	return NULL;
}

static void RR_proc_tick(struct run_queue *rq, struct proc_struct *proc)
{
	if (proc->time_slice > 0) {
		proc->time_slice--;
	}
	if (proc->time_slice == 0) {
		// 这样在下一次中断来后执行trap函数时，
		// 会由于当前进程程成员变量need_resched
		// 标识为1而执行schedule函数，从而把
		// 当前执行进程放回就绪队列末尾，
		proc->need_resched = 1;
	}
}

struct sched_class RR_sched_class = {
    .name = "RR_scheduler",
    .init = RR_init,
    .enqueue = RR_enqueue,
    .dequeue = RR_dequeue,
    .pick_next = RR_pick_next,
    .time_tick = RR_proc_tick,
};