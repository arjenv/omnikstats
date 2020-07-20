/*
**	omnikfunctions.c
**
**	few functions to ease recurring math
**
**	Author: Beach
**	May, 23, 2013
*/

/* 
**	ctonr()
**
** 	converts a binary string to a float
**	
**	src: source (binary) string
**	nrofbytes: should be clear, src is not necessarily a NULL terminated string
**	div: the divider. Result of the concersion is divided by this number
**
**	Returns the result (float)
*/

#include <sys/socket.h>
#include <netdb.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

float ctonr(unsigned char * src, int nrofbytes, int div) {
	int i, flag=0;
	float sum=0;

//	sanity check
	if (nrofbytes<=0 || nrofbytes>4) 
		return -1;

	for (i=nrofbytes; i>0; i--) {
		sum += (float) (src[i-1] * pow(256, nrofbytes-i));
		if (src[i-1] == 0xff)
			flag++;
	}
	if (flag == nrofbytes) // all oxff
		sum = 0;
	sum /= (float) div;
	return sum;
}
 
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
**	function yyyymmdd(int datetime);
**
**	Returns either the date (yyyymmdd) or the time(hh:mm)
**	depending on datetime (0=date, 1=time, 2=year, 3=time+seconds)
**
*/

char *getdatetime(char *now, int datetime) {

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	switch(datetime) {
		case 0:
			strftime(now, 10, "%Y%m%d", timeinfo);
		break;
		case 1:
			strftime(now, 10, "%R", timeinfo);
		break;
		case 2:
			strftime(now, 10, "%Y", timeinfo);
		break;
		case 3:
			strftime(now, 10, "%H:%M:%S", timeinfo);
		break;
		default:
			strcpy(now, "no data");
		break;
	}
	return(now);
}

