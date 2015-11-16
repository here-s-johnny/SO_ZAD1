/*****************************************************************************
    Systemy Operacyjne 
    Zadanie 1.

    Autor: Jan Gajl
    Nr indeksu: 332124
    Data: 14.11.2015    


*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "err.h"

#define BUF_SIZE 6000

int main(int argc, char *argv[]) {

    if (argc != 2)
        fatal("Wrong number of arguments!!!!\n");

    int pipe_out_dsc[2], pipe_in_dsc[2];
    char pipe_in_dsc_str[10], pipe_out_dsc_str[10];
    char answer[BUF_SIZE];

    char *expression = argv[1];
    char outcome_so_far[BUF_SIZE];
    outcome_so_far[1] = '\0';
    char stack[BUF_SIZE];
    stack[1] = '\0';

    int length_of_expr = strlen(expression);
    int length_of_outcome = strlen(outcome_so_far);
    int length_of_stack = strlen(stack);

    if (length_of_expr > 0) {

        if (pipe(pipe_out_dsc) == -1)
            syserr("Error in pipe\n");
        if (pipe(pipe_in_dsc) == -1)
            syserr("Error in pipe\n");

        switch(fork()) {
            case -1:
                syserr("Error in fork\n");

            case 0:
                //closing the unused ends of both pipes
                if (close(pipe_in_dsc[1]) == -1)
                    syserr("Error in close pipe_in_dsc[1]\n");
                if (close(pipe_out_dsc[0]) == -1)
                    syserr("Error in close pipe_out_dsc[0]\n");

                sprintf(pipe_in_dsc_str, "%d", pipe_in_dsc[0]);
                sprintf(pipe_out_dsc_str, "%d", pipe_out_dsc[1]);
                execl("./W", "W(i)", pipe_in_dsc_str, pipe_out_dsc_str, (char *) 0);
                syserr("Error in execl\n");

            default:
                //closing the unused ends of both pipes
                if (close(pipe_in_dsc[0]) == -1)
                    syserr("Error in close pipe_in_dsc[0]\n");
                if (close(pipe_out_dsc[1]) == -1)
                    syserr("Error in close pipe_out_dsc[1]\n");

                // writing the 3 strings and their lengths to the pipe
                if (write(pipe_in_dsc[1], &length_of_expr, sizeof(int)) == -1)
                    syserr("Error in writing the length of expression\n");
                if (write(pipe_in_dsc[1], expression, strlen(expression)) == -1)
                    syserr("Error in writing the expression\n");

                if (write(pipe_in_dsc[1], &length_of_stack, sizeof(int)) == -1)
                    syserr("Error in writing the length of stack\n");
                if (write(pipe_in_dsc[1], stack, strlen(stack)) == -1)
                    syserr("Error in writing the stack\n");

                if (write(pipe_in_dsc[1], &length_of_outcome, sizeof(int)) == -1)
                    syserr("Error in writing the length of outcome_so_far\n");
                if (write(pipe_in_dsc[1], outcome_so_far, strlen(outcome_so_far)) == -1)
                    syserr("Error in writing the outcome_so_far\n");

                // closing pipe after writing
                if (close(pipe_in_dsc[1]) == -1)
                    syserr("Error in close (pipe_in_dsc[1]\n");

                if (wait(0) == -1)
                    syserr("Error in wait\n");

                if (read(pipe_out_dsc[0], answer, sizeof(answer)) == -1 )
                    syserr("Error in reading the answer\n");
                //closing pipe after reading
                if (close(pipe_out_dsc[0]) == -1)
                    syserr("Error in close (pipe_out_dsc[0]\n");

                printf("%s\n", answer);
                exit(0);
        }
    }
}