#include <stdio.h>
#include <stdlib.h>


char *load_file(char *fname) {
	FILE *txt;
	txt = fopen(fname, "r");
	fseek(txt, 0L, SEEK_END);
	size_t s = ftell(txt);
	rewind(txt);
	char *t = malloc(s * sizeof(char));
	size_t f = fread(t, sizeof(char), s, txt);

	fclose(txt);
	return t;

}
