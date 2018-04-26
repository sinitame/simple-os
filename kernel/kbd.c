#include "kbd.h"
#include "stdio.h"
#include <cpu.h>
#include <inttypes.h>

void keyboard_data(char *str){
  printf("Codes ASCII de la touche : ");
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%d ", str[i]);
    }
    if ((str[0] >= 32) && (str[0] < 127)) {
        printf(", caractère correspondant : '%c' ", str[0]);
    }
    printf("\n");
}

void tic_CLAVIER(void){
  outb(0x20,0x20);
  uint8_t code = inb(0x60);
  do_scancode((uint32_t)code);
}
