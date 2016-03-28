
/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/timer.h>


MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of memory.c functions */
static int mytimer_open(struct inode *inode, struct file *filp);
static int mytimer_release(struct inode *inode, struct file *filp);
static ssize_t mytimer_read(struct file *filp,
		char *buf, size_t count, loff_t *f_pos);
static ssize_t mytimer_write(struct file *filp,
		const char *buf, size_t count, loff_t *f_pos);
static void mytimer_exit(void);
static int mytimer_init(void);
static int isfree(void);
static void initialize(int);
static int issame();
/* Structure that declares the usual file */
/* access functions */
struct file_operations mytimer_fops = {
	read: mytimer_read,
	write: mytimer_write,
	open: mytimer_open,
	release: mytimer_release
};
struct timers
{
char msg[128];
int occupied;
struct timer_list timer1;
};

/* Declaration of the init and exit functions */
module_init(mytimer_init);
module_exit(mytimer_exit);

static unsigned delay=0;
static unsigned capacity = 128;
static unsigned bite = 128;
static struct timers t[10];


/* Global variables of the driver */
/* Major number */
static int mytimer_major = 61;

/* Buffer to store data */
char *mytimer_buffer;
/* length of the current message */
static int mytimer_len;

void timer1_routine(unsigned long data)
{
printk(KERN_ALERT "%s\n" ,t[data].msg);
t[data].occupied = 0;
memset(t[data].msg,'\0' ,128);
del_timer_sync(&t[data].timer1); /* Deleting the timer */
}


void initialize(int no)
{t[no].occupied = 1;
int j;
int i;
int tout;
i=0; tout=0;
while(mytimer_buffer[i]!=' ')
{tout= tout*10 + (mytimer_buffer[i]-'0');
i++;
}


++i;
for(j=0;mytimer_buffer[i]!='\0';i++,j++)
t[no].msg[j]= mytimer_buffer[i];
t[no].msg[j]='\0';
memset(mytimer_buffer, 0, capacity);
init_timer(&t[no].timer1);
t[no].timer1.function = timer1_routine;
t[no].timer1.data=no;
delay=tout;

t[no].timer1.expires = jiffies + delay * HZ; /* delay seconds */

add_timer(&t[no].timer1); /* Starting the timer */

}

int isfree()
{int i=0;
while(i<=9 && t[i].occupied==1)
i++;

if(i<=9) return i;

else return -1;
}

int issame()
{int i=0;
char ch[128];
int blah=0;
int index=0;
while(mytimer_buffer[blah]!=' ')
blah++;
blah++;
for(index=0;mytimer_buffer[blah]!='\0';blah++,index++)
ch[index]= mytimer_buffer[blah];
ch[index]='\0';
for(i=0;i<=9;i++)
{
if(strcmp(ch, t[i].msg)==0)
{
t[i].occupied = 1;
int j;
int k=0;
int tout;
tout=0;
while(mytimer_buffer[k]!=' ')
{tout= tout*10 + (mytimer_buffer[k]-'0');
k++;
}



memset(mytimer_buffer, 0, capacity);
printk(KERN_ALERT "The timer %s was updated!\n", ch);
init_timer(&t[i].timer1);
t[i].timer1.function = timer1_routine;
t[i].timer1.data=i;
delay=tout;

t[i].timer1.expires = jiffies + delay * HZ; /* delay seconds */

add_timer(&t[i].timer1); /* Starting the timer */

break;

}

}
if(i==10)
return 0;
else return 1;
}

static int mytimer_init(void)
{	t[0].occupied = 0;
	t[1].occupied = 0;
	t[2].occupied = 0;
	t[3].occupied = 0;
	t[4].occupied = 0;
	t[5].occupied = 0;
	t[6].occupied = 0;
	t[7].occupied = 0;
	t[8].occupied = 0;
	t[9].occupied = 0;
	int i=0, j=0;
	for(i=0;i<=9;i++)
	{for(j=0;j<128;j++)
	t[i].msg[j] = '\0';	
        }
	int result;

	/* Registering device */
	result = register_chrdev(mytimer_major, "mytimer", &mytimer_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mytimer: cannot obtain major number %d\n", mytimer_major);
		return result;
	}

	/* Allocating mytimer for the buffer */
	mytimer_buffer = kmalloc(capacity, GFP_KERNEL); 
	if (!mytimer_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	memset(mytimer_buffer, 0, capacity);
	mytimer_len = 0;

	printk(KERN_ALERT "Inserting mytimer module\n"); 
	return 0;
	//init_timer(&timer1);
fail: 
	mytimer_exit(); 
	return result;
}

static void mytimer_exit(void)
{
	/* Freeing the major number */
	unregister_chrdev(mytimer_major, "mytimer");

	/* Freeing buffer memory */
	if (mytimer_buffer)
	{
		kfree(mytimer_buffer);
	}

	printk(KERN_ALERT "Removing mytimer module\n");
	



}





static int mytimer_open(struct inode *inode, struct file *filp)
{
	//printk(KERN_INFO "open called \n");
	/* Success */
	return 0;
}

static int mytimer_release(struct inode *inode, struct file *filp)
{
	//printk(KERN_INFO "release called \n");
	/* Success */

	return 0;
}

static ssize_t mytimer_read(struct file *filp, char *buf, 
							size_t count, loff_t *f_pos)
{ 


	/* end of buffer reached */
	if (*f_pos >= mytimer_len)
	{
		return 0;
	}

	/* do not go over then end */
	if (count > mytimer_len - *f_pos)
		count = mytimer_len - *f_pos;

	/* do not send back more than a bite */
	if (count > bite) count = bite;

	/* Transfering data to user space */ 
	if (copy_to_user(buf, mytimer_buffer + *f_pos, count))
	{
		return -EFAULT;
	}

	

	
	/* Changing reading position as best suits */ 

	*f_pos += count; 
	return count; 
}

static ssize_t mytimer_write(struct file *filp, const char *buf,
							size_t count, loff_t *f_pos)
{

int free;
int same;
	/* end of buffer reached */
	if (*f_pos >= capacity)
	{
		printk(KERN_INFO
			"write called: process id , command , count , buffer full\n"
			/*current->pid, current->comm, count*/);
		return -ENOSPC;
	}

	/* do not eat more than a bite */
	if (count > bite) count = bite;

	/* do not go over the end */
	if (count > capacity - *f_pos)
		count = capacity - *f_pos;

	if (copy_from_user(mytimer_buffer + *f_pos, buf, count))
	{
		return -EFAULT;
	}


	*f_pos += count;
	mytimer_len = *f_pos;

same = issame();
if(same==0)
{	
free = isfree();
if(free<=9)
initialize(free);
else 
{
printk(KERN_ALERT "Aditional timers unavailable! Exiting...");
mytimer_exit();
}

}	
return count;
}

