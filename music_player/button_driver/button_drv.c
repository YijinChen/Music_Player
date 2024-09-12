#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/signal.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/capi.h>
#include <linux/kernelcapi.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/moduleparam.h>

#include "button_drv.h"


static int major = 0;

static struct button_operations *p_button_opr;
static struct class *button_class;

//static:the function cannot be called from other files within the same project. avoid name conflicts.
//inode: a pointer to an inode structure, which represents the file's metadata in the filesystem. 
//The inode contains information such as the device number, file type, permissions, and more.
//file: a pointer to a file structure, which represents an open file. 
//It contains information about the file, such as the file operations associated with it, the current file offset, and flags.
static int button_open (struct inode *inode, struct file *file) 
{
	int minor = iminor(inode); //get minor from node
	p_button_opr->init(minor);
	//p_button_opr: a pointer to a structure that contains function pointers for various operations related to the button device.
	return 0;
}

static ssize_t button_read (struct file *file, char __user *buf, size_t size, loff_t *off)
{
	unsigned int minor = iminor(file_inode(file)); // get minor from file
	char level;
	int err;
	
	level = p_button_opr->read(minor); // read the voltage level from hardward
	err = copy_to_user(buf, &level, 1);
	return 1;
}

static struct file_operations button_fops = {
	.open = button_open,
	.read = button_read,
};

//“struct button_operations *opr” is refered by lower layer code
void register_button_operations(struct button_operations *opr)
{
	int i;

	p_button_opr = opr;
	for (i = 0; i < opr->count; i++)
	{
		device_create(button_class, NULL, MKDEV(major, i), NULL, "100ask_button%d", i);
	}
}

void unregister_button_operations(void)
{
	int i;

	for (i = 0; i < p_button_opr->count; i++)
	{
		device_destroy(button_class, MKDEV(major, i));
	}
}

//use EXPORT_SYMBOL(), so these two functions can be used by other driver program, also declare these two functions in header file
EXPORT_SYMBOL(register_button_operations);
EXPORT_SYMBOL(unregister_button_operations);


int button_init(void)
{
	major = register_chrdev(0, "100ask_button", &button_fops);

	button_class = class_create(THIS_MODULE, "100ask_button");
	//the button_class can only be created when the lower layer code provide the “struct button_operations *opr”, in above function register_button_operations()
	if (IS_ERR(button_class))
		return -1;
	
	return 0;
}

void button_exit(void)
{
	class_destroy(button_class);
	unregister_chrdev(major, "100ask_button");
}

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");


