/*
 *  linux/fs/read_write.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <stdarg.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>


extern int rw_char(int rw,int dev, char * buf, int count, off_t * pos);
extern int read_pipe(struct m_inode * inode, char * buf, int count);
extern int write_pipe(struct m_inode * inode, char * buf, int count);
extern int block_read(int dev, off_t * pos, char * buf, int count);
extern int block_write(int dev, off_t * pos, char * buf, int count);
extern int file_read(struct m_inode * inode, struct file * filp,
		char * buf, int count);
extern int file_write(struct m_inode * inode, struct file * filp,
		char * buf, int count);

int sys_lseek(unsigned int fd,off_t offset, int origin)
{
	struct file * file;
	int tmp;

	if (fd >= NR_OPEN || !(file=current->filp[fd]) || !(file->f_inode)
	   || !IS_SEEKABLE(MAJOR(file->f_inode->i_dev)))
		return -EBADF;
	if (file->f_inode->i_pipe)
		return -ESPIPE;
	switch (origin) {
		case 0:
			if (offset<0) return -EINVAL;
			file->f_pos=offset;
			break;
		case 1:
			if (file->f_pos+offset<0) return -EINVAL;
			file->f_pos += offset;
			break;
		case 2:
			if ((tmp=file->f_inode->i_size+offset) < 0)
				return -EINVAL;
			file->f_pos = tmp;
			break;
		default:
			return -EINVAL;
	}
	return file->f_pos;
}

int sys_read(unsigned int fd,char * buf,int count)
{
	struct file * file;
	struct m_inode * inode;

	if (fd>=NR_OPEN || count<0 || !(file=current->filp[fd]))
		return -EINVAL;
	if (!count)
		return 0;
	verify_area(buf,count);
	inode = file->f_inode;
	if (inode->i_pipe)
		return (file->f_mode&1)?read_pipe(inode,buf,count):-EIO;
	if (S_ISCHR(inode->i_mode))
		return rw_char(READ,inode->i_zone[0],buf,count,&file->f_pos);
	if (S_ISBLK(inode->i_mode))
		return block_read(inode->i_zone[0],&file->f_pos,buf,count);
	if (S_ISDIR(inode->i_mode) || S_ISREG(inode->i_mode)) {
		if (count+file->f_pos > inode->i_size)
			count = inode->i_size - file->f_pos;
		if (count<=0)
			return 0;
		return file_read(inode,file,buf,count);
	}
	if (S_ISPROC(inode->i_mode))
	{
		return read_proc(inode->i_zone[0],buf,count,&file->f_pos);//读proc
	}
	
	printk("(Read)inode->i_mode=%06o\n\r",inode->i_mode);
	return -EINVAL;
}

int sys_write(unsigned int fd,char * buf,int count)
{
	struct file * file;
	struct m_inode * inode;
	
	if (fd>=NR_OPEN || count <0 || !(file=current->filp[fd]))
		return -EINVAL;
	if (!count)
		return 0;
	inode=file->f_inode;
	if (inode->i_pipe)
		return (file->f_mode&2)?write_pipe(inode,buf,count):-EIO;
	if (S_ISCHR(inode->i_mode))
		return rw_char(WRITE,inode->i_zone[0],buf,count,&file->f_pos);
	if (S_ISBLK(inode->i_mode))
		return block_write(inode->i_zone[0],&file->f_pos,buf,count);
	if (S_ISREG(inode->i_mode))
		return file_write(inode,file,buf,count);
	printk("(Write)inode->i_mode=%06o\n\r",inode->i_mode);
	return -EINVAL;
}
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args; int i;
    va_start(args, fmt);
    i=vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}
int read_proc(int i_zone,char* buf,int count,off_t * pos)
{
	struct task_struct ** p;
	char* str=NULL;
	int size=0;
	int output_count=0;
	int output_size=0;
	if (i_zone==0)
	{
		str=(char *)malloc(sizeof(char)*1024);
		size=sprintf(str,"pid state father counter start_time\n");
		for (p = &LAST_TASK ; p >= &FIRST_TASK ; --p)
		{
			if (*p)
			{
				size+=sprintf(str+size,"%d\t%d\t%d\t%d\t%d\n",(*p)->pid,(*p)->state,(*p)->father,(*p)->counter,(*p)->start_time);
			}
			
		}
		*(str+size)='\0';
	}
	//处理 硬盘文件，这个我就直接copy了
	struct super_block * sb; 
    struct buffer_head * bh;
    int total_blocks, total_inodes;
    int used_blocks=0, free_blocks=0;
    int i,j,k;
    char * db=NULL;
    unsigned short s_imap_blocks;
    unsigned short s_zmap_blocks;
    
	//硬盘总共有多少块(空闲 + 非空闲)，有多少inode索引节点等信息都放在super块中。
    sb=get_super(current->root->i_dev);
    total_blocks = sb->s_nzones;
    total_inodes=sb->s_ninodes;
	s_imap_blocks = sb->s_imap_blocks;
	s_zmap_blocks = sb->s_zmap_blocks;
	//hdinfo: 打印出硬盘的一些信息，
	//s_imap_blocks、ns_zmap_blocks、
	//total_blocks、free_blocks、used_blocks、total_inodes
    if(i_zone==1) 
    {
        for(i=0;i<sb->s_zmap_blocks;i++)
        {
            bh=sb->s_zmap[i];
            db=(char*)bh->b_data;
            for(j=0;j<1024;j++){
                for(k=1;k<=8;k++){
                        if((used_blocks+free_blocks)>=total_blocks)
                            break;
                        if( *(db+j) & k)
                            used_blocks++;
                        else
                            free_blocks++;
                }
            }
        }

        str=(char*)malloc(sizeof(char*)*512);
        size=sprintf(str,"s_imap_blocks:%d\ns_zmap_blocks:%d\n",s_imap_blocks,s_zmap_blocks);
        size+=sprintf(str+size,"total_blocks:%d\nfree_blcoks:%d\nused_blocks:%d\ntotal_indoes:%d\n",total_blocks,free_blocks,used_blocks,total_inodes);
    }



   	for(output_size = (*pos); (output_size < (*pos) + count) && (*(str+output_size)!= '\0'); output_size++)
	{ 
		put_fs_byte(*(str+output_size), buf++);
		output_count++;
	}
	(*pos)+=output_size;
   return output_count;

}