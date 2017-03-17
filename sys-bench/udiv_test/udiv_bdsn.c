/** 
 * Kernel Module to test the implementation of
 * UDIV and SDIV in ARMv7-A processor.
 *
 * Copyright (C) 2014 Dumi Loghin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
