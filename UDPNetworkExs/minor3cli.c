/* 	Author: Giovanni Rebosio gdr0051 10849933
	Course: CSCE3600.003
	Date: 4/18/2025
	Description: Client to server via UDP to ping occasionally
	Modified from Dr. Mosquera's public single_udp_cli.c code. */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>		
#include <sys/time.h>	//for timestamps to calculate ping time
#include <stdlib.h>		//for strtok

struct timeval tv;		//time for packets
struct timeval timeout;	//time for timeout

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
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    int server_struct_length = sizeof(server_addr);
	int cliTime = 0;		//packet to server time
	int srvTime = 0;		//packet to client time
	int lostPackets = 0;	//dropped packet count
	int packetTimes[10];	//all packet times
	fd_set readfds;
	
	// Clean buffers:
	memset(server_message, '\0', sizeof(server_message));
	memset(client_message, '\0', sizeof(client_message));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	

    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(a);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//loop 10 times to send 10 pings
	for(int i = 0; i < 10; i++)
	{
		//set fds
		FD_ZERO(&readfds);
		FD_SET(socket_desc, &readfds);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		
		// Clean buffers:
		memset(server_message, '\0', sizeof(server_message));
		memset(client_message, '\0', sizeof(client_message));

		// Send a ping to server, noting the time:
		gettimeofday(&tv,NULL);
		cliTime = tv.tv_usec;
		sprintf(client_message, "%d", cliTime);
		if(sendto(socket_desc, client_message, strlen(client_message), 0,
			 (struct sockaddr*)&server_addr, server_struct_length) < 0){
			printf("Unable to send message\n");
			return -1;
		}
		printf("%d: Sent... ", i);	//indicate packet was sent
		
		//wait for 1 second or until server replies
		int waiting = select(socket_desc + 1, &readfds, NULL, NULL, &timeout);
		if(waiting == 0)	//server did not respond
		{
			printf("Timed Out\n");
			lostPackets++;
			packetTimes[i] = 0;
		}
		else if(waiting < 0)	//something is wrong with select
		{
			printf("Select error\n");
			return -1;
		}
		else	//server replied
		{
			// Receive the server's response:
			if(recvfrom(socket_desc, server_message, sizeof(server_message), 0,
			 (struct sockaddr*)&server_addr, &server_struct_length) < 0){
			printf("Error while receiving server's msg\n");
			return -1;
			}
			
			//note time recieved
			gettimeofday(&tv,NULL);
			cliTime = tv.tv_usec;
			
			//calculate time it took
			char* output;
			srvTime = strtol(server_message, &output, 10) + 1;
			cliTime = (cliTime - srvTime) / 1000;
			sprintf(server_message, "%d", cliTime);
			
			//print time it took
			printf("RTT=.%sms\n", server_message);
			
			//add packet time to array of times
			srvTime = strtol(server_message, &output, 10) + 1;
			packetTimes[i] = srvTime;
		}
	}
	
	//Calculate end results
	int avgMs = 0;
	int minTime = 1000;
	int maxTime = 0;
	for(int i = 0; i < 10; i++)
	{
		if(packetTimes[i] != 0)	//if time did not timeout
		{
			avgMs = avgMs + packetTimes[i];	
			if(packetTimes[i] < minTime)	//if this time is smaller than current min
				minTime = packetTimes[i];
			if(packetTimes[i] > maxTime)	//if this time is larger than current max
				maxTime = packetTimes[i];
		}
	}
	avgMs = avgMs / 10;
	
	//Print end results
	printf("10 packets sent, %d packets recieved, %d%% packets lost\n", 10 - lostPackets, lostPackets * 10);
	printf("min: .%dms, max: .%dms, avg: .%dms\n", minTime, maxTime, avgMs);
	
    // Close the socket:
    close(socket_desc);

    return 0;
}
