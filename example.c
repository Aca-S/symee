#include "symee.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	expressionInfo expressionState;
	double res;
	expressionState = evaluateExpression("123.5 - 32pi + 2sin(pi/2) + max(phi, avg(1, e))", &res);
	if(expressionState.status == VALID)
		printf("%lf\n", res);
	else
		printf("Expression error at position %d\n", expressionState.position);
	
	return 0;
}
