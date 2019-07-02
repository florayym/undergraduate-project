#include "sched.h"
#include "sched_MLFQ.h"
#include "sched_HRRN.h"
#include "sched_FCFS.h"


static inline void sched_class_enqueue(struct proc_struct *proc, struct job_struct *job, enum sched_level level)
{
	if (level == LONG_TERM_SCHED) {
		job_sched_class->enqueue(jq, job);
	}
	else if (level == LOW_LEVEL_SCHED) {
		proc_sched_class->enqueue(rq, proc);
	}
	else if (level == INTERMEDIATE_LEVEL_SCHED) {
		intermediate_sched_class->enqueue(sq, proc);
	}
}


static inline void sched_class_dequeue(struct proc_struct *proc, struct job_struct *job, enum sched_level level)
{
	if (level == LONG_TERM_SCHED) {
		job_sched_class->dequeue(jq, job);
	}
	else if (level == LOW_LEVEL_SCHED) {
		proc_sched_class->dequeue(rq, proc);
	}
	else if (level == INTERMEDIATE_LEVEL_SCHED) {
		intermediate_sched_class->dequeue(sq, proc);
	}
}


static inline struct proc_struct *sched_class_pick_next(enum sched_level level)
{
	if (level == LONG_TERM_SCHED) {
		return job_sched_class->pick_next(jq);
	}
	else if (level == LOW_LEVEL_SCHED) {
		return proc_sched_class->pick_next(rq);
	}
	else if (level == INTERMEDIATE_LEVEL_SCHED) {
		return intermediate_sched_class->pick_next(sq);
	}

}


static void sched_class_time_tick(struct proc_struct *proc, struct job_struct *job, enum sched_level level)
{
	if (level == LONG_TERM_SCHED) {
		job_sched_class->time_tick(jq, job);
	}
	else if (level = LOW_LEVEL_SCHED) {
		if (proc != idleproc) {
			proc_sched_class->time_tick(rq, proc);
		}
		else {
			proc->need_resched = 1;
		}
	}
}


static struct run_queue __rq[4];
static struct job_queue __jq[1];
static struct suspended_queue __sq[1];


void sched_init(void)
{
	cpu_timer = malloc(sizeof(struct cpu_timer_t));
	if (cpu_timer == NULL) {
		exit(EXIT_FAILURE);
	}
	cpu_timer->normal_running = 0;

	jq = __jq;
	sq = __sq;

	rq = __rq;
	list_init(&rq->rq_link);
	rq->max_time_slice = 2;// 8
	int i;
	for (i = 1; i < sizeof(__rq) / sizeof(__rq[0]); i++) {
		list_add_before(&(rq->rq_link), &(__rq[i].rq_link));
		__rq[i].max_time_slice = rq->max_time_slice * (1 << i);
	}
	/* bind low_level_schedule method class */
	proc_sched_class = &MLFQ_sched_class;
	proc_sched_class->init(rq);

	job_sched_class = &HRRN_sched_class;
	job_sched_class->init(jq);

	intermediate_sched_class = &FCFS_sched_class;
	intermediate_sched_class->init(sq);

	proc_init();			// init process table(list) & idleproc
	job_init();			// init job table(list) & idlejob
}


void job_create(void)
{
	struct job_struct *job;
	if ((job = job_fork(jq)) != NULL) {
		sched_class_enqueue(NULL, job, LONG_TERM_SCHED);
	}
}


void proc_suspend(const int pid)
{
	struct proc_struct *proc;
	if (((proc = find_proc(pid)) == NULL) || (proc->state != PROC_RUNNABLE)) {
		return;
	}
	sched_class_dequeue(proc, NULL, LOW_LEVEL_SCHED);
	sched_class_enqueue(proc, NULL, INTERMEDIATE_LEVEL_SCHED);
	proc->state = PROC_SUSPENDED;
}


void proc_active(const int pid)
{
	struct proc_struct *proc;
	if (((proc = find_proc(pid)) == NULL) || (proc->state != PROC_SUSPENDED)) {
		return;
	}
	sched_class_dequeue(proc, NULL, INTERMEDIATE_LEVEL_SCHED);
	sched_class_enqueue(proc, NULL, LOW_LEVEL_SCHED);
	proc->state = PROC_RUNNABLE;
}


