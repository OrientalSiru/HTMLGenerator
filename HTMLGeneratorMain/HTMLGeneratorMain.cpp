// HTMLGeneratorMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HTMLGenerator.h"
#include <time.h>

int main()
{
	clock_t begin = clock();
	HTMLGenerator HTMLGen(1);
	clock_t end = clock();

	printf("elapsed = %d\n", end - begin);

    return 0;
}

