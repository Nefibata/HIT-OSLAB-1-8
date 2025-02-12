
#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h>

int  main(){
    int i=0;
    sem_t *molecule;
    sem_t *empty;
    sem_t *full;
    key_t k1=1;
    int shmid;
    int * speac;
    molecule=sem_open("molecule",O_CREAT,1);
    full=sem_open("full",O_CREAT,0);
    empty=sem_open("empty",O_CREAT,10);
    shmid=shmget(k1,10*sizeof(int),0666|IPC_CREAT);
    while (i<500)
    {
    sem_wait(empty);
    sem_wait(molecule);
    speac = (int *)shmat(shmid,0,0);
    speac[full->__align]=i;
    sem_post(molecule);
    sem_post(full);
    i++;
    }
    return 0;

}