void high_level_schedule(void)
{
	struct job_struct *next;
	/*
	 * 高级调度的pick_next要实现基于优先级的下一个查找！！！
	 * 而不是直接取队列中的next！！！
	 * 因为优先级每个钟头都在变化，而其在队列中的位置并不随之而改变，
	 * 所以调度的时候麻烦些，要一一比较，但这样平时就不用总是动位置了，每个周期都比较，开销太大！
	 */
	if ((next = sched_class_pick_next(LONG_TERM_SCHED)) != NULL) {
		sched_class_dequeue(NULL, next, LONG_TERM_SCHED);
		if (currentjob != idlejob) {
			currentjob->state = JOB_COMPLETE;
			currentjob->need_resched = 0;
		}
	}
	else {
		next = idlejob;
	}
	currentjob = next;
	if (currentjob != idlejob) {
		currentjob->state = JOB_RUNNING;
		int i;
		for (i = 0; i < currentjob->runs; i++) {
			struct proc_struct *proc;
			if ((proc = do_fork(rq)) != NULL) {
				sched_class_enqueue(proc, NULL, LOW_LEVEL_SCHED);
			}
		}
	}
}


void low_level_schedule(void)
{
	struct proc_struct *next;
	currentproc->need_resched = 0;
	
	/* For current process */
	if (currentproc != idleproc) {
		if (currentproc->run_next_slice != 0) {
			/* Not idle, not done， enqueue MLFQ(ready process queue) */
			currentproc->state = PROC_RUNNABLE;
			sched_class_enqueue(currentproc, NULL, LOW_LEVEL_SCHED);
		}
		else {
			/* Done and not idle */
			currentproc->state = PROC_ZOMBIE;
		}
	}
	
	/* To decide next process */
	if ((next = sched_class_pick_next(LOW_LEVEL_SCHED)) == NULL) {
		/* 没有其它实在存在的进程在MLFQ队列中 */
		next = idleproc;
		next->need_resched = 1;
		/* Job need to resched too!!! */
		currentjob->need_resched = 1;
	}
	else {
		/* 选择MLFQ中的下一个进程跑，所以出队 */
		next->state = PROC_RUNNING;
		sched_class_dequeue(next, NULL, LOW_LEVEL_SCHED);
	}
	
	if (next != currentproc) {
		proc_run(next);
	}
}


/* 更新在跑的，以及动态优先级的作业后备队列(作业是非抢占式的） */
unsigned int update_cpu()
{	
	/* job timer because it is dynamic priority */
	struct list_ele_t *list = &(jq->job_list), *le = list;
	struct job_struct *job;
	while ((le = list_next(le)) != list) {	// 大于1个才更新
		job = le2job(le, job_link);
		sched_class_time_tick(NULL, job, LONG_TERM_SCHED);
	}
	/* currentproc proc timer update */
	sched_class_time_tick(currentproc, NULL, LOW_LEVEL_SCHED);
	/* update schedule on different scales if needed */
	/* judge first */
	if (!currentjob->need_resched && currentproc->need_resched) {
		low_level_schedule();
	}
	if (currentjob->need_resched) {
		high_level_schedule();
	}
	if (!currentjob->need_resched && currentproc->need_resched) {
		low_level_schedule();
	}
	/* cpu timer update */
	return cpu_timer->normal_running++;
}


void proc_menu(char *display, const char op)
{
	return display_pids(display, op);
}


void task_manager(int *array)
{
	int i = 0;
	struct run_queue *nrq;
	struct list_ele_t *list = &(rq->rq_link), *le = list;
	 *(array + i++) = jq->job_num;
	 *(array + i++) = currentjob->jid;
	do {
		nrq = le2rq(le, rq_link);
		 *(array + i++) = nrq->proc_num;
	} while ((le = list_next(le)) != list);
	 *(array + i++) = currentproc->pid;
	 *(array + i++) = currentproc->time_slice;
	 *(array + i++) = currentproc->run_next_slice + currentproc->time_slice;
	 *(array + i++) = sq->suspended_num;
	//strcat(buffer, "\n#blocked process: ");
	// *(array + i++) = 
	//printf("%d\n", );以及被blocked之后剩余的时间
	//strcat(buffer, "\n\n");
}