// Vedic CPU Scheduler for Android Kernel
// Compile as kernel module or patch into kernel source
// For now: reference implementation

#include <linux/sched.h>
#include <linux/cpufreq.h>

#define PHI 1.618033988749895

static bool is_ida_task(struct task_struct *p) {
    return p->prio > 120;
}

static bool is_pingala_task(struct task_struct *p) {
    return p->prio < 100;
}

static unsigned int vedic_next_freq(struct task_struct *p) {
    unsigned int base_freq = cpufreq_quick_get(0);
    if (is_pingala_task(p))
        return base_freq * PHI;
    else if (is_ida_task(p))
        return base_freq / PHI;
    else
        return base_freq;
}
