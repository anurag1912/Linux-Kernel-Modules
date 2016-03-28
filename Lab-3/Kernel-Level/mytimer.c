
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
#include<linux/timer.h>


MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of memory.c functions */
static int mytimer_fasync(int fd, struct file *filp, int mode);
static int mytimer_open(struct inode *inode, struct file *filp);
static int mytimer_release(struct inode *inode, struct file *filp);
static ssize_t mytimer_read(struct file *filp,
		char *buf, size_t count, loff_t *f_pos);
static ssize_t mytimer_write(struct file *filp,
		const char *buf, size_t count, loff_t *f_pos);
static void mytimer_exit(void);
static int mytimer_init(void);
static int proc_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data );
/*------MY FUNCTIONS--------*/
//static int isfree(void);
static void initialize(void);
//static int issame(void);
/*------MY FUNCTIONS/--------*/


/* Structure that declares the usual file */
/* access functions */
struct file_operations mytimer_fops = {
	read: mytimer_read,
	write: mytimer_write,
	open: mytimer_open,
	release: mytimer_release,
	fasync: mytimer_fasync
};

/*-----For Multiple Timers--------*/ 
struct timers
{
char msg[128];		//User message
int occupied;		//Is timer occupied???
struct timer_list timer1;
};

/* Declaration of the init and exit functions */
module_init(mytimer_init);
module_exit(mytimer_exit);

static unsigned long j=0;
static unsigned delay=0; //For timer delay in seconds
static struct proc_dir_entry *proc_entry;
static unsigned capacity = 256;		//buffer capacity
static unsigned bite = 128;		//bites to read
static struct timers t[1];		//MULTIPLE TIMERS (10 timer capacity)
static char message[256];
static int processid=0;
char *commandid;
struct fasync_struct *async_queue;
/* Global variables of the driver */
/* Major number */
static int mytimer_major = 61;
static char *cookie_pot;  // Space for mytimer strings
static int cookie_index;  // Index to write next mytimer
static int next_mytimer;  // Index to read next mytimer
/* Buffer to store data */
char *mytimer_buffer;
/* length of the current message */
static int mytimer_len;

/*-----------------------------------------------------------------START OF FUNCTIONS-------------------------------------------------------------------------*/

/* //Timer-expiry function// */

static int mytimer_fasync(int fd, struct file *filp, int mode) {
	return fasync_helper(fd, filp, mode, &async_queue);
}



static int proc_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data )
{
	int len;

  if (off > 0) {
    *eof = 1;
    return 0;
  }
  printk("Name of the module: mytimer\n");
  printk("Time since module is lodaded: %u sec(s)\n",((jiffies-j)/msecs_to_jiffies(1000*1)));
  
if(t[0].occupied==1)  
{printk("Process id:%d\n",processid);
printk("Command id:%s\n",commandid);
printk("Time in seconds to expiration: %u sec(s)\n", (t[0].timer1.expires-jiffies)/msecs_to_jiffies(1000*1));	
}
else
printk("Time in seconds to expiration: %u sec(s). No timer initialized!\n",0);	 

return len;
}


void timer1_routine(unsigned long data)
{
//printk(KERN_ALERT "%s\n" ,t[data].msg);		//data variable updated at initialize function
t[data].occupied = 0;				//Timer free about to be deleted
memset(t[data].msg,'\0' ,128);	
if (!async_queue){

del_timer(&t[0].timer1);
t[0].occupied = 0;				//Timer free about to be deleted
memset(t[0].msg,'\0' ,128);	
memset(mytimer_buffer, '\0', capacity);

}

		if (async_queue)
		{



del_timer(&t[0].timer1);
t[0].occupied = 0;				//Timer free about to be deleted
memset(t[0].msg,'\0' ,128);	
memset(mytimer_buffer, '\0', capacity);
kill_fasync(&async_queue, SIGIO, POLL_IN);
}

	del_timer(&t[data].timer1);				//Message deleted and reset for new timer
 		/* Deleting the timer */
}



/* //Timer initialize function// */

void initialize()
{t[0].occupied = 1;				//Timer is occupied with message and is ready
int j;							
int i;
int tout;
i=0; tout=0;					
while(mytimer_buffer[i]!=' ')			//Format is time SPACE msg
{tout= tout*10 + (mytimer_buffer[i]-'0');	//Calculating time...
i++;
}


++i;						//Skip space
for(j=0;mytimer_buffer[i]!='\0';i++,j++)	
t[0].msg[j]= mytimer_buffer[i];		//Initialize message
t[0].msg[j]='\0';				//Enter /0
memset(mytimer_buffer, 0, capacity);		//Empty buffer for new timer
init_timer(&t[0].timer1);			/*Initialize sequence for timers*/
t[0].timer1.function = timer1_routine;
t[0].timer1.data=0;
delay=tout;

t[0].timer1.expires = jiffies + delay * HZ; 	/* delay seconds */

add_timer(&t[0].timer1); 			/* Starting the timer */

}


/* //Where is there space for a new timer???// */
/*
int isfree()					
{int i=0;
while(i<=9 && t[i].occupied==1)
i++;

if(i<=9) return i;				//Space available at i

else return -1;					//No space for new timers
}
*/

