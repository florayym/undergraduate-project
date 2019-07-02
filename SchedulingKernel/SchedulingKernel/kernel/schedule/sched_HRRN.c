#include "sched_HRRN.h"

static struct sched_class *sched_class;

static void HRRN_init(struct job_queue *jq)
{
	list_init(&(jq->job_list));
	jq->job_num = 0;
}

/* HRRN special enqueue */
static void HRRN_enqueue(struct job_queue *jq, struct job_struct *job)
{
	struct list_ele_t *list = &(jq->job_list), *le = list;
	do {
		le = list_next(le);
		struct job_struct *job_compare = le2job(le, job_link);
		if (jq->job_num == 0 || job->prio > job_compare->prio) {
			list_add_before(le, &(job->job_link));
			break;
		}
	} while (le != list);
	job->jq = jq;
	jq->job_num++;
}

static void HRRN_dequeue(struct job_queue *jq, struct job_struct *job)
{
	list_del_init(&(job->job_link));
	job->jq = NULL;
	jq->job_num--;
}

static struct job_struct *HRRN_pick_next(struct job_queue *jq)
{
	struct job_struct *cjob, *mjob;
	struct list_ele_t *list = &(jq->job_list), *current = list_next(list), *maxi = current;
	if (current != list) {
		while ((current = list_next(current)) != list) {
			cjob = le2job(current, job_link);
			mjob = le2job(maxi, job_link);
			if (cjob->prio > mjob->prio) {
				maxi = current;
			}
		}
		return le2job(maxi, job_link);
	}
	return NULL;
}

static void HRRN_job_tick(struct job_queue *jq, struct job_struct *job)
{
	job->prio = ++job->waiting_time * 0.1 / job->estimated_time + 1;
}

struct sched_class HRRN_sched_class = {
    .name = "HRRN_scheduler",
    .init = HRRN_init,
    .enqueue = HRRN_enqueue,
    .dequeue = HRRN_dequeue,
    .pick_next = HRRN_pick_next,
    .time_tick = HRRN_job_tick,
};