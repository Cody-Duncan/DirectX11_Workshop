#include "pempek_assert.h"

// custom Assert statements
/*
	ASSERT - standard assert
	ASSERT_WARNING - print the assertion message to stderr
	ASSERT_DEBUG   - print the assertion message to stderr and prompt the user for action via console
	ASSERT_ERROR   - throw an AssertionException
	ASSERT_FATAL   - abort the program
	ASSERT_CUSTOM  - unused
	ASSERT_USED    - attaches to the return value of a function, warns the return from that function has gone unused.

	Severity in order from lowest to highest
	WARNING, DEBUG, ERROR, FATAL

*/
#define ASSERT          PPK_ASSERT
#define ASSERT_WARNING  PPK_ASSERT_WARNING
#define ASSERT_DEBUG    PPK_ASSERT_DEBUG
#define ASSERT_ERROR    PPK_ASSERT_ERROR
#define ASSERT_FATAL    PPK_ASSERT_FATAL
#define ASSERT_CUSTOM   PPK_ASSERT_CUSTOM
#define ASSERT_USED     PPK_ASSERT_USED
