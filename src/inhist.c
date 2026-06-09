// ==========================================================================
// $Workfile:   inhist.c  $
// $Revision:   1.57  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
//
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1996 TeCom Inc. All rights reserved
// ==========================================================================

/*
$Log:   J:/intrlane/src/laneview/vcs/INHIST.C_V  $
 *
 *    Rev 1.57   Apr 01 1998 13:44:06   tmges
 * Change GetFastHistoryDay to a FetchTable
 * call & then decompress() the results.
 *
 *    Rev 1.56   Mar 19 1998 17:44:42   tmges
 * 1.  Change date display while reading
 *      daily history to be "1998" rather than
 *      "98".
 *
 *    Rev 1.55   Mar 16 1998 09:58:50   tmges
 * 1. Change addkey type to INCADD from
 *      REGADD on index conversion.
 *
 *    Rev 1.54   Mar 16 1998 00:18:10   tmges
 * 1. Close history file before returning when
 *     there's no history.
 *
 *    Rev 1.53   Mar 15 1998 22:58:16   tmges
 * UpdateHistoryFast() now calls FetchTable
 *
 *    Rev 1.52   Mar 13 1998 02:26:04   tmges
 * 1. Search for first Whole House record
 *     corrected.
 *
 *
 *    Rev 1.51   Mar 13 1998 01:50:26   tmges
 * 1.  Final change to Convert2Release160,
 *      I hope!
 *
 *    Rev 1.50   Mar 13 1998 01:33:26   tmges
 * 1.  Removed unused stuff
 * 2. Y2K changes on index file accesses
 * 3.  Conversion to Release 1.6 (Y2K)
 *      format for index file.
 *
 *    Rev 1.49   Mar 11 1998 10:52:24   tmmsl
 * 1. changed the function UpdateHistoryFast()
 *     to accept a number of days back to log in.
 *
 *    Rev 1.48   22 Jul 1997 11:09:28   TMMSL
 * added the function FirstHistory().  It
 * calculates the value for
 * BILLING_STATEMENT_MAX which used to
 * be a #define.  This allows the billing
 * functions to "roll" to the first month in the
 * history, instead of an arbitrary two or
 * three years ago.
 *
 *    Rev 1.47   16 Jul 1997 09:17:30   TMGES
 * "bug fix" for fast log-in.
 *
 *
 *    Rev 1.46   14 Jul 1997 16:01:38   TMGES
 * New UpdateHistoryFast() routine.
 *
 *    Rev 1.45   09 Jul 1997 12:31:38   TMGES
 * handle _daylight flag
 *
 *    Rev 1.44   25 Jun 1997 10:02:42   TMMSL
 * revision 4.1.1 final check in
 * visual changes
 * new configurable estimated bill
 * support for billing dates
 *
 *    Rev 1.43   27 May 1997 11:12:50   TMMSL
 * 1.3.1 updates
 *
 *    Rev 1.42   23 Feb 1997 20:05:12   TMMSL
 * 1.3 user interface changes
 *
 *    Rev 1.41   11 Feb 1997 11:21:38   TMMSL
 * minor changes
 *
 *    Rev 1.40   20 Dec 1996 07:55:56   TMMSL
 * removed references to button()
 *
 *    Rev 1.39   19 Dec 1996 16:07:20   STGES
 * 1. changed parameters to
 *     BuildLoadCtrlHistory
 *
 *    Rev 1.38   19 Dec 1996 10:36:10   STGES
 * 1. Make certain that BuildLoadCtrlHistory()
 *     has meaningful error messages and
 *     valid return values.
 *
 *    Rev 1.37   15 Dec 1996 05:52:56   STGES
 * Program re-write
 *
 *
 *    Rev 1.36   13 Dec 1996 21:50:28   TMMSL
 * changed all user output to msgbox()
 *
 *    Rev 1.35   13 Dec 1996 07:15:22   STGES
 * Move Graphs to Graphs.C
 *
 *    Rev 1.34   13 Dec 1996 00:32:18   STGES
 * 1. Fixed update_history() to allow logon
 *     from Midnight to 12:15a.
 * 2. Moved Monthly_cost() to graphics.c
 *
 *
 *    Rev 1.33   12 Dec 1996 07:46:26   STGES
 * 1. Take daily energy cost graph out.
 *
 *    Rev 1.32   12 Dec 1996 03:12:14   STGES
 *  Needed to move graphs to Graphs.c
 *
 *    Rev 1.31   11 Dec 1996 07:11:30   TMMSL
 * added functionality to device status menu
 *
 *    Rev 1.30   22 Nov 1996 19:28:20   TMMSL
 * update LWINDOW_T object for utility usage
 * menu screen
 *
 *    Rev 1.29   15 Nov 1996 02:04:16   TMMSL
 * 1. removed printer related code
 * 2. converted utility usage to new
 *     screen format
 * 3. updated for new rate files
 * 4. removed references to holiday files
 * 5. removed all code that had been
 *     previously inside #if NO... ...#endif
 *
 *    Rev 1.28   01 Nov 1996 22:14:28   TMMSL
 * removed #if ONE_BOX ... #if TWO_BOX stuff
 * all one box code removed
 *
 *    Rev 1.27   Sep 03 1996 15:09:04   STGES
 * new copyright info inserted
 *
 *    Rev 1.26   Aug 20 1996 09:16:00   DYBBN
 * Fix for problem report # 7 - Corrected total kwh calculations on graphs to
 * not include the kwh during an interval where a credit is being given.
 *
 *    Rev 1.25   Aug 19 1996 11:01:44   DYBBN
 * Fix for problem report #s 12 and 13 - Monthly average not being
 * calculated correctly for the current month.
 *
 *    Rev 1.24   Aug 19 1996 10:47:04   DYBBN
 * Fixed problem report #16 - Historical numeric display would sometimes
 * show +Mon/-Mon on buttons when it should say +Day/ -Day.
 *
 *    Rev 1.23   Aug 16 1996 16:34:16   DYBBN
 * Fixed rate scale calculations for daily_usage and daily_cost graphs.
 *
 *    Rev 1.22   Aug 16 1996 14:47:14   DYBBN
 * 1) Corrected problem with a previous fix to the build_load_ctrl_history subroutine.
 * 2) Graphs will only show a credit or penalty occured if the kwh value for the interval
 *     is greater than zero.
 *
 *    Rev 1.21   Aug 16 1996 09:35:58   DYBBN
 * Changed build_load_ctrl_history subroutine so that it would not store
 * history for load controls where no credit or penalty amounts were accumulated.
 *
 *    Rev 1.20   Aug 12 1996 15:06:54   DYBBN
 * 1) New subroutines to support the Load Control History display.
 * 2) Changes for new 'flags' field added to the history data structure.
 * 3) Changed graphs to plot the actual electricity rate cost instead of
 *     shading the grid lines to indicate peak periods.
 * 4) Changed graphs and numeric displays to show a different color
 *     line for kwh intervals or different color x axis labels during load
 *     control operations.
*/

