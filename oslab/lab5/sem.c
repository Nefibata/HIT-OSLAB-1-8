#include <unistd.h> /* NULL */
#include <linux/sem.h> /* sem_t */
#include <asm/segment.h> /* get_fs_byte */
#include <asm/system.h> /* cli, sti */


#define nameLength 10
#define sem_tMaxleng 10
sem_t sem_tArr[sem_tMaxleng]={
	{"\0",0,NULL}, {"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL},
    {"\0",0,NULL}, {"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL},{"\0",0,NULL}
};

int arrLength=0;

int my_strcmp( char* str1,  char* str2)
{
	
	while (*str1 == *str2)
	{
		if (*str1 == '\0')
			return 1;//相等
		str1++;
		str2++;
	}
    return 0;
}
void my_strcp( char* str1,  char* str2)
{
    int i;
	
	for ( i = 0; i < nameLength; i++)
    {
        str2[i]=str1[i];
    }
    return;
    
}



sem_t* sys_sem_open(const char *name, unsigned int value){
    int i;
    char* temp[nameLength];
    sem_t *re;
    for ( i = 0; i < nameLength; i++)
    {
        temp[i]=get_fs_byte(name+i);
    }
    for ( i = 0; i < arrLength; i++)
    {
        if(my_strcmp(temp,sem_tArr[i].name)){
            re=&sem_tArr[i];
            return re;
        }
    }
    i=arrLength;
    sem_tArr[i].value=value;
    sem_tArr[i].arr=NULL;
    my_strcp(temp,sem_tArr[i].name);
    arrLength++;
    re=&sem_tArr[i];
    return re;
}
int sys_sem_wait(sem_t *sem){
    if(sem == NULL){return-1;}
    cli();
    while (sem->value<=0)
    {
       sleep_on(&(sem->arr));
    }
    sem->value=sem->value-1;
    sti();
    return 0;
}
int sys_sem_post(sem_t *sem){
    if(sem == NULL){return-1;}
    cli();
    sem->value=sem->value+1;
    if (sem->value>0)
    {
        wake_up(&(sem->arr));
    }
    sti();
    return 0;
}
int sys_sem_unlink(const char *name){
    int i;
    char temp[nameLength];
    for ( i = 0; i < nameLength; i++)
    {
        temp[i]=get_fs_byte(name+i);
    }

    for ( i = 0; i < arrLength; i++)
    {
        if(my_strcmp(temp,sem_tArr[i].name)){

            break;
        }
    }
    for ( ; i+1 < sem_tMaxleng; i++)
    {
        sem_tArr[i]=sem_tArr[i+1];
    }
    arrLength--;
    return 0;
}
