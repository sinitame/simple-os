/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 *           Matthieu Moy 2008                       *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "readcmd.h"
#include "mem.h"
#include "kbd.h"
/*
static void *xrealloc(void *ptr, size_t previous_size, size_t size)
{
	void *p = mem_alloc(size);
	if (!p) return NULL;

	memcpy(p,ptr,previous_size);
	mem_free(ptr, previous_size);
	return p;
}
*/


/* Read a line from standard input and put it in a char[] */
char *readline(char *prompt)
{
	size_t buf_len = 16;
	char *buf = mem_alloc(buf_len * sizeof(char));

	printf("%s", prompt);
	int n = cons_read(buf,16);
	if (n>0){
		return(buf);
	}
	else{
		xfree(buf);
		return NULL;
	}
	printf("\n");

}



#define READ_CHAR *(*cur_buf)++ = *(*cur)++
#define SKIP_CHAR (*cur)++

static void read_single_quote(char ** cur, char ** cur_buf) {
	SKIP_CHAR;
	while(1) {
		char c = **cur;
		switch(c) {
                case '\'':
                        SKIP_CHAR;
                        return;
                case '\0':
                        //fprintf(stderr, "Missing closing '\n");
												printf("Missing closing '\n");
                        return;
                default:
                        READ_CHAR;
                        break;
                }
	}
}

static void read_double_quote(char ** cur, char ** cur_buf) {
	SKIP_CHAR;
	while(1) {
		char c = **cur;
		switch(c) {
		case '"':
			SKIP_CHAR;
			return;
		case '\\':
			SKIP_CHAR;
			READ_CHAR;
			break;
                case '\0':
                        //fprintf(stderr, "Missing closing \"\n");
												printf("Missing closing \"\n");
                        return;
		default:
			READ_CHAR;
			break;
		}
	}
}

static void read_word(char ** cur, char ** cur_buf) {
	while(1) {
		char c = **cur;
		switch (c) {
		case '\0':
		case ' ':
		case '\t':
		case '<':
		case '>':
		case '|':
			**cur_buf = '\0';
			return;
		case '\'':
			read_single_quote(cur, cur_buf);
			break;
		case '"':
			read_double_quote(cur, cur_buf);
			break;
		case '\\':
			SKIP_CHAR;
			READ_CHAR;
			break;
		default:
			READ_CHAR;
			break;
		}
	}
}

/* Split the string in words, according to the simple shell grammar. */
static char **split_in_words(char *line)
{
	char *cur = line;
	char *buf = mem_alloc(strlen(line) + 1);
	char *cur_buf;
	char **tab = 0;
	size_t l = 0;
	char c;

	while ((c = *cur) != 0) {
		char *w = 0;
		switch (c) {
		case ' ':
		case '\t':
			/* Ignore any whitespace */
			cur++;
			break;
		case '&':
		        w = "&";
			cur++;
			break;
		case '<':
			w = "<";
			cur++;
			break;
		case '>':
			w = ">";
			cur++;
			break;
		case '|':
			w = "|";
			cur++;
			break;
		default:
			/* Another word */
			cur_buf = buf;
			read_word(&cur, &cur_buf);
			w = strdup(buf);
		}
		if (w) {
			tab = xrealloc(tab ,(l + 1) * sizeof(char *));
			tab[l++] = w;
		}
	}
	tab = xrealloc(tab, (l + 1) * sizeof(char *));
	tab[l++] = 0;
	xfree(buf);
	return tab;
}



/* Fonctions perso pour bien parser les arguments */
void read_word_cmd(char ** cur, char ** cur_buf) {
	while(1) {
		char c = **cur;
		switch (c) {
		case '\0':
		case '/':
		case '|':
			**cur_buf = '\0';
			return;

		default:
			READ_CHAR;
			break;
		}
	}
}

//permet de récuperer le nom de la commande exécutée
char *get_cmd_name(char *cmd)
{
	char *cur = cmd;
	char *buf = mem_alloc(strlen(cmd) + 1);
	char *cur_buf;
	char **tab = 0;
	size_t l = 0;
	char c;

	while ((c = *cur) != 0) {
		char *w = 0;
		switch (c) {
		case '/':
			/* On ignore les slashs */
			cur ++;
			break;
		default:
			/* Nouveau mot */
			cur_buf = buf;
			read_word_cmd(&cur, &cur_buf);
			w = strdup(buf);
		}
		if (w) {
			tab = xrealloc(tab,(l + 1) * sizeof(char *));
			tab[l++] = w;
		}
	}
	tab = xrealloc(tab,(l + 1) * sizeof(char *));
	tab[l++] = 0;
	xfree(buf);
	return tab[l-2];
}

