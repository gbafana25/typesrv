#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>


#include "multiplayer.h"
#include "loader.h"
#include "config.h"

#define PLAYER_NUM player_num
#define POLL_ARRAY_NUM PLAYER_NUM+1


void start_server(txt_segment ts) {
	struct sockaddr_in serv, cli;
	struct pollfd players[POLL_ARRAY_NUM];
	player parray[PLAYER_NUM];
	memset(&players, 0, sizeof(players));
	memset(&parray, 0, sizeof(parray));
	
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
		
		poll(players, POLL_ARRAY_NUM, -1);

		
		for(int i = 0; i < pind; i++) {
			if(players[i].revents & POLLIN) {
				if(players[i].fd == soc && p_num <= PLAYER_NUM - 1) {
					// new client wants to connect
					len = sizeof(cli);
					memset(&cli, 0, sizeof(cli));
					players[pind].fd = accept(soc, (struct sockaddr *) &cli, &len); 
					//recv(players[pind].fd, parray[p_num].uname, sizeof(parray[p_num].uname), 0);
					//write(STDOUT_FILENO, parray[p_num].uname, strlen(parray[p_num].uname));
					players[pind].events = POLLIN;
					p_num += 1;
					pind += 1;

				} else {
					char out;
					recv(players[i].fd, &out, sizeof(char), 0);
					if(iscntrl(out)) {
						//exit(0);
						close(players[i].fd);
					}

				}
			} else if(p_num > PLAYER_NUM - 1) {
				waiting_for_players = 0;
			}
			
			
			

		}
	}

	for(int i = 1; i < pind; i++) {
		send(players[i].fd, &ts.size, sizeof(int), 0);
		send(players[i].fd, ts.buf, sizeof(char) * ts.size, 0);
		parray[pind-1].txt_pos = 0;

	}

	int place = 1;
	while(1) {
		poll(players, POLL_ARRAY_NUM, -1);
		char sp = ' ';	
		for(int i = 1; i < pind; i++) {
			if(players[i].revents & POLLIN) {
				char in;
				recv(players[i].fd, &in, sizeof(char), 0); 
				//write(STDOUT_FILENO, &ts.buf[parray[i].txt_pos], 1);
				int res;
				if(strncmp(&in, &ts.buf[parray[i].txt_pos], 1) == 0) {
					//printf("Correct\n");
					res = 1;
					send(players[i].fd, &res, sizeof(int), 0);
					parray[i].txt_pos += 1;
					
						
				} else {
					res = 0;
					send(players[i].fd, &res, sizeof(int), 0);
				}	
				
				if(parray[i].txt_pos == ts.size) {
					printf("\nPlayer %d finished!\n", i);
					res = 2;
					send(players[i].fd, &res, sizeof(int), 0);
					//send(players[i].fd, &place, sizeof(int), 0);
					//printf("%c\n", place+48);
					place += 1;
					close(players[i].fd);
				}
			}
		}
	}



}
