#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define MAX_SIZE 257
union Semun semun,semun3;
int shmid, sem1, sem2,sem3;

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}


int main()
{
    
    key_t key_id = 65;
    shmid = shmget(key_id, MAX_SIZE*sizeof(char), IPC_CREAT | 0666);
    sem1 = semget(66, 1, 0666 | IPC_CREAT);
    sem2 = semget(67, 1, 0666 | IPC_CREAT);
    sem3 = semget(70, 1, 0666 | IPC_CREAT);
    

    if (shmid == -1 || sem1 == -1 || sem2 == -1 || sem3==-1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
    {
        printf("\nShared memory ID = %d\n", shmid);
    }

    semun.val=0;
    
    if (semctl(sem1, 0, SETVAL, semun) == -1 ||semctl(sem2, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    int *shmaddr = shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in writer");
        exit(-1);
    }

    while(1)
    {
        
    down(sem3);  //semaphore to prevent multiple clients enter 
    printf("\nClient\n");
    printf("\nEnter your message:\n");
    int i=0;
    char ch;
    char text[MAX_SIZE]=" ";
    for(i=0; i<256; i++)
    {
        ch=getchar();
        if(ch==10)//enter key pressed
            break;
        else
            text[i]=ch;          
    }
    text[i]='\0';

    //client write message in shared memory
    strcpy((char *)shmaddr, text);
    printf("\nClient write: %s\n", (char *)shmaddr);
    up(sem1); //up the server to read message

    down(sem2); //down the client to write
    printf("\nClient read: %s\n", (char *)shmaddr);

    up(sem3); //client end critical section
    }
    return 0;
}
