#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt){
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) (){}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#endif

/* Creates enumeration of possible error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Creates enumeration of possible lval types */
enum { LVAL_NUM, LVAL_ERR }

/* Create a new Lisp Value Struct */

typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Use the operator string to see which operation to perform */

long eval_op(long x, char* op, long y){
    if (strcmp(op, "+") == 0) { return x + y; }
    if (strcmp(op, "-") == 0) { return x - y; }
    if (strcmp(op, "*") == 0) { return x * y; }
    if (strcmp(op, "/") == 0) { return x / y; }
    return 0;
}

long eval(mpc_ast_t* t) {

  /* If tagged as number return it directly */
  if (strstr(t->tag, "number")){
    return atoi(t->contents);
  }

  /* The operator is always a second child */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  long x = eval(t->children[2]);

  /* Iterate the remaining children and combining */
  int i=3;
  while (strstr(t->children[i]->tag, "expr")){
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char** argv){

  /* New Parsers */
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  /* Define the rules for polish notation */

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                 \
      number : /-?[0-9]+/;                            \
      operator : '+' | '-' | '*' | '/';               \
      expr : <number> | '(' <operator> <expr>+ ')';   \
      lispy: /^/ <operator> <expr>+ /$/;              \
    ",
  Number, Operator, Expr, Lispy);

  /* Print Version and Exit Information */
  puts("Klisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1){

    /* Output our prompt and get input */
    char* input = readline("klispy> ");

    /* Read a line of user input of maximum size 2048 */
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* Prints the result of our evaluation */
      long result = eval(r.output);
      printf("%li\n", result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise print and delete the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    /* Free retrieved input */
    free(input);
  }

  /* Undefine and delete our parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
