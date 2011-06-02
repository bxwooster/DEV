#define SINGLETHREADED true

#include "Tasking.hpp"

#ifdef TASK_LITE
#else
#define NOMINMAX
#include <Windows.h>
#include <process.h>
#include <unordered_map>
#include <utility>
#include <exception>
#endif


namespace Tasking {

#ifdef TASK_LITE

std::list<task_t*> __tasks;
std::list<task_t*>::iterator __task_marker;

#else

thread_local local_t* __task_local;

struct shared_t
{
	uint volatile blocked;
	cache_line _________; // literally

	bool failed;
	std::exception_ptr exception;

	std::vector<task_t*> ready;
	std::unordered_multimap<task_t*, task_t*> children;
	cache_line ________;
};

void pause(uint ticks)
{
	for (uint i = 0; i < ticks; ++i)
		_mm_pause();
}

int thread_function(shared_t* shared, local_t locals[], const uint thread_count, const uint thread_index)
{
	bool failed = false;
	std::exception_ptr exception = NULL;

	local_t* local = &locals[thread_index];
	__task_local = local;

	std::unordered_multimap<void*, task_t*> owners;
	std::unordered_map<void*, access_enum> usage;

	std::vector<local_t*> sleepers;
	sleepers.push_back( local );
	
	for (;;)
	{
		owners.clear();
		usage.clear();

		while(InterlockedCompareExchange(&shared->blocked, 1, 0))
			pause(10);

		// critical block
		if (!failed)
		{
			if (local->scheduled.empty())
			{
				auto range = shared->children.equal_range(local->current);
				for (auto it = range.first; it != range.second; ++it)
				{
					task_t* task = it->second;
					if (0 == --task->__num_parents)
						shared->ready.push_back(task);
				}
			}
			else
			{
				for (uint i = 0; i < local->scheduled.size(); ++i)
				{
					task_t* task = local->scheduled[i];
					task->__num_parents = 0;
					for (uint j = 0; j < task->num_deps; ++j)
					{
						void* dep = task->deps[j];
						access_enum required = task->flags[j];
						if (required != read_access && required != write_access)
						{
							failed = true;
						}
						bool good = (required == read_access  && usage[dep] <= read_access) ||
									(required == write_access && usage[dep] == no_access  ) ;
						if (!good)
						{
							auto range = owners.equal_range(dep);
							for (auto it = range.first; it != range.second; ++it)
							{
								task_t* owner = it->second;
								++task->__num_parents;
								shared->children.insert( std::make_pair(owner, task) );
							}
							owners.erase(range.first, range.second);
						}
						usage[dep] = required;
						owners.insert( std::make_pair(dep, task) );
					}
					if (0 == task->__num_parents)
						shared->ready.push_back(task);
				}

				auto range = shared->children.equal_range(local->current);
				for (auto it = range.first; it != range.second; ++it)
				{
					task_t* task = it->second;
					--task->__num_parents;

					for (uint j = 0; j < task->num_deps; ++j)
					{
						void* dep = task->deps[j];
						access_enum required = task->flags[j];
						bool good = (required == read_access && usage[dep] <= read_access) ||
										(required == write_access && usage[dep] == no_access);
						if (good)
						{
							if (0 == task->__num_parents)
								shared->ready.push_back(task);
						}
						else
						{
							auto range = owners.equal_range(dep);
							for (auto it = range.first; it != range.second; ++it)
							{
								task_t* owner = it->second;
								++task->__num_parents;
								shared->children.insert( std::make_pair(owner, task) );
							}
						}
					}
				}
			}
		}
		else
		{
			shared->failed = true;
			shared->exception = exception;
		}
			
		shared->children.erase(local->current);
		delete local->current;
		local->current = NULL;
		
		sleepers.resize(1); // just our thread
		for (uint i = 0; i < thread_count; i++)
		{
			if (locals[i].sleeping && i != thread_index)
				sleepers.push_back( &locals[i] );
		}

		bool all_done = ( thread_count == sleepers.size() && 0 == shared->ready.size() );
		if (shared->failed || all_done)
		{
			for (uint j = 0; j < sleepers.size(); j++)
			{
				SetEvent( sleepers[j]->wake_up );
			}
		}

		uint n = std::min( sleepers.size(), shared->ready.size() );
		for (uint j = 0; j < n; j++)
		{
			sleepers[j]->current = shared->ready.back();
			shared->ready.pop_back();
			if (j > 0)
			{
				sleepers[j]->sleeping = false;
				SetEvent( sleepers[j]->wake_up );
			}
		}

		bool sleep = local->sleeping = (!local->current);

		// critical block ends
		InterlockedExchange(&shared->blocked, 0);

		if (sleep)
		{
			//std::cout << "thread " << thread_index << " sleeping" << std::endl;
			WaitForSingleObject(local->wake_up, INFINITE);			
			if (!local->current)
				return 0; // we were null-signalled, terminate
			//std::cout << "thread " << thread_index << " waking up" << std::endl;
		}

		local->scheduled.resize(0);

		try
		{
			local->current->run();
		}
		catch(...)
		{
			failed = true;
			exception = std::current_exception();
		};
	}
}

void run_task_manager_internal(task_t* initial_task)
{
	struct context_t
	{
		shared_t* shared;
		local_t* locals;
		uint thread_count;
		uint thread_index;
		HANDLE thread_handle;

		static uint __stdcall thread_entry(void* p)
		{
			context_t& c = *static_cast<context_t*>(p);
			return thread_function(c.shared, c.locals, c.thread_count, c.thread_index);
		}
	};

    SYSTEM_INFO info = {};
    GetSystemInfo(&info);
	uint thread_count = SINGLETHREADED ? 1 : info.dwNumberOfProcessors;

	std::vector<local_t> locals (thread_count);
	std::vector<context_t> contexts (thread_count);

	shared_t shared;
	shared.blocked = 0;
	shared.failed = false;
	shared.ready.push_back(initial_task);	

	for (uint i = 0; i < thread_count; ++i)
	{
		locals[i].wake_up = CreateEvent(NULL, false, false, NULL);
		locals[i].current = NULL;
		locals[i].sleeping = false;
	}

	for (uint i = 0; i < thread_count; ++i)
    {
		contexts[i].shared = &shared;
        contexts[i].locals = &locals[0];
		contexts[i].thread_count = thread_count;
        contexts[i].thread_index = i;
		
        if (i) contexts[i].thread_handle = 
			(HANDLE)_beginthreadex(0, 0, &context_t::thread_entry, &contexts[i], 0, 0);
		Sleep(1);
    }

    context_t::thread_entry(&contexts[0]);

    for (size_t i = 1; i < thread_count; ++i)
    {
        WaitForSingleObject(contexts[i].thread_handle, INFINITE);
        CloseHandle(contexts[i].thread_handle);
    }

	if (shared.failed) std::rethrow_exception(shared.exception);
}

#endif

} // namespace Tasking
