#include "symee.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUMBER_BUFFER 64
#define FC_NAME_BUFFER 64
#define FUNCTION_ARGS 64
#define MAX_TOKENS 1024

/*-----OPERATOR, FUNCTION AND CONSTANT TABLES-----*/
_operator operators[] = 
{
	{'+', addO, 1},
	{'-', subO, 1},
	{'*', mulO, 2},
	{'/', divO, 2}, 
	{'%', remO, 2},
	{'^', powO, 3},
	{'>', greO, 0},
	{'<', lesO, 0},
	{'=', equO, 0},
};

_function functions[] = 
{
	{"max", maxF},
	{"min", minF},
	{"avg", avgF},
	{"sin", sinF},
	{"cos", cosF},
	{"tan", tanF},
	{"ctan", ctanF},
	{"sinh", sinhF},
	{"cosh", coshF},
	{"tanh", tanhF},
	{"coth", cothF},
	{"sqrt", sqrtF},
	{"abs", absF},
	{"ln", lnF},
	{"log", logF},
	{"exp", expF},
	{"floor", floorF},
	{"ceil", ceilF},
	{"round", roundF},
	{"trunc", truncF},
};

_constant constants[] = 
{
	{"e", 2.71828182845904523536},
	{"pi", 3.14159265358979323846},
	{"phi", 1.61803398874989484820},
};

int numOfOperators = sizeof(operators) / sizeof(_operator);
int getOperatorPosition(char code)
{
	int i;
	for(i = 0; i < numOfOperators; i++)
	{
		if(code == operators[i].code)
			return i;
	}
	return -1;
}
int numOfFunctions = sizeof(functions) / sizeof(_function);
int getFunctionPosition(char *code)
{
	int i;
	for(i = 0; i < numOfFunctions; i++)
	{
		if(strcmp(code, functions[i].code) == 0)
			return i;
	}
	return -1;
}

int numOfConstants = sizeof(constants) / sizeof(_constant);
int getConstantPosition(char *code)
{
	int i;
	for(i = 0; i < numOfConstants; i++)
	{
		if(strcmp(code, constants[i].code) == 0)
			return i;
	}
	return -1;
}

/*-----STACK IMPLEMENTATION-----*/
stackNode *newStackNode(token data) {
	stackNode *node = (stackNode*)malloc(sizeof(stackNode));
	if(node == NULL)
		return NULL;
	node->data = data;
	node->next = NULL;
	return node;
}

status push(stackNode **root, token data) {
	stackNode *node = newStackNode(data);
	if(node == NULL)
		return INSUFFICIENT_MEMORY;
	node->next = *root;
	*root = node;
	return OK;
}

void _pop(stackNode **root) {
	stackNode *temp = *root;
	*root = (*root)->next;
	free(temp);
}

status pop(stackNode **root, token *popped) {
	if(*root == NULL)
		return EMPTY_STACK;
	*popped = (*root)->data;
	_pop(root);
	return OK;
}

void freeStack(stackNode **root) {
	while(*root != NULL)
		_pop(root);
}

/*-----QUEUE IMPLEMENTATION-----*/
queue *createQueue() {
	queue *root = (queue*)malloc(sizeof(queue));
	if(root == NULL)
		return NULL;
	root->input = NULL;
	root->output = NULL;
	return root;
}

status add(queue *root, token data) {
	return push(&(root->input), data);
}

status get(queue *root, token *popped) {
	status s;
	if(root->output == NULL)
	{
		while(root->input != NULL)
		{
			token tmp;
			if((s = pop(&(root->input), &tmp)) != OK)
				return s;
			if((s = push(&(root->output), tmp)) != OK)
				return s;
		}
	}
	return pop(&(root->output), popped);;
}

void freeQueue(queue *root) {
	freeStack(&(root->input));
	freeStack(&(root->output));
	if(root != NULL)
		free(root);
}

