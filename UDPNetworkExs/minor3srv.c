/* 	Author: Giovanni Rebosio gdr0051 10849933
	Course: CSCE3600.003
	Date: 4/18/2025
	Description: Server via UDP to recieve ping occasionally
	Modified from Dr. Mosquera's public single_udp_srv.c code. */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>	//for timestamps to calculate ping time
#include <stdlib.h>		//for rand

struct timeval tv;		//timestamps for packets and time for seed

int main(int argc, char* argv[])
{
	int a = 0;
	//make sure port is specified
	if(argc == 2)
	{
		char* changeToInt;
		a = strtol(argv[1], &changeToInt, 10);
		printf("%d\n", a);
		if(!(a > 1023 && a < 49152))
		{
			printf("Specify port in range 1024-49151\n");
			return -1;
		}
	}
	else
	{
		printf("Specify port in range 1024-49151\n");
		return -1;
	}
	
    int socket_desc;
    struct sockaddr_in server_addr, client_addr;
    char server_message[2000], client_message[2000];
    int client_struct_length = sizeof(client_addr);
	unsigned int seed;
	int srvTime = 0;
	int cliTime = 0;

    // Clean buffers:
	memset(server_message, '\0', sizeof(server_message));
	memset(client_message, '\0', sizeof(client_message));

    // Create UDP socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(a);
    server_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    printf("Listening for incoming messages...\n\n");

	while(1) //listen indefinitely
	{
		// Receive client's message:
		if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
			 (struct sockaddr*)&client_addr, &client_struct_length) < 0){
			printf("Couldn't receive\n");
			return -1;
		}
		else
		{
			//get timestamp of recieve
			gettimeofday(&tv,NULL);
			srvTime = tv.tv_usec;
			
			//get seed from current time and use to randomly choose a number 0-9
			seed = tv.tv_sec ^ tv.tv_usec;
			int packetDropped = rand_r(&seed) % 10;
			
			//if packet is not fake dropped
			if(packetDropped > 2)
			{
				//indicate ping recieved
				printf("[client]: Ping\n");
				
				// Calculate ping and send ping time back:
				char* output;
				cliTime = strtol(client_message, &output, 10) + 1;
				srvTime = srvTime - cliTime;
				sprintf(server_message, "%d", srvTime);
				//printf("server_message = %s\n", server_message);
				if (sendto(socket_desc, server_message, strlen(server_message), 0,
					 (struct sockaddr*)&client_addr, client_struct_length) < 0){
					printf("Can't send\n");
					return -1;
					 }
			}
			else	//indicate fake dropped packet
				printf("[server]: dropped packet\n");
				
			// Clean buffers:
			memset(server_message, '\0', sizeof(server_message));
			memset(client_message, '\0', sizeof(client_message));
		}
	}

    // Close the socket:
    close(socket_desc);

    return 0;
}
