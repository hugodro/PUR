#include <stdio.h>
#include <ctype.h>

main(int argc, char **argv)
{
    int i;

    printf("    unsigned short _pctype[256]= {\t\n");

    for (i= 0; i < 256; i++) {
	printf("%2x, ", _pctype[i]);
	if (((i + 1) % 5) == 0) {
	    printf("\n\t");
	}
    }
    printf("    };\n\n");
    printf("int mb_cur_max= %d;\n", __mb_cur_max);
}
