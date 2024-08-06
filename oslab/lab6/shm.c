#include <linux/mm.h>        
#include <unistd.h>     
#include <errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#define shmNum 10

typedef unsigned int key_t;
typedef struct shm
{
    unsigned int size;
    unsigned int key;
    unsigned long page;
}shmT;
shmT arr [ shmNum]={{0,0,0}};

int sys_shmget(key_t key, size_t size, int shmflg){
    int i,page;
    if (size>PAGE_SIZE)
    {
       errno=EINVAL;
       return -1;
    }
    for ( i = 0; i < shmNum; i++)
    {
        if (arr[i].key==key)
        {
            return arr[i].page;
        }
        
    }
    page=get_free_page();
    if (page==0)
    {
        errno=EINVAL;
        return -1;
    }
    for ( i = 0; i < shmNum; i++)
    {
        if (arr[i].key==0)
        {
            arr[i].key=key;
            arr[i].page=page;
            arr[i].size=size;
            return i;
        }
        
    }
    return 0;
}
void * sys_shmat(int shmid, const void *shmaddr, int shmflg){
    if (shmid>shmNum)
    {
        errno=EINVAL;
        return -1;
    }
    
    int base,temp;
    base=get_base(current->ldt[1]);
    temp=base+current->brk;
    put_page(arr[shmid].page,temp);
    current->brk+=PAGE_SIZE;
    return (void*)current->brk;
}


