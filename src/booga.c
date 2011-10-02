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
#include "booga.h"

static int booga_major = BOOGA_MAJOR;
static int booga_nr_devs = BOOGA_NR_DEVS;
module_param(booga_major, int, 0);
module_param(booga_nr_devs, int, 0);
MODULE_AUTHOR("Ashok Gelal");
MODULE_LICENSE("GPL");

static booga_stats *booga_device_stats;

static int booga_open(struct inode *inode, struct file *filp);
static int booga_release(struct innode *inode, struct file *filp);
static ssize_t booga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t booga_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int __init booga_init(void);
static void __exit booga_exit(void);

static struct file_operations booga_fops = {
		.llseek	= NULL,
		.read	= booga_read,
		.write	= booga_write,
		.ioctl	= NULL,
		.open	= booga_open,
		.release = booga_release,
};

static int booga_open(struct inode *inode, struct file *filp){
	int num = NUM(inode->i_rdev);

	if (num >= booga_nr_devs) return -ENODEV;
	filp->f_op = &booga_fops;

	booga_device_stats->num_open++;
	MODULE_USAGE_UP;
	return 0;
}

static int booga_release(struct innode *inode, struct file *filp){
	booga_device_stats->num_close++;
	MODULE_USAGE_DOWN;
	return 0;
}

static ssize_t booga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
	printk("<1>booga_read invoked.\n");
	booga_device_stats->num_read++;
	return 0;
}

static ssize_t booga_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
	printk("<1>booga_write invoked.\n");
	booga_device_stats->num_write++;
	return count;
}

static void init_booga_device_stats(void){
	booga_device_stats->num_read = 0;
	booga_device_stats->num_write = 0;
	booga_device_stats->num_open = 0;
	booga_device_stats->num_close = 0;
}

static int booga_read_procmem(char *buf, char **start, off_t offset, int count, int *eof, void *data){
	int len = 0;
	len = sprintf(buf, "bytes read = %ld\n", booga_device_stats->num_read);
	len += sprintf(buf+len, "bytes written = %ld\n", booga_device_stats->num_write);
	len += sprintf(buf+len, "number of opens= %ld\n", booga_device_stats->num_open);
	len += sprintf(buf+len, "number of outputs= %ld\n", booga_device_stats->num_close);
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
	printk("<1> booga device driver version 0.1: loaded at major number %d\n", booga_major);

	booga_device_stats = (booga_stats *)kmalloc(sizeof(booga_stats), GFP_KERNEL);

	if(!booga_device_stats){
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
	kfree(booga_device_stats);
	unregister_chrdev(booga_major, "booga");
	printk("<1> booga device driver version 0.1: unloaded\n");
}

module_init(booga_init);
module_exit(booga_exit);
