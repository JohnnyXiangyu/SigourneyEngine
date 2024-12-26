#pragma once
#include "dummy_resource_one.res.h"
#include "dummy_resource_two.res.h"

void SE_UD_DummySystemOne(const SE_UD_ResourceOne* param1, SE_UD_ResourceTwo* param2);
void SE_UD_DummySystemTwo(SE_UD_ResourceOne* param1, const SE_UD_ResourceTwo* param2);
void SE_UD_DummySystemThree(const SE_UD_ResourceOne* param1, const SE_UD_ResourceTwo* param2);
