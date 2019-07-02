#include "job.h"
#include "../schedule/sched.h"

struct job_struct *idlejob = NULL;

struct job_struct *currentjob = NULL;

static struct job_struct *alloc_job(void)
{
	struct job_struct *job = malloc(sizeof(struct job_struct));
	if (job != NULL) {
		job->state = JOB_UNINIT;
		job->jid = -1;
		job->waiting_time = 0;
		srand((int)time(0));
		job->runs = rand() % 9 + 2;
		job->estimated_time = job->runs * 3;
		job->prio = job->waiting_time / job->estimated_time + 1;
		job->need_resched = 0;
		job->jq = NULL;
		job->job_link.next = NULL;
		job->job_link.prev = NULL;
	}
	return job;
}

// set_job_name - set the name of job
char *set_job_name(struct job_struct *job, const char *name)
{
	memset(job->name, 0, sizeof(job->name));
	return memcpy(job->name, name, JOB_NAME_LEN);
}

// get_job_name - get the name of job
char *get_job_name(struct job_struct *job)
{
	static char name[JOB_NAME_LEN + 1];
	memset(name, 0, sizeof(name));
	return memcpy(name, job->name, JOB_NAME_LEN);
}

// get_jid - alloc a unique jid for job
int get_jid(struct job_queue *jq)
{
	struct job_struct *job;
	int jid = rand() % (MAX_JID - 1) + 1;
	
	struct list_ele_t *list = &(jq->job_list), *le = list;
	while ((le = list_next(le)) != list) {
		job = le2job(le, job_link);
		if (jid == job->jid) {
			jid += 2;
		}
	}
	return jid;
}

void job_init(void)
{
	if ((idlejob = alloc_job()) == NULL) {
		exit(EXIT_FAILURE);
	}
	idlejob->jid = 0;	// idle job id
	idlejob->prio = 0;
	idlejob->need_resched = 1;
	idlejob->state = JOB_READY;
	set_job_name(idlejob, "idle");
	currentjob = idlejob;
}

struct job_struct *job_fork(struct job_queue *jq)
{
	struct job_struct *job;
	if ((job = alloc_job()) == NULL) {
		return NULL;
	}
	job->jid = get_jid(jq);
	job->state = JOB_READY;
	set_job_name(job, "job"); // £¿£¿£¿
	return job;
}