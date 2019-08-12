#include "symee.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUMBER_BUFFER 64
#define FC_NAME_BUFFER 64
#define FUNCTION_ARGS 64

/*-----OPERATOR, FUNCTION AND CONSTANT TABLES-----*/
_operator operators[] = 
{
	{'+', addO, 1},
	{'-', subO, 1},
	{'*', mulO, 2},
	{'/', divO, 2}, 
	{'%', remO, 2},
	{'^', powO, 3},
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
	{"sqrt", sqrtF},
	{"abs", absF},
	{"log", logF},
	{"exp", expF},
};

_constant constants[] = 
{
	{"e", 2.71828182845904523536},
	{"pi", 3.14159265358979323846},
	{"x", 2.54},
	{"y", 3.87},
	{"z", 3},
	{"w", 7.46},
	{"a", -2},
	{"b", 3},
	{"c", -1},
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
	free(root);
}

/*-----TOKEN IMPLEMENTATION-----*/
int tokenize(token *tokenArray, char *str) {
	if(str == NULL)
		return 0;
	char functionBuffer[FC_NAME_BUFFER];
	char numberBuffer[NUMBER_BUFFER];
	
	int pos;
	int tokenCnt = 0;
	while(*str != '\0')
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
				numberBuffer[i] = *str;
				i++;
				*str++;
			}
			numberBuffer[i] = '\0';
			tokenArray[tokenCnt].type = NUMBER;
			tokenArray[tokenCnt].data.number = atof(numberBuffer);
			tokenCnt++;
			i = 0;
			continue;
		}
		/*Looking for a function call*/
		else if(isalpha(*str))
		{
			int i = 0;
			while(isalpha(*str))
			{
				functionBuffer[i] = *str;
				i++;
				*str++;
			}
			functionBuffer[i] = '\0';
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
					printf("Unknown constant %s", functionBuffer);
					return -1;
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
					printf("Unknown function %s", functionBuffer);
					return -1;
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
				case '(': tokenArray[tokenCnt].type = LEFT_PARENTHESIS; tokenCnt++; break;
				case ')': tokenArray[tokenCnt].type = RIGHT_PARENTHESIS; tokenCnt++; break;
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
						printf("Unknown uperator %c", *str);
						return -1;
					}
					break;
			}
		}
		*str++;
	}
	return tokenCnt;
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
