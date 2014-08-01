/* 
 * Kernel Module to test the implementation of
 * UDIV and SDIV in ARMv7-A processor.
 *
 * Copyright (c) 2014 Dumi Loghin 
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define IN_THUMB 	0x01000000
#define IN_BOTH		0x02000000

static int __init bdsn_init(void)
{
	u32 val;

	printk(KERN_INFO "BDSN Custom Kernel Module\n");

	asm volatile("mrc p15, 0, %0, c0, c2, 0" : "=r" (val));	
	printk(KERN_INFO "ID_ISAR0  = 0x%08x\n", val);

	if ((val & IN_BOTH) == IN_BOTH)
		printk(KERN_INFO "UDIV in both ARM and Thumb IS");
	else {
		if ((val & IN_THUMB) == IN_THUMB)
			printk(KERN_INFO "UDIV only in Thumb IS");
		else
			printk(KERN_INFO "UDIV not implemented");
	}

	return 0;
}

static void __exit bdsn_exit(void)
{
	printk(KERN_INFO "BDSN Kernel Module exit.\n");
}

module_init(bdsn_init);
module_exit(bdsn_exit);
