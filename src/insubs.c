/*   Copyright 1996 TeCom Inc. All rights reserved                  */

/*
$Log:   J:/intrlane/src/laneview/vcs/INSUBS.C_V  $
 *
 *    Rev 1.60   Jul 30 1998 10:54:50   tmges
 * Make hour show at top left of screens be
 * "12" not "0" from midnight to 1:00 am.
 *
 *    Rev 1.59   Jun 30 1998 13:02:56   tmges
 * Y2K to screen top.
 *
 *    Rev 1.58   May 01 1998 13:43:12   tmges
 * Made Today's KWH value at top of
 * screens be sum of hourly usage, rather
 * than difference of Current dial reading and
 * Midnight dial reading.
 *
 *    Rev 1.57   Mar 12 1998 21:06:34   tmmsl
 * made trim return the length of
 * the new string
 *
 *    Rev 1.56   Mar 11 1998 23:45:34   tmges
 * 1. Update top changed:
 *     A.   Make Year 4 char & add 1900 to it.
 *     B.  Remove "Date:" & "Time:" & move
 *           top around.
 *
 *    Rev 1.55   Mar 11 1998 11:19:14   tmmsl
 * changes to UpdateTop() & ShowTop()
 *
 *    Rev 1.54   25 Jun 1997 10:03:02   TMMSL
 * revision 4.1.1 final check in
 * visual changes
 * new configurable estimated bill
 * support for billing dates
 *
 *    Rev 1.53   27 May 1997 11:13:06   TMMSL
 * 1.3.1 updates
 *
 *    Rev 1.52   12 May 1997 16:49:56   TMMSL
 * Minor bug fixes for 4.1.1 LaneView Release
 *
 *    Rev 1.51   18 Mar 1997 08:11:16   TMMSL
 * Changes for InterLane Release 1.3
 *
 *    Rev 1.50   23 Feb 1997 20:05:24   TMMSL
 * 1.3 user interface changes
 *
 *    Rev 1.49   27 Dec 1996 14:48:58   TMMSL
 * removed unreferenced variables to
 * get a good compile after changes in inio.c
 *
 *    Rev 1.48   21 Dec 1996 11:20:52   STGES
 * 1. Removed much unused code.
 *
 *
 *    Rev 1.47   20 Dec 1996 19:00:56   STGES
 * 1. Temporarily move device_set[] to this
 *     file.
 *
 *    Rev 1.46   20 Dec 1996 18:45:14   STGES
 * 1. Changed references to extern load_ctrl[]
 *     to load_ctrl_tbl[] which is defined in
 *     "inio.h"
 *
 *
 *    Rev 1.45   20 Dec 1996 07:55:22   TMMSL
 * removed references to enter()
 * removed references to button()
 *
 *    Rev 1.44   19 Dec 1996 16:09:00   STGES
 * 1. Removed load_ctrl info screens
 *     see WHMENU.C
 *
 *    Rev 1.43   16 Dec 1996 04:18:48   STGES
 * 1. Took unused parts of round() out
 *
 *    Rev 1.42   16 Dec 1996 02:21:46   TMMSL
 * removed conflicting references to
 * events and devices
 *
 *    Rev 1.41   15 Dec 1996 11:47:52   TMMSL
 * commented out status_screen()
 *
 *    Rev 1.40   15 Dec 1996 11:38:34   STGES
 * 1. Removed status_screen
 *
 *    Rev 1.39   15 Dec 1996 05:53:06   STGES
 * Program re-write
 *
 *
 *    Rev 1.38   14 Dec 1996 06:33:52   STGES
 * 1. Took extraneous code out of Load
 *     Management screens.
 *
 *    Rev 1.37   14 Dec 1996 05:33:10   TMMSL
 * removed all event functions
 *
 *    Rev 1.36   13 Dec 1996 21:51:56   TMMSL
 * 1. removed mouse fields and other old stuff
 * 2. removed show_page_of .....()
 *
 *    Rev 1.35   13 Dec 1996 07:17:10   STGES
 * 1. Remove under utility control status
 *     display.
 *
 *    Rev 1.34   11 Dec 1996 07:18:20   TMMSL
 * 1. rmv demo screens
 * 2. rmv change_day_type()
 * 3. rmv print_msg()
 * 4. add FitText
 *
 *    Rev 1.33   22 Nov 1996 19:41:16   TMMSL
 * 1. changed show_top() to UpdateTop()
 * 2. modernized UpdateTop();
 * 3. in middle of R 1.2, removed HUGE chunks
 *     of unused code
 *
 *    Rev 1.32   15 Nov 1996 02:35:38   TMMSL
 * 1. removed user_input()
 * 2. removed draw_top()
 * 3. removed utility_usage_menu()
 * 4. removed function_not_supported()
 * 5. changed all getch() to kbchar()
 * 6. changed all interval processing funcions
 *     to accept a (PKT_INFO * pi) as parameter
 *     to avoid addressing the wrong packet
 *     during processing
 * 7. changed show_top_status() to
 *     UpdateTop()
 * 8. added ShowTitle()
 *
 *    Rev 1.31   01 Nov 1996 22:40:36   TMMSL
 * 1. removed #if ONE_BOX ... #if TWO_BOX
 *     stuff ... all one box code removed
 * 2. changed waitkb() to user_input()
 *         user_input is the main loop for LV
 *
 *    Rev 1.30   Sep 03 1996 15:09:22   STGES
 * new copyright info inserted
 *
 *    Rev 1.29   Aug 20 1996 18:08:40   DYBBN
 * Added a check to verify that the entered Load Control Override end time is
 * greater than or equal to the entered start time.
 *
 *    Rev 1.28   Aug 16 1996 17:15:24   DYBBN
 * Fixed buttons not being selectable by the mouse on the
 * Load Control History screen.
 *
 *    Rev 1.27   Aug 16 1996 14:45:02   DYBBN
 * Added an infomormative message to be displayed when the user
 * clears an override of a load control operation.
 *
 *    Rev 1.26   Aug 15 1996 14:43:48   DYBBN
 * Changed house_graph subroutine so it would display a kwh label
 * for the Energy Usage graph and a kw label for the Energy Monitor graph.
 *
 *    Rev 1.25   Aug 13 1996 16:18:50   DYBBN
 * Fixed spacing problems on Load Control Device Status screen.
 *
 *    Rev 1.24   Aug 13 1996 15:27:06   DYBBN
 * Changed customer charge for the Variable Rate (now called End-Use Rate) to be the
 * same as the charge for the TOD rate.
 *
 *    Rev 1.23   Aug 12 1996 15:34:26   DYBBN
 * 1) Added subroutines and data structures to support new load
 *     control screens.
 * 2) Changes to support Thermostat controls.
 * 3) Changes to display rates screen with the new load control
 *     rates.
 * 4) Changes to estimated bill statement subroutines to use the
 *     new billing structures and display the new load control
 *     credit and penalty amounts.
 *
*/

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>

