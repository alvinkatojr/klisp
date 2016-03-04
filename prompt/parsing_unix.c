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
#include <editline/history.h>
#endif

int main(int argc, char** argv){

  /* New Parsers */
  mpc_paraser_t* Number = mpc_new("number");
  mpc_paraser_t* Operator = mpc_new("operator");
  mpc_paraser_t* Expr = mpc_new("expr");
  mpc_paraser_t* Lispy = mpc_new("lispy");

  /* Define the rules for polish notation */

  mpca_lang(MPCA_LANG_DEFAULT,
  "
    number : /-?[0-9]+/;
    operator : '+' | '-' | '*' | '/'
    expr : <number> | '(' <operator> <expr>+ ')';
    lispy: /^/ <operator> <expr>+ /$/;
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

    /* Free retrieved input */
    free(input);
  }

  /* Undefine and delete our parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
