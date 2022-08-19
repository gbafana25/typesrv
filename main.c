#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>

#include "loader.h"
#include "server.h"


int main() {
	txt_segment pa;		
	char *d = load_file("a.txt");
	pa.size = strlen(d);
	pa.buf = d;
	//printf("Segment length: %d\n", pa.size);
	//printf("%s\n", pa.buf);
	start_server(pa);




}
