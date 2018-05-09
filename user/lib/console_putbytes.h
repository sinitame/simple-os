#include "../kernel/cpu.h"
#include "../shared/inttypes.h"

#define MEM_VIDEO 0xB8000
#define LIG 24
#define COL 79

uint16_t *ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c);

void efface_ecran(void);

void place_curseur(uint32_t lig, uint32_t col);

void traite_car(char c);

void defilement(void);

void console_putbytes(const char *chaine, int32_t taille);
