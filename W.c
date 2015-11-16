#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "err.h"

#define BUF_SIZE 6000

void to_ONP(char expr[BUF_SIZE], char outcome_so_far[BUF_SIZE],
            char stack[BUF_SIZE]) {
// function performing one step of convertion, updating the 3 strings piped down

    int len_o, len_s;
    char c = expr[0];
    if (c >= 'a' && c <= 'z') {
        len_o = strlen(outcome_so_far);
        outcome_so_far[len_o] = c;
        outcome_so_far[len_o + 1] = ' ';
        outcome_so_far[len_o + 2] = '\0';
    } else if (c >= '0' && c <= '9') { 
        len_o = strlen(outcome_so_far);
        if (!(expr[1] >= '0' && expr[1] <= '9')) {
            outcome_so_far[len_o] = c;
            outcome_so_far[len_o + 1] = ' ';
            outcome_so_far[len_o + 2] = '\0';
        } else {
            outcome_so_far[len_o] = c;
        }
    } else if (c == ' ') {
    } else if (c == '+' || c == '-') {
        len_o = strlen(outcome_so_far);
        len_s = strlen(stack);
        while (stack[len_s - 1] == '+' || stack[len_s -1] == '-' ||
                stack[len_s - 1] == '*' || stack[len_s - 1] == '/'
                || stack[len_s - 1] == '^' ) {
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
            outcome_so_far[len_o] = stack[len_s-1];
            stack[len_s-1] = '\0';
            outcome_so_far[len_o + 1] = ' ';
            outcome_so_far[len_o + 2] = '\0';
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
            }
            len_s = strlen(stack);
            stack[len_s] = c;
            stack[len_s + 1] = '\0';
    } else if (c == '*' || c == '/') {
        len_o = strlen(outcome_so_far);
        len_s = strlen(stack);
        while (stack[len_s - 1] == '*' || stack[len_s - 1] == '/'
                || stack[len_s - 1] == '^') {
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
            outcome_so_far[len_o] = stack[len_s-1];
            stack[len_s-1] = '\0';
            outcome_so_far[len_o + 1] = ' ';
            outcome_so_far[len_o + 2] = '\0';
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
        }
        len_s = strlen(stack);
        stack[len_s] = c;
        stack[len_s + 1] = '\0'; 
    } else if (c == '^') {
        len_o = strlen(outcome_so_far);
        len_s = strlen(stack);
        while (stack[len_s - 1] == '^') {
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
            outcome_so_far[len_o] = stack[len_s-1];
            stack[len_s-1] = '\0';
            outcome_so_far[len_o + 1] = ' ';
            outcome_so_far[len_o + 2] = '\0';
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
        }
        len_s = strlen(stack);
        stack[len_s] = c;
        stack[len_s + 1] = '\0'; 

    } else if (c == '(') {
        len_s = strlen(stack);
        stack[len_s] = c;
        stack[len_s + 1] = '\0';
    } else if (c == ')') {
        len_s = strlen(stack);
        while (stack[len_s - 1] != '(') {
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
            outcome_so_far[len_o] = stack[len_s-1];
            stack[len_s-1] = '\0';
            outcome_so_far[len_o + 1] = ' ';
            outcome_so_far[len_o + 2] = '\0';
            len_o = strlen(outcome_so_far);
            len_s = strlen(stack);
        }
        stack[len_s - 1] = '\0';
    } else {}
}

