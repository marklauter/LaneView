/*   Copyright 1996 TeCom Inc. All rights reserved                  */

/*
$Log:   J:/intrlane/src/laneview/vcs/INUTIL.C_V  $
 *
 *    Rev 1.47   Mar 13 1998 03:34:22   tmges
 * Removed unused stuff
 *
 *    Rev 1.46   Mar 11 1998 11:20:56   tmmsl
 * text changes
 *
 *    Rev 1.45   14 Jul 1997 16:17:14   TMMSL
 * changed
 *
 *    Rev 1.44   25 Jun 1997 10:03:08   TMMSL
 * revision 4.1.1 final check in
 * visual changes
 * new configurable estimated bill
 * support for billing dates
 *
 *    Rev 1.43   27 May 1997 11:13:10   TMMSL
 * 1.3.1 updates
 *
 *    Rev 1.42   12 May 1997 16:49:58   TMMSL
 * Minor bug fixes for 4.1.1 LaneView Release
 *
 *    Rev 1.41   19 Mar 1997 09:18:26   TMMSL
 * changed status display
 *
 *    Rev 1.40   18 Mar 1997 08:11:20   TMMSL
 * Changes for InterLane Release 1.3
 *
 *    Rev 1.39   23 Feb 1997 20:05:28   TMMSL
 * 1.3 user interface changes
 *
 *    Rev 1.38   30 Dec 1996 11:12:46   TMMSL
 * fix memset call
 *
 *    Rev 1.37   30 Dec 1996 11:06:28   STGES
 * 1. in close_day():
 *     A. Set billing_read variable = FALSE.
 *     B. clear dawn_dusk_times structure
 *         so it'll be re-read if needed.
 *
 *
 *    Rev 1.36   27 Dec 1996 14:18:12   TMMSL
 * removed unused variables and functions
 *
 *    Rev 1.35   22 Dec 1996 15:02:24   STGES
 * 1. Remove device_set[]
 *
 *    Rev 1.34   20 Dec 1996 07:56:20   TMMSL
 * removed references to button()
 *
 *    Rev 1.33   19 Dec 1996 16:05:46   TMMSL
 * removed reference to button()
 *
 *    Rev 1.32   16 Dec 1996 02:21:50   TMMSL
 * removed conflicting references to
 * events and devices
 *
 *    Rev 1.31   15 Dec 1996 11:32:46   TMMSL
 * fixed area_clear() in Status()
 *
 *    Rev 1.30   15 Dec 1996 02:44:48   TMMSL
 * 1
 *
 *    Rev 1.29   14 Dec 1996 05:33:14   TMMSL
 * removed all event functions
 *
 *    Rev 1.28   14 Dec 1996 00:41:12   TMMSL
 * removed references to insert_event()
 *
 *    Rev 1.27   13 Dec 1996 21:54:20   TMMSL
 * 1. added load control notification to Status()
 * 2. removed show_page_of...()
 *
 *    Rev 1.26   12 Dec 1996 05:15:06   TMMSL
 * changed Status()
 *
 *    Rev 1.25   11 Dec 1996 07:23:26   TMMSL
 * 1. rmv delete_device()
 * 2. chg adjust_events()
 * 3. add TableToDisk()
 * 4. add TableFromDisk()
 *
 *    Rev 1.24   22 Nov 1996 19:47:08   TMMSL
 * removed calendar();
 *
 *    Rev 1.23   15 Nov 1996 02:46:26   TMMSL
 * 1. added SCREEN_OBJECT_T type to
 *     calendar function
 *
 *    Rev 1.22   01 Nov 1996 22:42:38   TMMSL
 * 1. removed #if ONE_BOX ... #if TWO_BOX
 *     stuff ... all one box code removed
 * 2. changed how screens are defined
 *
 *    Rev 1.21   Sep 03 1996 15:09:30   STGES
 * new copyright info inserted
 *
 *    Rev 1.20   Aug 19 1996 10:34:38   DYBBN
 * Fixed problem report #11 - Incorrect positioning on action field for
 * Random and Dawn/Dusk schedules.
 *
 *    Rev 1.19   Aug 12 1996 15:39:10   DYBBN
 * 1) Changes to allow password to control ability to edit/add
 *      load control devices.
 * 2) Changed text from TEMS to LaneView.
 * 3) Changed device edit/add subroutines to support the new
 *     device data structures.
 * 4) Added to device change subroutine a help window to
 *     explain priority values.
 * 5) Added to device change ability to edit monitor device
 *     addresses associated with devices capable of being
 *     under load control.
*/

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>

