#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>


#include "multiplayer.h"
#include "loader.h"

#define PLAYER_NUM 2


void start_server(txt_segment ts) {
	struct sockaddr_in serv, cli;
	struct pollfd players[PLAYER_NUM+1];
	player parray[PLAYER_NUM];
	memset(&players, 0, sizeof(players));
	/*
	char *txtpass = load_file("a.txt");
	printf("%s\n", txtpass);
	printf("%d\n", strlen(txtpass));
	*/
	printf("%d\n", ts.size);
	printf("%s\n", ts.buf);
	int opt = 1;
	int pind = 1;
	int waiting_for_players = 1;
	int p_num = 0;
	int len;
	char buf[100];
	memset(&buf, 0, sizeof(buf));
	int soc = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9001);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(soc, (struct sockaddr *) &serv, sizeof(serv));
	listen(soc, 4);

	players[0].fd = soc;
	players[0].events = POLLIN;

	memset(&cli, 0, sizeof(cli));
	socklen_t cs = sizeof(cli);	

	// loop that waits for players to connect

	while(waiting_for_players) {
		
		poll(players, 4, -1);

		
		for(int i = 0; i < pind; i++) {
			if(players[i].revents & POLLIN) {
				if(players[i].fd == soc && p_num <= PLAYER_NUM - 1) {
					// new client wants to connect
					len = sizeof(cli);
					memset(&cli, 0, sizeof(cli));
					players[pind].fd = accept(soc, (struct sockaddr *) &cli, &len); 
					recv(players[pind].fd, parray[p_num].uname, sizeof(parray[p_num].uname), 0);
					write(STDOUT_FILENO, parray[p_num].uname, strlen(parray[p_num].uname));
					players[pind].events = POLLIN;
					parray[p_num].txt_pos = 0;
					parray[p_num].comp_pos = 0;
					p_num += 1;
					pind += 1;

				} else {
					char out;
					recv(players[i].fd, &out, sizeof(char), 0);
					if(iscntrl(out)) {
						printf("%s left\n", parray[i-1].uname);
						//exit(0);
						close(players[i].fd);
					}
					//printf("%d\n", parray[i-1].txt_pos);
					//printf("%c\n", out);

				}
			} else if(p_num > PLAYER_NUM - 1) {
				write(STDOUT_FILENO, "Full\n", 5);
				waiting_for_players = 0;
			}
			
			
			

		}
	}

	for(int i = 1; i < pind; i++) {
		send(players[i].fd, &ts.size, sizeof(int), 0);
		send(players[i].fd, ts.buf, sizeof(char) * ts.size, 0);

	}

	while(1) {
		poll(players, 4, -1);
		
		for(int i = 1; i < pind; i++) {
			if(players[i].revents & POLLIN) {
				char in;
				recv(players[i].fd, &in, sizeof(char), 0); 
				write(STDOUT_FILENO, &in, 1);
			}
		}
	}




}
