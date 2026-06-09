#ifndef _inutility_h
#define _inutility_h

extern int today;
extern int this_month;
extern int this_year;

extern long ltime;

extern struct tm* tod;

extern int IsSubmeter(DEVICE* dev);
extern int TableToDisk(void* table,
                       int size_of_table_item,
                       int table_entry_count,
                       UCHAR* file_name);

//returns version number for success or FALSE for failure
extern int TableFromDisk(void* table,
                         int size_of_table_item,
                         UCHAR* file_name);

extern void Status(UCHAR* status);
extern int next_day(UCHAR* month, UCHAR* day, UCHAR* year);

#endif
