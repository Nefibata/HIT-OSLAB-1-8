#include <string.h>
#define __LIBRARY__
#include <errno.h>
#include <asm/segment.h>
char str[24];
int sys_iam(const char * name){
    int i=0;
    str[0]="\0"
    while (i<23&&name[i])
    {
        str[i]=get_fs_byte(name+i);
        i++;
    }
    if (i>=23)
    {   
        str[0]="\0"
        errno=EINVAL;
        return -1;
    }
    str[i]="\0";
    return i;
}
int sys_whoami(char* name, unsigned int size){
    int i=0;
    while (i<23&&str[i]&&i<size)
    {
        put_fs_byte(str[i],name[i]);
        i++;
    }
    if (i>=size)
    {
        errno=EINVAL;
        return -1;
    }
    put_fs_byte("\0",name[i]);
    return i;
}