#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void printManPage(void);

int main(int argc, char **argv) {
	char line[256];
	int ii, count = 0;
	char buffer[256];
	
	/* Check to see if the nibbler successfully has mknod run
	   Assumes that nibbler is tied to /dev/nibbler */
	FILE * pFile;
	pFile = fopen("/dev/mytimer", "r+");
	if (pFile==NULL) {
		fputs("mytimer module isn't loaded\n",stderr);
		return -1;
	}

	// Check if in status mode
	/*if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		while (fgets(line, 256, pFile) != NULL) {
			printf("%s\n", line);
		}
	}
*/
	// Check if in timer set mode
	if (argc == 4 && strcmp(argv[1], "-s") == 0) 
	{
	int i=0, j=0;
	for(i; *(*(argv + 2) + i) != '\0'; i++)
	buffer[i] = *(*(argv + 2) + i);
	buffer[i++]=' ';
	for(j;*(*(argv + 3) + j) != '\0';i++,j++)
	buffer[i]= *(*(argv + 3) + j);
	buffer[i]='\0';		
	fputs(buffer, pFile);
	




}

	// Otherwise invalid
	else {
		printManPage();
	}

	fclose(pFile);
	return 0;
}

void printManPage() {
	printf("Error: invalid use.\n");
	printf(" ktimer [-flag] [message]\n");
	printf(" -s: write [message] to the module after timer expires\n");
}
