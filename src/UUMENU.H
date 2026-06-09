#ifndef _utility_usage_menu_screen_h
#define _utility_usage_menu_screen_h

extern int uuDateSelection;
extern int uu_selected_month;
extern int uu_selected_day;
extern int uu_selected_year;
extern USHORT dbd_id;

extern void UtilityUsageBtnClick(void);
extern void UtilityUsageMenu(void);
extern void ParseDate(char* buf, int* mon, int* day, int* year);
extern void DisplayByDayMenu(USHORT device_id);

#endif
