/*
* shelly interface program

KUSIS ID: 40836 NAME: ÖZGE DEMİR
KUSIS ID: 	NAME: BERRİN ÖNSİPER

*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define MAX_LINE       80 /* 80 chars per line, per command, should be enough. */

int parseCommand(char inputBuffer[], char *args[],int *background, int *redirection, char *rdarg[], int *argct);

void execute(char *args[]);

int main(void)
{
      char inputBuffer[MAX_LINE]; 	        /* buffer to hold the command entered */
      int background; /* equals 1 if a command is followed by '&' */
      int redirection; /*equals 0 if no redirection , 1 if >, 2 if >> */
      char *rdarg[MAX_LINE];
      char *args[MAX_LINE/2 + 1];	        /* command line (of 80) has max of 40 arguments */
      pid_t child;            		/* process id of the child process */
      int status;           		/* result from execv system call*/
      int shouldrun = 1;
      int argct;

      char path[MAX_LINE+5];
      char path2[MAX_LINE+9];
      strcpy(path, "/bin/");
      strcpy(path2, "/usr/bin/");
      int i, upper;
      int j=0;
      while (shouldrun){            		/* Program terminates normally inside setup */
            background = 0;

            shouldrun = parseCommand(inputBuffer,args,&background, &redirection, rdarg, &argct);       /* get next command */

            if (strncmp(inputBuffer, "exit", 4) == 0)
            shouldrun = 0;     /* Exiting from myshell*/

            if (shouldrun) {
                  /*
                  After reading user input, the steps are
                  (1) Fork a child process using fork()
                  (2) the child process will invoke execv()
                  (3) if command included &, parent will invoke wait()
                  */

                  child = fork();
                  if(child == 0)
                  {
                        strcpy(path+5, args[0]);
                        strcpy(path2+9, args[0]);
                        shouldrun = 0;
                        printf("Args\n");
                        for( j=0; j<argct; j++)
                        {
                              printf("%s\n", args[j]);
                        }
                        printf("RD Mode %d, RD Arg:%s\n", redirection, *rdarg);


                        if(execv(path, args) == -1)
                        {
                              if(execv(path2, args) == -1)
                              {
                                    printf("ERROR: command not found \n");
                              }
                        }


                  }else if(child < 0)
                  {
                        printf("Child could not be found\n");
                  }else{
                        // background
                        waitpid(child, &status, 0);
                  }

            }
      }
      return 0;
}

/**
* The parseCommand function below will not return any value, but it will just: read
* in the next command line; separate it into distinct arguments (using blanks as
* delimiters), and set the args array entries to point to the beginning of what
* will become null-terminated, C-style strings.
*/

int parseCommand(char inputBuffer[], char *args[],int *background, int *redirection, char *rdarg[], int *ctt)
{
      int length,		/* # of characters in the command line */
      i,		/* loop index for accessing inputBuffer array */
      start,		/* index where beginning of next command parameter is */
      ct,	        /* index of where to place the next parameter into args[] */
      command_number;	/* index of requested command number */

      ct = 0;
      int mystart = -1;
      int rdCount = 0;
      *redirection = 0;
      int llength = 0;
      int ignore = 0;
      int abb  = 0;

      /* read what the user enters on the command line */
      do {
            printf("shelly>");
            fflush(stdout);
            length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
      }
      while (inputBuffer[0] == '\n'); /* swallow newline characters */

      /**
      *  0 is the system predefined file descriptor for stdin (standard input),
      *  which is the user's screen in this case. inputBuffer by itself is the
      *  same as &inputBuffer[0], i.e. the starting address of where to store
      *  the command that is read, and length holds the number of characters
      *  read in. inputBuffer is not a null terminated C-string.
      */
      start = -1;

      if (length == 0)
      exit(0);            /* ^d was entered, end of user command stream */

      /**
      * the <control><d> signal interrupted the read system call
      * if the process is in the read() system call, read returns -1
      * However, if this occurs, errno is set to EINTR. We can check this  value
      * and disregard the -1 value
      */

      if ( (length < 0) && (errno != EINTR) ) {
            perror("error reading the command");
            exit(-1);           /* terminate with error code of -1 */
      }

      /**
      * Parse the contents of inputBuffer
      */

      for (i=0;i<length;i++) {
            /* examine every character in the inputBuffer */

            switch (inputBuffer[i]){
                  case ' ':
                  case '\t' :               /* argument separators */
                  if (rdCount < 1)
                  {
                        if(start != -1){
                              args[ct] = &inputBuffer[start];    /* set up pointer */
                              ct++;
                        }
                        inputBuffer[i] = '\0'; /* add a null char; make a C string */
                        start = -1;
                  }
                  break;

                  case '\n':                 /* should be the final char examined */
                  if (rdCount < 1)
                  {
                        if (start != -1){
                              args[ct] = &inputBuffer[start];
                              ct++;
                        }
                        inputBuffer[i] = '\0';
                        args[ct] = NULL; /* no more arguments to this command */
                  }
                  break;

                  default :             /* some other character */
                  if (rdCount < 1)
                  {
                        if (start == -1)
                        start = i;
                        if (inputBuffer[i] == '&') {
                              *background  = 1;
                              inputBuffer[i-1] = '\0';
                        }
                  }
                  if(rdCount == 0)
                  {
                        if (inputBuffer[i] == '>')
                        {

                              rdCount = 1;
                              *redirection = 1;
                        }
                  } else if(rdCount == 1)
                  {

                        if (inputBuffer[i] == '>' && !abb)
                        {
                              *redirection = 2;
                              rdCount = 2;
                              ignore = 1;
                        } else if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t')
                        {
                              rdCount = 1;
                              ignore = 1;
                              abb = 1;
                        }else
                        {
                              rdCount = 2;
                              ignore = 0;
                        }

                  }
                  if (rdCount == 2 )
                  {
                        if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t' || inputBuffer[i] == '\n')
                        {
                              if (mystart != -1)
                              {
                                    inputBuffer[llength] = '\0';
                                    printf("End: %d\n", llength);
                                    llength++;
                                    rdCount = -1;
                              }
                        } else
                        {
                              if (mystart == -1)
                              {
                                    mystart = i;
                                    printf("Mystart: %d\n",mystart);
                              } else //mystart != -1
                              {

                              }
                              llength++;
                        }
                  }
                  if (ignore)
                  {
                        if(rdCount == 2 && (inputBuffer[i] == ' ' || inputBuffer[i] == '\t'))
                        {

                        } else
                        {
                              ignore = 0;
                        }
                  }
                  // if (rdCount == 3)
                  // {
                  //       if (inputBuffer[i] == ' ' || inputBuffer[i] == '\t')
                  //       {
                  //             *rdarg[llength] = '\0';
                  //             llength++;
                  //             rdCount = -1;
                  //       }
                  // }
                  // if (rdCount == 2 && !ignore)
                  // {
                  //       *rdarg[llength] = inputBuffer[i];
                  //       llength++;
                  //       //rdCount = 3;
                  // }
                  // if(ignore) ignore = 0;
                  break;


            } /* end of switch */
      }    /* end of for */

      /**
      * If we get &, don't enter it in the args array
      */
      if (mystart != -1)
      {
            *rdarg = &inputBuffer[mystart];
            printf("rdarg %s\n", *rdarg);
      }else
      {
            printf("mystart = -1");
      }
      if (*background)
      args[--ct] = NULL;

      args[ct] = NULL; /* just in case the input line was > 80 */

      *ctt = ct;
      printf("Llength: %d\n", llength);
      printf("rdCount: %d\n", rdCount);

      return 1;

} /* end of parseCommand routine */
