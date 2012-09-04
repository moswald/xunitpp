#include "xUnit++.h"

//
// FilteredTestsRunner has a tendency to not get linked into the test dlls.
// This is the simplest valid test library possible, and is intended to ensure
// this error does not happen in the future.
//
FACT(BareTest)
{
}
