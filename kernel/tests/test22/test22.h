/*******************************************************************************
 * Ensimag - Projet Systeme
 * Copyright 2013 - Damien Dejean <dam.dejean@gmail.com>
 *
 * Test 21 : Common definitions
 *******************************************************************************/
#ifndef _TEST21_H_
#define _TEST21_H_

#include "processus.h"
#include "test22.h"
#include "sh_mem.h"
#include "stddef.h"

#define MAGIC_COOKIE        0xDEADBEEFu
#define MALICIOUS_SUCCESS   0xBAAAAAAD

extern int malicious(void *arg);
extern int pagefault(void *arg);
extern int test22(void *arg);

#endif /* _TEST21_H_ */
