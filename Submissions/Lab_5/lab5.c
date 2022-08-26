#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int exit_code;
int search_value;
int pid_child1;
int pid_child2;

void handler(int signum);

int main(int argc , char* argv[])
{
    if(argc < 3)
    {
          printf("\nPlease, Enter a value to search for and the numbers array to search in.\n");
          exit(-1);
    }
    
    search_value = atoi(argv[1]); //argc 10 argv ./file 5 1 2 3 4 5 6 7 8  
    int n = argc-2;                 //                0 1 2 3 4 5 6 7 8 9
    int* arr = (int*)malloc(n * sizeof(int));
    for(int i=0; i<n; i++)
    {
       arr[i]=atoi(argv[i+2]);
    }

    exit_code = n;//last position is n-1, if found 0:n-1, if not n 
    signal(SIGUSR1, handler);



    pid_child1=fork();
    if(pid_child1==0) //child1
    {
       printf("\nI am the first child, PID = %d, PPID = %d\n", getpid(), getppid());
       for(int i=0; i<n/2; i++) //8/2=4 0 1 2 3 , 9/2=4 0 1 2 3
       {
           if(search_value==arr[i])//found
           {
               kill(getppid(), SIGUSR1);
               exit(i);
           }          
       }
        sleep(3);//not found
        printf("\nChild 1 Terminates\n");
        exit(exit_code);
    }
    else 
    {
       pid_child2=fork();
       if(pid_child2==0) //child2
       {
          printf("\nI am the second child, PID = %d, PPID = %d\n", getpid(), getppid());
          for(int i=n/2; i<n; i++) //8/2=4 4 5 6 7 , 9/2=4 4 5 6 7 8
          {
             if(search_value==arr[i])//found
             {
               kill(getppid(), SIGUSR1);
               exit(i);
              }   
          }
       
        sleep(3);//not found
        printf("\nChild 2 Terminates\n");
        exit(exit_code);
       }
       
     else //parent
     {
        printf("\nI am the parent, PID = %d\n", getpid());

        //waits for two children
        sleep(5);
        int stat_loc1;
        int stat_loc2;
        int pid1;
        int pid2;
        pid1 = waitpid(pid_child1, &stat_loc1, 0);
        pid2 = waitpid(pid_child2, &stat_loc2, 0);

       // printf("\nA child with PID=%d terminated\n", pid1);
       //printf("\nA child with PID=%d terminated\n", pid2);

       //if not found in "both" two children
        if(((stat_loc1 >> 8) == exit_code) && ((stat_loc2 >> 8) == exit_code)) 
          printf("\nValue Not Found\n");
          exit(0);
        
     }
 
    }   
    return 0;
    }

void handler(int signum)
{
    int stat_loc;
    int pid = wait(&stat_loc);
    if(!(stat_loc & 0x00FF)) 
    {
        int code = stat_loc >> 8;

        int ch;
        if(pid == pid_child1)
           ch = 1;
        else
           ch = 2;

        if(code != exit_code)//found
        {
            printf("\nChild %d: Value %d found at position %d\n", ch, search_value, code);
            for(int i=0; i<3; i++)
            {
                killpg(getpgrp(), SIGKILL);
            }
        }
    }
    signal(SIGUSR1, handler);
}

/*Non-code question: Who will be the process executing the code in the handler?
(Parent or First Child or Second Child). How did you know?
Answer:
parent
Note: The handler should be called whenever SIGUSR1 is called.
according to this note -> when a child send SIGUSR1 to the parent then handler will be called.

Non-code question: Why is it not possible that the signal handler may not receive an exit code? 
wait return a default status in stat_loc (if child terminated by a signal) 
but we can overwrite it to know if the child terminated by a signal and return a valid position 
or not terminated by a signal and return the exit_code  

*/