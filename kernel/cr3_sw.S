.text
.globl cr3_sw
# l'adresse du nouveau pagedir est a esp+4
# prototype void cr3_sw(void * new_pagedir)
cr3_sw:
movl 4(%esp), %cr3
ret
