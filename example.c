#include "symee.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *str = "5+sin(3*pi/2)";
	token tokenArray[1024];
	queue *outputQueue = createQueue();
	int tokenCnt = tokenize(tokenArray, str);
	if(tokenCnt != -1)
	{
		shuntingYard(outputQueue, tokenArray, tokenCnt);
		printf("%lf\n", evaluate(outputQueue));
	}

	return 0;
}

/*TODO	- implement rest of math.h functions
	- implement safety checks for functions using buffers to make sure they don't overflow
	- implement better error handling*/