/*-----TOKENIZATION IMPLEMENTATION-----*/
expressionInfo tokenize(token *tokenArray, char *str) {
	expressionInfo info;
	if(str == NULL)
	{
		info.status = EMPTY_EXPRESSION;
		info.position = 0;
		return info;
	}
	char functionBuffer[FC_NAME_BUFFER];
	char numberBuffer[NUMBER_BUFFER];
	int pos;
	int leftParCnt = 0, rightParCnt = 0;
	int tokenCnt = 0;
	while(*str != '\0' && tokenCnt < MAX_TOKENS)
	{
		char arityMode = 2;
		if(tokenCnt == 0 || tokenArray[tokenCnt - 1].type == OPERATOR || tokenArray[tokenCnt - 1].type == LEFT_PARENTHESIS || tokenArray[tokenCnt - 1].type == COMMA)
			arityMode = 1;
		
		/*Looking for a number*/
		if(isdigit(*str) || *str == '.')
		{
			int i = 0;
			while(isdigit(*str) || *str == '.')
			{
				if(i < NUMBER_BUFFER - 1)
				{
					numberBuffer[i] = *str;
					i++;
				}	
				*str++;
			}
			numberBuffer[i] = '\0';
			tokenArray[tokenCnt].type = NUMBER;
			tokenArray[tokenCnt].data.number = atof(numberBuffer);
			tokenCnt++;
			i = 0;
			continue;
		}
		/*Looking for a function call or constant*/
		else if(isalpha(*str))
		{
			int i = 0;
			while(isalpha(*str) || isdigit(*str))
			{
				if(i < FC_NAME_BUFFER - 1)
				{
					functionBuffer[i] = *str;
					i++;
				}	
				*str++;
			}
			functionBuffer[i] = '\0';
			/*Checking for implicit multiplication before the function/constant*/
			if(tokenCnt != 0 && (tokenArray[tokenCnt - 1].type == NUMBER || tokenArray[tokenCnt - 1].type == RIGHT_PARENTHESIS))
			{
				pos = getOperatorPosition('*');
				if(pos != -1)
				{
					tokenArray[tokenCnt].type = OPERATOR;
					tokenArray[tokenCnt].data.operator.arity = 2;
					tokenArray[tokenCnt].data.operator.baseData = operators[pos];
					tokenCnt++;
				}
				else
				{
					info.status = INVALID_OPERATOR;
					info.position = tokenCnt;
					return info;
				}
			}
			if(*str != '(')
			{
				pos = getConstantPosition(functionBuffer);
				if(pos != -1)
				{
					tokenArray[tokenCnt].type = CONSTANT;
					tokenArray[tokenCnt].data.constant.baseData = constants[pos];
				}
				else
				{
					info.status = INVALID_CONSTANT;
					info.position = tokenCnt;
					return info;
				}
			}
			else
			{
				pos = getFunctionPosition(functionBuffer);
				if(pos != -1)
				{
					tokenArray[tokenCnt].type = FUNCTION;
					tokenArray[tokenCnt].data.function.arity = 0;
					tokenArray[tokenCnt].data.function.baseData = functions[pos];
				}
				else
				{
					info.status = INVALID_FUNCTION;
					info.position = tokenCnt;
					return info;
				}
			}
			tokenCnt++;
			i = 0;
			continue;
		}
		/*Looking for an operator*/
		else
		{
			switch(*str)
			{
				case '(':
					/*Checking for implicit multiplication before the left parenthesis*/
					if(tokenCnt != 0 && (tokenArray[tokenCnt - 1].type == NUMBER || tokenArray[tokenCnt - 1].type == RIGHT_PARENTHESIS))
					{
						pos = getOperatorPosition('*');
						if(pos != -1)
						{
							tokenArray[tokenCnt].type = OPERATOR;
							tokenArray[tokenCnt].data.operator.arity = 2;
							tokenArray[tokenCnt].data.operator.baseData = operators[pos];
							tokenCnt++;
						}
						else
						{
							info.status = INVALID_OPERATOR;
							info.position = tokenCnt;
							return info;
						}
					}
					tokenArray[tokenCnt].type = LEFT_PARENTHESIS; tokenCnt++; leftParCnt++; break;
				case ')': tokenArray[tokenCnt].type = RIGHT_PARENTHESIS; tokenCnt++; rightParCnt++; break;
				case ',': tokenArray[tokenCnt].type = COMMA; tokenCnt++; break;
				case ' ': case '\n': case '\t': case '\r': break;
				default:
					pos = getOperatorPosition(*str);
					if(pos != -1)
					{
						tokenArray[tokenCnt].type = OPERATOR;
						tokenArray[tokenCnt].data.operator.arity = arityMode;
						tokenArray[tokenCnt].data.operator.baseData = operators[pos];
						tokenCnt++;
					}
					else
					{
						info.status = INVALID_OPERATOR;
						info.position = tokenCnt;
						return info;
					}
					break;
			}
		}
		*str++;
	}
	if(leftParCnt != rightParCnt)
		info.status = MISMATCHED_PARENTHESES;
	else
		info.status = VALID;
	info.position = tokenCnt;
	return info;
}

