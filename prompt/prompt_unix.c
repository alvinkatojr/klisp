#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>



#include <editline/readline.h>

int main(int argc, char** argv){

  /* Print Version and Exit Information */
  puts("Klisp Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  /* In a never ending loop */
  while (1){

    /* Output our prompt and get input */
    char* input = readline("klispy> ");

    /* Read a line of user input of maximum size 2048 */
    add_history(input);

    /* Echo input back to user */
    printf("No you're a %s\n", input);

    /* Free retrieved input */
    free(input);
  }

  return 0;
}
