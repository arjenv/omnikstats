/*
** omnikstats.c
** 
** This program searches the LAN for your Omnik inverter and -if found-
** downloads statistics from it
** 
** Author: Beach
** V1.1 sept,13 2013
** V1.1 Changed: Added optional IP & serialnumber to be read from omnik.conf file
*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "omnikstats.h"

struct statistics stats;

static void usage(const char *name)
{
        fprintf(stderr, "Usage: %s [-v] [-l]\n\n"
                "Where:\n"
        "\t-v be verbose\n"
        "\t-vv be more verbose\n"
        "\t-vvv Even more verbose\n"
        "\t-l Log statistics in a CSV file\n"
        "\n", name);
        exit(1);
}

int main(int argc, char *argv[])
{
	int i;
	char server_reply[256];
	int opt;

	stats.verbose = 0;
	stats.logcsv = 0;
	strcpy(stats.IPnumber, "");
	stats.serial_number = 0;

        while ((opt = getopt(argc, argv, "vl")) != -1) {
                switch (opt) {
                case 'v':       //verbose`
                        stats.verbose += 1;
                        break;
                case 'l':       // Log to CSV
                        stats.logcsv = 1;
			break;
                case '?':
                        usage(argv[0]);
                }
        }

	if (stats.verbose) {
		printf("Verbose %d\n", stats.verbose);
		if (stats.logcsv)
			printf("Data will be logged to /<year>/data.csv\n");
	}

	// Read the config file
	if (omnikreadconfig()) {
		printf("Error in config file\n");
		exit(1);
	}

			
	// If not predefined try and search for the Omnik
	if (stats.IPnumber[0] == '\0') {
		if ((i=omniksearch()) !=0) {
			printf("Error in UDP: %d\n", i);
			exit(1);
		}
	}

	// Get the  statistics
	if ((i=omnikgetstats(server_reply)) != 0) {
		printf("Error in TCP: %d\n", i);
		exit(1);
	}

	// Fill the stats structure print to stdout if 2nd argument=1
	omnikfillstruct(server_reply);

	// append it to a CSV data file
	if (stats.logcsv) omnikcsv();

	// send it to pvoutput.org
	omnikpvoutput();


	return 0;
}