#include "cebus.h"
#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "frame_3d.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "fnsmsg.h"
#include "calendar.h"
#include "device.h"
#include "inside.h"
#include "event.h"
#include "inio.h"
#include "msgbox.h"
#include "inutil.h"

extern int process_interval(PKT_INFO* incoming_packet);
extern int process_interval$(PKT_INFO* incoming_packet);
extern int IsSubmeter(DEVICE*);
extern INTERVAL_DATA$ interval$;

void movez(char*, char*, int);
void add_dollar(char*);
void draw_top(void);

extern PKT_INFO pkt_info;

extern struct tm* tod;
extern long ltime;
long time_delta;

char dayofweek[7][4] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"};

//extern          int     today;
extern char buffer[];
extern METER meter;

INTERVAL_DATA interval;
INTERVAL_DATA meter_interval;

//used by vgacon.lib
#define HB_T 23
#define HB_L 74
#define HB_B 25
#define HB_R 79

int hb_fields[2][4] = {
    (HB_T - 1) * 14,
    (HB_L - 1) * 8,
    HB_B * 14 - 1,
    HB_R * 8 - 1,
    -1, -1, -1, -1};

void add_dollar(char* cp) {
  int i;

  i = strlen(cp);
  while ((--i) >= 0) {
    if (*(cp + i) == ' ') {
      *(cp + i) = '$';
      break;
    }
  }
}

void DrawTop(void) {
  frame_3d(0, 0, 34, 639, FT_RAISED);
  area_clear(1, 33, 1, 638, BG_WHT + FG_WHT);
  //ptext("Date:",2, 3, BG_WHT + FG_BLK);
  //ptext("Time:",16, 3, BG_WHT + FG_BLK);
  ptext("Today's kWh:", 2, 150, BG_WHT + FG_BLK);
  ptext("Today's $$$:", 16, 150, BG_WHT + FG_BLK);
  ptext("Rate of Consumption", 2, 340, BG_WHT + FG_BLK);
  ptext("Cost per kWh", 2, 510, BG_WHT + FG_BLK);
}

double round(double value, int precision) {
  long intvalue;

  //exponent = pow(10, (double)precision);

  intvalue = (long)(value * pow(10, (double)precision) + 0.5);
  return ((double)intvalue / pow(10, (double)precision));
}

