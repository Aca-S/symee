#include "symee.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
	expressionInfo expressionState;
	double res;
	expressionState = evaluateExpression(argv[1], &res);
	if(expressionState.status == VALID)
		printf("%lf\n", res);
	else
		printf("Expression error at position %d\n", expressionState.position);
	
	return 0;
}
