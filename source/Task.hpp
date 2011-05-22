#pragma once

#define RunTask(Task, ...) \
{ \
	Task task = { __VA_ARGS__ }; \
	task.run(); \
}

#define In(Type) Type const &
#define InOut(Type) Type &
#define Out(Type) Type *