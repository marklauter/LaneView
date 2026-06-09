/*   Copyright 1996 TeCom Inc. All rights reserved                  */
#ifndef _inside_h
#define _inside_h

typedef struct
{
  time_t start_time;
  USHORT duration; // in minutes
  USHORT device_id;
  UCHAR level;
  UCHAR override;
  double kwh;
  double credit;
  double penalty;
} DEVICE_LOAD_CTRL;

extern void Wait(long int wait_ticks);
extern void lv_stop(void);

extern int in_bar_graph;

extern int in_monitor;

extern int secured_user;
extern int field_changed;
extern int show_time;
extern int mouse_found;
extern int main_selection;
extern int events_used;
extern int moods_used;
extern int current_day_type;
extern int mon_devices_used;
extern int buckets_used;
extern int show_utility_message;
extern int LVLITE;
extern int DEMO;
extern int LOGGED_IN;
extern int cebus_found;
extern char icon[17][298];
extern char busy[490];

extern char* wday[7];
extern UCHAR version[];

extern CUSTOMER cust_info;
extern DAYS_RATES todays_rates;
extern METER meter;
extern USHORT monitored_devices[MAX_HISTORY];
extern void(_far* Sender)();
extern char confirmation;

extern void DisplayIcon(int top, int left, char far* icon);
extern ULONG GetResource(char* name, char* buffer);

#define MAX_LOAD_CTRL_HISTORY 90

#endif
