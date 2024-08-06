# ifndef _SEM_H_
# define _SEM_H_
#define nameLength 10

#include<linux/sched.h>
typedef struct sem
{
    char name[nameLength];
    int value;
    struct tast_struct *arr;
    

}sem_t;
#endif
