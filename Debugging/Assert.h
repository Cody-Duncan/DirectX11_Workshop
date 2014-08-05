#include "pempek_assert.h"

// custom prefix
#define ASSERT          PPK_ASSERT
#define ASSERT_WARNING  PPK_ASSERT_WARNING
#define ASSERT_DEBUG    PPK_ASSERT_DEBUG
#define ASSERT_ERROR    PPK_ASSERT_ERROR
#define ASSERT_FATAL    PPK_ASSERT_FATAL
#define ASSERT_CUSTOM   PPK_ASSERT_CUSTOM
#define ASSERT_USED     PPK_ASSERT_USED



PPK_ASSERT_ACTION_TYPE AssertHandler(
	const char* file,
	int line,
	const char* function,
	const char* expression,
	int level,
	const char* message
);

void SetAssertionHanlders();