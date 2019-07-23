#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


int main1(int argc, char const *argv[])
{

	int pid = fork();

	if(pid == 0){
		printf("%s\n","child" );
		sleep(2);
	}else if(pid >0){
		printf("%s\n","parent" );

		while(-1 != wait(NULL)){
			printf("%s\n","child dead" );
		}

		printf("%s\n", "----end----");
	}else{
		printf("%s\n", strerror(errno));
	}

	return 0;
}