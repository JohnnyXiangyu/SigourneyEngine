#pragma once
#include "generation_collector.h"

const int SE_TS_TaskCount = SE_TS_TASK_COUNT;

typedef struct _SE_TS_TaskNode
{
	int ChildCount;

} SE_TS_TaskNode;

typedef struct _SE_TS_TaskGraph
{
	SE_TS_TaskNode TaskNodes[SE_TS_TASK_COUNT];
} SE_TS_TaskGraph;
