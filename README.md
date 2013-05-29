# Omnik Statistics
=====
Omnikstats will search for your (WIFI connected) Omnik solar inverter and then download its statistics. 
Statistics can be logged to a CSV data file and/or uploaded to PVoutput.org

With a 5 minute interval cronjob, this program can upload statistics to PVoutput.org

You only have to edit omnik.conf with your APIkey and System ID, to be provided by PVoutput.org

## Installation and Setup

* You may have to update first:
	sudo apt-get update
* The curl development must be installed:
	sudo apt-get install libcurl4-openssl-dev
* Clone the source with sudo git clone https://github.com/arjen/omnikstats.git
* Edit omnik.conf with your API key and SystemID
* do a make clean
* do a make all
* run: ./omnikstats -? for the usage.

### cronjob
The following cronjob command starts omnikstats every 5 minutes between 6 AM and 11 PM
(no use to get statistics during night time)

*/5 6-22 * * * /\<path to omnikstats>/omnikstats

UPDATE

On my raspberry, the cronjob did not start in the correct directory. 
Further, pvoutput only registers every 10 minutes
so edit cronjob with: (enter sudo crontab -e)

*/10 6-22 * * * cd \<path to omnikstats> && /&ltpath to omnikstats>/omnikstats

## REMARKS

* If no key and/or systemID is provided the program will still run (with an error from PVoutput)
* This program might not work for all Omnik inverters. Contact me for possible solutions.
* If you have more than 1 string in your solar system, it will get the statistics and -if desired- will
	log this to CSV. However, only the first string will be uploaded to PVoutput.org (for now)

