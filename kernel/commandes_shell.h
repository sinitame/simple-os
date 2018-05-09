#ifndef __COMMANDES_SHELL_H__
#define __COMMANDES_SHELL_H__



int create_process(char ** l);
int ps(void *arg);
int sleep(void *arg);
int clear(void *arg);
int help(void *arg);
int print(void *arg);
int echo(void *arg);

#endif