#include "ctstdr.h" /* req'd by ctree */
#include "ctdecl.h" /* req'd by ctree */
#include "ctoptn.h" /* used to access ctree's innards */
#include "ctstrc.h" /* used to access ctree's innards */

#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <bios.h>
#include <malloc.h>
#include "2box.h"
#include "cebus.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "lwindow.h"
#include "uumenu.h"
#include "calendar.h"
#include "fnsmsg.h"
#include "device.h"
#include "inhist.h"
#include "inio.h"
//#include "dsdmenu.h"
#include "whmenu.h"
#include "inutil.h"
#include "msgbox.h"
#include "progind.h"

#define NO 0
#define KEY_DUP_ERR 2 /* duplicate key found by ADDKEY */
#define LEOF_ERR 30   /* drn exceeds logical end of file */

extern void flushdos(int);
//extern void button(int, int, int, int);
extern void rev(char*);
extern int get_cursor(void);
extern void set_cursor(int);
extern void clear_button(int, int, int, int);
extern void norm(char*);
extern int enter(int, int);
extern void beep(void);
extern int waitkb(int);
extern void movez(char*, char*, int);
extern int htoi(char*);
extern int collect_intervals(USHORT, int, int, int, HISTORY_DAY*);
extern void outside_time(struct tm*);
extern int IsSubmeter(DEVICE*);
extern int user_log(int);
extern void user_log_end_level(void);
//extern int  is_on_peak(int, int, int, int);
extern void add_dollar(char*);
extern int read_rates(UCHAR month, UCHAR day, UCHAR year, DAYS_RATES* days_rates);
extern double round(double value, int precision);
//extern int  printmsg(void);
extern COUNT isam_err;
extern int device_set[], device_count;

int monthly_cost_effect(int today_flag, USHORT device_id);
//int display_type_menu(USHORT);
int next_history(char*);

EXTERN COUNT uerr_cod; /* req'd by ctree */
POINTER current_record;

extern CUSTOMER cust_info;

int open_history();

extern long req_id;

extern int day_tab[2][13];

extern UCHAR _far buffer[];
extern int today;
extern int this_month;
extern int this_year;
extern int enter_key;
extern struct tm* tod;
extern long ltime;

extern struct CEBUSDATA {
  int IRQ;
  int HouseCode;
  int BasePort;
  int OutsideHC;
} cebusSetup;

char* wday[7] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"};

char* day_abr[7] =
    {
        "Su",
        "M",
        "T",
        "W",
        "T",
        "F",
        "Sa"};

struct HIST_TRAN hist_tran;
POINTER recbyte;

DEVICE_LOAD_CTRL* load_ctrl_history;
double monthly_total_credit;
double monthly_total_penalty;
int load_ctrl_hist_count;

UCHAR first_day[9];

extern DAYS_RATES todays_rates;

void ctree_error(void) {
  char work[80];

  sprintf(work, "ctree error: %d", uerr_cod);
  Status(work);
}

void close_history(void) {
  CLSFIL(1, COMPLETE);
  CLSFIL(2, COMPLETE);
  flushdos(0);
}

