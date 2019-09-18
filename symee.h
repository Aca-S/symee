#ifndef _MISTEXPR_H_
#define _MISTEXPR_H_

typedef enum {
	OPERATOR,
	FUNCTION,
	NUMBER,
	CONSTANT,
	VARIABLE,
	LEFT_PARENTHESIS,
	RIGHT_PARENTHESIS,
	COMMA,
} tokenType;

typedef enum {
	VALID,
	INVALID_OPERATOR,
	INVALID_FUNCTION,
	INVALID_CONSTANT,
	EMPTY_EXPRESSION,
	MISMATCHED_PARENTHESES,
} expressionStatus;

typedef struct {
	expressionStatus status;
	int position;
} expressionInfo;

typedef struct {
	char code;
	double (*function)(double, double);
	char precedence;
} _operator;

typedef struct {
	char *code;
	double (*function)(double*, int);
} _function;

typedef struct {
	char *code;
	double value;
} _constant;

typedef struct {
	char *code;
	double value;
} _variable;

typedef struct {
	_operator baseData;
	char arity;
} operator;

typedef struct {
	_function baseData;
	char arity;
} function;

typedef struct {
	_constant baseData;
} constant;

typedef struct {
	_variable baseData;
} variable;

typedef union {
	double number;
	operator operator;
	function function;
	constant constant;
	variable variable;
} tokenData;

typedef struct {
	tokenType type;
	tokenData data;
} token;

int getOperatorPosition(char code);
int getFunctionPosition(char *code);
int getConstantPosition(char *code);
int getVariablePosition(char *code);
int bindVariable(_variable var);
void freeVariables();
expressionInfo tokenize(token *tokenArray, char *str);

typedef enum {
	OK,
	INSUFFICIENT_MEMORY,
	EMPTY_STACK,
} status;

typedef struct _stackNode {
	token data;
	struct _stackNode *next;
} stackNode;

stackNode *newStackNode(token data);
status push(stackNode **root, token data);
void _pop(stackNode **root);
status pop(stackNode **root, token *popped);
void freeStack(stackNode **root);

typedef struct {
	stackNode *input, *output;
} queue;

queue *createQueue();
status add(queue *root, token data);
status get(queue *root, token *popped);
void freeQueue(queue *root);

void shuntingYard(queue *outputQueue, token *tokenArray, int tokenCnt);
double getOperationResult(double first, double second, token operation);
token doOperation(stackNode **evaluationStack, token operation);
double evaluate(queue *outputQueue);
expressionInfo evaluateExpression(char *str, double *result);

#endif