#include "cebus.h"
#include "2box.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "ismenu.h"
#include "frame_3d.h"
#include "device.h"
#include "inside.h"
#include "inio.h"
#include "billing.h"

extern int kbchar(void);
extern int user_log(int);
extern void user_log_end_level(void);
extern int user_download(void);
extern void position(int, int);
extern void area_clear(int, int, int, int, int);
extern void beep(void);
extern int day_of_week(int, int, int);
//extern int  copy_file(char*, char*);
//extern int  download_history(void);
//int  is_controlled(DEVICE*);

//#define MAX_ATTEMPTS 32
extern char buffer[85];
extern int day_tab[2][13];
extern DEVICE_HISTORY device_history[];
extern METER meter;
extern struct CEBUSDATA {
  int IRQ;
  int HouseCode;
  int BasePort;
  int OutsideHC;
} cebusSetup;

unsigned long id;
unsigned long ackid;
PKT_INFO pkt_info;

int today;
int this_month;
int this_year;

struct tm* tod;
long ltime;
long last_complete;

extern INTERVAL_DATA interval;

char house_code[16] = {
    0x06, /* A */
    0x0E, /* B */
    0x02, /* C */
    0x0A, /* D */
    0x01, /* E */
    0x09, /* F */
    0x05, /* G */
    0x0D, /* H */
    0x07, /* I */
    0x0F, /* J */
    0x03, /* K */
    0x0B, /* L */
    0x00, /* M */
    0x08, /* N */
    0x04, /* O */
    0x0C, /* P */
};

char CEBUS_FAIL_MSG[] = "CeBus Failure";
char CEBUS_IDLE_MSG[] = "CeBus Idle";
char CEBUS_ID_MSG[] = "CeBus ID not found";
char CEBUS_TIMEDOUT_MSG[] = "CeBus Timed Out";

extern void movez(char*, char*, int);
extern int enter(int, int); /* input from keyboard
            parameters: type (INTEGER, ALPHA, FLOAT)
                   OK_F9 / NO_F9
            returns:   -1 if ESC entered,
                  -1x for F1 thru F9
                  -20 for F10
                  or entry length   */

close_day() {
  int j, hist;

  // new day --
  memset(&dawn_dusk_time, 0, sizeof(DAWN_DUSK_TIME));
  //billing_read = FALSE;

  meter.first_today = meter.current_dial;
  meter.todays_dollars = 0;
  for (j = 0; j < 24; ++j) {
    meter.current_day[j] = 0;
    meter.current_day$[j] = 0;
    meter.tod_current_day$[j] = 0;
    meter.var_current_day$[j] = 0;
  }
  for (hist = 0; hist < MAX_HISTORY; ++hist) {
    for (j = 0; j < 24; ++j) {
      device_history[hist].current_day[j] = 0;
      device_history[hist].current_day$[j] = 0;
    }
  }
  if ((interval.newday == 'M') || (interval.newday == 'Y')) { // new month -- close out
    meter.last_mo_start_dial = meter.month_start_dial;
    meter.month_start_dial = meter.current_dial;
    for (j = 0; j < 24; ++j) {
      meter.last_month[j] = meter.mtd[j];
      meter.last_month$[j] = meter.mtd$[j];
      meter.tod_last_month$[j] = meter.tod_mtd$[j];
      meter.var_last_month$[j] = meter.var_mtd$[j];
      meter.mtd[j] = 0;
      meter.mtd$[j] = 0;
      meter.tod_mtd$[j] = 0;
      meter.var_mtd$[j] = 0;
    }
    for (hist = 0; hist < MAX_HISTORY; ++hist) {
      for (j = 0; j < 24; ++j) {
        device_history[hist].last_month[j] = device_history[hist].mtd[j];
        device_history[hist].last_month$[j] = device_history[hist].mtd$[j];
        device_history[hist].mtd[j] = 0;
        device_history[hist].mtd$[j] = 0;
      }
    }
  }
  if (interval.newday == 'Y') { // new year -- close out
    for (hist = 0; hist < MAX_HISTORY; ++hist) {
      for (j = 0; j < 24; ++j) {
        device_history[hist].ytd[j] = 0;
        device_history[hist].ytd$[j] = 0;
      }
    }
    for (j = 0; j < 24; ++j) {
      meter.ytd[j] = 0;
      meter.ytd$[j] = 0;
      meter.tod_ytd$[j] = 0;
      meter.var_ytd$[j] = 0;
    }
  }
  return (0);
}