void create_history(void) {
  COUNT datno, datlen, filmod;
  COUNT keyno, keylen, keytyp, keydup, nomemb;
  UCOUNT xtdsiz;
  UCHAR err[100];

  unlink("HISTORY.NDX");

  /********************/
  /* create history file */
  /********************/
  datno = 1;                         /* file 1                */
  datlen = sizeof(struct HIST_TRAN); /* record len            */
  xtdsiz = (unsigned)32 * 1024;      /* update directory   */
  filmod = (PERMANENT | EXCLUSIVE);
  if (CREDAT(datno, "HISTORY.DAT", datlen, xtdsiz, filmod)) {
    printf("\nError %d, creating %s\n", uerr_cod, "HISTORY.DAT");
    exit(1);
  }

  /******************************/
  /* create index file          */
  /******************************/

  keyno = 2;                    /* index date,time index     */
  keylen = 14;                  /* DEV.YYYMMDDHHQ           */
  keytyp = 0;                   /* alphanumeric              */
  keydup = 0;                   /* duplicates NOT allowed    */
  nomemb = 0;                   /* only one index here!      */
  xtdsiz = (unsigned)32 * 1024; /* force directory updates   */

  filmod = (PERMANENT | EXCLUSIVE);
  if (CREIDX(keyno, "HISTORY.NDX", keylen, keytyp, keydup,
             nomemb, xtdsiz, filmod)) {
    sprintf(err, "Error %d creating HISTORY.NDX|"
                 "Check disk space.             ",
            uerr_cod);
    msgbox(err, "Fatal Error", MB_OK);
    ExceptionLog(err);
    lv_stop();
  }
}

#if LOAD_CONTROL_TECO
int CreateLoadCtrlIndex(void) {
  char work[80];
  COUNT filmod;
  COUNT keyno;
  COUNT keylen;
  COUNT keytyp;
  COUNT keydup;
  COUNT nomemb;
  UCOUNT xtdsiz;

  unlink("LOADCTRL.NDX");

  /******************************/
  /* create index file          */
  /******************************/

  keyno = 3;                    /* index date,time index     */
  keylen = 14;                  /* YYMMLDEV.DDHHQ            */
  keytyp = 0;                   /* alphanumeric              */
  keydup = 0;                   /* duplicates NOT allowed    */
  nomemb = 0;                   /* only one index here!      */
  xtdsiz = (unsigned)32 * 1024; /* force directory updates   */

  filmod = (PERMANENT | EXCLUSIVE);
  if (CREIDX(keyno, "LOADCTRL.NDX", keylen, keytyp, keydup,
             nomemb, xtdsiz, filmod)) {
    sprintf(work, "Error %d creating LOADCTRL.NDX", uerr_cod);
    msgbox(work, "Warning", MB_OK);
    return (1);
  }

  return (0);
}

