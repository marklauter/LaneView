#ifndef _calendar_h
#define _calendar_h

extern void calendar(void);
extern int day_of_week(int month, int day, int year);

extern char* mname[];
extern int day_tab[2][13];
// direction for calendar to display
#define CAL_BACK -1
#define CAL_NOW 0
#define CAL_NEXT 1

#endif
