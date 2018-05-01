/*
 * Ensimag - Projet système
 * Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
 *
 * Stub for console_putbytes system call.
 */

 #include "../shared/console.h"
 #include "../../kernel/cpu.h"
 #include "../shared/inttypes.h"
 #include <string.h>
 #include <stdio.h>

 #define MEM_VIDEO 0xB8000
 #define LIG 24
 #define COL 79

 static uint32_t curseur_lig, curseur_col;
 void place_curseur(uint32_t lig, uint32_t col);

 uint16_t *ptr_mem(uint32_t lig, uint32_t col){
 	return (uint16_t*)(MEM_VIDEO+2*(lig*80+col));
 	}

 void ecrit_car(uint32_t lig, uint32_t col, char c){
 	uint16_t* adr;
 	adr=ptr_mem(lig,col);
 	*adr = 15 << 8; //blanc sur fond noir par defaut
 	*adr=*adr+c;
 	}

 void efface_ecran(void){
 	int i,j;
 	for(i=0;i<=LIG;i++){
 		for(j=0;j<=COL;j++){
 			ecrit_car(i,j,' ');
 		}
 	}
 	place_curseur(0, 0);
 }


 void place_curseur(uint32_t lig, uint32_t col){
 	uint16_t pos;
 	uint8_t haut,bas;
 	pos=col + (80*lig);
 	outb(0x0f, 0x3d4);

 	bas=pos & 0x00ff;
 	outb(bas, 0x3d5);

 	outb(0x0e, 0x3d4);

 	pos=pos>>8;
 	haut=pos & 0x00ff;
 	outb(haut, 0x3d5);
 	curseur_lig=lig;
 	curseur_col=col;


 }

 void defilement(void){
  memmove(ptr_mem(0,0),ptr_mem(1,0),128);
  memmove(ptr_mem(1,0),ptr_mem(2,0),(LIG)*COL*2);
  //place_curseur(curseur_lig+1,0);
  for(int j=0;j<=COL;j++){
    ecrit_car(LIG,j,' ');
  }
  place_curseur(LIG,0);
 }

 void traite_car(char c){
 	if (c<32) {
 		switch(c){
 			case 8:
 				if (curseur_lig>0) { place_curseur(curseur_lig, curseur_col-1); }
 			break;

 			case 9:
 				if (curseur_col<COL-7) { place_curseur(curseur_lig, curseur_col+8); }
 				else { place_curseur(curseur_lig, COL); }
 			break;

 			case 10:
 				if (curseur_lig<LIG) {
 					place_curseur(curseur_lig+1, 0);
 				}
 				else{
 					defilement();
 				}
 			break;

 			case 12:
 				efface_ecran();
 			break;

 			case 13:
 				place_curseur(curseur_lig,0);
 			break;
 		}
 	}
 	else {
 		ecrit_car(curseur_lig, curseur_col, c);
 		if (curseur_col==COL && curseur_lig<LIG) {
 			place_curseur(curseur_lig+1,0);
 		}
        else if(curseur_lig>=LIG && curseur_col==COL){
            defilement();
        }
 		else {
 			place_curseur(curseur_lig,curseur_col+1);
 		}
 	}
}

 void console_putbytes(const char *chaine, int taille){
 	int i;
 	for(i=0;i<taille;i++){
 		traite_car(chaine[i]);
 	}
 }
