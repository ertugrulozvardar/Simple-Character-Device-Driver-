#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/ioctl.h>
#include<linux/proc_fs.h>

#define memory_size 1024
char char_array[100] = "hi I am the data which you want to read :)\n";
static int length = 1;


//Define the ioctl code 

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int32_t value = 0;

dev_t device = 0;
static struct class *dev_class;
static struct cdev project_cdev;
uint8_t *kernel_buffer;

//Functions initialized for regular file system and IOCTL based file system

static int __init char_dev_driver_init(void);
static void __exit char_dev_driver_exit(void);
static int f_open(struct inode *inode, struct file *file);
static ssize_t f_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t f_write(struct file *filp, const char *buf, size_t len, loff_t* off);
static int release(struct inode *inode, struct file *file);
static long char_ioctl(struct file *file, unsigned int cmnd, unsigned long arg);


//Functions initialized for proc file system

static int proc_open(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t proc_write(struct file *filp, const char *buf, size_t len, loff_t* off);
static int proc_release(struct inode *inode, struct file *file);


//Specifying the file operations for regular file system and IOCTL based file system
static struct file_operations fileopts = {
	.owner	       	= THIS_MODULE,
	.read		= f_read,
	.write		= f_write,
	.open		= f_open,
	.unlocked_ioctl = char_ioctl,
	.release	= release,
};

//Specifying the file operations for proc file system
static struct file_operations proc_fileopts = {
	.read		= proc_read,
	.write		= proc_write,
	.open		= proc_open,
	.release	= proc_release,
};

static int proc_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Procfs file is opened..\n");
	return 0;
}

static int proc_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Procfs file is released..\n");
}

static ssize_t proc_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Procfs file reading..\n");
	if(length) {
		length = 0;
	}
	else {
		length = 1;
		return 0;
	}
	
	copy_to_user(buf, char_array, 40);
	return length;
}

static ssize_t proc_write(struct file *filp, const char *buf, size_t len, loff_t* off) {
	printk(KERN_INFO "Procfs is writing..\n");
	copy_from_user(char_array, buf, length);
	return length;
}
static int f_open(struct inode *inode, struct file *file) {
	/* Physical Memory Creation */
	if((kernel_buffer = kmalloc(memory_size, GFP_KERNEL)) == 0) {
		printk(KERN_INFO "Memory cannot be allocated to the kernel.\n");
		return -1;
	}
	printk(KERN_INFO "Device file successfully opened!\n");
	return 0;

}

static release(struct inode *inode, struct file *file) {
	kfree(kernel_buffer);
	printk(KERN_INFO "Device file closed.\n");
	return 0;
}

static ssize_t f_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
	copy_to_user(buf, kernel_buffer, memory_size);
	printk(KERN_INFO "Reading Data..: DONE!..\n");
	return memory_size;
}

static ssize_t f_write(struct file *filp, const char __user *buf, size_t len, loff_t * off) {
	copy_from_user(kernel_buffer, buf, len);
	printk(KERN_INFO "Writing data..: Successfully written!..\n");
	return len;
}

static long char_ioctl(struct file *file, unsigned int cmnd, unsigned long arg) {
	switch(cmnd) {
		case WR_DATA:
			copy_from_user(&value, (int32_t*)arg, sizeof(value));
			printk(KERN_INFO "value = %d\n", value);
			break;
		case RD_DATA:
			copy_to_user((int32_t*)arg, &value, sizeof(value));
			break;
		}
return 0;
}


static int __init char_dev_driver_init(void) {

	/* Major number allocation */
	if ((alloc_chrdev_region(&device, 0, 1, "project_Dev")) < 0) {
		 printk(KERN_INFO "Major number cannot be allocated!\n");
	}

	printk(KERN_INFO"Major = %d Minor = %d..\n", MAJOR(device), MINOR(device));



	/* char device structure creation */

		 cdev_init(&project_cdev, &fileopts);

	/* Adding the character device to the system */
		 if ((cdev_add(&project_cdev, device, 1)) < 0) {
			printk(KERN_INFO "Device cannot be added to the system.\n");
			goto r_class;
		}

	/* Class struct creation */
		 if ((dev_class = class_create(THIS_MODULE, "my_class")) == NULL ) {
			 printk(KERN_INFO "Struct class cannot be created!\n");
			 goto r_class;
		}
	/* device creation */

		if ((device_create(dev_class, NULL, device, NULL, "project_Dev")) == NULL ) {
			printk(KERN_INFO "Device cannot be crated!\n");
			goto r_device;
		}
	/* Create the Proc entry here.. */
		proc_create("project_char_proc", 0666, NULL, &proc_fileopts);


		printk(KERN_INFO "Inserting device driver...finished properly...\n");
	     	return 0;

r_device:
		class_destroy(dev_class);




r_class:
		unregister_chrdev_region(device, 1);
		return -1;



}


void __exit char_dev_driver_exit(void) {
	device_destroy(dev_class, device);
	class_destroy(dev_class);
	cdev_del(&project_cdev);
	unregister_chrdev_region(device, 1);
	printk(KERN_INFO "Device driver is removed successfully!\n");
}

module_init(char_dev_driver_init);
module_exit(char_dev_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("İAE");
MODULE_DESCRIPTION("Our Character Device Driver");
