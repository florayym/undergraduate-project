#include "sched_FCFS.h"

static struct sched_class *sched_class;

static void FCFS_init(struct suspended_queue *sq)
{
	list_init(&(sq->suspended_list));
	sq->suspended_num = 0;
}

static void FCFS_enqueue(struct suspended_queue *sq,
			 struct proc_struct *proc)
{
	list_add_before(&(sq->suspended_list), &(proc->suspened_link));
	proc->sq = sq;
	sq->suspended_num++;
}

static void FCFS_dequeue(struct suspended_queue *sq, struct proc_struct *proc)
{
	list_del_init(&(proc->suspened_link));
	proc->sq = NULL;
	sq->suspended_num--;
}

static struct proc_struct *FCFS_pick_next(struct suspended_queue *sq)
{
	/* Maybe only need to find proc by hash linked pid??? */
	return NULL;
}

static void FCFS_proc_tick(struct suspended_queue *sq,
			   struct proc_struct *proc)
{
	/* Still, maybe don't need to do this? */
}

struct sched_class FCFS_sched_class = {
    .name = "FCFS_scheduler",
    .init = FCFS_init,
    .enqueue = FCFS_enqueue,
    .dequeue = FCFS_dequeue,
    .pick_next = FCFS_pick_next,
    .time_tick = FCFS_proc_tick,
};