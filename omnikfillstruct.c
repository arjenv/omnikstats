/*
**	omnikfillstruct.c
** 
**	Fills the stats structure with data from Omnik
**
**	Returns: None
** 
**	Author: Beach
**	V1.0 may,25 2013
*/

#include <stdio.h>
#include <ifaddrs.h>
#include <string.h>
#include <math.h>

#include "omnikstats.h"


void omnikfillstruct(char *server_reply) {

	int i;
	long SN=0;
	char SNI[20];
	int string[3] = {1,1,1};
	stats.strings = 3;

	for (i=0; i<3; i++) { //how many solar strings?
		if (!ctonr(&server_reply[33+2*i], 2, 1)) {
			string[i] = 0;
			stats.strings--;
		}
	}

	if (stats.verbose) 
		printf("Serial Number: ");
	for (i=0; i<4; i++) {
		if (stats.verbose) 
			printf("%x", (unsigned char) server_reply[7-i]);
		SN += (long) (unsigned char) server_reply[7-i] * pow(256, (3-i));
	}
	if (stats.verbose)
		printf("\t\t%ld\n", SN);
	stats.serial_number = SN;

	strncpy(SNI, &server_reply[15], 16);
	SNI[16] = 0;
	if (stats.verbose) 
		printf("ID Inverter: \t\t\t%s\n", SNI);
	stats.temperature = ctonr(&server_reply[31], 2, 10);
	if (stats.verbose)
		printf("Temperature:\t\t\t%.1f\n", stats.temperature);
	for (i=0; i<3; i++) {
		if (string[i]) {
			stats.PVVoltageDC[i] = ctonr(&server_reply[33+2*i], 2, 10);
			stats.IVCurrentDC[i] = ctonr(&server_reply[39+2*i], 2, 10);
			stats.PVVoltageAC[i] = ctonr(&server_reply[51+2*i], 2, 10);
			stats.IVCurrentAC[i] = ctonr(&server_reply[45+2*i], 2, 10);
			stats.PVPower[i] = ctonr(&server_reply[59+2*i], 2, 1);
			if (stats.verbose) {
				printf("PV%d Voltage (DC):\t\t%.1f\n", i+1, stats.PVVoltageDC[i]);
				printf("IV%d Voltage (DC):\t\t%.1f\n", i+1, stats.IVCurrentDC[i]);
				printf("PV%d Voltage (AC):\t\t%.1f\n", i+1, stats.PVVoltageAC[i]);
				printf("IV%d Voltage (AC):\t\t%.1f\n", i+1, stats.IVCurrentAC[i]);
				printf("PV%d Power:\t\t\t%.0f\n", i+1, stats.PVPower[i]);
			}
		}
	}
	stats.frequency = ctonr(&server_reply[57], 2, 100);
	if (stats.verbose) 
		printf("Frequency (AC):\t\t\t%.2f\n", stats.frequency);
	stats.PowerToday = 1000 * ctonr(&server_reply[69], 2, 100);
	if (stats.verbose)
		printf("Total Power today (Wh):\t\t%.0f\n", stats.PowerToday);
	stats.TotalPower = ctonr(&server_reply[71], 4, 10);
	if (stats.verbose)
		printf("Total Power since reset (kWh):\t%.1f\n", stats.TotalPower);
	stats.TotalHours = ctonr(&server_reply[75], 4, 1);
	if (stats.verbose)
		printf("Total Hours since reset:\t%.0f\n\n", stats.TotalHours);
}

