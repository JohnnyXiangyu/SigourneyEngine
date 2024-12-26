#include "thread_pool.h"
#include <stdio.h>

static void ThreadWorkRoutine(void* payload)
{
	int data = (int)(intptr_t)payload;
	printf("work: %d\n", data);
}

int main()
{
	printf("main begin\n");

	SE_TP_ThreadPool pool = SE_TP_CreateThreadPool(10);
	SE_TP_InitializeWorkers(&pool, 2);

	const int totalWorkAmount = 10;

	// TODO: higher directive, make the basic task node code gen first

	// TODO: the next step is to make the generic thread pool a scheduling-aware scheduler, to which point it becomes the scheduler
	// i.e. scheduler is thread pool
	// after that, we ditch the current thread pool design, maybe save it for something else
	// something to consider: maybe we should implement self-populating tasks and optimize it for the general purpose version before ditching

	for (int trial = 0; trial < 2; trial++)
	{
		for (int i = 0; i < totalWorkAmount; i++)
		{
			SE_TP_PostThreadWork(&pool, &ThreadWorkRoutine, (void*)(intptr_t)i, i);
		}

		int finishedCount = 0;
		while (finishedCount < totalWorkAmount)
		{
			SE_TP_WhenAny(&pool);
			finishedCount++;
		}
	}

	SE_TP_FinalizeThreadPool(&pool);

	printf("main end\n");
	return 0;
}
