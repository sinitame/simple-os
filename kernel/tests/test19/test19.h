#ifndef TEST19_H
#define TEST19_H

#include <stdio.h>
#include "processus.h"
#include "message_queue.h"
#include "sh_mem.h"
#include "stddef.h"

extern int test19_crc(void *arg);
extern int cons_reader(void *arg);
extern int cons_redN(void *arg);
extern int cons_rd0(void  *arg);
extern int test19_crl(void *arg);

#endif