next_day(UCHAR* month, UCHAR* day, UCHAR* year) {
  int leap;

  if ((*year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }
  if (++(*day) > day_tab[leap][*month]) {
    *day = 1;
    if (++(*month) > 12) {
      *month = 1;
      ++(*year);
    }
  }
  return (0);
}

prev_day(UCHAR* month, UCHAR* day, UCHAR* year) {
  int leap;

  if ((*year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }
  if (--(*day) == 0) {
    --(*month);
    if (*month == 0) {
      *month = 12;
      --(*year);
    }
    *day = day_tab[leap][*month];
  }
  return (0);
}

#if 0
int is_controlled(DEVICE* dev)
{
   if (dev->code_letter != '@')
   {
      return 1;
   }
   if (dev->cebus_type >= 0 && dev->cebus_type < 5)
   {
      return 1;
   }
   return 0;
}
#endif

int TableToDisk(void* table, int size_of_table_item, int table_entry_count, UCHAR* file_name) {
  FILE* stream;
  int dev_ver;
  if (DEMO == TRUE) {
    return SUCCESS;
  }

  dev_ver = DEVICES_VERSION;
  if ((stream = fopen(file_name, "wb")) == NULL) {
    return FAILURE;
  } else {
    fwrite(&dev_ver, sizeof(int), 1, stream);
    fwrite(&table_entry_count, sizeof(int), 1, stream);
    fwrite(table, size_of_table_item, table_entry_count, stream);
    fclose(stream);
  }
  return SUCCESS;
}

int TableFromDisk(void* table, int size_of_table_item, UCHAR* file_name) {
  FILE* stream;
  int dev_ver;
  int count;

  if ((stream = fopen(file_name, "rb")) == NULL) {
    return FALSE; //failure
  } else {
    fread(&dev_ver, sizeof(int), 1, stream);
    fread(&count, sizeof(int), 1, stream);
    fread(table, size_of_table_item, count, stream);
    fclose(stream);
  }
  if (dev_ver != DEVICES_VERSION) {
    count = 0;
  }
  return count;
}

void Status(UCHAR* status) {
  UCHAR work[25];

  area_clear(460, 474, 72, 392, FG_WHT);

  ptext("Status: ", 460, 7, BG_TRANSPARENT + FG_BLK);
  ptext(status, 460, 74, BG_TRANSPARENT + FG_BLK);

  frame_3d(459, 4, 16, 62, FT_PRESSED); //Status

  frame_3d(459, 70, 16, 324, FT_PRESSED); //*status text

  frame_3d(459, 601, 16, 34, FT_PRESSED); //OVR

  if (over_write == TRUE) {
    ptext("OVR", 460, 605, BG_WHT + FG_BLK);
  } else {
    ptext("INS", 460, 605, BG_WHT + FG_BLK);
  }

  frame_3d(459, 398, 16, 199, FT_PRESSED); //load control in progress

  if (interval.shedding_status > 0) {
    if (interval.shedding_status & 1) {
      ptext("Utility Control: Ovr Rd", 460, 402, BG_WHT + FG_LGRN);
    } else {
      sprintf(work, "Utility Control: Pri %d", interval.shedding_status / 8);
      ptext(work, 460, 402, BG_WHT + FG_RED);
    }
  } else {
    sprintf(work, "Utility Control: None");
    ptext(work, 460, 402, BG_WHT + FG_BLK);
  }
  if (secured_user) {
    ptext("SU", 460, 370, BG_WHT + FG_BLK);
  }
}
