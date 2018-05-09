/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readcmd.h"
#include "listchainee.h"
#include "processus.h"
#include "mem.h"
#include "shell.h"
#include "commandes_shell.h"
#include "errno.h"



Liste jobs;

void terminate(char *line) {

	if (line)
	  xfree(line);
	printf("exit\n");
	exit(0);
}


int shell(void *arg) {
	(void)arg;
	jobs = NULL;

	while (1) {


		struct cmdline *l;
		char *line=0;
		//int i, j;
		char *prompt = "shell>";
		/* Readline use some internal memory structure that
		   	can not be cleaned at the end of the program. Thus
		   	one memory leak per command seems unavoidable yet */
			line = readline(prompt);
/*
			if (line == 0 || ! strncmp(line,"exit", 4)) {
				terminate(line);
			}
*/
			//add_history(line);

			/* parsecmd xfree line and set it up to 0 */
			l = parsecmd( & line);

			//printf("Commade:'%s', args: '%s', rin: %s, rout: '%s' \n", l->seq[0][0],l->seq[0][1],l->in,l->out);

/*
			// If input stream closed, normal termination
			if (!l) {

				terminate(0);
			}



			if (l->err) {
				// Syntax error, read another command
				printf("error: %s\n", l->err);
				continue;
			}*/


 		int pid = create_process(l->seq[0]);

	/*int statut;

	void fin_process(int signum){
		int status,pid;
		Liste tmp = jobs;
		if ((pid=waitpid(-1,&status,WNOHANG))>0){
			while (tmp != NULL){
				if (tmp->pid == pid){
					gettimeofday(&tmp->time_finish, NULL);
					return;
				}
				tmp = tmp->suiv;
			}
		}

	}

	struct sigaction p;
    	p.sa_handler = fin_process;
    	sigaction(SIGCHLD, &p, NULL);


*/

	switch(pid){
		case -1:
			return -1;
			break;

		default:
/*
			jobs = ajoutQueue(jobs,pid,l->seq[0][0],0);
			if (!strcmp(l->seq[0][0],"jobs")){
				jobs = changeStatus(jobs);
				afficherListe(jobs);
			}
			jobs = supprimerElementsFinis(jobs);
			if (!(l->bg)){
				while(wait(&statut) != pid);
			}
			if(!strcmp(l->seq[0][0] , "ulimit")){
				change_tmp_execute(l);
			}
*/
			break;
		}
	}
}
