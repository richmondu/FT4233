#include "TestExamples.h"
#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <unistd.h>
#endif // _WIN32



// Sample test case
bool ExampleTest1()
{
	return true;
}

// Sample test case
bool ExampleTest2()
{
#ifdef _WIN32
	Sleep(3000);
#else
	sleep(3000);
#endif

	return true;
}

// Sample test case
bool ExampleTest3()
{
#ifdef _WIN32
	Sleep(61000);
#else
	sleep(2000);
#endif

	return true;
}
