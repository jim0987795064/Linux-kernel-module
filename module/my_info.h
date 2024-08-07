#ifndef MY_INFO_H
#define MY_INFO_H


#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/seq_file.h>
#include <linux/utsname.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
#include <linux/percpu.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <linux/vmalloc.h>
#include <linux/cma.h>
#include <asm/page.h>
#include <linux/cpufreq.h>
#include <linux/sysinfo.h>
#include <linux/time.h>



#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define PROC_NAME "my_info"



#endif