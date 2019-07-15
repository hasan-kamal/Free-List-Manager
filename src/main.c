/*

@author Hasan Kamal

*/

#include <stdio.h>
#include "free_list_manager.c"


int main(){

	// initialise list
	initialise_list();

	// main loop
	char input[INPUT_MAX];
	char *i_buffer = input;
	size_t cap = INPUT_MAX;

	char command[INPUT_MAX];
	int n;
	do{
		printf(">");
		getline(&i_buffer, &cap, stdin);
		sscanf(input, "%s", command);
		if(strcmp(command, "malloc")==0){
			sscanf(input, "%s %d", command, &n);
			malloc_list(n);
		}else if(strcmp(command, "free")==0){
			sscanf(input, "%s %d", command, &n);
			free_list(n);
		}else if(strcmp(command, "display")==0){
			display_list();
		}else if(strcmp(command, "exit")!=0){
			printf("Invalid command. Please try again\n");
		}

	}while(strcmp(command, "exit")!=0);

	// deallocate list
	deallocate_list();

	return 0;
}