/* //Is the timer the same one as initialized before???// */
/* First retrieving the message and then checking to see if space is available if exact message found at location calculate new update time */
/*
int issame()
{

int i=0;
char ch[128];
int blah=0;
int index=0;
while(mytimer_buffer[blah]!=' ')
blah++;
blah++;
for(index=0;mytimer_buffer[blah]!='\0';blah++,index++)		//Retrieve new message
ch[index]= mytimer_buffer[blah];				//Store new message
ch[index]='\0';
for(i=0;i<=9;i++)
{
if(strcmp(ch, t[i].msg)==0)					//If FOUND TIMER, then...
{
t[i].occupied = 1;						//Timer occupied
//int j;
int k=0;
int tout;
tout=0;								//UPDATE!!
while(mytimer_buffer[k]!=' ')
{tout= tout*10 + (mytimer_buffer[k]-'0');
k++;
}



memset(mytimer_buffer, 0, capacity);
printk(KERN_ALERT "The timer %s was updated!\n", ch);		//Timer UPDATED MESSAGE HAS TO BE FROM USER LEVEL in LAB 2
init_timer(&t[i].timer1);					//Initialization of timer as before
t[i].timer1.function = timer1_routine;				
t[i].timer1.data=i;						//VERY IMPORTANT!! Data is updated to the index value to be identified!
delay=tout;

t[i].timer1.expires = jiffies + delay * HZ; 		

add_timer(&t[i].timer1); 				

break;

}

}
if(i==10)
return 0;							//No space available!!!
else return 1;							//Success timer updated!
}
*/


static int mytimer_init(void)
{j=jiffies;
	
t[0].occupied = 0;					//No timers occupied!
	int i=0;
	for(i=0;i<128;i++)
t[0].msg[i] = '\0';					//Message initialized to all /0
        
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
	mytimer_buffer = kmalloc(capacity, GFP_KERNEL); 	//Pointer to block of memory of size "capacity" allocated and first location's address stored in mytimer_buffer!!
	if (!mytimer_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	memset(mytimer_buffer, 0, capacity);			//Initialize buffer to 0
	mytimer_len = 0;
memset(message, 0, capacity);



proc_entry = create_proc_entry( "mytimer", 0644, NULL );

    if (proc_entry == NULL) {

      //ret = -ENOMEM;
      vfree(cookie_pot);
      printk(KERN_INFO "mytimer: Couldn't create proc entry\n");

    } else {

      cookie_index = 0;
      next_mytimer = 0;
      proc_entry->read_proc = proc_read;
      //proc_entry->write_proc = mytimer_write;
      proc_entry->owner = THIS_MODULE;
      printk(KERN_INFO "mytimer: Module loaded.\n");

    }


	printk("Inserting mytimer module\n"); 
	printk("Inserted mytimer module!\n"); 
	return 0;




	
fail: 
	mytimer_exit(); 
	return result;
}

static void mytimer_exit(void)
{
	/* Freeing the major number */
	unregister_chrdev(mytimer_major, "mytimer");

	/* Freeing buffer memory */				//Deallocate buffer
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
	mytimer_fasync(-1, filp, 0);
	return 0;
}

static ssize_t mytimer_read(struct file *filp, char *buf, 
							size_t count, loff_t *f_pos)
{int i=0; 
int j=0;
int k=0;

/*if (!async_queue){

del_timer(&t[0].timer1);
t[0].occupied = 0;				//Timer free about to be deleted
memset(t[0].msg,'\0' ,128);	
memset(mytimer_buffer, '\0', capacity);

}
*/
	/* end of buffer reached */
	//if (*f_pos >= mytimer_len)
	//{
	//	return 0;
//printk(KERN_ALERT "Returned 0...");		
//}

	/* do not go over then end */
	//if (count > mytimer_len - *f_pos)
	//	count = mytimer_len - *f_pos;

	/* do not send back more than a bite */
	//if (count > bite) count = bite;

int timeoffset=0;
int temp=0;
//for(i=0;i<10;i++)
//{


if(t[i].occupied==1)
{

while(t[i].msg[j]!='\0')
{message[k]=t[i].msg[j];
k++;j++;
}
message[k]=' ';
k++;
timeoffset=(t[i].timer1.expires-jiffies)/msecs_to_jiffies(1000*1);
temp=timeoffset;
j=1;
while((timeoffset=timeoffset/10)!=0)
j=j*10;


timeoffset=temp;

while(j!=0)
{temp=timeoffset;
message[k]= temp/j + '0';
timeoffset%=j;
j=j/10;
k++;

}
message[k]='/';
k++;
}
//j=0;

//}

message[k]='\0';

if(t[i].occupied==1){
sprintf(mytimer_buffer, "%s",message);}
else 
	memset(mytimer_buffer, '\0', capacity);


	/* Transfering data to user space */ 
	if (copy_to_user(buf, mytimer_buffer + *f_pos, count))
	{
		return -EFAULT;
	}

	
memset(mytimer_buffer, 0 ,capacity);
memset(message , 0, capacity);	
	/* Changing reading position as best suits */ 

	//*f_pos += count; 
	
	return count; 
}

static ssize_t mytimer_write(struct file *filp, const char *buf,
							size_t count, loff_t *f_pos)
{

//int free;
//int same;
/*
int length=0;
int blah=0;
int index=0;
int length2=0;
/* end of buffer reached */
processid=current->pid;
commandid=current->comm;


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


	//*f_pos += count;
	mytimer_len = *f_pos;

if(t[0].occupied==0)
	initialize();
/*
else
{
while(mytimer_buffer[blah]!=' ')
blah++;
blah++;
for(;mytimer_buffer[blah]!='\0';blah++)		
length++;
blah-=length;
for(index=0;t[0].msg[index]==mytimer_buffer[blah]&&mytimer_buffer[blah]!='\0'&&t[0].msg[index]!='\0';blah++,index++)
length2++;


if(length==length2)
{printk("Timer %s already exists and has %lu seconds remaining to expiration!",t[0].msg,(t[0].timer1.expires-jiffies)/msecs_to_jiffies(1000*1));

}
else
	printk("A timer exists already");
memset(mytimer_buffer, 0, capacity);



}*/	

return count;
}
