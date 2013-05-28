/*
**	omnikgetstats.c
** 
**	Connects to the Omnik via TCP
**	Generates and sends the 'magic' message to the Omnik
**	receives the statistics
**
**	Returns: None zero on error
**	binary data (string) from the Omnik
** 
**	Author: Beach
**	V1.0 may,23 2013
*/

#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>

#include "omnikstats.h"


int omnikgetstats(char *Omnik_address, long serialnr, char *server_reply) {

	int checksum = 0;
	int i;
	int socket_desc;
	struct sockaddr_in server;
     
	// generate the magic message
	// first 4 bytes are fixed x68 x02 x40 x30
	// next 8 bytes are the reversed serial number twice(hex)
	// next 2 bytes are fixed x01 x00
	// next byte is a checksum (2x each binary number form the serial number + 115)
	// last byte is fixed x16

	char magicmessage[] = {0x68, 0x02, 0x40, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x16};
	for (i=0; i<4; i++) {
		magicmessage[4+i] = magicmessage[8+i] = ((serialnr>>(8*i))&0xff);
		checksum += magicmessage[4+i];
	}
	checksum *= 2;
	checksum += 115;
	checksum &= 0xff;
	magicmessage[14] = checksum;
//	printf("checksum %x\n", checksum);
//	for (i=0; i<16; i++) {
//		printf("%x ", (unsigned char) magicmessage[i]);
//	}
//	printf("\n\n");

	// Now create a TCP socket for communication with the Omnik
	if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0)) == -1) {
		printf("Could not create TCP socket");
		return(1);
	}
         
	server.sin_addr.s_addr = inet_addr(Omnik_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(OMNIKPORT); //port 8899
 
	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0) {
		printf("TCP connect error: %s\n", strerror(errno));
		return(1);
	}
     
	puts("Connected\n");
     
	//Send the magic message
	if (send(socket_desc , magicmessage , 16 , 0) < 0) {
		puts("Send failed");
		return(1);
	}
//	printf("Data Send %s\n", (char*) magicmessage);
     
	//Receive a reply from the server

	memset(server_reply, 0, 256);
	if ((i = (recv(socket_desc, server_reply , 256 , 0))) < 0) {
		puts("recv failed");
		return(1);
	}
//	printf("Reply received %d nr of Bytes\n", i);
//	for (i=0; i<256; i++) {
//		printf("Byte %d\t%#x\t%u\t%c\n",i,server_reply[i], server_reply[i], server_reply[i]);
//	}
	return(0);
}
