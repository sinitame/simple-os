#include "kbd.h"
#include "stdio.h"
#include <string.h>
#include "mem.h"
#include "../shared/errno.h"
#include "../shared/stdlib.h"
#include <cpu.h>
#include <inttypes.h>




char *strdup(const char *src)
{
  char *str;
	int len = strlen(src);
	str = mem_alloc(len+1);
  if (str) {
    strcpy(str,src);
  }
  return str;


}

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
  char ascii_car = '.';
  unsigned long indice = 0;
	if (length <= 0) {
		return 0;
	}


	do {
    if (!empty_buff(&stdin)){
      get_buffer(&stdin,&ascii_car);

      switch(ascii_car){
        case 13 :
          break;
        case 127 :
          if (indice>0) {
            string[--indice] = 0;
          }
          break;
        default :
          string[indice++] = ascii_car;
          break;
        }
    }
	} while (ascii_car !=13 && indice < length);
  return indice;

}

int cons_write(const char *str, long size){
  /*int i;

  if ((int)strlen(str) >= size){
    for ( i=0; i<size;i++){
      printf("%c",str[i]);
    }
    return 0;
  } else{
    return -1;
  }
  */
  console_putbytes(str,size);
  return size;
}

void cons_echo(int on){
  on = !on;
}

void keyboard_data(char *str){


    for (int i = 0; str[i] != '\0'; i++) {
      if ((0<=str[i])&&(str[i]<127)){
        traite_car(str[i]);
      } else if (str[i] == 127){
        printf("\b");
        printf(" ");
        printf("\b");
      }
        add_buff(&stdin, str[i]);
    }





}

void tic_CLAVIER(void){
  outb(0x20,0x20);
  uint8_t code = inb(0x60);
  do_scancode((uint32_t)code);
  ordonnancement();
}