int BuildLoadCtrlHistory(int do_year, int do_mon, DEVICE_LOAD_CTRL* load_ctrl_history) {
  char work[80];
  char search_key[20];
  char last_key[20];
  char found_key[20];
  char new_key[20];

  UCHAR current_level;

  int i;
  int found_ctrl_level;
  int device_ctrl_level;

  USHORT device_index;
  USHORT current_id;

  struct tm tm_work;

  DEVICE_LOAD_CTRL* current_history;

  if (open_history()) {
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    return (1);
  }

  if (CreateLoadCtrlIndex() != 0) {
    CLSFIL(3, COMPLETE);
    close_history();
    return (1);
  }

  // Populate the new index file
  for (i = 0; i < dev_list.devices_used; ++i) {
    if ((dev_list.devices[i].priority > 0) && (IsSubmeter(&dev_list.devices[i]))) {
      sprintf(search_key, "%04X%3d%2d      ",
              dev_list.devices[i].device_slot, do_year, do_mon);
      strcpy(last_key, search_key);
      while (1) {
        if (next_history(last_key) == 0) {
          if (memcmp(last_key, search_key, 9) != 0) {
            break;
          }

          found_ctrl_level = (hist_tran.ht_flags >> 2) & 7;
          device_ctrl_level = hist_tran.ht_flags & 3;
          if ((found_ctrl_level > 0) && (device_ctrl_level <= found_ctrl_level)) {
            sprintf(new_key, "%03d%02d%2d%04X%d%2d%d",
                    hist_tran.ht_yr, hist_tran.ht_mo, hist_tran.ht_da,
                    hist_tran.ht_device_id, found_ctrl_level,
                    hist_tran.ht_hr, hist_tran.ht_qthr);

            if (ADDKEY(3, new_key, recbyte, REGADD)) {
              sprintf(work, "BuildLoadCtrl, ADDKEY err: %d", uerr_cod);
              msgbox(work, "Warning", MB_OK);
              CLSFIL(3, COMPLETE);
              close_history();
              unlink("LOADCTRL.NDX");
              return (1);
            }
          }
        } else {
          break;
        }
      }
    }
  }

  current_history = load_ctrl_history;
  monthly_total_credit = monthly_total_penalty = 0.0;
  load_ctrl_hist_count = 0;

  // now use the newly built index file...
  recbyte = FRSKEY(3, found_key);
  current_id = current_level = 0;

  while (recbyte) {
    if (REDREC(1, recbyte, &hist_tran)) {
      CLSFIL(3, COMPLETE);
      close_history();
      sprintf(work, "next_history, REDREC err: %d", uerr_cod);
      msgbox(work, "Warning", MB_OK);
      return (1);
    }

    // test for a new control operation
    if ((hist_tran.ht_device_id != current_id) ||
        (((hist_tran.ht_flags >> 2) & 7) != current_level) ||
        (hist_tran.ht_qthr * 15 != tm_work.tm_min) ||
        (hist_tran.ht_hr != tm_work.tm_hour) ||
        (hist_tran.ht_da != tm_work.tm_mday) ||
        (hist_tran.ht_mo - 1 != tm_work.tm_mon) ||
        (hist_tran.ht_yr != tm_work.tm_year)) {
      if (++load_ctrl_hist_count > MAX_LOAD_CTRL_HISTORY) {
        load_ctrl_hist_count--;
        CLSFIL(3, COMPLETE);
        close_history();
        msgbox("Memory size exceeded", "Warning", MB_OK);
        return (1);
      }

      if (current_id != 0) {
        if ((current_history->credit > 0.0) || (current_history->penalty > 0.0)) {
          current_history++;
        } else {
          load_ctrl_hist_count--;
        }
      }

      memset(current_history, 0, sizeof(DEVICE_LOAD_CTRL));
      current_id = hist_tran.ht_device_id;
      current_level = (hist_tran.ht_flags >> 2) & 7;

      tm_work.tm_year = hist_tran.ht_yr;
      tm_work.tm_mon = hist_tran.ht_mo - 1;
      tm_work.tm_mday = hist_tran.ht_da;
      tm_work.tm_hour = hist_tran.ht_hr;
      tm_work.tm_min = hist_tran.ht_qthr * 15;
      tm_work.tm_sec = 0;
      tm_work.tm_isdst = _daylight;
      current_history->start_time = mktime(&tm_work);
      for (i = 0; i < dev_list.devices_used; i++) {
        if (dev_list.devices[i].device_slot == hist_tran.ht_device_id) {
          device_index = i;
          break;
        }
      }
      for (i = 0; i < dev_list.devices_used; i++) {
        if ((dev_list.devices[i].monitor_hc == dev_list.devices[device_index].cebus_hc) &&
            (dev_list.devices[i].monitor_unit == dev_list.devices[device_index].cebus_unit) &&
            (dev_list.devices[i].monitor_subunit == dev_list.devices[device_index].cebus_subunit)) {
          current_history->device_id = i;
          break;
        }
      }
      current_history->level = (hist_tran.ht_flags >> 2) & 7;
      current_history->override = 'N';
    }

    current_history->duration += 15;
    current_history->kwh += hist_tran.ht_kwh;

    if (hist_tran.ht_flags & 32) // Penalty
    {
      current_history->override = 'Y';
      current_history->penalty += hist_tran.ht_kwh_cost * hist_tran.ht_kwh;
      monthly_total_penalty += hist_tran.ht_kwh_cost * hist_tran.ht_kwh;
    } else if (hist_tran.ht_flags & 64) // Credit
    {
      current_history->credit += hist_tran.ht_kwh_cost * hist_tran.ht_kwh;
      monthly_total_credit += hist_tran.ht_kwh_cost * hist_tran.ht_kwh;
    }

    tm_work.tm_min += 15;
    mktime(&tm_work);

    recbyte = NXTKEY(3, found_key);
  }

  CLSFIL(3, COMPLETE);
  close_history();
  unlink("LOADCTRL.NDX");

  return (0);
}
#endif

int open_history() {
  flushdos(0);
  if (OPNFIL(1, "HISTORY.DAT", PERMANENT | EXCLUSIVE)) {
    if (uerr_cod == 12) /* file not found */
    {
      create_history();
      return (0);
    }
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    return (1);
  }
  if (OPNFIL(2, "HISTORY.NDX", PERMANENT | EXCLUSIVE)) {
    CLSFIL(1, COMPLETE);
    msgbox("HISTORY NDX not opened.", "Warning", MB_OK);
    return (1);
  }
  return (0);
}

next_history(char* last_key) {
  char key[30];

  if ((recbyte = GTKEY(2, last_key, key)) == 0) {
    if (uerr_cod == 0) {
      return (1);
    } else {
      ctree_error();
      return (1);
    }
  }
  memcpy(last_key, key, 15);
  if (REDREC(1, recbyte, &hist_tran)) {
    ctree_error();
    return (1);
  }
  return (0);
}

add_hist_rec(void) {
  char key[30];

  recbyte = NEWREC(1);
  if (recbyte == (ULONG)0) {
    ctree_error();
    return (1);
  }

  sprintf(key, "%04X%3d%2d%2d%2d%1d", hist_tran.ht_device_id,
          hist_tran.ht_yr, hist_tran.ht_mo,
          hist_tran.ht_da, hist_tran.ht_hr,
          hist_tran.ht_qthr);
  if (ADDKEY(2, key, recbyte, REGADD)) {
    if (uerr_cod == KEY_DUP_ERR) {
      //sprintf(work, "Error %s duplicated.  Error: ", key);
      //ctree_error(work);
      return (0);
    } else {
      ctree_error();
    }
    RETREC(1, recbyte);
    return (1);
  }
  if (WRTREC(1, recbyte, &hist_tran)) {
    ctree_error();
    return (1);
  }
  LOKREC(1, FREE, recbyte);
  return (0);
}

