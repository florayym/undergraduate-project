#pragma once

#include "../../libs/defs.h"
#include "../../libs/list.h"

/* job's state in his life cycle */
enum job_state {
	JOB_UNINIT = 0,			// uninitialized
	JOB_READY,			// ready
	JOB_RUNNING,			// runnable(maybe running)
	JOB_COMPLETE,			// almost complete or dead(interupted), and wait parent proc to reclaim his resource
};

extern struct job_struct *idlejob, *currentjob;

#define JOB_NAME_LEN                15
#define MAX_JOB                     4096
#define MAX_JID                     (MAX_JOB * 8)

/* Job data structure */
struct job_struct {
	enum job_state state;			// Job state	    
	int jid;				// Job ID
	char name[JOB_NAME_LEN + 1];		// Job name
	double prio;				// ʹ�ø���Ӧ�����ȵ���ҵ�����㷨����Ϊ��ҵ���ȳ���Ƶ�ʽ�С������ѡ��ȽϺ������۽ϸߵ��㷨������㷨�����ö�̬���ȼ����е���
	unsigned int estimated_time;		// Ԥ��Ҫ�����ʱ��
	unsigned int waiting_time;		// ���������ĵȴ�ʱ��
	volatile bool need_resched;		// bool value: need to be rescheduled to release CPU?
	unsigned int runs;			// һ����ҵ��������������
	struct job_queue *jq;			// backup job queue contains Job
	struct list_ele_t job_link;		// the entry linked in job queue
};

#define le2job(le, member) to_struct((le), struct job_struct, member);

void job_init(void);
struct job_struct *job_fork(struct job_queue *jq);
char *set_job_name(struct job_struct *job, const char *name);
char *get_job_name(struct job_struct *job);