//#include <WinUser.h>
#include <Windows.h>
#include <stdio.h>
#include "kernel/schedule/sched.h"

/*
 * Warning C4996 Disabled
 */

void manual_operation();
//void automatic_operation();
void replay();
void update();
void file(const char *, const char *);
int display_menu(const char);

FILE *fp;

int main(void)
{
	char function;

	/* init scheduler */
	sched_init();
	update_cpu();

	printf("Please choose the way to run(1-manually, 2-automatically, 3-replay)\n");
	while ((function = getch()) == EOF);
	getch();
	if (function == '3') {
		replay();
	}
	else if ((function == '1') || (function == '2')) {
		file("out/out.log", "w+");
		if (function == '1') {
			manual_operation();
		}
		else if (function == '2') {
			//automatic_operation();
		}
		fclose(fp);
	}
	else {
		printf("Error! Exiting...\n\n");
	}
	system("pause");
	return 0;
}


void manual_operation()
{
	printf("Run manually!\nSuccessful initialization time:0\n\n");
	fprintf(fp, "Run manually!\nSuccessful initialization time:0\n\n");

	char c;
	static unsigned int t = 0;
	while (1) {
		while ((c = getch()) == EOF);
		getch();
		switch (c)
		{
		case '1':
			job_create();
			break;
		case '-':
			proc_suspend(display_menu('-'));
			break;
		case '+':
			proc_active(display_menu('+'));
			break;
		case '\x1b':
			return;
		default:
			break;
		}
		srand((int)time(0));
		update();
	}
}

/* 重放 */
void replay()
{
	char ch;
	printf("Replay!\n\n");
	file("out/out.log", "r");
	while ((ch = fgetc(fp)) != EOF) {
		printf("%c", ch);
	}
	fclose(fp);
}


int display_menu(const char op)
{
	int choosenpid;
	char *menu;
	menu = malloc(100 * sizeof *menu);
	sprintf(menu, "\
		======= Pid Menu =======");
	proc_menu(menu, op);
	strcat(menu, "\n\
		========= End =========\n");
	//update(menu);
	printf("%s", menu);
	fprintf(fp, "%s", menu);
	scanf(" %d", &choosenpid);
	fprintf(fp, "%d\n\n", choosenpid);
	free(menu);
	return choosenpid;
}


void file(const char *filename, const char *mode)
{
	fp = fopen(filename, mode);
	if (fp == NULL) {
		fprintf(stderr, "Can't open log file %s!\n", filename);
		exit(1);
	}
}


void update()
{
	int *array;
	unsigned int cpu_time;
	const char *const_Format = "\
		time %u\n\
		#ready job: %u\
		running jid : %d\n\
		#ready process(1, 2, 3, 4): %u\t%u\t%u\t%u\n\
		running pid: %d\
		current time slice left: %d\
		total time left: %d\n\
		#suspended process: %u\n\
		#blocked process: NaN\n\n";
	array = malloc(11 * sizeof *array);

	cpu_time = update_cpu();		// 时间流逝
	task_manager(array);			// 获取相关调度信息 

	printf(const_Format, cpu_time,
	       *(array + 0),  *(array + 1),  *(array + 2),  *(array + 3),  *(array + 4),
	        *(array + 5),  *(array + 6),  *(array + 7),  *(array + 8),  *(array + 9));
	fprintf(fp, const_Format, cpu_time,
		*(array + 0), *(array + 1), *(array + 2), *(array + 3), *(array + 4),
		*(array + 5), *(array + 6), *(array + 7), *(array + 8), *(array + 9));
	free(array);
	return;
}