update_history(USHORT device_id, int display_line) {
  UCHAR key[30];
  UCHAR last_key[30];
  UCHAR dev_description[14];
  int month;
  int day;
  int year;
  int i;
  int leap;
  int result;
  HISTORY_DAY history_day;
  struct tm outside_tm;
  int device_index;

  outside_time(&outside_tm);

  sprintf(last_key, "%04X9999999999", device_id);
  recbyte = LTKEY(2, last_key, key);
  if (recbyte == 0) {
    if (uerr_cod == 0) {
      month = day = year = 0;
    } else {
      ctree_error();
      close_history();
      return (1);
    }
  } else {
    movez(last_key, key, 4);
    if (device_id == (unsigned)htoi(last_key)) {
      movez(last_key, key + 9, 2);
      day = atoi(last_key);
      movez(last_key, key + 7, 2);
      month = atoi(last_key);
      movez(last_key, key + 4, 3);
      year = atoi(last_key);
    } else {
      month = day = year = 0;
    }
  }
  device_index = LookupDev(device_id);
  if (device_index == -1) {
    sprintf(dev_description, "Whole House");
  } else {
    sprintf(dev_description, "%.13s", dev_list.devices[device_index].desc);
    trim(dev_description);
  }
  while (1) {
    if (display_line > 0) {
      area_clear(219 + 115, 233 + 115, 175, 467, FG_WHT);
      sprintf(buffer, "Updating %s, %02d/%02d/%04d", dev_description, month, day, year + 1900);
      ptext(buffer, 219 + 115, 175, BG_WHT + FG_BLK);
    }
    result = collect_intervals(device_id, month, day, year, &history_day);
    if (result == SUCCESS) {
      // put in file
      hist_tran.ht_device_id = device_id;
      hist_tran.ht_yr = history_day.year;
      hist_tran.ht_mo = history_day.month;
      hist_tran.ht_da = history_day.day;
      hist_tran.ht_hr = history_day.first_period / 4;
      hist_tran.ht_qthr = history_day.first_period % 4;
      for (i = 0; i < history_day.number_periods; ++i) {
        hist_tran.ht_kwh = (double)(history_day.period[i].kwh) / 10000.0;
        hist_tran.ht_kwh_cost = (double)(history_day.period[i].cost_kwh) / 100000.0;
        hist_tran.ht_rate_type = history_day.period[i].rate_type;
        hist_tran.ht_flags = history_day.period[i].flags;
        add_hist_rec();
        if (++hist_tran.ht_qthr > 3) {
          hist_tran.ht_qthr = 0;
          ++hist_tran.ht_hr;
        }
      }
      year = history_day.year;
      month = history_day.month;
      day = history_day.day;
      ++day;
      if ((year % 4) == 0) {
        leap = 1;
      } else {
        leap = 0;
      }
      if (day > day_tab[leap][month]) {
        day = 1;
        ++month;
        if (month > 12) {
          month = 1;
          ++year;
        }
      }
      if ((year > outside_tm.tm_year) ||
          ((year == outside_tm.tm_year) && (month > (outside_tm.tm_mon + 1))) ||
          ((year == outside_tm.tm_year) && (month == (outside_tm.tm_mon + 1)) && (day > outside_tm.tm_mday)))
        break;
    } else if (result == FAILURE) {
      close_history();
      return (1);
    } else // Most likely Midnight to 12:15am -- no history available
    {
      break;
    }
  }
  return (0);
}

extern prev_day(UCHAR* month, UCHAR* day, UCHAR* year);

