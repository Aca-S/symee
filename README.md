# SYMEE (Shunting Yard Math Expression Evaluator)

SYMEE is a simple math expression evaluator based on Edsger Dijkstra's "Shunting Yard" algorithm which is used to convert infix expressions into postfix ones.
The evaluator currently consists of three main points:
- Tokenizer (used to split the input expression string into tokens)
- Infix to postfix converter (based on the Shunting Yard algorithm)
- Evaluator (used to calculate the final expression value)

### Features
- Supports custom operators, functions, constants
- Number of max function arguments defined in the code (set to 64 by default, can change it to whatever)
- Composite functions are supported

### Building guide
gcc example.c symee.c functions.c -lm

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
- Add the constant code and value near to the constants table near the top of symee.c

### What features I plan to add:
- Runtime addition of variables
- Possible redesign of the tokenization to notation conversion process by feeding each token directly to the infix to postfix converter instead of holding all the tokens in a special token array (could provide a significant performance boost)
