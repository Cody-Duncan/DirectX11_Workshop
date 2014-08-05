#include "Assert.h"

PPK_ASSERT_ACTION_TYPE AssertHandler(
	const char* file,
	int line,
	const char* function,
	const char* expression,
	int level,
	const char* message)
{
	//run default handler
	static bool _ignore = false;
	//pempek::assert::implementation::handleAssert(file, line, function, expression, level, _ignore, message);

	// do custom handling stuff

	return PPK_ASSERT_ACTION_BREAK;
}

void SetAssertionHanlders()
{
	//pempek::assert::implementation::setAssertHandler(AssertHandler);
}