int UpdateHistoryFast(int days_back) {
  UCHAR key[30];
  UCHAR last_key[30];
  UCHAR work[50];
  int month;
  int day;
  int year;
  UCHAR m;
  UCHAR d;
  UCHAR y;
  int dmonth;
  int dday;
  int dyear;
  int i;
  int leap;
  int result;
  int j;
  int k;
  int intr;
  long int dt_work1;
  long int dt_work2;
  FAST_HISTORY_DAY_T* fhd;
  FAST_PERIOD_DATA* pptr;
  struct tm outside_tm;

  outside_time(&outside_tm);
  open_history();

  if (days_back < 0) {
    // find last whole house entry in history file....
    sprintf(last_key, "FFFF9999999999");
    recbyte = LTKEY(2, last_key, key);
    if (recbyte == 0) {
      if (uerr_cod == 0) {
        month = day = year = 0;
      } else {
        ctree_error();
        close_history();
        return (1);
      }
    } else {
      movez(last_key, key, 4);
      if (0x0FFFF == (unsigned)htoi(last_key)) {
        movez(last_key, key + 9, 2);
        day = atoi(last_key);
        movez(last_key, key + 7, 2);
        month = atoi(last_key);
        movez(last_key, key + 4, 3);
        year = atoi(last_key);
      } else {
        month = day = year = 0;
      }
    }
    // check for date older than whole house for individual devices...
    if ((month + day + year) != 0) {
      dt_work1 = (long)year * 10000L + (long)month * 100L + (long)day;
      for (i = 0; i < dev_list.devices_used; ++i) {
        if (IsSubmeter(&dev_list.devices[i])) {
          sprintf(last_key, "%04X9999999999", dev_list.devices[i].device_slot);
          recbyte = LTKEY(2, last_key, key);
          if (recbyte == 0) {
            if (uerr_cod == 0) {
              update_history(dev_list.devices[i].device_slot, 9); // "old way"
              area_clear(219 + 115, 233 + 115, 175, 467, FG_WHT);
            } else {
              ctree_error();
              close_history();
              return (1);
            }
          } else {
            movez(last_key, key, 4);
            if (dev_list.devices[i].device_slot == (unsigned)htoi(last_key)) {
              movez(last_key, key + 9, 2);
              dday = atoi(last_key);
              movez(last_key, key + 7, 2);
              dmonth = atoi(last_key);
              movez(last_key, key + 4, 3);
              dyear = atoi(last_key);
              dt_work2 = (long)dyear * 10000L + (long)dmonth * 100L + (long)dday;
              if (dt_work2 < dt_work1) {
                day = dday;
                month = dmonth;
                year = dyear;
                dt_work1 = (long)year * 10000L + (long)month * 100L + (long)day;
              }
            } else // no history for this device
            {
              update_history(dev_list.devices[i].device_slot, 9); // "old way"
              area_clear(219 + 115, 233 + 115, 175, 467, FG_WHT);
            }
          }
        }
      }
    }
  } else {
    m = this_month;
    d = today;
    y = this_year;
    for (i = 0; i < days_back; i++) {
      prev_day(&m, &d, &y);
    }
    month = m;
    day = d;
    year = y;
  }

  //   fhd = (void *)f_buffer;
  while (1) {
    sprintf(work, "For: %d/%02d/%04d   ", month, day, year + 1900);
    ptext(work, 205 + 115 + 14, 175, BG_WHT + FG_BLK);
    memset(f_buffer, 0, sizeof(FAST_HISTORY_DAY_T));
    sprintf(work, "%c%c%c", year, month, day);
    result = FetchTable(f_buffer,
                        sizeof(FAST_HISTORY_DAY_T),
                        FAST_HISTORY_TBL,
                        work,
                        3,
                        RETRY_COUNT,
                        1);
    //sprintf(work, "Got: %ld", TableSize);
    //Status(work);
    if (result == 10) {
      close_history();
      return (0); // no data for that day
    } else if (result == 0) {
      decompress(&f_buffer[10000], f_buffer, sizeof(FAST_HISTORY_DAY_T));
      fhd = (void*)&f_buffer[10000];

      // put in file
      hist_tran.ht_yr = fhd->year;
      hist_tran.ht_mo = fhd->month;
      hist_tran.ht_da = fhd->day;
      intr = fhd->first_interval;
      for (i = 0; i < fhd->number_intervals; ++intr, ++i) {
        pptr = &fhd->period[i]; // workaround for compiler bug
        // common
        hist_tran.ht_hr = intr / 4;
        hist_tran.ht_qthr = intr % 4;
        hist_tran.ht_rate_type = pptr->rate_type;

        // whole house
        hist_tran.ht_device_id = 0x0FFFF;
        if (pptr->meter_pulses == 0) {
          hist_tran.ht_kwh = (double)(pptr->meter_pulses * .0006);
        } else {
          hist_tran.ht_kwh = (double)(pptr->meter_pulses * ((double)(cust_info.meter_scalar) / 10000.0));
        }
        hist_tran.ht_kwh_cost = (double)pptr->cost_per_kwh / 100000.0;
        hist_tran.ht_flags = pptr->shedding_signal * 4;
        add_hist_rec();
        // sub-metered devices
        for (j = 0; j < dev_list.devices_used; ++j) {
          if (IsSubmeter(&dev_list.devices[j])) {
            k = dev_list.devices[j].history_bucket - 1;
            hist_tran.ht_device_id = dev_list.devices[j].device_slot;
            hist_tran.ht_kwh = (double)(pptr->device_watt_hrs[k]) * .0001;
            hist_tran.ht_flags = pptr->flags[k];
            if (hist_tran.ht_flags & 0x020) // penalty assessed
            {
              hist_tran.ht_kwh_cost = (double)pptr->penalty_per_kwh * 0.00001;
            }
            if (hist_tran.ht_flags & 0x040) // credit given
            {
              hist_tran.ht_kwh_cost = (double)pptr->credit_per_kwh * 0.00001;
            }
            add_hist_rec();
          }
        }
      }

      year = fhd->year;
      month = fhd->month;
      day = fhd->day;

      ++day;
      if ((year % 4) == 0) {
        leap = 1;
      } else {
        leap = 0;
      }
      if (day > day_tab[leap][month]) {
        day = 1;
        ++month;
        if (month > 12) {
          month = 1;
          ++year;
        }
      }
      if ((year > outside_tm.tm_year) ||
          ((year == outside_tm.tm_year) && (month > (outside_tm.tm_mon + 1))) ||
          ((year == outside_tm.tm_year) && (month == (outside_tm.tm_mon + 1)) && (day > outside_tm.tm_mday))) {
        break; // that's all  he has!
      }
    } else if (result == FAILURE) {
      close_history();
      return (1);
    } else // Most likely Midnight to 12:15am -- no history available
    {
      break;
    }
  }
  close_history();
  return (0);
}

