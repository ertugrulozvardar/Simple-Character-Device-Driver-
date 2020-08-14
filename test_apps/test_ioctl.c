#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int main() {

	int f_d;
	int32_t value, number;

	printf("Welcome to IOCTL based Character Device Driver operation from user space!\n");
	f_d = open("/dev/project_Dev", O_RDWR);

	if (f_d < 0) {
		printf("Device file cannot be opened..\n");
		return 0;

	}

	printf("Enter the data to send..\n");
	scanf("%d", &number);
	printf("Value is being write to the character device driver..\n");
	ioctl(f_d, WR_DATA, (int32_t*)&number);

	printf("Reading the value from character device driver..\n");
	ioctl(f_d, RD_DATA, (int32_t*)&value);
	printf("Value is: %d\n", value);

	printf("Closing the character device driver..\n");
	close(f_d);


}
