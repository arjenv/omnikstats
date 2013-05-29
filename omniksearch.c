/*
** 	omniksearch.c
**
**	Sends a broadcast message via UDP over the LAN network. 
**	Message = "WIFIKIT-214028-READ";
**
**	The Omnik should reply with its IP number and serial number,
**	that are required to connect via TCP and get statistics from 
**	the Omnik
**
**	Returns: None zero on error
**	The IP address of the Omnik
**	The Serial number of the Omnik
** 
** 	Author: Beach
** 	V1.0 may 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <string.h>
#include <errno.h>

#include "omnikstats.h"

int omniksearch(char *Omnik_address, long *serialnr) {

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int bytes_received, bytes_sent;
	struct sockaddr_storage their_address;
	char buffer[MAXBUFLEN], *serialnumber;
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	fd_set read_handle;
	struct timeval timeout_interval;
	int retval;
	struct ifaddrs *ifaddr, *ifa;
	int family, sinfo;
	char host[NI_MAXHOST], broadcast[NI_MAXHOST];
	char message[20] = "WIFIKIT-214028-READ";
	int allowbc = 1;
	struct sockaddr_in broadcast_address;
	struct hostent *he;
	int Omnikreply = 0;
	int totaltimeout = 0;
     

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return(EXIT_FAILURE);
	}

	// Walk through linked list, maintaining head pointer so we can free list later

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
		continue;

		family = ifa->ifa_addr->sa_family;

		// Display interface name and family (including symbolic form of the latter for the common families)
                
		if ((strstr(ifa->ifa_name, "lo") == NULL) && (family == AF_INET)) {
			if (stats.verbose > 1) 
				printf("%s  address family: %d%s\n", ifa->ifa_name, family,
					(family == AF_PACKET) ? " (AF_PACKET)" :
					(family == AF_INET) ?   " (AF_INET)" :
					(family == AF_INET6) ?  " (AF_INET6)" : "");

			// For an AF_INET* interface address, display the address

			if (family == AF_INET || family == AF_INET6) {
				sinfo = getnameinfo(ifa->ifa_addr,
					(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
					host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (sinfo != 0) {
					printf("getnameinfo() failed: %s\n", gai_strerror(sinfo));
					return(EXIT_FAILURE);
				}
				if (stats.verbose>1)
					printf("\thost address: <%s>\n", host);

				sinfo = getnameinfo(ifa->ifa_broadaddr,
					(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
					broadcast, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (sinfo != 0) {
					printf("getnameinfo() failed: %s\n", gai_strerror(sinfo));
					return(EXIT_FAILURE);
				}
				if (stats.verbose>1)
					printf("\tbroadcast address: <%s>\n", broadcast);
			}
		}
	}
	freeifaddrs(ifaddr);

	if ((he=gethostbyname(broadcast)) == NULL) {  // get the host info
		perror("gethostbyname");
		return(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, C_UDPPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			printf("omniksearch: socket %s %d\n", strerror(errno), errno);
			continue;
		}

//		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &allowbc, sizeof allowbc) == -1) {
//			printf("Error setsockopt (SO_REUSEADDR) %s %d\n", strerror(errno), errno);
//		}
	
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			printf("omniksearch: bind %s %d\n", strerror(errno), errno);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

    // this call is what allows broadcast packets to be sent:
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &allowbc, sizeof allowbc) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        return(1);
    }

	broadcast_address.sin_family = AF_INET;     // host byte order
	broadcast_address.sin_port = htons(UDPPORT); // short, network byte order
	broadcast_address.sin_addr = *((struct in_addr *)he->h_addr);
	memset(broadcast_address.sin_zero, '\0', sizeof broadcast_address.sin_zero);

	addr_len = sizeof their_address;

	// send the message

	if ((bytes_sent=sendto(sockfd, message, strlen(message), 0,
             (struct sockaddr *)&broadcast_address, sizeof broadcast_address)) == -1) {
		perror("sendto");
		return(1);
	}

	if (stats.verbose > 1)
		printf("sent %d bytes to %s\n", bytes_sent, inet_ntoa(broadcast_address.sin_addr));


	while((!Omnikreply) && (totaltimeout < 10)) {
		FD_ZERO(&read_handle);
		FD_SET(sockfd, &read_handle);

		timeout_interval.tv_sec = 2;
		timeout_interval.tv_usec = 500000;

		retval = select(sockfd+1, &read_handle, NULL, NULL, &timeout_interval);
		if (retval == -1)
			printf("Select error\n");
		else if (retval == 0) {
			printf("timeout\n");
			totaltimeout++; //avoid endless loop
		}
		else {
			// got some message	
			if (FD_ISSET(sockfd, &read_handle)) {
				addr_len = sizeof(their_address);
				if ((bytes_received = recvfrom(sockfd, buffer, MAXBUFLEN-1, 0, (struct sockaddr *) &their_address, &addr_len)) <0) {
					perror("Error in recvfrom.\n");
					break;
				}
				strcpy(Omnik_address, inet_ntop(their_address.ss_family, get_in_addr((struct sockaddr *)&their_address), s, sizeof s));
				if (strcmp(Omnik_address, host) != 0) 
					Omnikreply = 1;
			}
		}
	}
	if (stats.verbose > 1)
		printf("Omnik replied: %s\n", buffer);
	close(sockfd);
	if (strstr(buffer, Omnik_address) == NULL) { // Nop this is not from the Omnik
		printf("Reply is not from the Omnik");
		return(1);
	}

	if ((serialnumber = (strrchr(buffer, ','))) == NULL) {
		printf("No serial number found!\n");
		return(1);
	}
	*serialnr = atol(&serialnumber[1]);
	return(0);
}

