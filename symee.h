#ifndef _MISTEXPR_H_
#define _MISTEXPR_H_

typedef enum {
	OPERATOR,
	FUNCTION,
	NUMBER,
	CONSTANT,
	LEFT_PARENTHESIS,
	RIGHT_PARENTHESIS,
	COMMA,
} tokenType;

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

typedef union {
	double number;
	operator operator;
	function function;
	constant constant;
} tokenData;

typedef struct {
	tokenType type;
	tokenData data;
} token;

int getOperatorPosition(char code);
int getFunctionPosition(char *code);
int getConstantPosition(char *code);
int tokenize(token *tokenArray, char *str);

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
double evaluateExpression(char *str);

#endif
