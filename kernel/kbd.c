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
    
    buf->buffer = mem_alloc(taille * sizeof(char));
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


void keyboard_data(char *str){


  char data;

  printf("Codes ASCII de la touche : ");
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%d ", str[i]);
        add_buff(&stdin, str[i]);
    }
    if ((str[0] >= 32) && (str[0] < 127)) {
        get_buffer(&stdin,&data);
        printf(", caractère correspondant : '%c' ", data);
    }
    printf("\n");



}

void tic_CLAVIER(void){
  outb(0x20,0x20);
  uint8_t code = inb(0x60);
  do_scancode((uint32_t)code);
}
