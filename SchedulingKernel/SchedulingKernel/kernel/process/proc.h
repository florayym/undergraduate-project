#pragma once

#include <stdlib.h>
#include <string.h>
#include "../../libs/defs.h"
#include "../../libs/list.h"

extern struct proc_struct *idleproc, *initproc, *currentproc;

// process's state in his life cycle
enum proc_state {
	PROC_UNINIT = 0,  // uninitialized
	PROC_RUNNABLE,    // runnable
	PROC_RUNNING,	  // runnning
	PROC_SUSPENDED,   // sleeping
	PROC_ZOMBIE,      // almost dead, and wait parent proc to reclaim his resource
};

// Saved registers for kernel context switches.
// Don't need to save all the %fs etc. segment registers,
// because they are constant across kernel contexts.
// Save all the regular registers so we don't need to care
// which are caller save, but not the return register %eax.
// (Not saving %eax just simplifies the switching code.)
// The layout of context must match code in switch.S.
/*
struct context {
	uint32_t eip;
	uint32_t esp;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
};*/

#define PROC_NAME_LEN               15
#define MAX_PROCESS                 4096
#define MAX_PID                     (MAX_PROCESS * 2)

static int nr_process = 0;

/* Process life cycle management */
struct proc_struct {
	enum proc_state state;                      // Process state
	int pid;                                    // Process ID
	int run_next_slice;                         // the running times of Proces
	//uintptr_t kstack;                           // Process kernel stack
	volatile bool need_resched;                 // bool value: need to be rescheduled to release CPU?
	//struct mm_struct *mm;                       // Process's memory management field
	char name[PROC_NAME_LEN + 1];               // Process name
	struct list_ele_t list_link;                // Process link list 
	struct list_ele_t hash_link;                // Process hash list
	//int exit_code;                              // exit code (be sent to parent proc)
	struct run_queue *rq;                       // running queue contains Process
	struct suspened_queue *sq;		    // suspeneded queue contains Process
	struct list_ele_t run_link;                 // the entry linked in run queue
	struct list_ele_t suspened_link;	    // the entry linked in suspended queue
	int time_slice;                             // time slice for occupying the CPU
};

#define le2proc(le, member) to_struct((le), struct proc_struct, member)

extern struct list_ele_t proc_list;

static struct proc_struct *alloc_proc(void);
char *set_proc_name(struct proc_struct *, const char *);
char *get_proc_name(struct proc_struct *);
void display_pids(char *, const char);
static int get_pid(void);
void proc_init(void);
struct proc_struct *do_fork();
struct proc_struct *do_fork();
struct proc_struct *find_proc(int);
void proc_run(struct proc_struct *);

int do_yield(void);
int do_sleep(unsigned int time);
/*
int kernel_thread(int(*fn)(void *), void *arg, uint32_t clone_flags);
void may_killed(void);
int do_exit(int error_code);
int do_exit_thread(int error_code);
int do_execve(const char *name, int argc, const char **argv);
int do_wait(int pid, int *code_store);
int do_kill(int pid, int error_code);
int do_brk(uintptr_t *brk_store);
int do_mmap(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
int do_munmap(uintptr_t addr, size_t len);
int do_shmem(uintptr_t *addr_store, size_t len, uint32_t mmap_flags);
*/