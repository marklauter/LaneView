#ifndef _rates_h
#define _rates_h

extern int read_rates(int month, int day, int year, DAYS_RATES* dr); //entry point
extern int get_rates(int year, int month);
extern int read_file(char* fname);
extern int request_file(UCHAR* fname);

#endif