void UpdateTop(long* last_shown) {
  char work[100];
  int i;
  long my_time;
  long TotalWork;

  *last_shown = ltime;

  hide_mouse();

  //=============================
  // Time
  //=============================
  time(&my_time);
  my_time += time_delta;
  tod = localtime(&my_time);

  //=============================
  // Date
  //=============================
  //ptext(dayofweek[tod->tm_wday], 2, 46, BG_WHT + FG_BLK);   //GES
  ptext(dayofweek[tod->tm_wday], 2, 8, BG_WHT + FG_BLK); //GES
  sprintf(work, "%d/%d/%4d", tod->tm_mon + 1, tod->tm_mday, tod->tm_year + 1900);
  //ptext(work, 2, 82, BG_WHT + FG_BLK); //GES
  ptext(work, 2, 44, BG_WHT + FG_BLK); //GES

  if (tod->tm_hour > 11) {
    ptext("pm", 16, 114 - 38, BG_WHT + FG_BLK); // ges
  } else {
    ptext("am", 16, 114 - 38, BG_WHT + FG_BLK); //ges
  }
  if (tod->tm_hour > 12) {
    tod->tm_hour -= 12;
  }
  if (tod->tm_hour == 0) {
    tod->tm_hour = 12;
  }
  sprintf(work, "%02d:%02d:%02d", tod->tm_hour, tod->tm_min, tod->tm_sec);
  //ptext(work, 16, 46,BG_WHT +  FG_BLK);   //GES
  ptext(work, 16, 8, BG_WHT + FG_BLK); //GES

  //=============================
  // Today's Energy Usage
  //=============================
  TotalWork = 0;
  for (i = 0; i < 23; ++i) {
    TotalWork += meter.current_day[i];
  }
  sprintf(work, "%-7.2f", ((double)TotalWork) / 10000.0);
  //sprintf(work, "%-7.2f", (meter.current_dial - meter.first_today)/10000.0 );
  ptext(work, 2, 250, BG_WHT + FG_BLK);

  sprintf(work, "%-7.2f", (double)(meter.todays_dollars) / 100000.0);
  ptext(work, 16, 250, BG_WHT + FG_BLK);

  if (meter_interval.device_slot != 0) {
    sprintf(work, "%-7.4fkW", meter_interval.kwh / 10000.0);
    ptext(work, 16, 340, BG_WHT + FG_BLK);
    sprintf(work, "$%-7.2f", (double)(meter_interval.kwh * meter_interval.cost_kwh) / 1000000000.0);
    ptext(work, 16, 436, BG_WHT + FG_BLK);
  } else {
    if (DEMO == TRUE) {
      ptext("DEMO MODE", 16, 340, BG_WHT + FG_BLK);
    } else if (LVLITE == TRUE) {
      ptext("LITE", 16, 340, BG_WHT + FG_BLK);
    } else {
      ptext("Waiting...", 16, 340, BG_WHT + FG_BLK);
    }
  }

  //==============================================
  // Status line update
  //==============================================
  frame_3d(459, 398, 16, 199, FT_PRESSED); //load control in progress
  if (interval.shedding_status > 0) {
    ptext("Energy Control: Ovr Rd", 460, 402, BG_WHT + FG_RED);

    if (interval.shedding_status & 1) {
      area_clear(460, 474, 434, 595, FG_WHT);
    } else {
      sprintf(work, "Energy Control: Pri %1d ", interval.shedding_status / 8);
      ptext(work, 460, 402, BG_WHT + FG_RED);
    }
  }

  //==============================================
  // Rate Mode Status
  //==============================================
  if (meter_interval.device_slot != 0) {
    for (i = 4; i < 8; i++) {
      if (interval$.mode_byte2 & (0x01 << i)) {
        switch (i) {
        case 4: // low cost mode
          ptext("Low        ", 17, 511, FG_BLK + BG_WHT);
          break;
        case 5: // moderate cost mode
          ptext("Medium     ", 17, 511, FG_BLK + BG_WHT);
          break;
        case 6: // high cost mode
          ptext("High       ", 17, 511, FG_BLK + BG_WHT);
          break;
        case 7: // maximum mode
          ptext("Critical   ", 17, 511, FG_RED + BG_WHT);
          break;
        }
        break;
      }
    }
  } else {
    if (DEMO == TRUE) {
      //pbutton ( 16, 510, 15, 80, PB_DN, BG_CYN);
      ptext("Medium     ", 17, 511, FG_BLK + BG_WHT);
    }
    if (LVLITE == TRUE) {
      //pbutton ( 16, 510, 15, 80, PB_DN, BG_GRN);
      ptext("LVLITE   ", 17, 511, FG_BLK + BG_WHT);
    }
  }
  unhide_mouse();
}

// returns the new length of the string
int trim(char* str_to_trim) {
  int i;
  int j;

  j = i = strlen(str_to_trim);
  if (i) {
    while (*(str_to_trim + (--i)) == ' ') {
      --j;
      *(str_to_trim + i) = '\0';
    }
  }
  return (j);
}

void fillz(UCHAR* cp, USHORT len, UCHAR c) {
  memset(cp, c, len);
  *(cp + len) = '\0';
  return;
}

void movez(char* dest, char* source, int len) {
  memcpy(dest, source, len);
  *(dest + len) = '\0';
  return;
}

void FitText(char* dest, char* src, int len) {
  int slen;

  slen = strlen(src);
  if (slen < len) {
    memset(dest, ' ', len);
    memcpy(dest, src, slen);
  } else {
    memcpy(dest, src, len);
  }
}
