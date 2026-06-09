#ifndef _time_entry_h
#define _time_entry_h

// contains up to 5 text box chunks
// contains a spinner control

#define date_ent 0
#define time_ent 1
#define combo_ent 2
#define mo_yr_ent 3

typedef struct
{
  SCREEN_OBJECT_T so;
  UCHAR type; //date_entry, time_entry or combo_entry
  char hour;
  char minute;
  char month;
  char day;
  char year;
  char am_pm;
  char selected;
  void(_far* OnChange)();
} TIME_ENTRY_T;

extern void DisplayTimeEntry(TIME_ENTRY_T* te);
extern void TimeEntryClick(TIME_ENTRY_T* te);
extern void TimeEntryKeyPress(TIME_ENTRY_T* te, int key);
extern void TimeEntryMouseDown(TIME_ENTRY_T* te);
extern void TimeEntryMouseUp(TIME_ENTRY_T* te);

#endif
