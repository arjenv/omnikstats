/*
*	omnikreadconfig.c
*
*	Reads the config file and stores the data
*
*	V1.1 sept, 13 2013
*	V1.1 Changes: Added optional IPnumber and Serial number
*	Beach
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>

#include "omnikstats.h"

int omnikreadconfig(void) {

	FILE *pFile;
	char line[128], *pline;
	int len;

	if ((pFile = fopen("omnik.conf", "r")) == NULL) {
		printf("Cannot open config file\n");
		return(1);
	}     
	while ((fgets(line, sizeof line, pFile)) != NULL) {
		len = strlen(line);
//		printf("%s", line); //for debugging
		if ((line == strchr(line, '#')) || (len==1)) {
//			printf("starts with comment, skipping....\n");
			continue; //#=comment, skip line
		}
		for (pline=line; *pline; pline++, len--) {
			//make it all lowercase and get rid of whitespaces
			while (isspace(*pline)) memmove(pline, pline+1, len--);
			*pline = tolower(*pline);
		}
//		printf("nospaces(?) %s\n", line); //for debugging

		if (strstr(line, "omnikurl") != NULL) {
			if (strlen(line) < 68) {
				strcpy(stats.url, &line[8]);
				if (stats.verbose==2) printf("URL: %s\n", stats.url);
			}
		}
		if (strstr(line, "omnikapi") != NULL) {
			if (strlen(line) < 68) {
				strcpy(stats.key, &line[8]);
				if (stats.verbose==2) printf("KEY: %s\n", stats.key);
			}
		}
		if (strstr(line, "systemid") != NULL) {
			if (strlen(line) < 18) {
				strcpy(stats.ID, &line[8]);
				if (stats.verbose==2) printf("ID: %s\n", stats.ID);
			}
		}
		if (strstr(line, "ipnumber") != NULL) {
			if (strlen(line) < 25) {
				strcpy(stats.IPnumber, &line[8]);
				if (stats.verbose==2) printf("IP: %s\n", stats.IPnumber);
			}
		}
		if (strstr(line, "snumber") != NULL) {
			if (strlen(line) < 25) {
				stats.serial_number = atol(&line[7]);
				if (stats.verbose==2) printf("SN: %li\n", stats.serial_number);
			}
		}

	}
	return(0);
}

