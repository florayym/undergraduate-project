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
 * ����жϽ���proc��rq��Ϊ����time_silceΪ0,����Ҫ��rq���У�
 * ��rq[i]������rq[i+1]����������һ��rq����rq[3]�������
 * ������rq[3]�У�Ȼ�����RR_enqueue��proc���뵽rq[i+1]�С�
 */
static void MLFQ_enqueue(struct run_queue *rq, 
			 struct proc_struct *proc)
{
	struct run_queue *nrq = rq;
	/* proc->rq != NULL������ٴ���ӵĽ��� */
	if (proc->rq != NULL && proc->time_slice == 0) {
		nrq = le2rq(list_next(&(proc->rq->rq_link)), rq_link);
		/* ���Ѿ������һ�����У��򲻸ı����ڶ��У����FCFS���� */
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