#include "kbd.h"
#include "stdio.h"
#include <string.h>
#include "mem.h"
#include "../shared/errno.h"
#include "../shared/stdlib.h"
#include <cpu.h>
#include <inttypes.h>





int init_buff(buffer_clavier * buf, int taille)
{

    buf->buffer = mem_alloc(taille * sizeof(int));
    if(buf->buffer != NULL)
    {
        buf->taille = taille;
        buf->w = 0;
        buf->r = 0;
        return 0;
    }
      return -1;

}

int empty_buff(buffer_clavier * buf){
  return (buf->r == buf->w);
}

int full_buff(buffer_clavier *buf){
  return ((buf->r + 1) % buf->taille) == buf->w;
};

int add_buff(buffer_clavier *buf, char c){

    if(buf)
    {
        buf->buffer[buf->w] = c;
        buf->w = (buf->w + 1) % buf->taille;
        if(buf->w == buf->r)
        {
            buf->r = (buf->r + 1) % buf->taille;
        }

        return 0;
    }
      return -1;

}

int get_buffer(buffer_clavier *buf, char * c){

    if(buf && c && !empty_buff(buf))
    {
        *c = buf->buffer[buf->r];
        buf->r = (buf->r + 1) % buf->taille;

        return 0;
    } else {
      return -1;
    }
}

unsigned long cons_read(char *string, unsigned long length)
{
  char ascii_car;
  unsigned long indice = 0;
	if (length <= 0) {
		return 0;
	}


	do {
    if (!empty_buff(&stdin)){
      get_buffer(&stdin,&ascii_car);
  		if (ascii_car != 13) {
  			string[indice] = ascii_car;
        indice ++;
      }
    }

	} while (ascii_car !=13 && indice < length);
  return indice;

}

int cons_write(const char *str, long size){
  int i;

  if ((int)strlen(str) >= size){
    for ( i=0; i<size;i++){
      printf("%c",str[i]);
    }
    return 0;
  } else{
    return -1;
  }
}

void cons_echo(int on){
  on = !on;
}

void keyboard_data(char *str){


  printf("Codes ASCII de la touche : ");
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%d ", str[i]);
        add_buff(&stdin, str[i]);
    }

    printf("\n");



}

void tic_CLAVIER(void){
  outb(0x20,0x20);
  uint8_t code = inb(0x60);
  do_scancode((uint32_t)code);
}