int main(int argc, char *argv[]) {

    int pipe_out_dsc[2], pipe_in_dsc[2];

    int in_dsc, out_dsc;
    char pipe_in_dsc_str[10], pipe_out_dsc_str[10];
    char expr[BUF_SIZE], outcome_so_far[BUF_SIZE], stack[BUF_SIZE];
    char answer[BUF_SIZE];
    int length_of_expr, length_of_outcome, length_of_stack;

    if (argc != 3)
        fatal("Usage: %s <read_fd> (W(i))\n", argv[0]);

    in_dsc = atoi(argv[1]);
    out_dsc = atoi(argv[2]);

    // reading the 3 strings and their sizes from pipe
    if (read(in_dsc, &length_of_expr, sizeof(int)) == -1)
        syserr("Error in reading length of expr (W(i))\n");
    if (read(in_dsc, expr, length_of_expr) == -1)
        syserr("Error in reading expr (W(i))\n");

    if (read(in_dsc, &length_of_stack, sizeof(int)) == -1)
        syserr("Error in reading length of stack (W(i))\n");
    if (read(in_dsc, stack, length_of_stack) == -1)
        syserr("Error in reading stack (W(i))\n");

    if (read(in_dsc, &length_of_outcome, sizeof(int)) == -1)
        syserr("Error in reading length of outcome (W(i))\n");
    if (read(in_dsc, outcome_so_far, length_of_outcome) == -1)
        syserr("Error in reading outcome (W(i))\n");

    // closing the pipe after reading
    if (close(in_dsc) == -1)
        syserr("Error in closing in_dsc (W(i))\n");
    expr[length_of_expr] = '\0';
    stack[length_of_stack] = '\0';
    outcome_so_far[length_of_outcome] = '\0';

    to_ONP(expr,outcome_so_far,stack);
    // printf("%s : %s\n", outcome_so_far, stack); /////// USUNAC TO!!!!

    length_of_expr --;
    length_of_stack = strlen(stack);
    length_of_outcome = strlen(outcome_so_far);

    if (length_of_expr > 0) {
        if (pipe(pipe_out_dsc) == -1)
            syserr("Error in pipe (W(i))\n");
        if (pipe(pipe_in_dsc) == -1)
            syserr("Error in pipe (W(i))\n");

        switch (fork()) {
            case -1:
                syserr("Error in fork (W(i))\n");

            case 0:
                //closing the unused ends of both pipes
                if (close(pipe_in_dsc[1]) == -1)
                    syserr("Error in close pipe_in_dsc[1] (W(i))\n");
                if (close(pipe_out_dsc[0]) == -1)
                    syserr("Error in close pipe_out_dsc[0] (W(i))\n");

                sprintf(pipe_in_dsc_str, "%d", pipe_in_dsc[0]);
                sprintf(pipe_out_dsc_str, "%d", pipe_out_dsc[1]);
                execl("./W", "W(i)", pipe_in_dsc_str, pipe_out_dsc_str, (char *) 0);
                syserr("Error in execl W(i)\n");

            default:
                //closing the unused ends of both pipes
                if (close(pipe_in_dsc[0]) == -1)
                    syserr("Error in close pipe_in_dsc[0] (W(i))\n");
                if (close(pipe_out_dsc[1]) == -1)
                    syserr("Error in close pipe_out_dsc[1] (W(i))\n");

                // writing the 3 strings and their lengths to pipe
                if (write(pipe_in_dsc[1], &length_of_expr, sizeof(int)) == -1)
                    syserr("Error in writing the length of expr (W(i))\n");
                if (write(pipe_in_dsc[1], expr + 1, length_of_expr) == -1)
                    syserr("Error in writing the expr (W(i))\n");

                if (write(pipe_in_dsc[1], &length_of_stack, sizeof(int)) == -1)
                    syserr("Error in writing the length of stack (W(i))\n");
                if (write(pipe_in_dsc[1], stack, length_of_stack) == -1)
                    syserr("Error in writing the stack (W(i))\n");
                
                if (write(pipe_in_dsc[1], &length_of_outcome, sizeof(int)) == -1)
                    syserr("Error in writing the length of outcome_so_far (W(i))\n");
                if (write(pipe_in_dsc[1], outcome_so_far, length_of_outcome) == -1)
                    syserr("Error in writing the outcome_so_far (W(i))\n");

                // closing pipe after writing
                if (close(pipe_in_dsc[1]) == -1)
                    syserr("Error in close (pipe_in_dsc[1] (W(i))\n");

                // waiting for son to finish working        
                if (wait(0) == -1)
                    syserr("Error in wait (W(i))\n");

                if (read(pipe_out_dsc[0], answer, sizeof(answer)) == -1)
                    syserr("Error in reading the answer (W(i))\n");
                if (close(pipe_out_dsc[0]) == -1)
                    syserr("Error in close (pipe_out_dsc[0] (W(i))\n");

                if (write(out_dsc, answer, sizeof(answer)) == -1)
                    syserr("Error in writing the answer (W(i))\n");
                if (close(out_dsc) == -1)
                    syserr("Error in close (out_dsc[0] (W(i))\n");


                exit(0);
        }
    } else {
        length_of_stack = strlen(stack);
        length_of_outcome = strlen(outcome_so_far);
        while (length_of_stack > 0) {
            outcome_so_far[length_of_outcome] = stack[length_of_stack - 1];
            stack[length_of_stack - 1] = '\0';
            outcome_so_far[length_of_outcome + 1] = ' ';
            outcome_so_far[length_of_outcome + 2] = '\0';
            length_of_stack = strlen(stack);
            length_of_outcome = strlen(outcome_so_far);
        }
        // printf("%s\n", outcome_so_far);

        if (write(pipe_out_dsc[1], outcome_so_far, sizeof(answer)) == -1)
            syserr("Error in writing the answer from the bottom (W(i))\n");
        if (close(pipe_out_dsc[1]) == -1)
            syserr("Error in close (pipe_out_dsc[1] (W(i))\n");


        exit(0);
    }
}