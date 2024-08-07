#include"my_info.h"

/* This function is called at the beginning of a sequence.
 * ie, when:
 *   - the /proc file is read (first time)
 *   - after the function stop (end of sequence)
 */

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
    static unsigned long counter = 0;

    /* beginning a new sequence? */
    if (*pos == 0)
    {
        /* yes => return a non null value to begin the sequence */
        return &counter;
    }

    /* no => it is the end of the sequence, return end to stop reading */
    *pos = 0;
    return NULL;
}

/* This function is called after the beginning of a sequence.
 * It is called untill the return is NULL (this ends the sequence).
 */
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    unsigned long *tmp_v = (unsigned long *)v;
    (*tmp_v)++;
    (*pos)++;
    return NULL;
}

/* This function is called at the end of a sequence. */
static void my_seq_stop(struct seq_file *s, void *v)
{
    /* nothing to do, we use a static value in start() */
}

/* This function is called for each "step" of a sequence. */
static int my_seq_show(struct seq_file *s, void *v)
{
    // initailize struct cpu && mem information
    struct sysinfo meminfo;
    struct timespec64 uptime;
    struct timespec64 idle;
    si_meminfo(&meminfo);
    unsigned long pages[NR_LRU_LISTS];

    /* unsigned int cpu = 0;
     struct cpuinfo_x86 *c = &cpu_data(cpu);
     int PhsicalBit = c->x86_phys_bits,
         VirtualBit = c->x86_virt_bits,*/
    int Least_Recently_Used;
    int i;
    u64 nsec;
    u32 rem;

    for (Least_Recently_Used = LRU_BASE; Least_Recently_Used < NR_LRU_LISTS; Least_Recently_Used++)
        pages[Least_Recently_Used] = global_node_page_state(NR_LRU_BASE + Least_Recently_Used);

    unsigned long totalmem = (meminfo.totalram * meminfo.mem_unit)/1024;
    unsigned long freemem = (meminfo.freeram * meminfo.mem_unit)/1024;
    unsigned long buffermem = (meminfo.bufferram * meminfo.mem_unit)/1024;
    unsigned long active = ((pages[LRU_ACTIVE_ANON] + pages[LRU_ACTIVE_FILE]) * meminfo.mem_unit)/1024;
    unsigned long inactive = (pages[LRU_INACTIVE_ANON] + pages[LRU_INACTIVE_FILE] * meminfo.mem_unit)/1024;
    unsigned long shmem = (meminfo.sharedram * meminfo.mem_unit)/1024;
    unsigned long dirty = ((global_node_page_state(NR_FILE_DIRTY)) * meminfo.mem_unit)/1024;
    unsigned long writeBack = ((global_node_page_state(NR_WRITEBACK)) * meminfo.mem_unit)/1024;
    unsigned long kernelStack = global_zone_page_state(NR_KERNEL_STACK_KB);
    unsigned long pageTables = ((global_zone_page_state(NR_PAGETABLE)) * meminfo.mem_unit)/1024;


    nsec = 0;
    for_each_possible_cpu(i)
    nsec += (__force u64) kcpustat_cpu(i).cpustat[CPUTIME_IDLE];

    get_monotonic_boottime(&uptime);
    idle.tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
    idle.tv_nsec = rem;


    seq_printf(s, "==========Version==========\n");
    seq_printf(s, "%s version %s",
               utsname()->sysname, utsname()->release);
    seq_printf(s, "\n\n==========CPU==========\n");
    unsigned int cpu = 0;
    for_each_online_cpu(cpu)
    {

        struct cpuinfo_x86 *c = &cpu_data(cpu);
        int PhsicalBit = c->x86_phys_bits,
            VirtualBit = c->x86_virt_bits;
        seq_printf(s, "%-16s : %d\n", "processor", c->cpu_index);
        seq_printf(s, "%-16s : %s\n", "model name", c->x86_model_id);
        seq_printf(s, "%-16s : %d\n", "physical id", c->phys_proc_id);
        seq_printf(s, "%-16s : %d\n", "core id", c->cpu_core_id);
        seq_printf(s, "%-16s : %d\n", "cpu cores", c->booted_cores);
        seq_printf(s, "%-16s : %d KB\n", "cache size", c->x86_cache_size);
        seq_printf(s, "%-16s : %d \n", "clflush size", c->x86_clflush_size);
        seq_printf(s, "%-16s : %d \n", "cache_alignment", c->x86_cache_alignment);
        seq_printf(s, "%-16s : %d bits physical %d bits virtual\n\n",
                   "address sizes", PhsicalBit, VirtualBit);
    }
    seq_printf(s, "==========Memory==========\n");
    seq_printf(s, "%-16s : %lu kB\n", "MemTotal", totalmem);
    seq_printf(s, "%-16s : %lu kB\n", "MemFree", freemem);
    seq_printf(s, "%-16s : %lu kB\n", "Buffers", buffermem);
    seq_printf(s, "%-16s : %lu kB\n", "Active", active);
    seq_printf(s, "%-16s : %lu kB\n", "Inactive", inactive);
    seq_printf(s, "%-16s : %lu kB\n", "Shmem", shmem);
    seq_printf(s, "%-16s : %lu kB\n", "Dirty", dirty);
    seq_printf(s, "%-16s : %lu kB\n", "Writeback", writeBack);
    seq_printf(s, "%-16s : %lu kB\n", "Kernelstack", kernelStack);
    seq_printf(s, "%-16s : %lu kB\n", "PageTables", pageTables);

    seq_printf(s, "\n==========Time==========\n");
    seq_printf(s, "%-16s : %lu.%02lu (s)\n",
               "Uptime",
               (unsigned long) uptime.tv_sec,
               (uptime.tv_nsec / (NSEC_PER_SEC / 100)));
    seq_printf(s, "%-16s : %lu.%02lu (s)\n",
               "Idletime",
               (unsigned long) idle.tv_sec,
               (idle.tv_nsec / (NSEC_PER_SEC / 100)));


    return 0;
}

/* This structure gather "function" to manage the sequence */
static struct seq_operations my_seq_ops =
{
    .start = my_seq_start,
    .next = my_seq_next,
    .stop = my_seq_stop,
    .show = my_seq_show,
};

/* This function is called when the /proc file is open. */
static int my_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &my_seq_ops);
};

/* This structure gather "function" that manage the /proc file */
#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops =
{
    .proc_open = my_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = seq_release,
};
#else
static const struct file_operations my_file_ops =
{
    .open = my_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};
#endif

static int __init procfs4_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0, NULL, &my_file_ops);
    if (entry == NULL)
    {
        remove_proc_entry(PROC_NAME, NULL);
        pr_debug("Error: Could not initialize /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit procfs4_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    pr_debug("/proc/%s removed\n", PROC_NAME);
}

module_init(procfs4_init);
module_exit(procfs4_exit);

MODULE_LICENSE("GPL");
