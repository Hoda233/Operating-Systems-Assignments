//client 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

# define MAX_SIZE 257

struct msgbuff
{
    long mtype;
    char mtext[MAX_SIZE];
};


void send_client(key_t msgqid,struct msgbuff message)
{
    int send_val;
    send_val = msgsnd(msgqid, &message, sizeof(message.mtext), !IPC_NOWAIT);

    if(send_val == -1)
        perror("\nClient ----> Errror in send\n");
    else
        printf("\nClient ----> Message sent: %s\n", message.mtext);
}

void receive_client(key_t msgqid,long mtype)
{
    int rec_val;
    struct msgbuff message;

    rec_val = msgrcv(msgqid, &message, sizeof(message.mtext), mtype, !IPC_NOWAIT);

    if(rec_val == -1)
        perror("\nClient ----> Error in receive");
    else
        printf("\nClient ----> Message received: %s\n", message.mtext);

}
int main()
{
    //create message queues
    int up_msgq_id= msgget(5, 0666 | IPC_CREAT);
    int down_msgq_id= msgget(6, 0666 | IPC_CREAT);
   
    if (up_msgq_id== -1 || up_msgq_id== -1) 
    {
        perror("\nClient ----> Error in create\n");
        exit(-1);
    }
    printf("\nClient ----> UP Message Queue ID = %d, Down Message Queue ID = %d\n",up_msgq_id, down_msgq_id);

    //run client
    while(1)
    {
        printf("\nClient \n");
        struct msgbuff message;
        //take message from input
        printf("\nEnter your message (MAX 256 character):\n");
        int i=0;
        char ch;
        char text[MAX_SIZE];
        for(i=0; i<256; i++)
        {
            ch=getchar();
            if(ch==10)//Enter key pressed
              break;
            else
               text[i]=ch;          
        }
        text[i]='\0';

        //set msgtext from input message
        strcpy(message.mtext,text);
        message.mtext[i]='\0';
        //set type from pid
        message.mtype=getpid()%10000; //9999%10000=9999, 99998%10000=9998
        //send msg to server
        send_client(up_msgq_id,message);
        //receive from server
        receive_client(down_msgq_id,message.mtype);
    }
    return 0;
}