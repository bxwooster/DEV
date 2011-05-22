#pragma once

#define RunTask(Task, ...) \
{ \
	Task task = { __VA_ARGS__ }; \
	task.run(); \
}

#define In(Type) Type &
#define InOut(Type) Type &