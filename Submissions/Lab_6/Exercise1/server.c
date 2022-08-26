//server
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<signal.h>

# define MAX_SIZE 257

int up_msgq_id;
int down_msgq_id;

struct msgbuff
{
    long mtype;
    char mtext[MAX_SIZE];
};


/* reverse a message */
void reverse(char* msg, char* reversed) 
{
   int i;
   for (i=0; i<strlen(msg); ++i)
      reversed[i] = msg[strlen(msg) - i - 1];
}

void send_server(key_t msgqid,struct msgbuff message)
{
    int send_val;
    send_val = msgsnd(msgqid, &message, sizeof(message.mtext), !IPC_NOWAIT);

    if(send_val == -1)
        perror("\nServer ----> Errror in send\n");
    else
        printf("\nServer ----> Message sent: %s\n", message.mtext);
}

struct msgbuff receive_server(key_t msgqid)
{
    int rec_val;
    struct msgbuff message;

    rec_val = msgrcv(msgqid, &message, sizeof(message.mtext), 0, !IPC_NOWAIT);

    if(rec_val == -1)
        perror("\nServer ----> Error in receive");
    else
        printf("\nServer ----> Message received: %s\n", message.mtext);

    return message;
}

void handler(int signum)
{
    if(msgctl(up_msgq_id, IPC_RMID, (struct msqid_ds *) 0)==-1)
    {
        printf("\nerror in removing up queue\n");
        exit(-1);
    } 
    if(msgctl(down_msgq_id, IPC_RMID, (struct msqid_ds *) 0)==-1)
    {   
        printf("\nerror in removing down queue\n");
        exit(-1);
    }  
    printf("\nMessage Queues are removed\n");
    exit(0);
}

int main()
{
    //create message queues
    up_msgq_id= msgget(5, 0666 | IPC_CREAT);
    down_msgq_id= msgget(6, 0666 | IPC_CREAT);
   
    if (up_msgq_id== -1 || up_msgq_id== -1) 
    {
        perror("\nServer ----> Error in create\n");
        exit(-1);
    }
    printf("\nServer ----> UP Message Queue ID = %d, Down Message Queue ID = %d\n",up_msgq_id, down_msgq_id);


    signal(SIGINT, handler);

    //run server
    while(1)
    {
        printf("\nServer \n");
        //recieve from client
       struct msgbuff message= receive_server(up_msgq_id);

       //processing
       char reversed[MAX_SIZE]=" ";
       reverse(message.mtext, reversed);   
       strcpy(message.mtext,reversed);

       //send to client
       send_server(down_msgq_id, message);       
    }
    return 0;
}