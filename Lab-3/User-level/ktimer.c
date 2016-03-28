#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int);
void printManPage(void);
char *args;
int main(int argc, char **argv) {
	char line[128], line2[128];
	int ii=0, count = 0, oflags, pFile;
	char buffer[128];
	struct sigaction action, oa;
	
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/nibbler */
	
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer_example module isn't loaded\n");
		return 1;
	}

	
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		read(pFile,line, 128);			
		while(line[ii]!='\0')
		{if(line[ii]=='/')
		line[ii]='\n';
		ii++;
		}
printf("%s", line);
	}

	// Check if in timer set mode
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) 
	{
	int i=0, j=0;
	for(i; *(*(argv + 2) + i) != '\0'; i++)
	buffer[i] = *(*(argv + 2) + i);
	buffer[i++]=' ';
	for(j;*(*(argv + 3) + j) != '\0';i++,j++)
	buffer[i]= *(*(argv + 3) + j);
	buffer[i]='\0';	

	/* Scanning buffer */

read(pFile,line2, 128);
ii=0;
if(line2[ii]=='\0'){

	write(pFile, buffer, (strlen(buffer)+1));				
	args= *(argv+3);
	//printf("Message inputted: %s",buffer);
	
	memset(&action, 0, sizeof(action));
	action.sa_handler = sighandler;
	action.sa_flags = SA_SIGINFO;
	sigemptyset(&action.sa_mask);
	sigaction(SIGIO, &action, NULL);
	fcntl(pFile, F_SETOWN, getpid());
	oflags = fcntl(pFile, F_GETFL);
	fcntl(pFile, F_SETFL, oflags | FASYNC);
	//printf("Sleep\n");
	pause();
	}

else{
while(line2[ii]!=' ')
ii++;

char string1[ii+1];
ii=0;
while(line2[ii]!=' ')
{string1[ii]=line2[ii];
ii++;
}
string1[ii]='\0';
ii++;
int delay=0;


if(strcmp(argv[3],string1)==0)
{

while(line2[ii]!='\0')
{if(line2[ii]!='/')
delay = delay * 10 + (line2[ii]-'0');
ii++;

}
printf("Timer %s already exists and has %d seconds remaining to expiration!\n",string1,delay);


}
else 
printf("A timer exists already\n");

	




	



}
}
	// Otherwise invalid
	else {
		printManPage();
	}

	close(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -l: info for all timers\n");	
	printf(" -s: write [message] to the module after timer expires\n");
}

// SIGIO handler
void sighandler(int signo)
{	//printf("AWAKE!!!!");
	printf("%s\n",args);
}
