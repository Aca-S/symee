#include "symee.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *str = argv[1];
	printf("%s = %lf\n", str, evaluateExpression(str));

	return 0;
}

/* TODO - implement safety checks for functions and constants using buffers to make sure they don't overflow
		- implement implicit multiplication handling
		- implement better error handling */