/*-----SHUNTING YARD IMPLEMENTATION-----*/
void shuntingYard(queue *outputQueue, token *tokenArray, int tokenCnt) {
	stackNode *operationStack = NULL;
	stackNode *arityStack = NULL;
	token aritySignal;
	aritySignal.type = NUMBER;
	aritySignal.data.number = 1;
	token popped;
	int i;
	for(i = 0; i < tokenCnt; i++)
	{
		if(tokenArray[i].type == NUMBER || tokenArray[i].type == CONSTANT)
		{
			if(tokenArray[i].type == NUMBER)
				add(outputQueue, tokenArray[i]);
			else
			{
				token tmp;
				tmp.type = NUMBER;
				tmp.data.number = tokenArray[i].data.constant.baseData.value;
				add(outputQueue, tmp);
			}
		}
		else if(tokenArray[i].type == LEFT_PARENTHESIS)
			push(&operationStack, tokenArray[i]);
		else if(tokenArray[i].type == RIGHT_PARENTHESIS)
		{
			while(operationStack->data.type != LEFT_PARENTHESIS)
			{
				pop(&operationStack, &popped);
				add(outputQueue, popped);
			}
			pop(&operationStack, &popped);
			if(operationStack != NULL && operationStack->data.type == FUNCTION)
			{
				token poppedAritySignal;
				pop(&operationStack, &popped);
				pop(&arityStack, &poppedAritySignal);
				popped.data.function.arity = poppedAritySignal.data.number;
				add(outputQueue, popped);
			}
		}
		else if(tokenArray[i].type == OPERATOR)
		{
			if(tokenArray[i].data.operator.arity != 1)
			{
				while(operationStack != NULL && operationStack->data.type == OPERATOR && tokenArray[i].data.operator.baseData.precedence <= operationStack->data.data.operator.baseData.precedence)
				{
					pop(&operationStack, &popped);
					add(outputQueue, popped);
				}
			}
			push(&operationStack, tokenArray[i]);
		}
		else if(tokenArray[i].type == FUNCTION)
		{
			push(&operationStack, tokenArray[i]);
			push(&arityStack, aritySignal);
		}
		else if(tokenArray[i].type == COMMA)
		{
			while(operationStack->data.type != LEFT_PARENTHESIS && operationStack->data.type != COMMA)
			{
				pop(&operationStack, &popped);
				add(outputQueue, popped);
			}
			arityStack->data.data.number++;
		}
	}
	while(pop(&operationStack, &popped) == OK)
		add(outputQueue, popped);
	freeStack(&arityStack);
}

/*-----EVALUATION IMPLEMENTATION-----*/
double evaluate(queue *outputQueue) {
	stackNode *evaluationStack = NULL;
	token popped, result;
	result.type = NUMBER;
	while(get(outputQueue, &popped) == OK)
	{
		if(popped.type == NUMBER)
			push(&evaluationStack, popped);
		else if(popped.type == OPERATOR)
		{
			token arg1, arg2;
			switch(popped.data.operator.arity)
			{
				case 1:
					pop(&evaluationStack, &arg1);
					result.data.number = popped.data.operator.baseData.function(0, arg1.data.number);
					break;
				case 2:
					pop(&evaluationStack, &arg1);
					pop(&evaluationStack, &arg2);
					result.data.number = popped.data.operator.baseData.function(arg2.data.number, arg1.data.number);
					break;
			}
			push(&evaluationStack, result);
		}
		else if(popped.type == FUNCTION)
		{
			token arg;
			double args[FUNCTION_ARGS];
			int i;
			for(i = 0; i < popped.data.function.arity; i++)
			{
				pop(&evaluationStack, &arg);
				if(i < FUNCTION_ARGS)
					args[i] = arg.data.number;
			}
			result.data.number = popped.data.function.baseData.function(args, popped.data.function.arity);
			push(&evaluationStack, result);
		}
	}
	freeQueue(outputQueue);
	pop(&evaluationStack, &result);
	freeStack(&evaluationStack);
	return result.data.number;
}

expressionInfo evaluateExpression(char *str, double *result) {
	token tokenArray[MAX_TOKENS];
	queue *outputQueue = createQueue();
	expressionInfo info = tokenize(tokenArray, str);
	if(info.status == VALID)
	{
		shuntingYard(outputQueue, tokenArray, info.position);
		*result = evaluate(outputQueue);
	}
	return info;
}
