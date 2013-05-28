#include <stdio.h>
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
	}
	return(0);
}

