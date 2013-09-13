
#define C_UDPPORT "48899"	// the port users will be connecting to
#define UDPPORT 48899
#define OMNIKPORT 8899
#define MAXBUFLEN 100

struct statistics {
	int verbose;
	int logcsv;
	char url[60];
	char key[60];
	char ID[10];
	char IPnumber[20];
	char filename[200];
	int strings;
	long serial_number;
	float temperature;
	float PVVoltageDC[3];
	float IVCurrentDC[3];
	float PVVoltageAC[3];
	float IVCurrentAC[3];
	float frequency;
	float PVPower[3];
	float PowerToday;
	float TotalPower;
	float TotalHours;
};

struct statistics stats;

extern float ctonr(char * src, int nrofbytes, int div);
extern void *get_in_addr(struct sockaddr *sa);
extern int omniksearch(void);
extern int omnikgetstats(char *server_reply);
extern void omnikcsv(void);
extern char *getdatetime(char *now, int datetime);
extern int omnikpvoutput(void);
extern void omnikfillstruct(char *server_reply);
extern int omnikreadconfig(void);

