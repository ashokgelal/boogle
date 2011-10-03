/*
 ============================================================================
 Name        : booga.c
 Author      : Ashok Gelal
 Version     :
 Copyright   : All Wrongs Reserved
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/random.h>
#include <asm-generic/uaccess.h>
#include "booga.h"

static int booga_major = BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS;
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);
MODULE_AUTHOR("Ashok Gelal");
MODULE_LICENSE("GPL");

uint current_device;
uint random_phrases_count[4];
static booga_stats *booga_device_stats;

static int booga_open(struct inode *inode, struct file *filp);
static int booga_release(struct inode *inode, struct file *filp);
static ssize_t booga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t booga_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int __init booga_init(void);
static void __exit booga_exit(void);
static char* get_random_phrase(void);

static struct file_operations booga_fops = {
		.llseek	= NULL,
		.read	= booga_read,
		.write	= booga_write,
		.open	= booga_open,
		.release = booga_release,
};

static int booga_open(struct inode *inode, struct file *filp){
	int num = NUM(inode->i_rdev);
	if (num >= booga_nr_devs) return -ENODEV;
	current_device = num;
	printk("<1>booga_open invoked.%d\n", num);
	(booga_device_stats->dev+num)->usage++;
	filp->f_op = &booga_fops;

	booga_device_stats->num_open++;
	MODULE_USAGE_UP;
	return 0;
}
static int booga_release(struct inode *inode, struct file *filp){
	printk("<1>booga_release invoked.\n");
	booga_device_stats->num_close++;
	MODULE_USAGE_DOWN;
	return 0;
}

static ssize_t booga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	int result;
	int status;
	char *data;
	char *random;
	char *temp;
	int i;

	printk("<1>booga_read invoked.\n");
	data = (char *) kmalloc(sizeof(char)*count, GFP_KERNEL);
	data = (booga_device_stats->dev+current_device)->str;
	if (!data) {
		result = -ENOMEM;
		goto fail_malloc;
	}

	random = get_random_phrase();
	temp = random;
	for(i=0; i<count; i++){
		// if temp is point to the last character...
		if(*temp=='\0')
		{
			// reset the pointer so that it points to the first character from 'random' str
			temp = random;
			data[i] = ' ';
			continue;
		}
		// dereference temp and copy to dev->str
		data[i] = *temp;
		temp++;
	}

	//status = __copy_to_user(buf, data, count);
	//if (status > 0) {
		//printk("simple: Could not copy %d bytes\n", status);
	//}

	if(data)
		kfree(data);

	booga_device_stats->bytes_read += count;

	return count;

	fail_malloc:
		return result;
}

static ssize_t booga_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
	printk("<1>booga_write invoked.\n");
	booga_device_stats->bytes_written += count;
	return count;
}

static char* get_random_phrase(void){
	char randval;
	uint choice ;
	get_random_bytes(&randval, 1);
	choice = (randval & 0x7F) % 4;
	random_phrases_count[choice]++;
	switch(choice){
		case 0:
			return "booga! booga!";
		case 1:
			return "googoo! gaagaa!";
		case 2:
			return "neka! maka!";
		case 3:
			return "wooga! wooga!";
	}
	return "";
}

static void init_booga_device_stats(void){
	booga_device_stats->bytes_read = 0;
	booga_device_stats->bytes_written = 0;
	booga_device_stats->num_open = 0;
	booga_device_stats->num_close = 0;

	booga_device_stats->dev->number=0;
	booga_device_stats->dev->usage=0;

	(booga_device_stats->dev+1)->number=1;
	(booga_device_stats->dev+1)->usage=0;

	(booga_device_stats->dev+2)->number=2;
	(booga_device_stats->dev+2)->usage=0;

	(booga_device_stats->dev+3)->number=3;
	(booga_device_stats->dev+3)->usage=0;

	random_phrases_count[0] = 0;
	random_phrases_count[1] = 0;
	random_phrases_count[2] = 0;
	random_phrases_count[3] = 0;
}

static int booga_read_procmem(char *buf, char **start, off_t offset, int count, int *eof, void *data){
	int len = 0;
	len = sprintf(buf, "bytes read = %ld\n", booga_device_stats->bytes_read);
	len += sprintf(buf+len, "bytes written = %ld\n", booga_device_stats->bytes_written);
	len += sprintf(buf+len, "number of opens:\n");
	len += sprintf(buf+len, "\t/dev/booga0 = %d\n", (booga_device_stats->dev+0)->usage);
	len += sprintf(buf+len, "\t/dev/booga1 = %d\n", (booga_device_stats->dev+1)->usage);
	len += sprintf(buf+len, "\t/dev/booga2 = %d\n", (booga_device_stats->dev+2)->usage);
	len += sprintf(buf+len, "\t/dev/booga3 = %d\n", (booga_device_stats->dev+3)->usage);

	len += sprintf(buf+len, "strings outputs:\n");
	len += sprintf(buf+len, "\tbooga! booga!: %d\n", random_phrases_count[0]);
	len += sprintf(buf+len, "\tgoogoo! gaga!: %d\n", random_phrases_count[1]);
	len += sprintf(buf+len, "\twooga! wooga!: %d\n", random_phrases_count[2]);
	len += sprintf(buf+len, "\tneka! maka!: %d\n", random_phrases_count[3]);
	return len;
}

static __init int booga_init(void){
	int result;

	result = register_chrdev(booga_major, "booga", &booga_fops);
	if(result < 0){
		printk(KERN_WARNING "booga: can't get major %d\n", booga_major);
		return result;
	}

	if(booga_major == 0) booga_major = result;
	printk("<1> booga device driver version 0.1: loaded at major number %d and nr: %d\n", booga_major, booga_nr_devs);

	booga_device_stats = (booga_stats *)kmalloc(sizeof(booga_stats), GFP_KERNEL);

	if(!booga_device_stats){
		result = -ENOMEM;
		goto fail_malloc;
	}

	booga_device_stats->dev = (booga_device_ptr)kmalloc(sizeof(booga_device_ptr)*4, GFP_KERNEL);
	if(!booga_device_stats->dev){
		result = -ENOMEM;
		goto fail_malloc;
	}

	init_booga_device_stats();
	create_proc_read_entry("driver/booga", 0, NULL, booga_read_procmem, NULL);

	return 0;

	fail_malloc:
		unregister_chrdev(booga_major, "booga");
		remove_proc_entry("driver/booga", NULL);
		return result;
}

static __exit void booga_exit(void){
	remove_proc_entry("driver/booga", NULL);
	kfree(booga_device_stats->dev);
	kfree(booga_device_stats);
	unregister_chrdev(booga_major, "booga");
	printk("<1> booga device driver version 0.1: unloaded\n");
}

module_init(booga_init);
module_exit(booga_exit);