int initialize_ctree(void) {
  int i;
  COUNT bufs, fils, sect;
  UCHAR err[50];

  bufs = 6;
  fils = 10;
  sect = 4;
  if (i = INTREE(bufs, fils, sect)) {
    sprintf(err, "Could not initialize c-tree.|"
                 "Error code: %d               ",
            i);
    msgbox(err, "Warning", MB_OK);
    ExceptionLog(err);
    return (1);
  }
  return (0);
}

int process_interval_15m(INTERVAL_DATA_15* ivd) {
  struct HIST_TRAN save_hist;
  POINTER save_recbyte;

  if (open_history()) {
    return (1);
  }
  save_recbyte = recbyte;
  memcpy(&save_hist, &hist_tran, sizeof(struct HIST_TRAN));
  // put in file
  hist_tran.ht_device_id = ivd->device_id;
  hist_tran.ht_yr = ivd->yr;
  hist_tran.ht_mo = ivd->mo;
  hist_tran.ht_da = ivd->da;
  hist_tran.ht_hr = ivd->hr;
  hist_tran.ht_qthr = ivd->qthr;
  hist_tran.ht_kwh = (double)(ivd->kwh) / 10000.0;
  hist_tran.ht_kwh_cost = (double)(ivd->kwh_cost) / 100000.0;
  hist_tran.ht_rate_type = ivd->rate_type;
  hist_tran.ht_flags = ivd->flags;
  add_hist_rec();
  close_history();
  recbyte = save_recbyte;
  memcpy(&hist_tran, &save_hist, sizeof(struct HIST_TRAN));
  return (0);
}

extern int BILLING_STATEMENT_MAX;
void FirstHistory(void) {
  int m_dif;
  int y_dif;
  int day;
  int month;
  int year;
  char last_key[30];
  char key[30];

  time(&ltime);
  tod = localtime(&ltime);

  BILLING_STATEMENT_MAX = 0;
  open_history();
  // find first whole house entry in history file....
  sprintf(last_key, "FFFF          ");
  recbyte = GTKEY(2, last_key, key);
  if (recbyte == 0) // no entries found
  {
    if (uerr_cod == 0) // no errors simply no record found
    {
      month = day = year = 0;
    } else // some kind of error
    {
      ctree_error();
      close_history();
    }
  } else // a record was found
  {
    movez(last_key, key, 4);
    if (0x0FFFF == (unsigned)htoi(last_key)) // wazzit whole house?
    {
      movez(last_key, key + 9, 2);
      day = atoi(last_key);
      movez(last_key, key + 7, 2);
      month = atoi(last_key);
      movez(last_key, key + 4, 3);
      year = atoi(last_key);

      /////////////////////////////////////////////////
      y_dif = tod->tm_year - year;
      m_dif = tod->tm_mon + 1 - month;
      BILLING_STATEMENT_MAX = (y_dif * 12) + m_dif;
      /////////////////////////////////////////////////
    } else // no, treat like no record
    {
      month = day = year = 0;
      msgbox("ERROR: NOT WHOLE HOUSE", "ERROR", MB_OK);
    }
  }
  close_history();
}

LWINDOW_T reix_progWindow[1] = {290, 160, 310, 115, //window position
                                NULL,               //screen object
                                NULL,
                                0,                        //item count
                                0,                        //cursor position 0-79 left to right
                                0,                        //element that has current focus
                                fcsBtn,                   //focus type
                                "Re-building Index File", // title
                                NULL,                     //OnKeyPress();
                                0};

void WriteIndex(void) // Used by conversion function, below
{
  UCHAR err[100];
  char key[30];
  int i;

  sprintf(key, "%04X%3d%2d%2d%2d%1d", hist_tran.ht_device_id,
          hist_tran.ht_yr, hist_tran.ht_mo,
          hist_tran.ht_da, hist_tran.ht_hr,
          hist_tran.ht_qthr);

  if (i = ADDKEY(2, key, recbyte, INCADD)) {
    if (i == KEY_DUP_ERR) // This interval already present
    {
      RETREC(1, recbyte); // delete 2nd instance
    } else {
      sprintf(err, "cTree error <%d> adding to HISTORY.NDX \n", uerr_cod);
      msgbox(err, "Fatal Error!", MB_OK);
      ExceptionLog(err);
      lv_stop();
    }
  }
  return;
}

//==========================================================================
// Convert2Release160
//
// Routine History:
// Author               Date            Description
// GES                  3/12/98            Converts History index file
//                                    to Y2K-compliant format
//                  ** NOTE **
//   The Index entries originally were 13 characters long: DDDDYYMMDDHHQ
//               DDDD = Device Id (Hex number represented by ASCII
//               YY    = Year - 1900
//               MM   = Month in ASCII
//               DD   = Day of the month
//               HH   = Hour (00-23)
//               Q    = Quarter hour number (0-3)
//  The NEW format is 14 characters long: DDDDYYYMMDDHHQ
//                  YYY = Year - 1900, but will work well past the year 2000!!!
//
//
// Inputs:
//  Name               Type            Description
//   HISTORY.DAT                        The history Data file....
//
//
// Outputs:
//  Name               Type            Description
//   HISTORY.NDX                           The new Index file.....
//
// Description:
//   This routine called at program start-up will check for the need to
//   convert the history index....If needed, it will ask the operator
//   for the go-ahead, exit if the operator doesn't want to proceed, or
//   perform the conversion if allowed.
//==========================================================================

