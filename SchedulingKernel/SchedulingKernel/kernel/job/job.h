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
	double prio;				// 使用高响应比优先的作业调度算法，因为作业调度出现频率较小，可以选择比较合理但代价较高的算法，如该算法，采用动态优先级进行调度
	unsigned int estimated_time;		// 预计要求服务时间
	unsigned int waiting_time;		// 不断增长的等待时间
	volatile bool need_resched;		// bool value: need to be rescheduled to release CPU?
	unsigned int runs;			// 一项作业所包含的任务数
	struct job_queue *jq;			// backup job queue contains Job
	struct list_ele_t job_link;		// the entry linked in job queue
};

#define le2job(le, member) to_struct((le), struct job_struct, member);

void job_init(void);
struct job_struct *job_fork(struct job_queue *jq);
char *set_job_name(struct job_struct *job, const char *name);
char *get_job_name(struct job_struct *job);