//Renvoie un tableaux avec les arguments. Ex: ["ls","/usr", "NULL] pour la commande bin/ls /usr
char **arguments(char **liste_mots){
	char **arg = 0;
	int l = 1;
	arg = xrealloc(arg, sizeof(char *));
	arg[0] = get_cmd_name(liste_mots[0]);

	while(liste_mots[l] != 0){
		if (liste_mots[l]){
			arg = xrealloc(arg,(l+1)*sizeof(char *));
			arg[l] = liste_mots[l];
			l++;
		}
	}
	arg = xrealloc(arg,(l+1)*sizeof(char *));
	arg[l++]=NULL;

	return arg;
}

static void xfreeseq(char ***seq)
{
	int i, j;

	for (i=0; seq[i]!=0; i++) {
		char **cmd = seq[i];

		for (j=0; cmd[j]!=0; j++) xfree(cmd[j]);
		xfree(cmd);
	}
	xfree(seq);
}


/* xfree the fields of the structure but not the structure itself */
static void xfreecmd(struct cmdline *s)
{
	if (s->in) xfree(s->in);
	if (s->out) xfree(s->out);
	if (s->seq) xfreeseq(s->seq);
}

int sizecmd(struct cmdline *l){
	int n = 0;
	int i;
	for (i=0; l->seq[i]!=0; i++) {
		n++;
	}
	return n;
}


struct cmdline *parsecmd(char **pline)
{
	char *line = *pline;
	static struct cmdline *static_cmdline = 0;
	struct cmdline *s = static_cmdline;
	char **words;
	int i;
	char *w;
	char **cmd;
	char ***seq;
	size_t cmd_len, seq_len;


	if (line == NULL) {
		if (s) {
			xfreecmd(s);
			xfree(s);
		}
		return static_cmdline = 0;
	}

	cmd = mem_alloc(sizeof(char *));
	cmd[0] = 0;
	cmd_len = 0;
	seq = mem_alloc(sizeof(char **));
	seq[0] = 0;
	seq_len = 0;

	words = split_in_words(line);

	xfree(line);
	*pline = NULL;

	if (!s)
		static_cmdline = s = mem_alloc(sizeof(struct cmdline));
	else
		xfreecmd(s);
	s->err = 0;
	s->in = 0;
	s->out = 0;
	s->seq = 0;
	s->bg = 0;

	i = 0;
	while ((w = words[i++]) != 0) {
		switch (w[0]) {
		case '<':
			/* Tricky : the word can only be "<" */
			if (s->in) {
				s->err = "only one input file supported";
				goto error;
			}
			if (words[i] == 0) {
				s->err = "filename missing for input redirection";
				goto error;
			}
			s->in = words[i++];
			break;
		case '>':
			/* Tricky : the word can only be ">" */
			if (s->out) {
				s->err = "only one output file supported";
				goto error;
			}
			if (words[i] == 0) {
				s->err = "filename missing for output redirection";
				goto error;
			}
			s->out = words[i++];
			break;
		case '&':
			/* Tricky : the word can only be "&" */
			if (cmd_len == 0) {
				s->err = "misplaced ampersand";
				goto error;
			}
			if (s->bg == 1) {
				s->err = "only one ampersand supported";
				goto error;
			}
			s->bg = 1;
			break;
		case '|':
			/* Tricky : the word can only be "|" */
			if (cmd_len == 0) {
				s->err = "misplaced pipe";
				goto error;
			}

			seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
			seq[seq_len++] = cmd;
			seq[seq_len] = 0;

			cmd = mem_alloc(sizeof(char *));
			cmd[0] = 0;
			cmd_len = 0;
			break;
		default:
			cmd = xrealloc(cmd,(cmd_len + 2) * sizeof(char *));
			cmd[cmd_len++] = w;
			cmd[cmd_len] = 0;
		}
	}

	if (cmd_len != 0) {
		seq = xrealloc(seq, (seq_len + 2) * sizeof(char **));
		seq[seq_len++] = cmd;
		seq[seq_len] = 0;
	} else if (seq_len != 0) {
		s->err = "misplaced pipe";
		i--;
		goto error;
	} else
		xfree(cmd);
	xfree(words);
	s->seq = seq;
	return s;
error:
	while ((w = words[i++]) != 0) {
		switch (w[0]) {
		case '<':
		case '>':
		case '|':
			break;
		default:
			xfree(w);
		}
	}
	xfree(words);
	xfreeseq(seq);
	for (i=0; cmd[i]!=0; i++) xfree(cmd[i]);
	xfree(cmd);
	if (s->in) {
		xfree(s->in);
		s->in = 0;
	}
	if (s->out) {
		xfree(s->out);
		s->out = 0;
	}
	return s;
}
