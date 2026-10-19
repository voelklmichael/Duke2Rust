// PORT: reimplementation of the task manager (TASK_MAN in MACT386.LIB and
// AUDIO_WF.LIB). On DOS, tasks ran from the reprogrammed timer interrupt.
// Here each task runs on its own thread at its rate, asynchronously to the
// game, as the interrupt did (Duke uses one task: totalclock at 120 Hz).
#include <pthread.h>
#include <time.h>
#include "compat.h"
#include "task_man.h"

volatile int TS_InInterrupt = 0;

typedef struct { task t; pthread_t thread; volatile int quit; } tasknode;

static void *TS_TaskThread(void *arg)
{
	tasknode *n = (tasknode *)arg;
	struct timespec next;

	clock_gettime(CLOCK_MONOTONIC, &next);
	while (!n->quit)
	{
		long period = 1000000000L / (n->t.rate > 0 ? n->t.rate : 1);
		next.tv_nsec += period;
		while (next.tv_nsec >= 1000000000L) { next.tv_nsec -= 1000000000L; next.tv_sec++; }
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		if (n->t.active && !n->quit)
		{
			TS_InInterrupt = 1;
			n->t.TaskService(&n->t);
			TS_InInterrupt = 0;
		}
	}
	return NULL;
}

#define MAXTASKS 16
static task *scheduled[MAXTASKS];

	// The task does not run until TS_Dispatch.
task *TS_ScheduleTask(void (*Function)(task *), int rate, int priority, void *data)
{
	tasknode *n = (tasknode *)calloc(1, sizeof(tasknode));
	int i;
	n->t.TaskService = Function;
	n->t.data = data;
	n->t.rate = rate;
	n->t.priority = priority;
	n->t.active = 0;
	for (i = 0; i < MAXTASKS; i++) if (!scheduled[i]) { scheduled[i] = &n->t; break; }
	pthread_create(&n->thread, NULL, TS_TaskThread, n);
	return &n->t;
}

	// Starts all scheduled tasks.
void TS_Dispatch(void)
{
	int i;
	for (i = 0; i < MAXTASKS; i++) if (scheduled[i]) scheduled[i]->active = 1;
}

int TS_Terminate(task *ptr)
{
	tasknode *n = (tasknode *)ptr;   // t is the first member
	int i;
	for (i = 0; i < MAXTASKS; i++) if (scheduled[i] == ptr) scheduled[i] = NULL;
	n->quit = 1;
	pthread_join(n->thread, NULL);
	free(n);
	return TASK_Ok;
}

void TS_Shutdown(void) { }

void TS_SetTaskRate(task *Task, int rate) { Task->rate = rate; }