void Convert2Release160(void) {
  UCHAR err[100];
  int rc;
  ULONG counter;
  COUNT filmod;
  COUNT keyno;
  COUNT keylen;
  COUNT keytyp;
  COUNT keydup;
  COUNT nomemb;
  UCOUNT xtdsiz;
  double total;
  CTFILE file_hdr;
  FILE* stream;

  PROGRESS_INDICATOR_T pi = {245 + 115, 220, 200, 0, FG_BLU, 0};

  stream = fopen("History.ndx", "rb");
  if (stream == NULL) {
    return; // if there's no history file, it'll be created later.
  }

  fread(&file_hdr, 1, sizeof(CTFILE), stream);
  fclose(stream);

  if (file_hdr.length == 14) // it's the new size!!
  {
    return;
  }

  stream = fopen("HISTORY.DAT", "rb");
  if (stream == NULL) {
    return;
  } else {
    fread(&file_hdr, sizeof(CTFILE), 1, stream);
    fclose(stream);
    if (file_hdr.reclen != sizeof(hist_tran)) {
      strcpy(err, "The History file is not compatible|"
                  "with this version ...sorry        |");
      msgbox(err, "Data File Version Mis-match!      ", MB_OK);
      ExceptionLog(err);
      lv_stop();
    }
    total = (double)file_hdr.nument;
  }

  if (total == 0) {
    CLSFIL(1, COMPLETE);
    CLSFIL(2, COMPLETE);
    return; // all done!
  }

  rc = msgbox("   Your history index file     |"
              "   needs to be converted.      |"
              "   Is it OK to proceed now?    |"
              "[FYI: A large history file on  |"
              "     a slow computer may take  |"
              "     up to an hour to convert.]",
              "Index File Version Mis-match!", MB_YES_NO);
  switch (rc) {
  default:
    lv_stop();
    return;
  case MB_RESULT_YES:
    break;
  }

  // proceed with conversion.....

  glbWindow = reix_progWindow;
  PutWindow(glbWindow);
  glbWindow = NULL;

  /******************************/
  /* create NEW index file     */
  /******************************/
  unlink("HISTORY.NDX");

  keyno = 2;                      /* index date,time index    */
  keylen = 14;                    /* DEV.YYYMMDDHHQ           */
  keytyp = 0;                     /* alphanumeric   index       */
  keydup = 0;                     /* duplicates NOT allowed   */
  nomemb = 0;                     /* only one index here!       */
  xtdsiz = (UCOUNT)(32L * 1024l); /* force directory updates  */

  filmod = (PERMANENT | SHARED);

  if (CREIDX(keyno, "HISTORY.NDX", keylen, keytyp, keydup,
             nomemb, xtdsiz, filmod)) {
    sprintf(err, "Error %d creating HISTORY.NDX|"
                 "Check disk space.             ",
            uerr_cod);
    msgbox(err, "Fatal Error", MB_OK);
    ExceptionLog(err);
    lv_stop();
  }

  CLSFIL(2, COMPLETE);
  if (open_history()) {
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  PutIndicator(&pi);

  // locate beginning of actual data
  for (recbyte = 0; recbyte <= 128; recbyte += sizeof(struct HIST_TRAN)) {
    ;
  }

  counter = 0;
  while (1) {
    rc = REDREC(1, recbyte, &hist_tran);

    if (rc == LEOF_ERR) // end of file
    {
      break;
    }

    if (rc != 0) // Some other error....
    {
      sprintf(err, "cTree error <%d> reading HISTORY.DAT", uerr_cod);
      msgbox(err, "Fatal Error!", MB_OK);
      ExceptionLog(err);
      lv_stop();
    }

    if (*(unsigned char*)&hist_tran == 0x0ff) {
      recbyte += sizeof(struct HIST_TRAN);
      continue;
    }

    if (hist_tran.ht_mo > 12) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    } else if (hist_tran.ht_mo < 1) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    } else if (hist_tran.ht_da < 1) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    } else if (hist_tran.ht_da > 31) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    } else if (hist_tran.ht_yr < 95) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    }
    if ((hist_tran.ht_da == 0) || (hist_tran.ht_yr == 0)) {
      recbyte += sizeof(struct HIST_TRAN);
      continue; // leave bad data there, but don't put in index!!
    }

    // add to index
    WriteIndex();
    recbyte += sizeof(struct HIST_TRAN);

    // Using err, because it's there to be used
    if ((((++counter) % 200) == 0)) {
      pi.progress = (int)((counter / (total)) * 100.0);
      UpdateIndicator(&pi);
    }
  }

  CLSFIL(1, COMPLETE);
  CLSFIL(2, COMPLETE);
}
