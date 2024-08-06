
#define __LIBRARY__
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/sem.h>


_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t *,sem);
_syscall1(int,sem_post,sem_t *,sem);
_syscall1(int,sem_unlink,const char *,name);
    sem_t *molecule;
    sem_t *empty ;
    sem_t *full ;
int i=0;
int count=0;

int  producer(){

    int fw;

    sem_wait(empty);
    sem_wait(molecule);
    fw= open("buffer.txt",O_RDWR|O_CREAT|O_APPEND, 0666);
    write(fw,(char*)&i,sizeof(int));
    close(fw);
    i++;
    sem_post(molecule);
    sem_post(full);
    return 0;

}

int consumer(){


    int temp,fw,fw2;

    sem_wait(full);
    sem_wait(molecule);
    fw= open("buffer.txt",O_RDWR, 0666);
    fw2=open("result.txt",O_RDWR|O_CREAT|O_APPEND,0666);
    lseek(fw, sizeof(int)*count, SEEK_SET);
    count++;
    read(fw,(char*)&temp,sizeof(int));
    write(fw2,(char*)&temp,sizeof(int));
    close(fw);
    close(fw2);
    sem_post(molecule);
    sem_post(empty);
    return 0;
    
}
 
int main(){
    full = sem_open("full",0);
    empty= sem_open("empty",10);
molecule = sem_open("molecule",1);

    if (!fork())
    {
        while (1)
        {
            if (i>500)
            {
                exit(0);
            }
            
           producer();
        }
        
        
    }

    if (!fork())
    {
         while (1)
        {
            if (count>500)
            {
                exit(0);
            }
           consumer();
        }
    }
    wait(&i);
    return 0;

} 

 
