#include "symee.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUMBER_BUFFER 64
#define FCV_NAME_BUFFER 64
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

_variable *variables = NULL;

int numOfOperators = sizeof(operators) / sizeof(_operator);
int getOperatorPosition(char code) {
	int i;
	for(i = 0; i < numOfOperators; i++)
	{
		if(code == operators[i].code)
			return i;
	}
	return -1;
}

int numOfFunctions = sizeof(functions) / sizeof(_function);
int getFunctionPosition(char *code) {
	int i;
	for(i = 0; i < numOfFunctions; i++)
	{
		if(strcmp(code, functions[i].code) == 0)
			return i;
	}
	return -1;
}

int numOfConstants = sizeof(constants) / sizeof(_constant);
int getConstantPosition(char *code) {
	int i;
	for(i = 0; i < numOfConstants; i++)
	{
		if(strcmp(code, constants[i].code) == 0)
			return i;
	}
	return -1;
}

int numOfVariables = 0;
int bindVariable(_variable var) {
	numOfVariables++;
	variables = realloc(variables, numOfVariables * sizeof(_variable));
	if(variables == NULL)
		return -1;
	variables[numOfVariables - 1] = var;
	return 0;
}

void freeVariables() {
	if(variables != NULL)
	{
		free(variables);
		variables = NULL;
		numOfVariables = 0;
	}
}

int getVariablePosition(char *code) {
	int i;
	for(i = 0; i < numOfVariables; i++)
	{
		if(strcmp(code, variables[i].code) == 0)
			return i;
	}
	return -1;
}

/*-----GLOBAL STATUS INDICATOR-----*/
expressionInfo exprInfo;

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
int tokenize(token *tokenArray, char *str) {
	if(str == NULL)
	{
		exprInfo.status = EMPTY_EXPRESSION;
		exprInfo.realPosition = 0;
		exprInfo.tokenPosition = 0;
		return -1;
	}
	char nameBuffer[FCV_NAME_BUFFER];
	char numberBuffer[NUMBER_BUFFER];
	int pos;
	int leftParCnt = 0, rightParCnt = 0;
	int charCnt = 0, tokenCnt = 0;
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
				charCnt++;
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
				if(i < FCV_NAME_BUFFER - 1)
				{
					nameBuffer[i] = *str;
					i++;
				}
				*str++;
				charCnt++;
			}
			nameBuffer[i] = '\0';
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
					exprInfo.status = INVALID_OPERATOR;
					exprInfo.realPosition = charCnt;
					exprInfo.tokenPosition = tokenCnt;
					return -1;
				}
			}
			if(*str != '(')
			{
				pos = getConstantPosition(nameBuffer);
				if(pos != -1)
				{
					tokenArray[tokenCnt].type = CONSTANT;
					tokenArray[tokenCnt].data.constant.baseData = constants[pos];
				}
				else
				{
					/*If it wasn't found in the constants table, check if it's been bound as a variable*/
					pos = getVariablePosition(nameBuffer);
					if(pos != -1)
					{
						tokenArray[tokenCnt].type = VARIABLE;
						tokenArray[tokenCnt].data.variable.baseData = variables[pos];
					}
					else
					{
						exprInfo.status = INVALID_CONSTANT;
						exprInfo.realPosition = charCnt;
						exprInfo.tokenPosition = tokenCnt;
						return -1;
					}
				}
			}
			else
			{
				pos = getFunctionPosition(nameBuffer);
				if(pos != -1)
				{
					tokenArray[tokenCnt].type = FUNCTION;
					tokenArray[tokenCnt].data.function.arity = 0;
					tokenArray[tokenCnt].data.function.baseData = functions[pos];
				}
				else
				{
					exprInfo.status = INVALID_FUNCTION;
					exprInfo.realPosition = charCnt;
					exprInfo.tokenPosition = tokenCnt;
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
							exprInfo.status = INVALID_OPERATOR;
							exprInfo.realPosition = charCnt;
							exprInfo.tokenPosition = tokenCnt;
							return -1;
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
						exprInfo.status = INVALID_OPERATOR;
						exprInfo.realPosition = charCnt;
						exprInfo.tokenPosition = tokenCnt;
						return -1;
					}
					break;
			}
		}
		*str++;
		charCnt++;
	}
	if(leftParCnt != rightParCnt)
	{
		exprInfo.status = MISMATCHED_PARENTHESES;
		exprInfo.realPosition = charCnt;
		exprInfo.tokenPosition = tokenCnt;
		return -1;
	}
	exprInfo.status = VALID;
	exprInfo.realPosition = charCnt;
	exprInfo.tokenPosition = tokenCnt;
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
		if(tokenArray[i].type == NUMBER || tokenArray[i].type == CONSTANT || tokenArray[i].type == VARIABLE)
		{
			if(tokenArray[i].type == NUMBER)
				add(outputQueue, tokenArray[i]);
			else
			{
				token tmp;
				tmp.type = NUMBER;
				if(tokenArray[i].type == CONSTANT)
					tmp.data.number = tokenArray[i].data.constant.baseData.value;
				else
					tmp.data.number = tokenArray[i].data.variable.baseData.value;
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

int evaluateExpression(char *str, double *result) {
	token tokenArray[MAX_TOKENS];
	queue *outputQueue = createQueue();
	int tokenCnt = tokenize(tokenArray, str);
	if(tokenCnt == -1)
		return -1;
	shuntingYard(outputQueue, tokenArray, tokenCnt);
	*result = evaluate(outputQueue);
	return 0;
}
