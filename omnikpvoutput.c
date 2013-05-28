/*
**	omnikpvoutput.c
**
**	uploads data to pvoutput.org
**
**
**	By Beach
**	May,25 2013
**
*/

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <curl/easy.h>

#include "omnikstats.h"

size_t  write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {

	FILE *pFile;
	char buf[size*nmemb+1];
	char * pbuf = &buf[0];
	memset(buf, '\0', size*nmemb+1);
	size_t i = 0;
	for(;  i < nmemb ; i++) {
		strncpy(pbuf,ptr,size);
		pbuf += size;
		ptr += size;
	}
	if (stats.verbose) {
		printf("PVouput.org replied: ");
		printf("%s\n",buf);
	}
	if (stats.logcsv) {
		if (stats.verbose) 
			printf("Opening %s for writing.....\n", stats.filename);
		if ((pFile = fopen(stats.filename, "a")) == NULL) {
			printf("Could not open file %s\n", stats.filename);
		}
		else {
			if ((strstr(buf, "OK 200")) != NULL) 
				fprintf(pFile, "OK 200\n");
			else 
				fprintf(pFile, "%s\n", buf);
		}
	fclose(pFile);
	}
	return size * nmemb;
}

int omnikpvoutput(void) {

	CURL *curl;
	CURLcode res;
	char pvstring[200];
	char date[10], now[10];

	sprintf(pvstring, "%s?d=%s&t=%s&v1=%.0f&v2=%.0f&v5=%.1f&v6=%.1f&key=%s&sid=%s", 
		stats.url,
		getdatetime(date, 0),
		getdatetime(now, 1),
		stats.PowerToday,
		stats.PVPower[0],
		stats.temperature,
		stats.PVVoltageDC[0],
		stats.key, stats.ID);

	// get a curl handle
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, pvstring);
		if (stats.verbose > 2) 
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_data);
		res = curl_easy_perform(curl);
		// Check for errors 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
	}
	return 0;
}

