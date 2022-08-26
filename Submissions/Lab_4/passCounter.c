#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc , char* argv[])
{
    //arguments from command
     char *file = argv[1];
     int N = atoi(argv[2]);
     int pass_grade = atoi(argv[3]);

    //file open and read S
    FILE *fptr;
    fptr = fopen(file,"r");
    int S=0;
    char temp[10];
    fgets(temp,10,fptr);
    S=atoi(temp);

    //read students' grades
    int* grades = (int*)malloc(S * sizeof(int));
    int i = 0;
    int midterm;
    int final;
    for(int i=0; i<S; i++)
    {
        fgets(temp,10,fptr);
        sscanf(temp,"%d %d", &midterm, &final);
        grades[i] = midterm + final;
    }

     //create N processes 
    int group = S / N; //11/3 = 3 , group=3
    int pid;
    int* results=(int*)malloc(N * sizeof(int));
    int* PIDs=(int*)malloc(N * sizeof(int));
    for(int i=0; i<N; i++)
    {
        pid = fork();
    
        if(pid == 0) //child
        {
            int start = i*group; //0*3=0
            int end;
            if(i==N-1)
              end = S;
            else
               end = start + group;
    
            int passed_students = 0;
            for(int j = start; j < end; j++) 
            {
                if(grades[j] >= pass_grade) 
                   passed_students++;
            }  
          exit(passed_students);           
        }
        else 
        {
            PIDs[i] = pid;
        }
    }  

    //make parent wait for its children
    int stat_loc;
    for(int i=0; i<N; i++)
    { 
        pid = waitpid(PIDs[i], &stat_loc, 0);
        if(!(stat_loc & 0x00FF))
           results[i] = stat_loc>>8;
        else
           results[i]=0;               
    }
    //print results
    for(int i=0; i<N; i++)
        printf("%d ", results[i]);
    printf("\n");
}