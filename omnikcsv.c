/*
**	omnikcsv.c
** 
**	Prints the statistics to a CSV file
**
**	Returns: None
** 
**	Author: Beach
**	V1.0 may,24 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "omnikstats.h"


void omnikcsv(void) {

	int i;
	char date[10];
	char now[10];
	struct stat buffer;
	char filename[200];
	FILE *pFile;

	// check if file exists, otherwise create it 
	strcpy(filename, getdatetime(now, 2));
	strcat(filename, "/");
	strcat(filename, getdatetime(now,2));
	strcat(filename, "data.csv");
	printf("filename = %s\n", filename);
	strcpy(stats.filename, filename);
 
	if (stat(filename, &buffer) != 0) {
		printf("%s does not exist. Creating file....\n", filename);
		if ((i=mkdir(getdatetime(now, 2),S_IRWXU|S_IRWXG|S_IROTH)) != 0) {
			printf("Could not create directory %s, %s\n", getdatetime(now,2), strerror(errno));
			exit(1);
		}
		if ((pFile = fopen(filename, "w+")) == NULL) {
			printf("Could not create file %s\n", filename);
			exit(1);
		}
		fprintf(pFile, "Date,Time,Powertoday, PowerNow,Temperature,Voltage(DC),Current(DC), Voltage(AC),Current(AC),Frequency,TotalPower,Hours\n");
		fclose(pFile);
	}

	pFile = fopen(filename, "a");
	fprintf(pFile, "%s,%s,%.0f,%.1f,%.1f,%.1f,%.1f,%.1f,%.2f,%.2f,%.1f,%.0f\n", 
		getdatetime(date, 0),
		getdatetime(now, 3),
		stats.PowerToday,
		stats.PVPower[0],
		stats.temperature,
		stats.PVVoltageDC[0],
		stats.IVCurrentDC[0],
		stats.PVVoltageAC[0],
		stats.IVCurrentAC[0],
		stats.frequency,
		stats.TotalPower,
		stats.TotalHours);

	fclose(pFile);
}

