#pragma once
#include <stddef.h>
#include <vector>

#define thread_local __declspec(thread)

namespace Tasking
{

typedef char cache_line[64];
typedef unsigned int uint;
typedef void* HANDLE;

enum access_enum { no_access, read_access, write_access };
enum end_marker_enum { end_mark };

struct task_t
{
	virtual void run() = 0;

	(void*)* deps;
	const access_enum* flags;
	uint num_deps;

	uint __num_parents; // internal
};

struct local_t
{
    HANDLE wake_up;

	bool sleeping;
	task_t* current;
	std::vector<task_t*> scheduled;
	cache_line __________;
};

template<typename specific_task_t>
struct task_wrapper_t : task_t
{
	specific_task_t specific;

	task_wrapper_t(specific_task_t& task) :
		specific(task)
	{
		num_deps = offsetof(specific_task_t, __end__) / sizeof(void*);
		flags = &specific_task_t::__dp_end - num_deps;
		deps = (void**)&specific;
	}
	
	void run()
	{
		specific.run();
	}
};

extern thread_local local_t* __task_local;

void run_task_manager_internal(task_t* initial_task);

template<typename specific_task_t>
void run_taskmanager(specific_task_t specific)
{
	task_t* task = new task_wrapper_t<specific_task_t>(specific);
	run_task_manager_internal(task);
}

} // namespace Task

#define _CONC(a, b) __CONC(a, b)
#define __CONC(a, b) a##b
#define DO_NOT_OPTIMIZE_AWAY __declspec(dllexport)

#define RunTaskObject(specific_task_t, object) \
{\
	specific_task_t __task = object; \
	__task.run(); \
}

#define ScheduleTaskObject(specific_task_t, object) \
{\
	specific_task_t __task = object; \
	Tasking::task_t* __ptr = new Tasking::task_wrapper_t<specific_task_t>(__task); \
	Tasking::__task_local->scheduled.push_back(__ptr); \
}

#define RunTask(specific_task_t, ...) RunTaskObject(specific_task_t, { __VA_ARGS__ } );
#define ScheduleTask(specific_task_t, ...) ScheduleTaskObject(specific_task_t, { __VA_ARGS__ } );

#define In(Type) \
	DO_NOT_OPTIMIZE_AWAY static const Tasking::access_enum _CONC(__dp, __LINE__) = Tasking::read_access; \
	Type const&

#define InOut(Type) \
	DO_NOT_OPTIMIZE_AWAY static const Tasking::access_enum _CONC(__dp, __LINE__) = Tasking::write_access; \
	Type &

#define Out(Type) \
	DO_NOT_OPTIMIZE_AWAY static const Tasking::access_enum _CONC(__dp, __LINE__) = Tasking::write_access; \
	Type *

#define __End__ \
	static const Tasking::access_enum __dp_end = Tasking::no_access; \
	Tasking::end_marker_enum __end__;