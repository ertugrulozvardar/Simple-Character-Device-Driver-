#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int8_t write_buffer[1024];
int8_t read_buffer[1024];

int main() {

	int f_d;
	char selection;
	int true = 1;

	printf("Welcome to the test of our Character Device Driver!\n");
	f_d = open("/proc/project_char_proc", O_RDWR);
	if (f_d < 0) {
		printf("Device file cannot be opened..\n");
		return 0;

	}

	while(1) {

		printf("---------Please enter an integer to make a selection----------\n");
		printf("************************** 1) Write **************************\n");
		printf("************************** 2) Read **************************\n");
		printf("************************** 3) Exit **************************\n");
		scanf(" %c", &selection);
		printf(" Your selection = %c\n", selection);


		if(selection == '1') {

			printf("Enter a string to write into driver:\n");
			scanf(" %[^\t\n]s", write_buffer);
			printf("Data is being written..\n");
			write(f_d, write_buffer, strlen(write_buffer) + 1);
			printf("Successfully written.. Done!..\n");
		}

		else if (selection == '2') {

			printf("Reading data..\n");
			read(f_d, read_buffer, 1024);
			printf("Data has succesffuly read.. Done!..\n");
			printf("Data = %s\n\n", read_buffer);

		}

		else if (selection == '3') {

			close(f_d);
			exit(1);
		}

		else {
			printf("Please enter a valid number.\n");
		}

	}
	close(f_d);

}
