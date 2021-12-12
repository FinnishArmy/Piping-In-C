/***********************************************************************
name: Ronny Z Valtonen
	assignment4 -- acts as a pipe using ":" to seperate programs.
description:
	See CS 360 Processes and Exec/Pipes lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


/**********************************************************************
General Goal:
Basically reproduces the pipe ( | ) parameter that is used in the
terminal. Divides between two parameters which is denoted by a colon
( : ). Runs the first arguemtn in argv, pipes it to the second argument
if it exists, then runs the second argument.

Parameters:
int argc:         The number of arguments
char *arg[]:      The string array

General Run:
If you run, ./a.out ls : sort it will take "ls" run it, then
pass that information to "sort" then sort whatever ls provided.
If you run, ./a.out : ls, it will ignore the first arg, and runs Ls.

**********************************************************************/
int main(int argc, char *argv[]);


/**********************************************************************
./assignment4 <arg1> : <arg2>

    Where: <arg1> and <arg2> are optional parameters that specify the programs
    to be run. If <arg1> is specified but <arg2> is not, then <arg1> should be
    run as though there was not a colon. Same for if <arg2> is specified but
    <arg1> is not.
**********************************************************************/
int main(int argc, char *argv[]) {
  int fd[2];


  // something bad happened, return an error
  if (pipe(fd) == -1) {
    fprintf(stdout, "%s\n", strerror(errno));
    return -1; // just return something to end the program, for now
  }

  // recognize the " : " so we can get the pipe function working correctly
  int i;
  int k = 0;
  char *command1[argc];
  char *command2[argc];

  // for loop to read UP to " : "
  for (i = 1; i < argc && strcmp(argv[i], ":") != 0; i++) {
      command1[i] = argv[i];
      // printf("%s\n", command1[1]);
  }
  // no command
  command1[k] = NULL;
  // reset k to 0
  k = 0;


  // setup a fork
  int pid1 = fork();

  // something bad happened, return an error
  if (pid1 < 0) {
    fprintf(stdout, "%s\n", strerror(errno));
    return 2; // just return something to end the program, for now
  }

// parent PROCESS
  if (pid1 == 0) {
    // pip it out
    // dup the file discriptor of std
    // dup2(fd[1], 1);
    close(0); // close stdin
    dup(fd[1]); // create a copy of the pipes' reader in stdin's former spot
    close(fd[0]); // clean up fd0, as it won't be needed to directly read anymore
    close(fd[1]); // close in
    // execlp doesn't work for cat, so use execvp, instead
    if (strcmp(argv[1], "cat") == 0){
      execvp(argv[1], argv+1);
    } else { // execute nomrally
      execlp(command1[1], command1, NULL); // use clp as we wait for the input
      fprintf(stdout, "%s\n", strerror(errno)); // print an error if not existent
    }
  }

  // setup a fork
  int pid2 = fork();

  // something bad happened
  if (pid2 < 0) {
    fprintf(stdout, "%s\n", strerror(errno));
    return 3; // just return something to end the program, for now
  }

  // for loop reading AFTER " : "
  for (i++; i < argc && strcmp(argv[i], ":") != 0; i++) {
      command2[i] = argv[i];
  }
  // no command
  command2[k] = NULL;

  if (pid2 == 0)  {
    // pipe it in
    dup2(fd[0], 0);
    close(fd[1]); // close in
    close(0); // close stdin
    // dup(fd[0]); // dup the file discriptor of std
    close(fd[0]); // close read
    // // execlp doesn't work for cat, so use execvp, instead
    if (strcmp(argv[2], "cat") == 0){
      execvp(argv[2], argv+2);
    } else { // execute normally
      execlp(command2[2], command2, NULL); // use clp as we wait for the input
      if (strcmp(argv[1], "cat") != 0) {
        fprintf(stdout, "%s\n", strerror(errno)); // print an error if not existent
      }
    }

  }
  // I believe the last tests are not passing because I'm not dupping correctly?
  // Not entirely sure; will re-visit
  // 9-29-2021


  // waits for things to finish
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  return 0;
}
