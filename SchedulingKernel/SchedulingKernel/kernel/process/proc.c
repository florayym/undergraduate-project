#include "proc.h"
#include "../schedule/sched.h"

// the process set's list
struct list_ele_t proc_list;

#define HASH_SHIFT          10
#define HASH_LIST_SIZE      (1 << HASH_SHIFT)
#define pid_hashfn(x)       (hash32(x, HASH_SHIFT))

// has list for process set based on pid
static struct list_ele_t hash_list[HASH_LIST_SIZE];

// idle proc
struct proc_struct *idleproc = NULL;
// init proc
//struct proc_struct *initproc = NULL;
// currentproc proc
struct proc_struct *currentproc = NULL;

//void switch_to(struct context *from, struct context *to);

// alloc_proc - alloc a proc_struct and init all fields of proc_struct
static struct proc_struct *alloc_proc(void)
{
	struct proc_struct *proc = malloc(sizeof(struct proc_struct));
	if (proc != NULL) {
		proc->state = PROC_UNINIT;
		proc->pid = -1;
		//srand((int)time(0));
		proc->run_next_slice = rand() % 5 + 3;
		//proc->kstack = 0;
		proc->need_resched = 0;
		//proc->mm = NULL;
		memset(proc->name, 0, PROC_NAME_LEN);
		proc->list_link.next = NULL;
		proc->list_link.prev = NULL;
		proc->run_link.next = NULL;
		proc->run_link.prev = NULL;
		proc->rq = NULL;
		proc->sq = NULL;
		proc->suspened_link.next = NULL;
		proc->suspened_link.prev = NULL;
		proc->time_slice = 0;
	}
	return proc;
}


// set_proc_name - set the name of proc
char *set_proc_name(struct proc_struct *proc, const char *name)
{
	memset(proc->name, 0, sizeof(proc->name));
	return memcpy(proc->name, name, PROC_NAME_LEN);
}


// get_proc_name - get the name of proc
char *get_proc_name(struct proc_struct *proc)
{
	static char name[PROC_NAME_LEN + 1];
	memset(name, 0, sizeof(name));
	return memcpy(name, proc->name, PROC_NAME_LEN);
}


void display_pids(char *display, const char op)
{
	char temp[10];
	struct proc_struct *proc;
	struct list_ele_t *list = &proc_list, *le = list;
	while ((le = list_next(le)) != list) {
		proc = le2proc(le, list_link);
		if ((op == '-') && (proc->state == PROC_RUNNABLE)) {
			itoa(proc->pid, temp, 10);
			strcat(display, "\n\t\t");
			strcat(display, temp);
		}
		else if ((op == '+') && (proc->state == PROC_SUSPENDED)) {
			itoa(proc->pid, temp, 10);
			strcat(display, "\n\t\t");
			strcat(display, temp);
		}
	}
}


// set_links - set the relation links of process
static void set_links(struct proc_struct *proc)
{
	list_add(&proc_list, &(proc->list_link));
}

// remove_links - clean the relation links of process
static void remove_links(struct proc_struct *proc)
{
	list_del(&(proc->list_link));
}


// get_pid - alloc a unique pid for process
static int get_pid(void) {
	struct proc_struct *proc;
	struct list_ele_t *list = &proc_list, *le;
	static int next_safe = MAX_PID, last_pid = MAX_PID;
	if (++last_pid >= MAX_PID) {
		last_pid = 1;
		goto inside;
	}
	if (last_pid >= next_safe) {
	inside:
		next_safe = MAX_PID;
	repeat:
		le = list;
		while ((le = list_next(le)) != list) {
			proc = le2proc(le, list_link);
			if (proc->pid == last_pid) {
				if (++last_pid >= next_safe) {
					if (last_pid >= MAX_PID) {
						last_pid = 1;
					}
					next_safe = MAX_PID;
					goto repeat;
				}
			}
			else if (proc->pid > last_pid && next_safe > proc->pid) {
				next_safe = proc->pid;
			}
		}
	}
	return last_pid;
}


// proc_run - make process "proc" running on cpu
// NOTE: before call switch_to, should load  base addr of "proc"'s new PDT
void proc_run(struct proc_struct *proc)
{
	struct proc_struct *prev = currentproc, *next = proc;
	currentproc = proc;
	/* 上下文切换 */
	//switch_to(&(prev->context), &(next->context));
}


// hash_proc - add proc into proc hash_list
static void hash_proc(struct proc_struct *proc)
{
	list_add(hash_list + pid_hashfn(proc->pid), &(proc->hash_link));
}


// unhash_proc - delete proc from proc hash_list
static void unhash_proc(struct proc_struct *proc)
{
	list_del(&(proc->hash_link));
}


// find_proc - find proc frome proc hash_list according to pid
struct proc_struct *find_proc(int pid) {
	if (0 < pid && pid < MAX_PID) {
		struct list_ele_t *list = hash_list + pid_hashfn(pid), *le = list;
		while ((le = list_next(le)) != list) {
			struct proc_struct *proc = le2proc(le, hash_link);
			if (proc->pid == pid) {
				return proc;
			}
		}
	}
	return NULL;
}


/* do_yield - ask the scheduler to reschedule */
int do_yield(void)
{
	currentproc->need_resched = 1;
	return 0;
}


// do_fork - parent process for a new child process
struct proc_struct *do_fork()
{
	struct proc_struct *proc;

	if ((proc = alloc_proc()) == NULL) {
		return NULL;
	}
	proc->pid = get_pid();
	hash_proc(proc);
	proc->state = PROC_RUNNABLE;
	set_proc_name(proc, "proc");
	set_links(proc);
	return proc;
}


/*
 * proc_init - set up the first kernel thread idleproc "idle" by itself and 
 *           - create the second kernel thread init_main
 */
void proc_init(void) 
{
	int i;

	list_init(&proc_list);
	for (i = 0; i < HASH_LIST_SIZE; i++) {
		list_init(hash_list + i);
	}

	if ((idleproc = alloc_proc()) == NULL) {
		exit(EXIT_FAILURE);
	}

	idleproc->pid = 0;
	idleproc->run_next_slice = 0;
	idleproc->state = PROC_RUNNABLE;
	idleproc->need_resched = 1;	// 可以抢占
	set_proc_name(idleproc, "idle");
	currentproc = idleproc;
}