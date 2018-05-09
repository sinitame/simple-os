/*******************************************************************************
 * Ensimag - Projet Systeme
 * Copyright 2013 - Damien Dejean <dam.dejean@gmail.com>
 *******************************************************************************/
#ifdef TEST22_H

#include "test22.h"

int pagefault(void *arg)
{
        (void)arg;
        /* Try to fault */
        *((int*)0) = 0;
        /* We should have been killed ... */
        return (int)MALICIOUS_SUCCESS;
}

#endif
