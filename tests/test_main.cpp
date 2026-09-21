// doctest supplies main() for us -- this file's only job is to trigger
// that generation exactly once. Every other test_*.cpp just #includes
// doctest.h without this macro and defines TEST_CASE blocks.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
