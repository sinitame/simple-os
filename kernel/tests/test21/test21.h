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
#include "mem.h"

#define FILL_VALUE          0xCA
#define CHECKER_SUCCESS     0x12345678

extern int test21(void *arg);
extern int shm_checker(void *arg);

#endif /* _TEST21_H_ */
