/*
 * booga.h
 *
 *      Author: Ashok Gelal
 */

#ifndef __BOOGA_H
#define __BOOGA_H

#define MODULE_USAGE_UP { try_module_get(THIS_MODULE); }
#define MODULE_USAGE_DOWN { module_put(THIS_MODULE); }

#ifndef BOOGA_MAJOR
#define BOOGA_MAJOR 0
#endif

#ifndef BOOGA_NR_DEVS
#define BOOGA_NR_DEVS 4
#endif

/*
 * Split minors in two parts
 */
#define TYPE(dev) (MINOR(dev) >> 4)
#define NUM(dev) (MINOR(dev) & 0xf)

struct booga_stats{
	long int num_open;
	long int num_close;
	long int num_read;
	long int num_write;
};

typedef struct booga_stats booga_stats;
extern booga_stats Booga_Device_Stats;
#endif /* __BOOGA_H */
