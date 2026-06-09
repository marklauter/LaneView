#ifndef _insubs_h
#define _insubs_h

extern void DrawTop(void);
extern void status_screen(int thedevice);
extern int day_of_week(int month, int day, int year);
extern void add_dollar(char* cp);
extern int waitkb(int opt); //until whole program is completed

extern INTERVAL_DATA interval;
extern INTERVAL_DATA$ interval$;
extern INTERVAL_DATA meter_interval;
extern char dayofweek[7][4];

int trim(char* str_to_trim);
void fillz(UCHAR* cp, USHORT len, UCHAR c);
void movez(char* dest, char* source, int len);
void FitText(char* dest, char* src, int len);
double round(double value, int precision);
void UpdateTop(long* last_shown);
double round(double value, int precision);
void movez(char* dest, char* source, int len);

#endif
