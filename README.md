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
- add the function definition which the operator will use to functions.c (function takes in two double values and returns a double)
- add the function declaration to functions.h
- add the operator code, function name and operator precedence value to the operator table near the top of symee.c

**For functions:**
- add the function definition which the operator will use to functions.c (function takes in an array of double values and the number of arguments (int) and returns a double)
- add the function declaration to functions.h
- add the function code and function name to the functions table near the top of symee.c

**For constants:**
- add the constant code and value near to the constants table near the top of symee.c

### What features I plan to add:
- runtime addition of variables
- addition of the rest of math header functions and more custom ones
- more robust detection of invalid expressions
- possible redesign of the tokenization to notation conversion process by feeding each token directly to the infix to postfix converter instead of holding all the tokens in a special token array (could provide a significant performance boost)
- simple wrapper function for the entire process which just takes an expression string as input and returns the result
