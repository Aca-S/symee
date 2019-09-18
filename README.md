# SYMEE (Shunting Yard Math Expression Evaluator)

SYMEE is a simple math expression evaluator based on Edsger Dijkstra's "Shunting Yard" algorithm which is used to convert infix expressions into postfix ones.

### Features
- Supports custom operators, functions, constants
- Runtime binding of variables
- Number of max function arguments defined in the code (set to 64 by default, can change it to whatever)
- Composite functions are supported

### Building guide
gcc example.c symee.c functions.c -lm

### Code structure
The entire library is contained in four files, which are:
- symee.c
- symee.h
- functions.c
- functions.h

The symee (symee.c, symee.h) files contain the main part of the code (the evaluator itself) while the function (functions.c, functions.h) files implement custom operator and function definitions and declarations.

The evaluator consists of three main parts:
- Tokenizer (used to split the input expression string into tokens)
- Infix to postfix converter (based on the Shunting Yard algorithm)
- Evaluator (used to calculate the final expression value)

The process is summarized with the following diagram:

![process diagram](https://i.ibb.co/wQG5rTF/symee.png)

It is also important to know the token structure itself:

![token structure](https://i.ibb.co/bd3SMyP/symeetkn.png)

The structures _operator, _function and _constant contain only the base data of the appropriate token type, data which we know based on the token itself (such as precedence of operators) and not the tokens around it (such as arity).

### How to add custom operators/functions/constants?
**For operators:**
- Add the function definition which the operator will use to functions.c (function takes in two double values and returns a double)
- Add the function declaration to functions.h
- Add the operator code, function name and operator precedence value to the operator table near the top of symee.c

**For functions:**
- Add the function definition which the operator will use to functions.c (function takes in an array of double values and the number of arguments (int) and returns a double)
- Add the function declaration to functions.h
- Add the function code and function name to the functions table near the top of symee.c

**For constants:**
- Add the constant code and value to the constants table near the top of symee.c

### What features I plan to add:
- Possible redesign of the tokenization to notation conversion process by feeding each token directly to the infix to postfix converter instead of holding all the tokens in a special token array (could provide a significant performance boost)
