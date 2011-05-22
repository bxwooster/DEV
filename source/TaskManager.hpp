#pragma once

#define Run(Task, ...) \
{ \
	Task task = { __VA_ARGS__ }; \
	task.run(); \
}