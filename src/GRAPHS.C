// ==========================================================================
// $Workfile:   GRAPHS.C  $
// $Revision:   1.27  $
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

#include "ctstdr.h" /* req'd by ctree */
#include "ctdecl.h" /* req'd by ctree */
#include "ctoptn.h" /* used to access ctree's innards */
#include "ctstrc.h" /* used to access ctree's innards */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <bios.h>
#include <conio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>

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
//#include "dsdmenu.h"
#include "whmenu.h"
#include "inutil.h"
#include "inio.h"
#include "insubs.h"
#include "rate.h"
#include "graphs.h"
#include "msgbox.h"
#include "ismenu.h"
#include "calendar.h"
#include "frame_3d.h"
#include "checkbox.h"
#include "device.h"
#include "tmentry.h"
#include "mouse.h"

EXTERN COUNT uerr_cod;  // req'd by cTree
extern POINTER recbyte; // used by cTree routines.

static void horiz_percent_graph(
    int row,
    double total_kw,
    double total_dollars,
    double full_kw,
    double full_dollars);
static void DaysRatesDisplay(void);
static UCHAR DevBoxTxtList[10][14];
static double GetDeviceDollars(USHORT device_id);
static double GetDevicekW(USHORT device_id);
static double GetTotalkW(void);
static double GetTotalDollars(void);
static void USAGEDetailClick(void);
static void COSTDetailClick(void);
static void MeterHistorySwitchClick(void);
//==============================================
//
//   Daily Usage Section.
//
//==============================================
#define COST 0
#define USAGE 1

static USHORT du_device_id;
int du_do_mon = 0;
int du_do_day = 0;
int du_do_year = 0;
int detail_caller;
int detail_mode = 1;

int f_month;
int f_year;
int f_day;

static void duBackBtnClick(void);
static void duNextBtnClick(void);
//static void OL(void);
static void DailyUsageOverLay(void);
static void DailyCostOverLay(void);
static void MonthlyCostEffectiveOverLay(void);
static void MonthlyCostOverLay(void);
static void MonthlyUsageOverLay(void);
static void ReturnClick(void);
static void DeviceDetailSwitchClick(void);

void (*DUO_addr)() = DailyUsageOverLay;
void (*DCO_addr)() = DailyCostOverLay;
void (*MCEO_addr)() = MonthlyCostEffectiveOverLay;
void (*MCO_addr)() = MonthlyCostOverLay;
void (*MUO_addr)() = MonthlyUsageOverLay;
void (*MHSC_addr)() = MeterHistorySwitchClick;
void (*DDSC_addr)() = DeviceDetailSwitchClick;

#define DOTS_HIGH 220
#define TOP 80

#define DLC 2  // Column of Left hand labels
#define DLP 80 // Pixel position of Left Side
#define WIDTH 480

int which_graph;
//defines for use of which_graph
#define DAILY_USAGE 0
#define DAILY_COST 1
#define MONTHLY_USAGE 2
#define MONTHLY_COST 3
#define MONTHLY_COST_EFF 4

double FindMax(double peak) {
  double max;

  if (peak <= 0.05) {
    max = 0.05;
  }
  if (peak <= 0.10) {
    max = 0.10;
  }
  if (peak <= 0.25) {
    max = 0.25;
  } else if (peak <= 0.50) {
    max = 0.50;
  } else if (peak <= 1.00) {
    max = 1.00;
  } else if (peak <= 2.50) {
    max = 2.50;
  } else if (peak <= 5.00) {
    max = 5.00;
  } else if (peak <= 10.00) {
    max = 10.00;
  } else if (peak <= 25.00) {
    max = 25.00;
  } else if (peak <= 50.00) {
    max = 50.00;
  } else if (peak <= 100.00) {
    max = 100.00;
  } else if (peak <= 150.00) {
    max = 150.00;
  } else if (peak <= 200.00) {
    max = 200.00;
  } else if (peak <= 500.00) {
    max = 500.00;
  } else if (peak <= 1000.00) {
    max = 1000.00;
  } else if (peak <= 5000.00) {
    max = 5000.00;
  } else if (peak <= 10000.00) {
    max = 10000.00;
  } else if (peak <= 25000.00) {
    max = 25000.00;
  } else {
    max = 50000.00;
  }
  return (max);
}

static void FixClick(void);

CHECK_BOX_T fix_scale =
    {
        61, 82, 0, 0,        // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "Fix Vertical Axis", // Text...
        FALSE,               // Checked - TRUE or FALSE
        FixClick};

static void FixClick(void) {
  if (fix_scale.checked == TRUE) {
    return;
  }
  switch (which_graph) {
  case DAILY_USAGE:
    DailyUsageOverLay();
    break;
  case DAILY_COST:
    DailyCostOverLay();
    break;
  case MONTHLY_USAGE:
    MonthlyUsageOverLay();
    break;
  case MONTHLY_COST:
    MonthlyCostOverLay();
    break;
  case MONTHLY_COST_EFF:
    MonthlyCostEffectiveOverLay();
    break;
  }
}

PUSH_BUTTON_T device_btn[11] = {
    342, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    22,

    361, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    23,

    380, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    24,

    399, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    25,

    418, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    26,

    437, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    27,

    323, 132, 120, 17, // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    28,

    342, 132, 120, 17, // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    29,

    361, 132, 120, 17, // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    30,

    380, 132, 120, 17, // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "blank",           //text
    set,               //type
    up,                //state
    NULL,              //icon
    DailyUsageOverLay, //function
    31,

    323, 10, 120, 17,  // position
    FALSE,             //focus
    PUSH_BUTTON,       //type
    FALSE,             // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "Whole House",     //text
    set,               //type
    down,              //state
    NULL,              //icon
    DailyUsageOverLay, //function
    32};

//Daily Usage Screen
PUSH_BUTTON_T duBtnList[5] = {419, 149, 89, 33, // position
                              FALSE,            // focus
                              PUSH_BUTTON,      // type
                              TRUE,             // visible
                              BtnMouseDown,     // OnMouseDown()
                              BtnMouseUp,       // OnMouseUp()
                              BtnClick,         // OnClick()
                              BtnKeyPress,      // OnKeyPress()
                              PutButton,        // DisplayObject()
                              FALSE,            // default button
                              "\x9E Back \x9E", // text
                              push,             // type
                              up,               // state
                              NULL,             // icon
                              duBackBtnClick,   // function
                              33,

                              419, 248, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "\x9F Next \x9F",
                              push,
                              up,
                              NULL,
                              duNextBtnClick,
                              34,

                              419, 347, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "View|Detail", //tabular toggle switch
                              push,
                              up,
                              NULL,
                              //ViewDetailUsage, //void device_detail(int device_id)   static void ViewDetail(void);
                              USAGEDetailClick,
                              35,

                              419, 446, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "Cost|Graph",
                              push,
                              up,
                              NULL,
                              daily_cost, //void device_detail(int device_id)   static void ViewDetail(void);
                              36,

                              419, 545, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              TRUE,
                              NULL,
                              push,
                              up,
                              icon[12],
                              ReturnClick,
                              37};

SCREEN_OBJECT_T* duSO[17];
static void DailyUsageDraw(void);
LWINDOW_T duWindow[1] = {
    35, 0, 640, 445, //window position
    duSO,            //screen object
    DailyUsageDraw,
    17,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    "",               //title
    DefaultOnKeyPress //OnKeyPress();
};

//============================================================================
//   daily_usage() displays a graph of kwh usage for each day
//  DailyUsageOverLay() allows the user selection of a device to display
//============================================================================
void daily_usage(void) {
  int i;
  int j;
  UCHAR* rtnbtn[2] = {"Main|Menu", "Util.|Hist."};
  if (Sender == IntroScreen) {
    duBtnList[4].text = rtnbtn[0];
  } else if (Sender == UtilityUsageMenu) {
    duBtnList[4].text = rtnbtn[1];
  }

  //   if (uuDateSelection != 1)   // not DATE_DETAIL
  //   {
  //      du_do_mon = 0;
  //      du_do_day = 0;
  //      du_do_year = 0;
  //   }

  which_graph = DAILY_USAGE;
  detail_caller = USAGE;

  du_device_id = 0x0FFFF;
  glbWindow = duWindow;
  for (i = 0; i < 5; i++) {
    duSO[i] = &duBtnList[i].so;
  }
  for (i = 5; i < 16; i++) {
    duSO[i] = &device_btn[i - 5].so;
    device_btn[i - 5].so.visible = FALSE;
    //      device_btn[i-5].pbf = DailyUsageOverLay;
    device_btn[i - 5].pbf = DUO_addr;
  }
  device_btn[10].so.visible = TRUE;
  duSO[16] = &fix_scale.so;
  fix_scale.checked = FALSE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }
  PutWindow(glbWindow);
}

static void DailyUsageDraw(void) {
  DailyUsageOverLay();

  frame_3d(326, 280, 85, 350, FT_PRESSED);

  pbutton(336, 287, 14, 14, PB_DN, FG_BLU);
  pbutton(354, 287, 14, 14, PB_DN, FG_CYN);
  pbutton(372, 287, 14, 14, PB_DN, FG_YEL);

  ptext("15 Minute kWh", 337, 309, BG_WHT + FG_BLK);
  ptext("Hourly kWh", 355, 309, BG_WHT + FG_BLK);
  ptext("Cost Per kWh", 373, 309, BG_WHT + FG_BLK);
}

double dumax;
double dumax1;

void DailyUsageOverLay(void) {
  char found_key[30];
  char id_string[30];
  char device_name[14];
  char buff[150];
  char buff2[100];

  int i;
  int j;
  int col;
  long row;
  int last_row;
  int last_col;
  int color;
  int key_len;
  int do_mon;
  int do_day;
  int do_year;
  int avg_month;

  long ltime;

  struct tm* tod;

  double* interval_usage;
  double peak;
  double peak1;
  double total;
  double work;
  double hour_rate;

  UCHAR load_ctrl[96];
  UCHAR rate_type[24];

  DAYS_RATES days_rates;

  USHORT device_id;

  Busy();

  du_device_id = device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      du_device_id = device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }

  if (device_id == 0x0FFFF) {
    strcpy(device_name, "Whole House  ");
  } else {
    i = LookupDev(device_id);
    if (i != -1) {
      sprintf(device_name, "%.13s", dev_list.devices[i].desc);
    } else {
      strcpy(device_name, "");
    }
  }

  if ((du_do_mon == 0) && (du_do_day == 0) && (du_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
    du_do_mon = do_mon;
    du_do_day = do_day;
    du_do_year = do_year;
  } else // we've been here
  {
    do_mon = du_do_mon;
    do_day = du_do_day;
    do_year = du_do_year;
  }

  i = day_of_week(do_mon, do_day, do_year);

  sprintf(buff, "Energy Usage Graph for:  %s - %s, %d/%d/%4d     ", device_name, wday[i], do_mon, do_day, 1900 + do_year);
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  sprintf(buff2, "Energy used on %d/%d/%4d was", do_mon, do_day, 1900 + do_year);

  avg_month = 0;

  interval_usage = (double*)f_buffer;

  if (fix_scale.checked == FALSE) {
    dumax = 0;
    dumax1 = 0;
  }

  if (do_mon == tod->tm_mon + 1 && do_day == tod->tm_mday && do_year == tod->tm_year) {
    days_rates = todays_rates;
  } else {
    switch (read_rates((UCHAR)do_mon, (UCHAR)do_day, (UCHAR)do_year, &days_rates)) {
    case 1:
    case 2:
    case 5:
      sprintf(buff,
              "The rate information for|%02d/%02d/%4d is not available.",
              do_mon, do_day, do_year + 1900);
      msgbox(buff, "Information", MB_OK);
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      NotBusy();
      unhide_mouse();
      return;
    case -1:
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      NotBusy();
      unhide_mouse();
      return;
    }
  }

  if (open_history()) {
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    lv_stop();
  }
  memset(interval_usage, 0, 96 * 2 * sizeof(double));
  memset(load_ctrl, 0, 96);
  for (i = 0; i < 24; ++i) {
    switch (cust_info.rate_type) {
    case 0:
      rate_type[i] = 0;
      break;
    case 1:
      rate_type[i] = 2;
      break;
    case 2:
      rate_type[i] = 4;
      break;
    }
  }

  total = 0.0;
  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, do_day);
  strcpy(id_string, found_key);
  if (next_history(found_key) == 0) {
    while (1) {
      if (memcmp(found_key, id_string, key_len) != 0) {
        break;
      }
      i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;
      *(interval_usage + i) += hist_tran.ht_kwh;
      *(interval_usage + i + 96) += hist_tran.ht_kwh_cost;
      if ((hist_tran.ht_rate_type & 0x06) != 0) {                   // NOTE:  Outages are recorded as rate type 0, so skip
        rate_type[hist_tran.ht_hr] = hist_tran.ht_rate_type & 0x06; // get customer type
      }

      if (((hist_tran.ht_flags >> 2) & 7) > 0) {
        if (device_id == 0x0FFFF) {
          load_ctrl[i] = 3;
        } else {
          if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7)) {
            if (hist_tran.ht_flags & 64 && hist_tran.ht_kwh > 0) // credit
            {
              load_ctrl[i] = 1;
            } else if (hist_tran.ht_flags & 32 && hist_tran.ht_kwh > 0) // penalty
            {
              load_ctrl[i] = 2;
            } else {
              load_ctrl[i] = 3; // unknown
            }
          }
        }
      }
      if (load_ctrl[i] != 1) {
        total += hist_tran.ht_kwh;
      }
      if (next_history(found_key)) {
        break;
      }
    }
  }
  peak = peak1 = 0.0;
  for (i = 0; i < 24; ++i) {
    for (j = 0, work = 0.0; j < 4; ++j) {
      work += *(interval_usage + i * 4 + j);
    }
    if (work > peak) {
      peak = work;
    }
  }
  for (i = 0; i < 24; ++i) {
    switch (rate_type[i]) {
    case 0:
      hour_rate = (double)days_rates.rs_rate / 100000;
      break;
    case 2:
    case 3:
      hour_rate = (double)days_rates.rtod_rate[i] / 100000;
      break;
    case 4:
    case 5:
      hour_rate = (double)days_rates.rsv_rate[i] / 100000;
      break;
    }
    if (hour_rate > peak1) {
      peak1 = hour_rate;
    }
  }

  if (fix_scale.checked == FALSE) {
    dumax = FindMax(peak);
    //dumax1 = FindMax(peak1*100.0)/100.0;
    dumax1 = FindMax(peak1);
  }
  hide_mouse();
  //============================
  // BORDERS
  //============================
  pbutton(TOP - 1, DLP - 1, DOTS_HIGH + 3, WIDTH + 2, PB_DN, BG_WHT);

  ptext("\xA0/kWh", 60, 560, BG_WHT + FG_BLK);
  sprintf(buff, "%2.0f\xA0", dumax1 * 100);
  ptext(buff, 5 * 14 + 5, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dumax1 * .8 * 100);
  ptext(buff, 5 * 14 + 5 + 44, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dumax1 * .6 * 100);
  ptext(buff, 5 * 14 + 5 + 88, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dumax1 * .4 * 100);
  ptext(buff, 5 * 14 + 5 + 132, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dumax1 * .2 * 100);
  ptext(buff, 5 * 14 + 5 + 176, 570, FG_BLK + BG_WHT);
  ptext(" 0\xA0", 5 * 14 + 5 + 220, 570, FG_BLK + BG_WHT);

  ptext("kWh", 60, 47, BG_WHT + FG_BLK);

  if (dumax < 10.00) {
    sprintf(buff, "%4.2f", dumax);
    ptext(buff, 5 * 14 + 5, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", dumax * .8);
    ptext(buff, 5 * 14 + 5 + 44, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", dumax * .6);
    ptext(buff, 5 * 14 + 5 + 88, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", dumax * .4);
    ptext(buff, 5 * 14 + 5 + 132, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", dumax * .2);
    ptext(buff, 5 * 14 + 5 + 176, 45, FG_BLK + BG_WHT);
    ptext("0.00", 5 * 14 + 5 + 220, 45, FG_BLK + BG_WHT);
  } else {
    sprintf(buff, "%4.0f", dumax);
    ptext(buff, 5 * 14 + 5, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", dumax * .8);
    ptext(buff, 5 * 14 + 5 + 44, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", dumax * .6);
    ptext(buff, 5 * 14 + 5 + 88, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", dumax * .4);
    ptext(buff, 5 * 14 + 5 + 132, 45, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", dumax * .2);
    ptext(buff, 5 * 14 + 5 + 176, 45, FG_BLK + BG_WHT);
    ptext("   0", 5 * 14 + 5 + 220, 45, FG_BLK + BG_WHT);
  }

  ptext("M", 22 * 14, DLP - 1, FG_BLK + BG_WHT);
  for (i = 1; i < 12; ++i) {
    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 22 * 14, DLP + i * 20 - 4, FG_BLK + BG_WHT);
    } else {
      ptext(buff, 22 * 14, DLP + i * 20 - 8, FG_BLK + BG_WHT);
    }
  }
  ptext("N", 22 * 14, DLP + 12 * 20 - 4, FG_BLK + BG_WHT);
  for (i = 1; i < 12; ++i) {
    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 22 * 14, DLP + (i + 12) * 20 - 4, FG_BLK + BG_WHT);
    } else {
      ptext(buff, 22 * 14, DLP + (i + 12) * 20 - 8, FG_BLK + BG_WHT);
    }
  }
  ptext("M", 22 * 14, DLP + WIDTH - 4, FG_BLK + BG_WHT);

  /* Hourly average kWh - Vertical Bars */
  color = FG_CYN;
  for (i = 0; i < 24; ++i) {
    col = DLP + (i * 20);
    for (j = 0, work = 0.0; j < 4; ++j) {
      work += *(interval_usage + i * 4 + j);
    }
    row = 300 - (int)((work / dumax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    area_clear((int)row, TOP + DOTS_HIGH, col + 1, col + 19, color);
  }

  // GRID
  for (i = 1; i < 24; ++i) {
    vline(DLP + i * 20, TOP + 1, TOP + DOTS_HIGH, FG_GRY);
  }
  for (i = 0; i < 9; ++i) {
    hline(TOP + 22 + 22 * i, DLP + 1, DLP + WIDTH - 1, FG_GRY);
  }

  // Cost - Line Graph
  last_row = -1;
  color = FG_YEL;
  for (i = 0; i < 24; ++i) {
    switch (rate_type[i]) {
    case 0:
      hour_rate = (double)days_rates.rs_rate / 100000;
      break;
    case 2:
    case 3:
      hour_rate = (double)days_rates.rtod_rate[i] / 100000;
      break;
    case 4:
    case 5:
      hour_rate = (double)days_rates.rsv_rate[i] / 100000;
      break;
    }
    col = DLP + 1 + (i * 20);

    row = 300 - (int)((hour_rate / dumax1) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    line(col, (int)row, col + 19, (int)row, color);
    if (last_row != -1) {
      line(col, last_row, col, (int)row, color);
    } else {
      last_col = col;
      last_row = (int)row;
    }
    last_col = col;
    last_row = (int)row;
  }

  /* kWh - Line Graph */
  last_row = -1;
  for (i = 0; i < 96; ++i) {
    switch (load_ctrl[i]) {
    case 1:
      color = FG_LGRN; // load control credit
      break;
    case 2:
      color = FG_RED; // load control penalty
      break;
    case 3:
      color = FG_MAG; // load control active
      break;
    default:
      color = FG_BLU; // load control inactive
      break;
    }
    col = DLP + 3 + (i * 5);
    row = 300 - (int)((*(interval_usage + i) / dumax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    if (last_row != -1) {
      line(last_col, last_row, col, (int)row, color);
      line(last_col, last_row - 1, col, (int)row - 1, color);
    }
    last_col = col;
    last_row = (int)row;
  }

  sprintf(buff, " %.1f kWh.", total);
  strcat(buff2, buff);
  area_clear(391, 409, 287, 625, FG_WHT);
  ptext(buff2, 392, 287, BG_WHT + FG_BLK);

  time(&ltime);
  tod = localtime(&ltime);

  close_history();
  NotBusy();
  unhide_mouse();
}

void duBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = du_do_day + du_do_mon * 100L + du_do_year * 10000L;

  if (d1 != d2) {
    --du_do_day;
    if ((du_do_mon == 3) && ((du_do_year % 4) == 0) && (du_do_day == 0)) // leap year, March 0
    {
      du_do_mon = 2;
      du_do_day = 29;
    } else if (du_do_day == 0) {
      --du_do_mon;
      du_do_day = day_tab[0][du_do_mon];
    }
    if (du_do_mon == 0) {
      du_do_mon = 12;
      du_do_day = day_tab[0][du_do_mon];
      --du_do_year;
    }
    DailyUsageOverLay();
  }
}

void duNextBtnClick(void) {
  if ((du_do_day == today) &&
      (du_do_mon == this_month) &&
      (du_do_year == this_year)) {
    return;
  }
  ++du_do_day;
  if ((du_do_mon == 2) && ((du_do_year % 4) == 0) && (du_do_day == 29)) {
  } else if (du_do_day > day_tab[0][du_do_mon]) {
    ++du_do_mon;
    du_do_day = 1;
  }
  if (du_do_mon > 12) {
    du_do_mon = 1;
    ++du_do_year;
  }
  DailyUsageOverLay();
}

//==============================================================
//
//   Device Detail Section.
//
//==============================================================

int in_device_detail;
void DevDetToggleClick(void);
void DevDetExitClick(void);

PUSH_BUTTON_T DevDetButton[2] = { // BEGIN SCREEN OBJECT DEFINITION
    419, 545 - 99, 89, 33,        // position
    FALSE,                        // focus
    PUSH_BUTTON,                  // type
    TRUE,                         // visible
    BtnMouseDown,                 // OnMouseDown()
    BtnMouseUp,                   // OnMouseUp()
    BtnClick,                     // OnClick()
    BtnKeyPress,                  // OnKeyPress()
    PutButton,                    // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    FALSE,             // default button
    "$$$/kWh",         // text
    push,              // type
    up,                // state
    NULL,              // icon
    DevDetToggleClick, // function
    38,

    // BEGIN SCREEN OBJECT DEFINITION
    419, 545, 89, 33, // position
    FALSE,            // focus
    PUSH_BUTTON,      // type
    TRUE,             // visible
    BtnMouseDown,     // OnMouseDown()
    BtnMouseUp,       // OnMouseUp()
    BtnClick,         // OnClick()
    BtnKeyPress,      // OnKeyPress()
    PutButton,        // DisplayObject()
    // END SCREEN OBJECT DEFINITION
    TRUE,            //default button
    "Return",        //text
    push,            //type
    up,              //state
    icon[12],        //icon
    DevDetExitClick, //function
    39};

SCREEN_OBJECT_T* DevDetSo[13];
static void DeviceDetailDraw(void);
LWINDOW_T DevDetWindow[1] = {
    35, 0, 640, 445, //window position
    DevDetSo,        //screen object
    DeviceDetailDraw,
    13,                   //item count
    0,                    //cursor position 0-79 left to right
    0,                    //element that has current focus
    fcsBtn,               //focus type
    "Enrgy Usage Detail", //title
    DefaultOnKeyPress     //OnKeyPress();
};

static int dddevice_ndx;

static void DeviceDetailSwitchClick(void) {
  int i;
  USHORT device_id;

  device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }
  device_detail(LookupDev(device_id));
}

void device_detail(int device_ndx) {
  UCHAR work[80];
  int i;
  int j;

  dddevice_ndx = device_ndx;

  if (device_ndx == -1) {
    in_device_detail = device_ndx;
  } else {
    in_device_detail = device_ndx + 1;
  }

  for (i = 0; i < 2; i++) {
    DevDetSo[i] = &DevDetButton[i].so;
  }

  for (i = 2; i < 13; i++) {
    DevDetSo[i] = &device_btn[i - 2].so;
    device_btn[i - 2].so.visible = FALSE;
    //      device_btn[i-2].pbf = DeviceDetailSwitchClick;
    device_btn[i - 2].pbf = DDSC_addr;
  }
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  if (device_ndx == -1) {
    sprintf(work, "Energy Usage Detail for Whole House   - Current Meter Dial: %11.4f", (double)meter.current_dial / 10000.0);
  } else {
    sprintf(work, "Energy Usage Detail for %13.13s - Current Meter Dial: %11.4f", dev_list.devices[device_ndx].desc, (double)meter.current_dial / 10000.0);
  }
  DevDetWindow[0].title = work;
  glbWindow = DevDetWindow;
  PutWindow(glbWindow);
}

static void DeviceDetailDraw(void) {
  char buff[80];
  int i;
  int row;
  int col;
  double total_cost;
  double cur_total;
  double mtd_total;
  double lmon_total;
  double ytd_total;
  ULONG *curr, *mtd, *lmon, *ytd;

  cur_total = mtd_total = lmon_total = ytd_total = 0;

  switch (dddevice_ndx) {
  case -1:
    if (detail_mode) {
      curr = meter.current_day$;
      mtd = meter.mtd$;
      lmon = meter.last_month$;
      ytd = meter.ytd$;
    } else {
      curr = meter.current_day;
      mtd = meter.mtd;
      lmon = meter.last_month;
      ytd = meter.ytd;
    }
    break;
  default:
    if (detail_mode) {
      curr = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].current_day$;
      mtd = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].mtd$;
      lmon = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].last_month$;
      ytd = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].ytd$;
    } else {
      curr = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].current_day;
      mtd = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].mtd;
      lmon = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].last_month;
      ytd = device_history[dev_list.devices[dddevice_ndx].history_bucket - 1].ytd;
    }
    break;
  }
  for (i = 0; i < 24; ++i) {
    if (detail_mode != 0) {
      cur_total += round(((double)(*(curr + i)) / 100000.0), 2);
      mtd_total += round(((double)(*(mtd + i)) / 100000.0), 2);
      lmon_total += round(((double)(*(lmon + i)) / 100000.0), 2);
      ytd_total += round(((double)(*(ytd + i)) / 100000.0), 2);
    } else {
      cur_total += round(((double)(*(curr + i)) / 10000.0), 1);
      mtd_total += round(((double)(*(mtd + i)) / 10000.0), 1);
      lmon_total += round(((double)(*(lmon + i)) / 10000.0), 1);
      ytd_total += round(((double)(*(ytd + i)) / 10000.0), 1);
    }
  }

  position(7, 2);
  text("  HOUR  TODAY THIS_MO LAST_MO  THIS YR", BG_WHT + FG_BLU);
  position(7, 42);
  text("  HOUR  TODAY THIS_MO LAST_MO  THIS YR", BG_WHT + FG_BLU);
  position(22, 33);

  if (detail_mode == 0) {
    ptext("By kWh", 390, 464, BG_WHT + FG_RED);
  } else {
    ptext("By $$$", 390, 464, BG_WHT + FG_RED);
  }

  for (i = 0; i < 24; ++i) {
    row = i % 12;
    col = i / 12;
    if (i == 0 || i == 12) {
      strcpy(buff, "12- 1");
    } else {
      if (i < 12) {
        sprintf(buff, "%2d-%2d", i, i + 1);
      } else {
        sprintf(buff, "%2d-%2d", i - 12, i - 12 + 1);
      }
    }
    if (i < 12) {
      strcat(buff, "am");
    } else {
      strcat(buff, "pm");
    }
    position(9 + row, col * 40 + 2);
    text(buff, BG_WHT + FG_BLU);
    position(9 + row, col * 40 + 10);
    total_cost = (double)(*(curr + i));
    if (detail_mode != 0) {
      total_cost /= 100000.0;
      sprintf(buff, "%5.2f", total_cost);
    } else {
      total_cost /= 10000.0;
      sprintf(buff, "%5.1f", total_cost);
    }
    text(buff, BG_WHT + FG_BLK);
    position(9 + row, col * 40 + 16);
    total_cost = (double)(*(mtd + i));
    if (detail_mode != 0) {
      total_cost /= 100000.0;
      sprintf(buff, "%7.2f", total_cost);
    } else {
      total_cost /= 10000.0;
      sprintf(buff, "%7.1f", total_cost);
    }
    text(buff, BG_WHT + FG_BLK);
    position(9 + row, col * 40 + 24);
    total_cost = (double)(*(lmon + i));
    if (detail_mode != 0) {
      total_cost /= 100000.0;
      sprintf(buff, "%7.2f", total_cost);
    } else {
      total_cost /= 10000.0;
      sprintf(buff, "%7.1f", total_cost);
    }
    text(buff, BG_WHT + FG_BLK);
    position(9 + row, col * 40 + 32);
    total_cost = (double)(*(ytd + i));
    if (detail_mode != 0) {
      total_cost /= 100000.0;
      sprintf(buff, "%8.2f", total_cost);
    } else {
      total_cost /= 10000.0;
      sprintf(buff, "%8.1f", total_cost);
    }
    text(buff, BG_WHT + FG_BLK);
  }

  position(22, 2);
  text("TOTALS:", BG_WHT + FG_BLK);
  position(22, 10);
  if (detail_mode != 0) {
    sprintf(buff, "%5.2f", cur_total);
  } else {
    sprintf(buff, "%5.1f", cur_total);
  }
  text(buff, BG_WHT + FG_BLK);
  position(22, 16);
  if (detail_mode != 0) {
    sprintf(buff, "%7.2f", mtd_total);
  } else {
    sprintf(buff, "%7.1f", mtd_total);
  }
  text(buff, BG_WHT + FG_BLK);
  position(22, 24);
  if (detail_mode != 0) {
    sprintf(buff, "%7.2f", lmon_total);
  } else {
    sprintf(buff, "%7.1f", lmon_total);
  }
  text(buff, BG_WHT + FG_BLK);
  position(22, 32);
  if (detail_mode != 0) {
    sprintf(buff, "%8.2f", ytd_total);
  } else {
    sprintf(buff, "%8.1f", ytd_total);
  }
  text(buff, BG_WHT + FG_BLK);
}

void update_device_detail(int device_ndx) {
  char buff[80];
  int i, row, col;
  double cur_total, mtd_total, lmon_total, ytd_total;
  double total_cost;
  ULONG *curr, *mtd, *lmon, *ytd;

  cur_total = mtd_total = lmon_total = ytd_total = 0;

  switch (device_ndx) {
  case -1:
    if (detail_mode) {
      curr = meter.current_day$;
      mtd = meter.mtd$;
      lmon = meter.last_month$;
      ytd = meter.ytd$;
    } else {
      curr = meter.current_day;
      mtd = meter.mtd;
      lmon = meter.last_month;
      ytd = meter.ytd;
    }
    break;
  default:
    if (detail_mode) {
      curr = device_history[dev_list.devices[device_ndx].history_bucket - 1].current_day$;
      mtd = device_history[dev_list.devices[device_ndx].history_bucket - 1].mtd$;
      lmon = device_history[dev_list.devices[device_ndx].history_bucket - 1].last_month$;
      ytd = device_history[dev_list.devices[device_ndx].history_bucket - 1].ytd$;
    } else {
      curr = device_history[dev_list.devices[device_ndx].history_bucket - 1].current_day;
      mtd = device_history[dev_list.devices[device_ndx].history_bucket - 1].mtd;
      lmon = device_history[dev_list.devices[device_ndx].history_bucket - 1].last_month;
      ytd = device_history[dev_list.devices[device_ndx].history_bucket - 1].ytd;
    }
    break;
  }

  for (i = 0; i < 24; ++i) {
    if (detail_mode != 0) {
      cur_total += round(((double)(*(curr + i)) / 100000.0), 2);
      mtd_total += round(((double)(*(mtd + i)) / 100000.0), 2);
      lmon_total += round(((double)(*(lmon + i)) / 100000.0), 2);
      ytd_total += round(((double)(*(ytd + i)) / 100000.0), 2);
    } else {
      cur_total += round(((double)(*(curr + i)) / 10000.0), 1);
      mtd_total += round(((double)(*(mtd + i)) / 10000.0), 1);
      lmon_total += round(((double)(*(lmon + i)) / 10000.0), 1);
      ytd_total += round(((double)(*(ytd + i)) / 10000.0), 1);
    }
  }

  i = interval.hour;
  row = i % 12;
  col = i / 12;
  if (i == 0 || i == 12) {
    strcpy(buff, "12- 1");
  } else {
    if (i < 12)
      sprintf(buff, "%2d-%2d", i, i + 1);
    else
      sprintf(buff, "%2d-%2d", i - 12, i - 12 + 1);
  }
  if (i < 12) {
    strcat(buff, "am");
  } else {
    strcat(buff, "pm");
  }
  position(9 + row, col * 40 + 2);
  text(buff, BG_WHT + FG_BLU);
  position(9 + row, col * 40 + 10);
  total_cost = (double)(*(curr + i));
  if (detail_mode != 0) {
    total_cost /= 100000.0;
    sprintf(buff, "%5.2f", total_cost);
  } else {
    total_cost /= 10000.0;
    sprintf(buff, "%5.1f", total_cost);
  }
  text(buff, BG_WHT + FG_BLK);
  position(9 + row, col * 40 + 16);
  total_cost = (double)(*(mtd + i));
  if (detail_mode != 0) {
    total_cost /= 100000.0;
    sprintf(buff, "%7.2f", total_cost);
  } else {
    total_cost /= 10000.0;
    sprintf(buff, "%7.1f", total_cost);
  }
  text(buff, BG_WHT + FG_BLK);
  position(9 + row, col * 40 + 24);
  total_cost = (double)(*(lmon + i));
  if (detail_mode != 0) {
    total_cost /= 100000.0;
    sprintf(buff, "%7.2f", total_cost);
  } else {
    total_cost /= 10000.0;
    sprintf(buff, "%7.1f", total_cost);
  }
  text(buff, BG_WHT + FG_BLK);
  position(9 + row, col * 40 + 32);
  total_cost = (double)(*(ytd + i));
  if (detail_mode != 0) {
    total_cost /= 100000.0;
    sprintf(buff, "%8.2f", total_cost);
  } else {
    total_cost /= 10000.0;
    sprintf(buff, "%8.1f", total_cost);
  }
  text(buff, BG_WHT + FG_BLK);

  position(22, 2);
  text("TOTALS:", BG_WHT + FG_BLK);
  position(22, 10);
  if (detail_mode != 0) {
    sprintf(buff, "%5.2f", cur_total);
  } else {
    sprintf(buff, "%5.1f", cur_total);
  }
  text(buff, BG_WHT + FG_BLK);
  position(22, 16);
  if (detail_mode != 0) {
    sprintf(buff, "%7.2f", mtd_total);
  } else {
    sprintf(buff, "%7.1f", mtd_total);
  }
  text(buff, BG_WHT + FG_BLK);
  position(22, 24);
  if (detail_mode != 0)
    sprintf(buff, "%7.2f", lmon_total);
  else
    sprintf(buff, "%7.1f", lmon_total);
  text(buff, BG_WHT + FG_BLK);
  position(22, 32);
  if (detail_mode != 0) {
    sprintf(buff, "%8.2f", ytd_total);
  } else {
    sprintf(buff, "%8.1f", ytd_total);
  }
  text(buff, BG_WHT + FG_BLK);
}

void DevDetToggleClick(void) {
  if (detail_mode) {
    detail_mode = FALSE;
  } else {
    detail_mode = TRUE;
  }
  device_detail(dddevice_ndx);
}

void DevDetExitClick(void) {
  in_device_detail = 0;
  if (detail_caller == COST) {
    daily_cost();
  } else {
    daily_usage();
  }
}

//===========================================================================
//
//  Utility usage horizontal graphs section.
//
//===========================================================================

// Currently using TYPE today, this month, last_month, and year-to-date.
extern CHECK_BOX_T view_percent;
void horizontal_graph(int graph_type) {
  int i;
  int j;
  int history_index;
  ULONG* hist_column;
  ULONG* hist$_column;
  double total_dollars;
  double total_kw;
  double dwork;
  double device_dollars;
  double device_kw;

  if (DEMO == TRUE) {
    in_bar_graph = FALSE;
    switch (graph_type) {
    case 0: // current day
    case 1:
      total_kw = GetTotalkW();
      total_dollars = GetTotalDollars();
      break;
    case 2: // this month
      total_kw = GetTotalkW() * 30;
      total_dollars = GetTotalDollars() * 30;
      break;
    case 3: // last month
      total_kw = GetTotalkW() * 31;
      total_dollars = GetTotalDollars() * 31;
      break;
    case 4: // ytd
      total_kw = GetTotalkW() * 365;
      total_dollars = GetTotalDollars() * 365;
      break;
    default:
      total_kw = GetTotalkW();
      total_dollars = GetTotalDollars();
      break;
    }
  } else {
    switch (graph_type) {
    case 0: // current day
      hist_column = &meter.current_day[0];
      hist$_column = &meter.current_day$[0];
      break;
    case 2: // this month
      hist_column = &meter.mtd[0];
      hist$_column = &meter.mtd$[0];
      break;
    case 3: // last month
      hist_column = &meter.last_month[0];
      hist$_column = &meter.last_month$[0];
      break;
    case 4: // ytd
      hist_column = &meter.ytd[0];
      hist$_column = &meter.ytd$[0];
      break;
    default:
      hist_column = &meter.current_day[0];
      hist$_column = &meter.current_day$[0];
      break;
    }
    if (graph_type == 1) // by date
    {
      total_kw = GetTotalkW();
      total_dollars = GetTotalDollars();
    } else {
      for (total_kw = 0, total_dollars = 0, i = 0; i < 24; ++i) {
        dwork = (double)(*(hist_column + i)) / 10000.0;
        total_kw += dwork;
        total_dollars += (double)(*(hist$_column + i)) / 100000.0;
      }
    }
  }

  area_clear(75, 398, 248, 637, FG_WHT);

  get_font_loc(font_array[BOLD]);
  if (view_percent.checked == TRUE) {
    ptext("kWh %", 70, 145, BG_WHT + FG_LGRN);
    ptext(" Cost %", 70, 186, BG_WHT + FG_YEL);
  } else {
    ptext("kWh", 70, 145, BG_WHT + FG_LGRN);
    ptext(" Cost $", 70, 186, BG_WHT + FG_YEL);
  }
  get_font_loc(font_array[LGHT]);

  if (view_percent.checked == TRUE) {
    horiz_percent_graph(0, total_kw, total_dollars, total_kw, total_dollars);
  } else {
    horiz_house_graph(0, total_kw, total_dollars, total_kw, total_dollars);
  }

  for (i = 0; i < mon_devices_used; ++i) {
    history_index = dev_list.devices[monitored_devices[i]].history_bucket - 1;
    if (DEMO == TRUE) {
      switch (graph_type) {
      case 0: // current day
      case 1:
        total_kw = GetTotalkW();
        total_dollars = GetTotalDollars();
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot);
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot);
        break;
      case 2: // this month
        total_kw = GetTotalkW() * 30;
        total_dollars = GetTotalDollars() * 30;
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot) * 30;
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot) * 30;
        break;
      case 3: // last month
        total_kw = GetTotalkW() * 31;
        total_dollars = GetTotalDollars() * 31;
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot) * 31;
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot) * 31;
        break;
      case 4: // ytd
        total_kw = GetTotalkW() * 365;
        total_dollars = GetTotalDollars() * 365;
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot) * 365;
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot) * 365;
        break;
      default:
        total_kw = GetTotalkW();
        total_dollars = GetTotalDollars();
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot);
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot);
        break;
      }
    } else {
      switch (graph_type) {
      case 0: // current day
        hist_column = &device_history[history_index].current_day[0];
        hist$_column = &device_history[history_index].current_day$[0];
        break;
      case 2: // this month
        hist_column = &device_history[history_index].mtd[0];
        hist$_column = &device_history[history_index].mtd$[0];
        break;
      case 3: // last month
        hist_column = &device_history[history_index].last_month[0];
        hist$_column = &device_history[history_index].last_month$[0];
        break;
      case 4: // ytd
        hist_column = &device_history[history_index].ytd[0];
        hist$_column = &device_history[history_index].ytd$[0];
        break;
      default:
        hist_column = &device_history[history_index].current_day[0];
        hist$_column = &device_history[history_index].current_day$[0];
        break;
      }
      if (graph_type == 1) // by date
      {
        total_kw = GetTotalkW();
        total_dollars = GetTotalDollars();
        device_dollars = GetDeviceDollars(dev_list.devices[monitored_devices[i]].device_slot);
        device_kw = GetDevicekW(dev_list.devices[monitored_devices[i]].device_slot);
      } else {
        for (device_kw = 0, device_dollars = 0, j = 0; j < 24; ++j) {
          dwork = ((double)(*(hist_column + j)) / 10000.0);
          device_kw += dwork;
          dwork = ((double)(*(hist$_column + j)) / 100000.0);
          device_dollars += dwork;
        }
      }
    }
    if (view_percent.checked == TRUE) {
      horiz_percent_graph(28 * (i + 1), device_kw, device_dollars,
                          total_kw, total_dollars);
    } else {
      horiz_house_graph(28 * (i + 1), device_kw, device_dollars,
                        total_kw, total_dollars);
    }
  }
}

//////////////////////////////////////////
// Draw whole house/Device bar graph pair.
//////////////////////////////////////////
void horiz_house_graph(int row, double total_kw, double total_dollars,
                       double full_kw, double full_dollars) {
  char work[80];
  int tmp;
  double dwork;

  if (full_kw == 0) {
    full_kw = 1.00;
  }

  if (full_dollars == 0) {
    full_dollars = 1.00;
  }

  if (total_dollars > full_dollars) {
    dwork = 1;
  } else {
    dwork = total_dollars / full_dollars;
  }

  tmp = (int)(dwork * 363) + 2;
  pbutton(98 + row, 265, 7, tmp, PB_DN, FG_YEL);

  if (total_kw > full_kw) {
    dwork = 1;
  } else {
    dwork = total_kw / full_kw;
  }

  tmp = (int)(dwork * 363) + 2;
  pbutton(89 + row, 265, 7, tmp, PB_DN, FG_LGRN);

  if (full_kw < 10.0) {
    sprintf(work, "%6.2f", total_kw);
  } else if (full_kw < 100.0) {
    sprintf(work, "%6.1f", total_kw);
  } else {
    sprintf(work, "%6.0f", total_kw);
  }
  ptext(work, 92 + row, 120, BG_WHT + FG_BLK);
  sprintf(work, "%7.2f", total_dollars);
  ptext(work, 92 + row, 188, BG_WHT + FG_BLK);
}

static void horiz_percent_graph(int row, double total_kw, double total_dollars,
                                double full_kw, double full_dollars) {
  char work[80];
  int tmp;
  double dwork;

  if (full_kw == 0) {
    full_kw = 1.00;
  }

  if (full_dollars == 0) {
    full_dollars = 1.00;
  }

  if (total_dollars > full_dollars) {
    dwork = 1;
  } else {
    dwork = total_dollars / full_dollars;
  }

  tmp = (int)(dwork * 363) + 2;
  pbutton(98 + row, 265, 7, tmp, PB_DN, FG_YEL);

  if (total_kw > full_kw) {
    dwork = 1;
  } else {
    dwork = total_kw / full_kw;
  }

  tmp = (int)(dwork * 363) + 2;
  pbutton(89 + row, 265, 7, tmp, PB_DN, FG_LGRN);

  if (full_kw < 10.0) {
    sprintf(work, "%6.2f", (total_kw / full_kw) * 100.0);
  } else if (full_kw < 100.0) {
    sprintf(work, "%6.1f", (total_kw / full_kw) * 100.0);
  } else {
    sprintf(work, "%6.0f", (total_kw / full_kw) * 100.0);
  }

  ptext(work, 92 + row, 120, BG_WHT + FG_BLK);

  sprintf(work, "%7.2f", (total_dollars / full_dollars) * 100.0);
  ptext(work, 92 + row, 188, BG_WHT + FG_BLK);
}

//=======================================================
//
//   Daily Cost Section.
//
//=======================================================

static USHORT dc_device_id;

static void dcBackBtnClick(void);
static void dcNextBtnClick(void);

//Daily Cost Screen
PUSH_BUTTON_T dcBtnList[5] = {419, 149, 89, 33, // position
                              FALSE,            // focus
                              PUSH_BUTTON,      // type
                              TRUE,             // visible
                              BtnMouseDown,     // OnMouseDown()
                              BtnMouseUp,       // OnMouseUp()
                              BtnClick,         // OnClick()
                              BtnKeyPress,      // OnKeyPress()
                              PutButton,        // DisplayObject()
                              FALSE,            // default button
                              "\x9E Day \x9E",  // text
                              push,             // type
                              up,               // state
                              NULL,             // icon
                              dcBackBtnClick,   // function
                              40,

                              419, 248, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "\x9F Day \x9F",
                              push,
                              up,
                              NULL,
                              dcNextBtnClick,
                              41,

                              419, 347, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "View|Detail", //tabular toggle switch
                              push,
                              up,
                              NULL,
                              COSTDetailClick, //void device_detail(int device_id)   static void ViewDetail(void);
                              42,

                              419, 446, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "kWh Graph",
                              push,
                              up,
                              NULL,
                              daily_usage,
                              43,

                              419, 545, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              TRUE,
                              NULL,
                              push,
                              up,
                              icon[12],
                              ReturnClick,
                              44};

SCREEN_OBJECT_T* dcSO[17];
static void DailyCostDraw(void);
LWINDOW_T dcWindow[1] = {
    35, 0, 640, 445, //window position
    dcSO,            //screen object
    DailyCostDraw,
    17,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    "",               //title
    DefaultOnKeyPress //OnKeyPress();
};

#undef DOTS_HIGH
#undef TOP
#undef DLC
#undef DLP

#define DOTS_HIGH 220
#define TOP 300 - DOTS_HIGH

#define DLC 2  // Column of Left hand labels
#define DLP 80 // Pixel position of Left Side

void daily_cost(void) {
  int i;
  int j;
  UCHAR* rtnbtn[2] = {"Main|Menu", "Util.|Hist."};

  if (Sender == IntroScreen) {
    dcBtnList[4].text = rtnbtn[0];
  } else if (Sender == UtilityUsageMenu) {
    dcBtnList[4].text = rtnbtn[1];
  }

  fix_scale.checked = FALSE;
  which_graph = DAILY_COST;
  detail_caller = COST;

  glbWindow = dcWindow;

  for (i = 0; i < 5; i++) {
    dcSO[i] = &dcBtnList[i].so;
  }
  dcSO[16] = &fix_scale.so;
  for (i = 5; i < 16; i++) {
    dcSO[i] = &device_btn[i - 5].so;
    device_btn[i - 5].so.visible = FALSE;
    //      device_btn[i-5].pbf = DailyCostOverLay;
    device_btn[i - 5].pbf = DCO_addr;
  }
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }
  PutWindow(glbWindow);
}

static void DailyCostDraw(void) {
  DailyCostOverLay();

  //frame for key
  frame_3d(326, 280, 85, 350, FT_PRESSED);
  pbutton(336, 287, 14, 14, PB_DN, FG_RED);
  pbutton(352, 287, 14, 14, PB_DN, FG_CYN);
  pbutton(368, 287, 14, 14, PB_DN, FG_BLU);
  pbutton(384, 287, 14, 14, PB_DN, FG_YEL);
  ptext("Standard", 337, 309, BG_WHT + FG_BLK);
  ptext("Time of Day", 353, 309, BG_WHT + FG_BLK);
  ptext("Variable", 369, 309, BG_WHT + FG_BLK);
  ptext("Cents per kWh", 385, 309, BG_WHT + FG_BLK);
}

double dcmax;
double dcmax1;

static void DailyCostOverLay(void) {
  char found_key[30];
  char id_string[30];
  char device_name[14];

  UCHAR do_mon;
  UCHAR do_day;
  UCHAR do_year;
  UCHAR load_ctrl[24];
  UCHAR rate_type[24];
  UCHAR buff[80];

  int i;
  int j;
  int col;
  long row;
  int last_row;
  int last_col;
  int color;
  int key_len;

  long ltime;

  double* interval_usage;
  double peak;
  double peak1;
  double total;
  double work_sum;
  double hour_rate;
  double rs_total, rst_total, rsv_total;

  DAYS_RATES days_rates;

  struct tm* tod;

  USHORT device_id;

  Busy();

  device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      dc_device_id = device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }

  if ((du_do_mon == 0) && (du_do_day == 0) && (du_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
    du_do_mon = do_mon;
    du_do_day = do_day;
    du_do_year = do_year;
  } else {
    do_mon = du_do_mon;
    do_day = du_do_day;
    do_year = du_do_year;
  }

  interval_usage = (double*)f_buffer;

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  if (do_mon == tod->tm_mon + 1 && do_day == tod->tm_mday && do_year == tod->tm_year) {
    days_rates = todays_rates;
  } else {
    switch (read_rates(do_mon, do_day, do_year, &days_rates)) {
    case 1:
    case 2:
    case 5:
      sprintf(buff,
              "The rate information for|%02d/%02d/%4d is not available.",
              do_mon, do_day, do_year + 1900);
      msgbox(buff, "Information", MB_OK);
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      NotBusy();
      unhide_mouse();
      return;
    case -1:
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      NotBusy();
      unhide_mouse();
      return;
    }
  }

  if (device_id == 0x0FFFF) {
    strcpy(device_name, "Whole House  ");
  } else {
    i = LookupDev(device_id);
    if (i != -1) {
      sprintf(device_name, "%.13s", dev_list.devices[i].desc);
    } else {
      strcpy(device_name, "             ");
    }
  }

  //===========================================================
  // Put title in title bar
  //===========================================================
  i = day_of_week(do_mon, do_day, do_year);
  sprintf(buff, "Energy Cost Graph for:  %s - %s, %d/%d/%4d     ", device_name, wday[i], do_mon, do_day, 1900 + do_year);
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  for (i = 0; i < 96 * 2; ++i) {
    *(interval_usage + i) = 0.0;
  }
  for (i = 0; i < 24; ++i) {
    load_ctrl[i] = 0;

    switch (cust_info.rate_type) {
    case 0:
      rate_type[i] = 0;
      break;
    case 1:
      rate_type[i] = 2;
      break;
    case 2:
      rate_type[i] = 4;
      break;
    }
  }

  total = 0.0;
  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, do_day);
  strcpy(id_string, found_key);
  if (next_history(found_key) == 0) {
    while (1) {
      if (memcmp(found_key, id_string, key_len) != 0) {
        break;
      }
      i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;
      *(interval_usage + i) += hist_tran.ht_kwh;
      *(interval_usage + i + 96) += hist_tran.ht_kwh_cost;
      //rate_type[hist_tran.ht_hr] = hist_tran.ht_rate_type & 0x06;      // get customer type
      if ((hist_tran.ht_rate_type & 0x06) != 0) {                   // NOTE:  Outages are recorded as rate type 0, so skip
        rate_type[hist_tran.ht_hr] = hist_tran.ht_rate_type & 0x06; // get customer type
      }

      if (hist_tran.ht_kwh > 0 && ((hist_tran.ht_flags >> 2) & 7) > 0) {
        if (device_id == 0x0FFFF) {
          load_ctrl[hist_tran.ht_hr] = 1;
        } else {
          if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7)) {
            load_ctrl[hist_tran.ht_hr] = 1;
          }
        }
      }

      if (!(hist_tran.ht_flags & 64)) // not a credit
      {
        total += hist_tran.ht_kwh;
      }

      if (next_history(found_key)) {
        break;
      }
    }
  }

  peak = peak1 = 0.0;
  rs_total = rst_total = rsv_total = 0.0;
  for (i = 0; i < 24; ++i) {
    // Sum the hour
    for (j = 0, work_sum = 0.0; j < 4; ++j) {
      work_sum += *(interval_usage + i * 4 + j);
    }

    // calculate dollars for that hour
    *(interval_usage + i * 3) =
        work_sum * ((double)days_rates.rs_rate) / 100000.0;
    *(interval_usage + i * 3 + 2) =
        work_sum * ((double)days_rates.rsv_rate[i]) / 100000.0;
    *(interval_usage + i * 3 + 1) =
        work_sum * ((double)days_rates.rtod_rate[i]) / 100000.0;

    rs_total += *(interval_usage + i * 3);
    rst_total += *(interval_usage + i * 3 + 1);
    rsv_total += *(interval_usage + i * 3 + 2);
  }
  for (i = 0; i < 72; ++i) {
    if ((*(interval_usage + i)) > peak) {
      peak = *(interval_usage + i);
    }
  }
  for (i = 0; i < 24; ++i) {
    switch (rate_type[i]) {
    case 0:
      hour_rate = (double)days_rates.rs_rate / 100000;
      break;
    case 2:
    case 3:
      hour_rate = (double)days_rates.rtod_rate[i] / 100000;
      break;
    case 4:
    case 5:
      hour_rate = (double)days_rates.rsv_rate[i] / 100000;
      break;
    }

    if (hour_rate > peak1) {
      peak1 = hour_rate;
    }
  }

  if (fix_scale.checked == FALSE) {
    //dcmax = FindMax(peak*100.0)/100.0;
    //dcmax1 = FindMax(peak1*100.0)/100.0;
    dcmax = FindMax(peak);
    dcmax1 = FindMax(peak1);
  }

  hide_mouse();
  //===========================================================
  // BORDERS
  //===========================================================
  pbutton(78, 79, 225, 483, PB_DN, FG_WHT);

  ptext("Cost", 60, 45, BG_WHT + FG_BLK);
  if (dcmax < 10.00) {
    sprintf(buff, "$%4.2f", dcmax);
    ptext(buff, 5 * 14 + 5, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", dcmax * .8);
    ptext(buff, 5 * 14 + 5 + 44, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", dcmax * .6);
    ptext(buff, 5 * 14 + 5 + 88, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", dcmax * .4);
    ptext(buff, 5 * 14 + 5 + 132, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", dcmax * .2);
    ptext(buff, 5 * 14 + 5 + 176, 36, FG_BLK + BG_WHT);
    ptext("$0.00", 5 * 14 + 5 + 220, 36, FG_BLK + BG_WHT);
  } else {
    sprintf(buff, "$%4.0f", dcmax);
    ptext(buff, 5 * 14 + 5, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", dcmax * .8);
    ptext(buff, 5 * 14 + 5 + 44, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", dcmax * .6);
    ptext(buff, 5 * 14 + 5 + 88, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", dcmax * .4);
    ptext(buff, 5 * 14 + 5 + 132, 36, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", dcmax * .2);
    ptext(buff, 5 * 14 + 5 + 176, 36, FG_BLK + BG_WHT);
    ptext("$   0", 5 * 14 + 5 + 220, 36, FG_BLK + BG_WHT);
  }

  ptext("\xA0/kWh", 60, 560, BG_WHT + FG_BLK);
  sprintf(buff, "%2.0f\xA0", dcmax1 * 100);
  ptext(buff, 5 * 14 + 5, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dcmax1 * .8 * 100);
  ptext(buff, 5 * 14 + 5 + 44, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dcmax1 * .6 * 100);
  ptext(buff, 5 * 14 + 5 + 88, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dcmax1 * .4 * 100);
  ptext(buff, 5 * 14 + 5 + 132, 570, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", dcmax1 * .2 * 100);
  ptext(buff, 5 * 14 + 5 + 176, 570, FG_BLK + BG_WHT);
  ptext(" 0\xA0", 5 * 14 + 5 + 220, 570, FG_BLK + BG_WHT);

  if (load_ctrl[0] == 0) {
    ptext("M", 22 * 14, DLP - 1, FG_BLK + BG_WHT);
  } else {
    ptext("M", 22 * 14, DLP - 1, FG_HWHT + BG_WHT);
  }
  for (i = 1; i < 12; ++i) {
    if (load_ctrl[i] == 0) {
      color = FG_BLK + BG_WHT;
    } else {
      color = FG_HWHT + BG_WHT;
    }

    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 22 * 14, DLP + i * 20 - 4, color);
    } else {
      ptext(buff, 22 * 14, DLP + i * 20 - 8, color);
    }
  }
  if (load_ctrl[12] == 0) {
    ptext("N", 22 * 14, DLP + 12 * 20 - 4, FG_BLK + BG_WHT);
  } else {
    ptext("N", 22 * 14, DLP + 12 * 20 - 4, FG_HWHT + BG_WHT);
  }
  for (i = 1; i < 12; ++i) {
    if (load_ctrl[i + 12] == 0) {
      color = FG_BLK + BG_WHT;
    } else {
      color = FG_HWHT + BG_WHT;
    }

    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 22 * 14, DLP + (i + 12) * 20 - 4, color);
    } else {
      ptext(buff, 22 * 14, DLP + (i + 12) * 20 - 8, color);
    }
  }
  if (load_ctrl[23] == 0) {
    ptext("M", 22 * 14, DLP + 24 * 20 - 4, FG_BLK + BG_WHT);
  } else {
    ptext("M", 22 * 14, DLP + 24 * 20 - 4, FG_HWHT + BG_WHT);
  }

  //==============================
  // Draw grid
  //==============================
  for (i = 1; i < 24; ++i) {
    vline(DLP + i * 20, TOP, 302, FG_GRY);
  }
  for (i = 0; i < 9; ++i) {
    hline(TOP + 22 + 22 * i, 81, 560, FG_GRY);
  }

  //==============================
  // Draw the graph
  //==============================
  color = FG_YEL;
  for (i = 0; i < 24; ++i) {
    col = DLP + (i * 20);
    // flat rate
    row = 301 - (int)(((*(interval_usage + i * 3)) / dcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 1, (int)row, 301, FG_RED);
    vline(col + 2, (int)row, 301, FG_RED);
    vline(col + 3, (int)row, 301, FG_RED);
    vline(col + 4, (int)row, 301, FG_RED);
    vline(col + 5, (int)row, 301, FG_RED);
    vline(col + 6, (int)row, 301, FG_RED);

    // TOD rate
    row = 301 - (int)(((*(interval_usage + i * 3 + 1)) / dcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 7, (int)row, 301, FG_CYN);
    vline(col + 8, (int)row, 301, FG_CYN);
    vline(col + 9, (int)row, 301, FG_CYN);
    vline(col + 10, (int)row, 301, FG_CYN);
    vline(col + 11, (int)row, 301, FG_CYN);
    vline(col + 12, (int)row, 301, FG_CYN);

    // VAR rate
    row = 301 - (int)(((*(interval_usage + i * 3 + 2)) / dcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 13, (int)row, 301, FG_BLU);
    vline(col + 14, (int)row, 301, FG_BLU);
    vline(col + 15, (int)row, 301, FG_BLU);
    vline(col + 16, (int)row, 301, FG_BLU);
    vline(col + 17, (int)row, 301, FG_BLU);
    vline(col + 18, (int)row, 301, FG_BLU);
  }

  //==============================
  // Draw yellow $/hr line
  //==============================
  last_row = -1;
  color = FG_YEL;
  for (i = 0; i < 24; ++i) {
    switch (rate_type[i]) {
    case 0:
      hour_rate = (double)days_rates.rs_rate / 100000;
      break;
    case 2:
    case 3:
      hour_rate = (double)days_rates.rtod_rate[i] / 100000;
      break;
    case 4:
    case 5:
      hour_rate = (double)days_rates.rsv_rate[i] / 100000;
      break;
    }

    col = DLP + 1 + (i * 20);
    //row= 300 - (int)((*(interval_usage+i+96)/max1)*DOTS_HIGH);
    row = 300 - (int)((hour_rate / dcmax1) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    line(col, (int)row, col + 19, (int)row, color);
    if (last_row != -1) {
      line(col, last_row, col, (int)row, color);
    }
    last_col = col;
    last_row = (int)row;
  }
  close_history();
  time(&ltime);
  tod = localtime(&ltime);

  //=========================================
  // Cost using various rates
  //=========================================
  area_clear(337, 383, 420, 625, FG_WHT);
  sprintf(buff, "$%-4.2f", rs_total);
  ptext(buff, 337, 420, BG_WHT + FG_BLK);

  sprintf(buff, "$%-4.2f", rst_total);
  ptext(buff, 353, 420, BG_WHT + FG_BLK);

  sprintf(buff, "$%-4.2f", rsv_total);
  ptext(buff, 369, 420, BG_WHT + FG_BLK);
  NotBusy();
  unhide_mouse();
}

void dcBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = du_do_day + du_do_mon * 100L + du_do_year * 10000L;

  if (d1 != d2) {
    --du_do_day;
    if ((du_do_mon == 3) && ((du_do_year % 4) == 0) && (du_do_day == 0)) // leap year, March 0
    {
      du_do_mon = 2;
      du_do_day = 29;
    } else if (du_do_day == 0) {
      --du_do_mon;
      du_do_day = day_tab[0][du_do_mon];
    }
    if (du_do_mon == 0) {
      du_do_mon = 12;
      du_do_day = day_tab[0][du_do_mon];
      --du_do_year;
    }
    DailyCostOverLay();
  }
}

void dcNextBtnClick(void) {
  if ((du_do_day == today) &&
      (du_do_mon == this_month) &&
      (du_do_year == this_year)) {
    return;
  }

  ++du_do_day;
  if ((du_do_mon == 2) && ((du_do_year % 4) == 0) && (du_do_day == 29)) {
  } else if (du_do_day > day_tab[0][du_do_mon]) {
    ++du_do_mon;
    du_do_day = 1;
  }

  if (du_do_mon > 12) {
    du_do_mon = 1;
    ++du_do_year;
  }
  DailyCostOverLay();
}

//
//
//
//   Monthly Cost Section.
//
//

static USHORT mc_device_id;
int mc_do_mon;
int mc_do_day;
int mc_do_year;

static void mcExitBtnClick(void);
static void mcBackBtnClick(void);
static void mcNextBtnClick(void);

#undef DOTS_HIGH
#undef TOP
#undef DLC
#undef DLP

#define DOTS_HIGH 210
#define TOP 81

#define DLC 2  // Column of Left hand labels
#define DLP 56 // Pixel position of Left Side

PUSH_BUTTON_T mcBtnList[4] = {419, 248, 89, 33,  // position
                              FALSE,             // focus
                              PUSH_BUTTON,       // type
                              TRUE,              // visible
                              BtnMouseDown,      // OnMouseDown()
                              BtnMouseUp,        // OnMouseUp()
                              BtnClick,          // OnClick()
                              BtnKeyPress,       // OnKeyPress()
                              PutButton,         // DisplayObject()
                              FALSE,             // default button
                              "\x9E Month \x9E", // text
                              push,              // type
                              up,                // state
                              NULL,              // icon
                              mcBackBtnClick,    // function
                              45,

                              419, 347, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "\x9F Month \x9F",
                              push,
                              up,
                              NULL,
                              mcNextBtnClick,
                              46,

                              419, 446, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "Usage|Graph",
                              push,
                              up,
                              NULL,
                              MonthlyUsage,
                              47,

                              419, 545, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              TRUE,
                              "Util.|Hist.",
                              push,
                              up,
                              icon[12],
                              mcExitBtnClick,
                              48};

SCREEN_OBJECT_T* mcSO[16];
static void MonthlyCostDraw(void);
LWINDOW_T mcWindow[1] = {
    35, 0, 640, 445, //window position
    mcSO,            //screen object
    MonthlyCostDraw,
    16,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    "",               //title
    DefaultOnKeyPress //OnKeyPress();
};

void MonthlyCost(void) {
  int i;
  int j;

  glbWindow = mcWindow;
  which_graph = MONTHLY_COST;
  for (i = 0; i < 4; i++) {
    mcSO[i] = &mcBtnList[i].so;
  }

  for (i = 4; i < 15; i++) {
    mcSO[i] = &device_btn[i - 4].so;
    device_btn[i - 4].so.visible = FALSE;
    //      device_btn[i-4].pbf = MonthlyCostOverLay;
    device_btn[i - 4].pbf = MCO_addr;
  }
  mcSO[15] = &fix_scale.so;
  fix_scale.checked = FALSE;
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  PutWindow(glbWindow);
}

static void MonthlyCostDraw(void) {
  //frame for key
  frame_3d(326, 280, 85, 350, FT_PRESSED);
  pbutton(336, 287, 14, 14, PB_DN, FG_RED);
  pbutton(352, 287, 14, 14, PB_DN, FG_CYN);
  pbutton(368, 287, 14, 14, PB_DN, FG_BLU);
  ptext("Standard", 337, 309, BG_WHT + FG_BLK);
  ptext("Time of Day", 353, 309, BG_WHT + FG_BLK);
  ptext("Variable", 369, 309, BG_WHT + FG_BLK);
  MonthlyCostOverLay();
}

double mcmax;
void MonthlyCostOverLay(void) {
  char found_key[30];
  char id_string[30];
  char buff[80];
  char device_name[14];

  UCHAR do_mon;
  UCHAR do_day;
  UCHAR do_year;

  int i;
  int col;
  long row;
  int color;
  int key_len;
  int leap;
  int dow;
  int load_ctrl[31];

  long ltime;

  double* interval_usage;
  double peak;
  double total;
  double rs_total;
  double rst_total;
  double rsv_total;
  double rs_avg;
  double rst_avg;
  double rsv_avg;
  double rs_est_total;
  double rst_est_total;
  double rsv_est_total;
  double work;

  struct tm* tod;

  DAYS_RATES days_rates;

  USHORT device_id;

  Busy();

  mc_device_id = device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      mc_device_id = device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }

  if ((mc_do_mon == 0) && (mc_do_day == 0) && (mc_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
  } else {
    do_mon = mc_do_mon;
    do_day = mc_do_day;
    do_year = mc_do_year;
  }

  mc_do_mon = do_mon;
  mc_do_day = do_day;
  mc_do_year = do_year;

  interval_usage = (double*)f_buffer;

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  if (device_id == 0x0FFFF) {
    strcpy(device_name, "Whole House  ");
  } else {
    i = LookupDev(device_id);
    if (i != -1) {
      sprintf(device_name, "%.13s", dev_list.devices[i].desc);
    } else {
      strcpy(device_name, "             ");
    }
  }

  sprintf(buff, "Monthly Cost for %s - %s, %4d   ", device_name, mname[do_mon], 1900 + do_year);
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  for (i = 0; i < 31 * 3; ++i) {
    *(interval_usage + i) = 0.0;
  }
  for (i = 0; i < 31; ++i) {
    load_ctrl[i] = 0;
  }

  if ((do_year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }

  dow = day_of_week(do_mon, 1, do_year);
  rs_total = rst_total = rsv_total = 0.0;
  total = 0.0;
  key_len = 11;

  pbutton(145, 200, 50, 240, PB_UP, FG_WHT);
  ptext("Calculating Costs...", 156, 240, BG_WHT + FG_BLU);

  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    if (do_mon == tod->tm_mon + 1 && tod->tm_mday == i + 1 && do_year == tod->tm_year) {
      days_rates = todays_rates;
    } else {
      switch (read_rates(do_mon, (UCHAR)(i + 1), do_year, &days_rates)) {
      case 1:
      case 2:
      case 5:
        sprintf(buff,
                "The rate information for|%02d/%02d/%4d is not available.",
                do_mon, do_day, do_year + 1900);
        msgbox(buff, "Information", MB_OK);
        close_history();
        time(&ltime);
        tod = localtime(&ltime);
        NotBusy();
        unhide_mouse();
        return;
      case -1:
        close_history();
        time(&ltime);
        tod = localtime(&ltime);
        NotBusy();
        unhide_mouse();
        return;
      }
    }

    sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, i + 1);
    strcpy(id_string, found_key);
    if (next_history(found_key) == 0) {
      while (1) {
        if (memcmp(found_key, id_string, key_len) != 0) {
          break;
        }

        work = hist_tran.ht_kwh;

        // calculate dollars for that hour
        *(interval_usage + i * 3) +=
            work * ((double)days_rates.rs_rate) / 100000.0;
        *(interval_usage + i * 3 + 2) +=
            work * ((double)days_rates.rsv_rate[hist_tran.ht_hr]) / 100000.0;
        *(interval_usage + i * 3 + 1) +=
            work * ((double)days_rates.rtod_rate[hist_tran.ht_hr]) / 100000.0;

        if (hist_tran.ht_kwh > 0 && ((hist_tran.ht_flags >> 2) & 7) > 0) {
          if (device_id == 0x0FFFF) {
            load_ctrl[hist_tran.ht_da - 1] = 1;
          } else {
            if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7)) {
              load_ctrl[hist_tran.ht_da - 1] = 1;
            }
          }
        }

        if (!(hist_tran.ht_flags & 64)) // not a credit
        {
          total += hist_tran.ht_kwh;
        }

        if (next_history(found_key)) {
          break;
        }
      }
      rs_total += *(interval_usage + i * 3);
      rst_total += *(interval_usage + i * 3 + 1);
      rsv_total += *(interval_usage + i * 3 + 2);
    }
  }

  peak = 0.0;
  for (i = 0; i < 31 * 3; ++i) {
    if (*(interval_usage + i) > peak) {
      peak = *(interval_usage + i);
    }
  }

  if (fix_scale.checked == FALSE) {
    //mcmax = FindMax(peak*100.0) / 100.0;
    mcmax = FindMax(peak);
  }

  hide_mouse();
  //=====================================
  // border
  //=====================================
  pbutton(80, 55, 212, 562, PB_DN, FG_WHT);

  if (mcmax < 10.00) {
    sprintf(buff, "$%4.2f", mcmax);
    ptext(buff, 5 * 14 + 5, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", mcmax * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", mcmax * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", mcmax * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.2f", mcmax * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLC * 7, FG_BLK + BG_WHT);
    ptext("$0.00", 5 * 14 + 5 + 210, DLC * 7, FG_BLK + BG_WHT);
  } else {
    sprintf(buff, "$%4.0f", mcmax);
    ptext(buff, 5 * 14 + 5, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", mcmax * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", mcmax * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", mcmax * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLC * 7, FG_BLK + BG_WHT);
    sprintf(buff, "$%4.0f", mcmax * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLC * 7, FG_BLK + BG_WHT);
    ptext("$   0", 5 * 14 + 5 + 210, DLC * 7, FG_BLK + BG_WHT);
  }

  area_clear(294, 320, 54, 618, BG_WHT + FG_WHT);
  for (i = 1; i <= day_tab[leap][do_mon]; ++i) {
    if (load_ctrl[i - 1] == 0) {
      color = FG_BLK + BG_WHT;
    } else {
      color = FG_HWHT + BG_WHT;
    }

    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 21 * 14, DLP + i * 18 - 10, color);
    } else {
      ptext(buff, 21 * 14, DLP + i * 18 - 16, color);
    }

    if (dow == 0 || dow == 6) {
      ptext(day_abr[dow], 22 * 14, DLP + i * 18 - 16, FG_BLK + BG_WHT);
    } else {
      ptext(day_abr[dow], 22 * 14, DLP + i * 18 - 10, FG_BLK + BG_WHT);
    }
    dow = ((dow + 1) % 7);
  }

  // GRID
  for (i = 1; i < 31; ++i) {
    vline(DLP + i * 18, TOP + 1, 290, FG_GRY);
  }
  for (i = 0; i < 9; ++i) {
    hline(TOP + 21 + 21 * i, DLP + 1, DLP + 31 * 18 + 1, FG_GRY);
  }

  /* RS cost - Vertical Bars */
  color = FG_RED;
  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = DLP + (i * 18);
    row = 290 - (int)(((*(interval_usage + i * 3)) / mcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 1, (int)row, 290, color);
    vline(col + 2, (int)row, 290, color);
    vline(col + 3, (int)row, 290, color);
    vline(col + 4, (int)row, 290, color);
    vline(col + 5, (int)row, 290, color);
  }

  /* RST cost - Vertical Bars */
  color = FG_CYN;
  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = DLP + (i * 18) + 5;
    row = 290 - (int)(((*(interval_usage + i * 3 + 1)) / mcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 1, (int)row, 290, color);
    vline(col + 2, (int)row, 290, color);
    vline(col + 3, (int)row, 290, color);
    vline(col + 4, (int)row, 290, color);
    vline(col + 5, (int)row, 290, color);
  }

  /* RSV cost - Vertical Bars */
  color = FG_BLU;
  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = DLP + (i * 18) + 10;
    row = 290 - (int)(((*(interval_usage + i * 3 + 2)) / mcmax) * DOTS_HIGH);
    if (row < TOP + 1) {
      row = TOP + 1;
    }
    vline(col + 1, (int)row, 290, color);
    vline(col + 2, (int)row, 290, color);
    vline(col + 3, (int)row, 290, color);
    vline(col + 4, (int)row, 290, color);
    vline(col + 5, (int)row, 290, color);
  }

  if ((do_mon == this_month) && (do_year == this_year)) {
    rs_avg = rs_total / do_day;
    rst_avg = rst_total / do_day;
    rsv_avg = rsv_total / do_day;
    rs_est_total = rs_avg * day_tab[leap][do_mon];
    rst_est_total = rst_avg * day_tab[leap][do_mon];
    rsv_est_total = rsv_avg * day_tab[leap][do_mon];
  } else {
    rs_avg = rs_total / day_tab[leap][do_mon];
    rst_avg = rst_total / day_tab[leap][do_mon];
    rsv_avg = rsv_total / day_tab[leap][do_mon];
    rs_est_total = rs_total;
    rst_est_total = rst_total;
    rsv_est_total = rsv_total;
  }

  //row= 290 - (int)((rs_avg/mcmax)*DOTS_HIGH);
  //if (row < TOP+1)
  //{
  //   row = TOP+1;
  //}
  //hline((int)row, DLP+1, DLP+31*18+1, FG_RED);
  //row= 290 - (int)((rst_avg/mcmax)*DOTS_HIGH);
  //if (row < TOP+1)
  //{
  //   row = TOP+1;
  //}
  //hline((int)row, DLP+1, DLP+31*18+1, FG_CYN);
  //row= 290 - (int)((rsv_avg/mcmax)*DOTS_HIGH);
  //if (row < TOP+1)
  //{
  //   row = TOP+1;
  //}
  //hline((int)row, DLP+1, DLP+31*18+1, FG_BLU);

  area_clear(337, 409, 407, 625, FG_WHT);
  sprintf(buff, "$%2.2f  ", rs_total);
  ptext(buff, 337, 407, BG_WHT + FG_BLK);

  sprintf(buff, "($%2.2f/day)  ", rs_avg);
  ptext(buff, 337, 487, BG_WHT + FG_BLK);

  sprintf(buff, "$%2.2f  ", rst_total);
  ptext(buff, 353, 407, BG_WHT + FG_BLK);

  sprintf(buff, "($%2.2f/day)  ", rst_avg);
  ptext(buff, 353, 487, BG_WHT + FG_BLK);

  sprintf(buff, "$%2.2f  ", rsv_total);
  ptext(buff, 369, 407, BG_WHT + FG_BLK);

  sprintf(buff, "($%2.2f/day)  ", rsv_avg);
  ptext(buff, 369, 487, BG_WHT + FG_BLK);

  close_history();

  time(&ltime);
  tod = localtime(&ltime);
  NotBusy();
  unhide_mouse();
}

void mcExitBtnClick(void) {
  UtilityUsageMenu();
}

void mcBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_month * 100L + f_year * 10000L;

  d2 = mc_do_mon * 100L + mc_do_year * 10000L;

  if (d1 != d2) {
    --mc_do_mon;
    if (mc_do_mon == 0) {
      mc_do_mon = 12;
      --mc_do_year;
    }
    MonthlyCostOverLay();
  }
}

void mcNextBtnClick(void) {
  if ((mc_do_mon == this_month) && (mc_do_year == this_year)) {
    return;
  }
  if (++mc_do_mon > 12) {
    mc_do_mon = 1;
    ++mc_do_year;
  }
  MonthlyCostOverLay();
}

//==========================================================
//
//   Monthly Usage Section.
//
//==========================================================

static USHORT mu_device_id;

static void muBackBtnClick(void);
static void muNextBtnClick(void);

PUSH_BUTTON_T muBtnList[4] = {419, 248, 89, 33,  // position
                              FALSE,             // focus
                              PUSH_BUTTON,       // type
                              TRUE,              // visible
                              BtnMouseDown,      // OnMouseDown()
                              BtnMouseUp,        // OnMouseUp()
                              BtnClick,          // OnClick()
                              BtnKeyPress,       // OnKeyPress()
                              PutButton,         // DisplayObject()
                              FALSE,             // default button
                              "\x9E Month \x9E", // text
                              push,              // type
                              up,                // state
                              NULL,              // icon
                              muBackBtnClick,    // function
                              49,

                              419, 347, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "\x9F Month \x9F",
                              push,
                              up,
                              NULL,
                              muNextBtnClick,
                              50,

                              419, 446, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "Cost|Graph",
                              push,
                              up,
                              NULL,
                              MonthlyCost,
                              51,

                              419, 545, 89, 33, // position
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              TRUE,
                              "Util.|Hist.",
                              push,
                              up,
                              icon[12],
                              ReturnClick,
                              52};

SCREEN_OBJECT_T* muSO[16];
static void MonthlyUsageDraw(void);
LWINDOW_T muWindow[1] = {
    35, 0, 640, 445, //window position
    muSO,            //screen object
    MonthlyUsageDraw,
    16,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    "",               //title
    DefaultOnKeyPress //OnKeyPress();
};

#undef DOTS_HIGH
#undef TOP
#undef DLC
#undef DLP

#define DOTS_HIGH 210
#define TOP 81

#define DLC 2  // Column of Left hand labels
#define DLP 56 // Pixel position of Left Side

double mumax;
void MonthlyUsage(void) {
  int i;
  int j;

  which_graph = MONTHLY_USAGE;
  glbWindow = muWindow;
  for (i = 0; i < 4; i++) {
    muSO[i] = &muBtnList[i].so;
  }
  fix_scale.checked = FALSE;

  for (i = 4; i < 15; i++) {
    muSO[i] = &device_btn[i - 4].so;
    device_btn[i - 4].so.visible = FALSE;
    //      device_btn[i-4].pbf = MonthlyUsageOverLay;
    device_btn[i - 4].pbf = MUO_addr;
  }
  muSO[15] = &fix_scale.so;
  fix_scale.checked = FALSE;
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  PutWindow(glbWindow);
}

static void MonthlyUsageDraw(void) {
  //frame for key
  frame_3d(326, 280, 85, 350, FT_PRESSED);
  pbutton(336, 287, 14, 14, PB_DN, FG_BLU);
  pbutton(352, 287, 14, 14, PB_DN, FG_YEL);
  ptext("Daily Energy Usage in kWh", 337, 309, BG_WHT + FG_BLK);
  ptext("Avg. Daily Energy Usage:", 353, 309, BG_WHT + FG_BLK);

  MonthlyUsageOverLay();
}

void MonthlyUsageOverLay(void) {
  char found_key[30];
  char id_string[30];
  char device_name[14];
  char buff[80];

  int i;
  int col;
  long row;
  int color;
  int key_len;
  int do_mon;
  int do_day;
  int do_year;
  int leap;
  int dow;
  int load_ctrl[31];

  long ltime;

  double avg_month;
  double* interval_usage;
  double peak;
  double total;

  struct tm* tod;

  USHORT device_id;

  Busy();
  pbutton(145, 200, 50, 240, PB_UP, FG_WHT);
  ptext("Calculating Usage...", 156, 240, BG_WHT + FG_BLU);

  mu_device_id = device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      mu_device_id = device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }

  if ((mc_do_mon == 0) && (mc_do_day == 0) && (mc_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
  } else {
    do_mon = mc_do_mon;
    do_day = mc_do_day;
    do_year = mc_do_year;
  }

  mc_do_mon = do_mon;
  mc_do_day = do_day;
  mc_do_year = do_year;

  interval_usage = (double*)f_buffer;

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  if (device_id == 0x0FFFF) {
    strcpy(device_name, "Whole House  ");
  } else {
    i = LookupDev(device_id);
    if (i != -1) {
      sprintf(device_name, "%.13s", dev_list.devices[i].desc);
    } else {
      strcpy(device_name, "             ");
    }
  }

  sprintf(buff, "Monthly Energy Usage for %s - %s, %4d     ", device_name, mname[do_mon], 1900 + do_year);
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  for (i = 0; i < 31; ++i) {
    *(interval_usage + i) = 0.0;
  }
  for (i = 0; i < 31; ++i) {
    load_ctrl[i] = 0;
  }

  if ((do_year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }

  dow = day_of_week(do_mon, 1, do_year);
  total = 0.0;
  key_len = 11;

  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, i + 1);
    strcpy(id_string, found_key);
    if (next_history(found_key) == 0) {
      while (1) {
        if (memcmp(found_key, id_string, key_len) != 0) {
          break;
        }
        *(interval_usage + i) += hist_tran.ht_kwh;

        if (hist_tran.ht_kwh > 0 && ((hist_tran.ht_flags >> 2) & 7) > 0) {
          if (device_id == 0x0FFFF) {
            load_ctrl[hist_tran.ht_da - 1] = 1;
          } else {
            if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7)) {
              load_ctrl[hist_tran.ht_da - 1] = 1;
            }
          }
        }

        if (!(hist_tran.ht_flags & 64)) // not a credit
        {
          total += hist_tran.ht_kwh;
        }

        if (next_history(found_key)) {
          break;
        }
      }
    }
  }

  peak = 0.0;
  for (i = 0; i < 31; ++i) {
    if (*(interval_usage + i) > peak) {
      peak = *(interval_usage + i);
    }
  }
  if (fix_scale.checked == FALSE) {
    mumax = FindMax(peak);
  }

  hide_mouse();
  //=====================================
  // border
  //=====================================
  pbutton(80, 55, 212, 562, PB_DN, FG_WHT);

  area_clear(294, 320, 55, 618, FG_WHT + BG_WHT);

  if (mumax < 10.00) {
    sprintf(buff, "%4.2f", mumax);
    ptext(buff, 5 * 14 + 5, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", mumax * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", mumax * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", mumax * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", mumax * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLC * 8, FG_BLK + BG_WHT);
    ptext("0.00", 5 * 14 + 5 + 210, DLC * 8, FG_BLK + BG_WHT);
  } else {
    sprintf(buff, "%4.0f", mumax);
    ptext(buff, 5 * 14 + 5, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", mumax * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", mumax * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", mumax * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLC * 8, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", mumax * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLC * 8, FG_BLK + BG_WHT);
    ptext("   0", 5 * 14 + 5 + 210, DLC * 8, FG_BLK + BG_WHT);
  }

  for (i = 1; i <= day_tab[leap][do_mon]; ++i) {
    if (load_ctrl[i - 1] == 0) {
      color = FG_BLK + BG_WHT;
    } else {
      color = FG_HWHT + BG_WHT;
    }

    sprintf(buff, "%d", i);
    if (i < 10) {
      ptext(buff, 21 * 14, DLP + i * 18 - 10, color);
    } else {
      ptext(buff, 21 * 14, DLP + i * 18 - 16, color);
    }

    if (dow == 0 || dow == 6) {
      ptext(day_abr[dow], 22 * 14, DLP + i * 18 - 16, FG_BLK + BG_WHT);
    } else {
      ptext(day_abr[dow], 22 * 14, DLP + i * 18 - 10, FG_BLK + BG_WHT);
    }
    dow = ((dow + 1) % 7);
  }

  // GRID
  for (i = 1; i < 31; ++i) {
    vline(DLP + i * 18, TOP + 1, 290, FG_GRY);
  }
  for (i = 0; i < 9; ++i) {
    hline(TOP + 21 + 21 * i, DLP + 1, DLP + 31 * 18, FG_GRY);
  }

  // Daily totals kWh - Vertical Bars

  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = DLP + (i * 18) + 7;
    row = 290 - (int)(((*(interval_usage + i)) / mumax) * (DOTS_HIGH - 1));
    if (row < TOP) {
      row = TOP + 1;
    }
    if (row > 290) {
      row = TOP + 1;
    }
    area_clear((int)row, 290, col - 3, col + 7, FG_BLU);
  }

  if ((do_mon == this_month) && (do_year == this_year)) {
    avg_month = total / today;
  } else {
    avg_month = total / day_tab[leap][do_mon];
  }

  sprintf(buff, "%-3.2f kWh     ", avg_month);
  ptext(buff, 353, 505, BG_WHT + FG_BLK);

  area_clear(375, 409, 287, 625, FG_WHT);
  sprintf(buff, "Total energy used during %s, %4d", mname[do_mon], 1900 + do_year);
  ptext(buff, 375, 287, BG_WHT + FG_BLK);
  sprintf(buff, "was %.1f kWh.", total);
  ptext(buff, 389, 287, BG_WHT + FG_BLK);

  //===================================
  // Daily Average usage line
  //===================================
  row = 290 - (int)((avg_month / mumax) * DOTS_HIGH);
  if (row < TOP) {
    row = TOP + 1;
  }
  if (row > 290) {
    row = TOP + 1;
  }
  hline((int)row, DLP + 1, DLP + 31 * 18, FG_YEL);

  close_history();
  time(&ltime);
  tod = localtime(&ltime);
  NotBusy();
  unhide_mouse();
}

void muBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_month * 100L + f_year * 10000L;

  d2 = mc_do_mon * 100L + mc_do_year * 10000L;

  if (d1 != d2) {
    --mc_do_mon;
    if (mc_do_mon == 0) {
      mc_do_mon = 12;
      --mc_do_year;
    }
    MonthlyUsageOverLay();
  }
}

void muNextBtnClick(void) {
  if ((mc_do_mon == this_month) && (mc_do_year == this_year)) {
    return;
  }
  if (++mc_do_mon > 12) {
    mc_do_mon = 1;
    ++mc_do_year;
  }
  MonthlyUsageOverLay();
}

//
//
//
//   Monthly Cost Effectiveness Section.
//
//

static USHORT mce_device_id;
static int mce_do_mon;
static int mce_do_day;
static int mce_do_year;

static void mceBackBtnClick(void);
static void mceNextBtnClick(void);

PUSH_BUTTON_T mceBtnList[3] = {419, 347, 89, 33, // position
                               FALSE,            // focus
                               PUSH_BUTTON,      // type
                               TRUE,             // visible
                               BtnMouseDown,     // OnMouseDown()
                               BtnMouseUp,       // OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               FALSE,            // default button
                               "\x9E Back \x9E", // text
                               push,             // type
                               up,               // state
                               NULL,             // icon
                               mceBackBtnClick,  // function
                               53,

                               419, 446, 89, 33, // position
                               FALSE,
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,
                               "\x9F Next \x9F",
                               push,
                               up,
                               NULL,
                               mceNextBtnClick,
                               54,

                               419, 545, 89, 33, // position
                               FALSE,
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               TRUE,
                               "Main|Menu",
                               push,
                               up,
                               icon[12],
                               ReturnClick,
                               55};

SCREEN_OBJECT_T* mceSO[16];
static void CostEffDraw(void);
LWINDOW_T mceWindow[1] = {
    35, 0, 640, 445, //window position
    mceSO,           //screen object
    CostEffDraw,
    16,               //item count
    0,                //cursor position 0-79 left to right
    0,                //element that has current focus
    fcsBtn,           //focus type
    "",               //title
    DefaultOnKeyPress //OnKeyPress();
};

//===========================================================================================
// NAME:   void MonthlyCostEffective()
//
// HISTORY:
// Author         Date         Description
// UNK            UNK            Initial Version
// Mark Lauter      27 Jan 97      added calling menu index number
//
//
// INPUTS:
// Name            Type         Description
// device_id      USHORT         id of device to display (0xFFFF = whole house)
// sender         int            id of calling menu screen
//
// OUTPUTS:
// Name            Type         Description
// N/A            N/A            N/A
//
// DESCRIPTION:
//
// Displays a graph of the monthly energy usage cost effectiveness.
//===========================================================================================
void MonthlyCostEffective(void) {
  int i;
  int j;

  which_graph = MONTHLY_COST_EFF;

  glbWindow = mceWindow;
  for (i = 0; i < 3; i++) {
    mceSO[i] = &mceBtnList[i].so;
  }
  mceSO[15] = &fix_scale.so;
  fix_scale.checked = FALSE;

  for (i = 3; i < 14; i++) {
    mceSO[i] = &device_btn[i - 3].so;
    device_btn[i - 3].so.visible = FALSE;
    //      device_btn[i-3].pbf = MonthlyCostEffectiveOverLay;
    device_btn[i - 3].pbf = MCEO_addr;
  }
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  PutWindow(glbWindow);
}

static void CostEffDraw(void) {
  //frame for key
  frame_3d(326, 280, 85, 350, FT_PRESSED);
  pbutton(336, 287, 14, 14, PB_DN, FG_BLU);
  pbutton(354, 287, 14, 14, PB_DN, FG_CYN);
  //pbutton(362, 285, 14, 14, PB_DN, FG_MAG);
  ptext("Daily Energy Usage", 337, 309, BG_WHT + FG_BLK);
  ptext("Daily Cents per kWh", 355, 309, BG_WHT + FG_BLK);
  MonthlyCostEffectiveOverLay();
}

#undef DLC
#undef DLP

#define DLC 2  // Column of Left hand labels
#define DLP 56 // Pixel position of Left Side

#undef DLC
#undef DLP
#undef DOTS_HIGH
#undef TOP
#undef WIDTH

#define DLC 2  // Column of Left hand labels
#define DLP 54 // Pixel position of Left Side
#define DOTS_HIGH 210
#define TOP 290 - DOTS_HIGH
#define WIDTH 532

double maxCE;
double maxCE1;

void MonthlyCostEffectiveOverLay(void) {
  char found_key[30];
  char id_string[30];
  char device_name[14];
  char buff[150];

  int i;
  int col;
  long row;
  int last_row;
  //   int         last_col;
  int color;
  int key_len;
  int do_mon;
  int do_day;
  int do_year;
  int leap;
  int dow;
  int load_ctrl[31];

  long ltime;

  double avg_month;
  double* interval_usage;
  //    double      max;
  //    double      max1;
  double peak;
  double peak1;
  double total;
  double total_cost;

  struct tm* tod;

  USHORT device_id;

  Busy();
  mce_device_id = device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      mce_device_id = device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }

  if ((mce_do_mon == 0) && (mce_do_day == 0) && (mce_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
  } else {
    do_mon = mce_do_mon;
    do_day = mce_do_day;
    do_year = mce_do_year;
  }

  mce_do_mon = do_mon;
  mce_do_day = do_day;
  mce_do_year = do_year;

  interval_usage = (double*)f_buffer;

  pbutton(145, 200, 50, 240, PB_UP, FG_WHT);
  ptext("Calculating Cost...", 156, 240, BG_WHT + FG_BLU);

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  if (device_id == 0x0FFFF) {
    strcpy(device_name, "Whole House  ");
  } else {
    i = LookupDev(device_id);
    if (i != -1) {
      sprintf(device_name, "%.13s", dev_list.devices[i].desc);
    } else {
      strcpy(device_name, "             ");
    }
  }

  sprintf(buff, "Monthly Cost Effectiveness for: %s - %s, %4d    ", device_name, mname[do_mon], 1900 + do_year);
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  for (i = 0; i < 31 * 2; ++i) {
    *(interval_usage + i) = 0.0;
  }

  for (i = 0; i < 31; ++i) {
    load_ctrl[i] = 0;
  }

  if ((do_year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }

  dow = day_of_week(do_mon, 1, do_year);
  total = 0.0;
  total_cost = 0.0;
  key_len = 11;

  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    sprintf(found_key, "%04X%3d%2d%2d    ", device_id, do_year, do_mon, i + 1);
    strcpy(id_string, found_key);
    if (next_history(found_key) == 0) {
      while (1) {
        if (memcmp(found_key, id_string, key_len) != 0) {
          break;
        }
        *(interval_usage + i) += hist_tran.ht_kwh;
        *(interval_usage + i + 31) += hist_tran.ht_kwh * hist_tran.ht_kwh_cost;
        total_cost += hist_tran.ht_kwh * hist_tran.ht_kwh_cost;
        if (hist_tran.ht_kwh > 0 && ((hist_tran.ht_flags >> 2) & 7) > 0) {
          if (device_id == 0x0FFFF) {
            load_ctrl[hist_tran.ht_da - 1] = 1;
          } else {
            if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7)) {
              load_ctrl[hist_tran.ht_da - 1] = 1;
            }
          }
        }

        if (!(hist_tran.ht_flags & 64)) // not a credit
        {
          total += hist_tran.ht_kwh;
        }

        if (next_history(found_key)) {
          break;
        }
      }
    }
  }

  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    if (*(interval_usage + i) > 0.0) {
      *(interval_usage + i + 31) /= *(interval_usage + i);
    }
  }

  peak = peak1 = 0.0;
  for (i = 0; i < 31; ++i) {
    if (*(interval_usage + i) > peak) {
      peak = *(interval_usage + i);
    }
    if ((*(interval_usage + i + 31)) > peak1) {
      peak1 = *(interval_usage + i + 31);
    }
  }

  if (fix_scale.checked == FALSE) {
    maxCE = FindMax(peak);
    maxCE1 = FindMax(peak1 * 100.0) / 100.0;
  }

  hide_mouse();
  // BORDERS
  pbutton(TOP - 1, DLP - 2, DOTS_HIGH + 3, WIDTH, PB_DN, BG_WHT);
  area_clear(294, 320, 50, 585, FG_WHT);

  ptext("\xA0/kWh", 5 * 14 + 5 - 16, 576, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", maxCE1 * 100);
  ptext(buff, 5 * 14 + 5, 590, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", maxCE1 * .8 * 100);
  ptext(buff, 5 * 14 + 5 + 42, 590, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", maxCE1 * .6 * 100);
  ptext(buff, 5 * 14 + 5 + 84, 590, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", maxCE1 * .4 * 100);
  ptext(buff, 5 * 14 + 5 + 126, 590, FG_BLK + BG_WHT);
  sprintf(buff, "%2.0f\xA0", maxCE1 * .2 * 100);
  ptext(buff, 5 * 14 + 5 + 168, 590, FG_BLK + BG_WHT);
  ptext(" 0\xA0", 5 * 14 + 5 + 210, 590, FG_BLK + BG_WHT);

  ptext("kWh", 60, DLP - 32, BG_WHT + FG_BLK);
  if (maxCE < 10.00) {
    sprintf(buff, "%4.2f", maxCE);
    ptext(buff, 5 * 14 + 5, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", maxCE * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", maxCE * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", maxCE * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.2f", maxCE * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLP - 40, FG_BLK + BG_WHT);
    ptext("0.00", 5 * 14 + 5 + 210, DLP - 40, FG_BLK + BG_WHT);
  } else {
    sprintf(buff, "%4.0f", maxCE);
    ptext(buff, 5 * 14 + 5, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", maxCE * .8);
    ptext(buff, 5 * 14 + 5 + 42, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", maxCE * .6);
    ptext(buff, 5 * 14 + 5 + 84, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", maxCE * .4);
    ptext(buff, 5 * 14 + 5 + 126, DLP - 40, FG_BLK + BG_WHT);
    sprintf(buff, "%4.0f", maxCE * .2);
    ptext(buff, 5 * 14 + 5 + 168, DLP - 40, FG_BLK + BG_WHT);
    ptext("   0", 5 * 14 + 5 + 210, DLP - 40, FG_BLK + BG_WHT);
  }

  for (i = 1; i <= day_tab[leap][do_mon]; ++i) {
    if (load_ctrl[i - 1] == 0) {
      color = FG_BLK + BG_WHT;
    } else {
      color = FG_HWHT + BG_WHT;
    }

    sprintf(buff, "%d ", i);
    if (i < 10) {
      ptext(buff, 21 * 14, DLP + i * 17 - 10, color);
    } else {
      ptext(buff, 21 * 14, DLP + i * 17 - 16, color);
    }

    if (dow == 0 || dow == 6) {
      ptext(day_abr[dow], 22 * 14, DLP + i * 17 - 16, FG_BLK + BG_WHT);
    } else {
      ptext(day_abr[dow], 22 * 14, DLP + i * 17 - 10, FG_BLK + BG_WHT);
    }
    dow = ((dow + 1) % 7);
  }

  /* Daily Cents/kWh - Vertical Bars */
  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = DLP + (i * 17);
    row = 290 - (int)(((*(interval_usage + i + 31)) / maxCE1) * DOTS_HIGH);
    if (row < TOP) {
      row = TOP + 1;
    }
    if (row > 290) {
      row = TOP + 1;
    }
    area_clear((int)row, 290, col + 1, col + 17, FG_CYN);
  }

  // GRID
  for (i = 1; i < 31; ++i) {
    vline(DLP + i * 17, TOP + 1, 290, FG_GRY);
  }
  for (i = 0; i < 9; ++i) {
    hline(TOP + 21 + 21 * i, DLP, DLP + 31 * 17 + 1, FG_GRY);
  }

  /* Daily kWh - Line */
  last_row = -1;
  for (i = 0; i < day_tab[leap][do_mon]; ++i) {
    col = 64 + (i * 17);
    row = 290 - (int)((*(interval_usage + i) / maxCE) * DOTS_HIGH);
    if (row < TOP) {
      row = TOP + 1;
    }
    if (row > 290) {
      row = TOP + 1;
    }
    hline((int)row - 1, DLP + i * 17, DLP + i * 17 + 17, FG_BLU);
    hline((int)row, DLP + i * 17, DLP + i * 17 + 17, FG_BLU);
    //hline((int)row-1, DLP+i*17, DLP+i*17+17, FG_BLU);
    if (last_row != -1) {
      if (last_row < row) {
        vline(DLP + i * 17, last_row, (int)row, FG_BLU);
        vline(DLP + i * 17 + 1, last_row - 1, (int)row, FG_BLU);
        //vline(DLP+i*17+17, last_row, (int)row, FG_BLU);
      } else {
        vline(DLP + i * 17, (int)row, last_row, FG_BLU);
        vline(DLP + i * 17 + 1, (int)row, last_row, FG_BLU);
        //vline(DLP+i*17+17, (int)row, last_row, FG_BLU);
      }
      //line(last_col, last_row, col, (int)row, FG_BLU);
      //line(last_col, last_row-1, col, (int)row-1, FG_BLU);
    }
    //last_col = col;
    last_row = (int)row;
  }

  if ((do_mon == this_month) && (do_year == this_year)) {
    avg_month = total / today;
  } else {
    avg_month = total / day_tab[leap][do_mon];
  }

  //monthly average usage
  row = 290 - (int)((avg_month / maxCE) * DOTS_HIGH);
  if (row < TOP) {
    row = TOP + 1;
  }
  if (row > 290) {
    row = TOP + 1;
  }
  //hline((int)row, DLP, DLP+31*17+1, FG_MAG);
  area_clear(374, 409, 287, 625, FG_WHT);
  sprintf(buff, "The average daily usage is %.0f kWh.", avg_month);
  ptext(buff, 374, 287, BG_WHT + FG_BLK);

  //monthly average cost
  if (total > 0) {
    avg_month = total_cost / total;
  }
  row = 290 - (int)((avg_month / maxCE1) * DOTS_HIGH);
  if (row < TOP) {
    row = TOP + 1;
  }
  if (row > 290) {
    row = TOP + 1;
  }
  //hline((int)row, DLP, DLP+31*17+1, FG_YEL);
  sprintf(buff, "The average daily cost per kWh is %.1f\xA0", avg_month * 100);
  ptext(buff, 390, 287, BG_WHT + FG_BLK);
  close_history();
  time(&ltime);
  tod = localtime(&ltime);
  NotBusy();
  unhide_mouse();
}

void mceBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_month * 100L + f_year * 10000L;

  d2 = mce_do_mon * 100L + mce_do_year * 10000L;

  if (d1 != d2) {
    --mce_do_mon;
    if (mce_do_mon == 0) {
      mce_do_mon = 12;
      --mce_do_year;
    }
    MonthlyCostEffectiveOverLay();
  }
}

void mceNextBtnClick(void) {
  if ((mce_do_mon == this_month) && (mce_do_year == this_year)) {
    return;
  }
  if (++mce_do_mon > 12) {
    mce_do_mon = 1;
    ++mce_do_year;
  }
  MonthlyCostEffectiveOverLay();
}

void FindFirstHistory(void) {
  //   int m_dif;
  //   int y_dif;
  //   int day;
  //   int month;
  //   int year;
  char last_key[30];
  char key[30];
  extern void ctree_error(void);

  time(&ltime);
  tod = localtime(&ltime);

  open_history();
  // find first whole house entry in history file....
  sprintf(last_key, "FFFF          ");
  recbyte = GTKEY(2, last_key, key);
  if (recbyte == 0) // no entries found
  {
    if (uerr_cod == 0) // no errors simply no record found
    {
      f_day = 0;
      f_month = 0;
      f_year = 0;
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
      f_day = atoi(last_key);
      movez(last_key, key + 7, 2);
      f_month = atoi(last_key);
      movez(last_key, key + 4, 3);
      f_year = atoi(last_key);
    } else // no, treat like no record
    {
      f_day = 0;
      f_month = 0;
      f_year = 0;
      msgbox("ERROR: NOT WHOLE HOUSE", "ERROR", MB_OK);
    }
  }
  close_history();
}

//////////////////////////////////////////
// Draw whole house/Device Monitor graph
//////////////////////////////////////////
//
//
//      MeterHistory section
//
//
//

static void mhBackBtnClick(void);
static void mhNextBtnClick(void);

PUSH_BUTTON_T mhBtnList[4] =
    {
        419, 347, 89, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        TRUE,             // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            // default button
        "\x9E Back \x9E", // text
        push,             // type
        up,               // state
        NULL,             // icon
        mhBackBtnClick,   // function
        56,

        419, 446, 89, 33, // position
        FALSE,
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        FALSE,
        "\x9F Next \x9F",
        push,
        up,
        NULL,
        mhNextBtnClick,
        57,

        419, 545, 89, 33, // position
        FALSE,
        PUSH_BUTTON,  // type
        TRUE,         // visible
        BtnMouseDown, // OnMouseDown()
        BtnMouseUp,   // OnMouseUp()
        BtnClick,     // OnClick()
        BtnKeyPress,  // OnKeyPress()
        PutButton,    // DisplayObject()
        TRUE,
        "Return",
        push,
        up,
        icon[12],
        DevDetExitClick,
        59};

SCREEN_OBJECT_T* mhSO[14];
static void MeterHistoryDraw(void);
LWINDOW_T mhWindow = {
    35, 0, 640, 445, //window position
    mhSO,            //screen object
    MeterHistoryDraw,
    14,                   //item count
    0,                    //cursor position 0-79 left to right
    0,                    //element that has current focus
    fcsBtn,               //focus type
    "Daily Detail Usage", //title
    DefaultOnKeyPress     //OnKeyPress();
};

void MeterHistory(void) {
  int i;
  int j;

  for (i = 0; i < 3; i++) {
    mhSO[i] = &mhBtnList[i].so;
  }
  for (i = 3; i < 14; i++) {
    mhSO[i] = &device_btn[i - 3].so;
    device_btn[i - 3].so.visible = FALSE;
    //      device_btn[i-3].pbf = MeterHistorySwitchClick;
    device_btn[i - 3].pbf = MHSC_addr;
  }
  device_btn[10].so.visible = TRUE;

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(DevBoxTxtList[j], dev_list.devices[i].desc, 13);
      trim(DevBoxTxtList[j]);
      device_btn[j].text = DevBoxTxtList[j];
      device_btn[j].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  glbWindow = &mhWindow;
  PutWindow(glbWindow);
}

static void MeterHistoryDraw(void) {
  char found_key[30];
  char id_string[30];
  char buff[80];
  int i;
  int j;
  int col;
  int row;
  int key_len;
  int do_mon;
  int do_day;
  int do_year;
  long ltime;
  struct tm* tod;
  double* interval_usage;
  double hour_usage;
  double hour_cost;
  double total_kwh;
  double total_cost;
  int load_ctrl[24];
  USHORT device_ndx;

  Busy();
  device_ndx = LookupDev(du_device_id);

  if ((du_do_mon == 0) && (du_do_day == 0) && (du_do_year == 0)) {
    if (uuDateSelection == 1) // specific date selected
    {
      do_mon = uu_selected_month;
      do_day = uu_selected_day;
      do_year = uu_selected_year;
    } else // first entrance do today
    {
      time(&ltime);
      tod = localtime(&ltime);
      this_month = tod->tm_mon + 1;
      today = tod->tm_mday;
      this_year = tod->tm_year;
      do_mon = this_month;
      do_day = today;
      do_year = this_year;
    }
    du_do_mon = do_mon;
    du_do_day = do_day;
    du_do_year = do_year;
  } else // we've been here
  {
    do_mon = du_do_mon;
    do_day = du_do_day;
    do_year = du_do_year;
  }

  du_do_mon = do_mon;
  du_do_day = do_day;
  du_do_year = do_year;

  if (device_ndx == -1) {
    sprintf(buff, "Detail for Whole House   - %d/%d/%4d", do_mon, do_day, 1900 + do_year);
  } else {
    sprintf(buff, "Detail for %13.13s - %d/%d/%4d ", dev_list.devices[device_ndx].desc, do_mon, do_day, 1900 + do_year);
  }
  ptext(buff, 41, 7, BG_BLU + FG_HWHT);

  interval_usage = (double*)f_buffer;

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  for (i = 0; i < 96; ++i) {
    *(interval_usage + i) = 0.0;
    *(interval_usage + i + 96) = 0.0;
  }

  for (i = 0; i < 24; ++i) {
    load_ctrl[i] = 0;
  }

  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", du_device_id, do_year, do_mon, do_day);

  strcpy(id_string, found_key);
  total_kwh = 0.0;
  total_cost = 0.0;
  if (next_history(found_key) == 0) {
    while (1) {
      if (memcmp(found_key, id_string, key_len) != 0) {
        break;
      }
      i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;
      *(interval_usage + i) += hist_tran.ht_kwh;
      //total_kwh += hist_tran.ht_kwh;
      *(interval_usage + i + 96) += hist_tran.ht_kwh * hist_tran.ht_kwh_cost;
      //total_cost += hist_tran.ht_kwh*hist_tran.ht_kwh_cost;

      if (((hist_tran.ht_flags >> 2) & 7) > 0) {
        if (du_device_id == -1)
          load_ctrl[hist_tran.ht_hr] = 1;
        else if ((hist_tran.ht_flags & 3) <= ((hist_tran.ht_flags >> 2) & 7))
          load_ctrl[hist_tran.ht_hr] = 1; // unknown
      }

      if (next_history(found_key)) {
        break;
      }
    }
  }
  close_history();

  hide_mouse();
  position(7, 9);
  text("  HOUR   kWh     Cost", BG_WHT + FG_BLU);
  position(7, 49);
  text("  HOUR   kWh     Cost", BG_WHT + FG_BLU);

  //hline(int top, int left, int right, int color);
  hline(85 + 17, 15, 639 - 14, FG_GRY);
  hline(86 + 17, 16, 639 - 15, FG_HWHT);

  for (i = 0; i < 24; ++i) {
    row = i % 12;
    col = i / 12;
    if (i == 0 || i == 12) {
      strcpy(buff, "12- 1");
    } else {
      if (i < 12) {
        sprintf(buff, "%2d-%2d", i, i + 1);
      } else {
        sprintf(buff, "%2d-%2d", i - 12, i - 12 + 1);
      }
    }
    if (i < 12) {
      strcat(buff, "am");
    } else {
      strcat(buff, "pm");
    }
    position(9 + row, col * 40 + 9);
    if (load_ctrl[i]) {
      text(buff, BG_WHT + FG_HWHT);
    } else {
      text(buff, BG_WHT + FG_BLU);
    }

    position(9 + row, col * 40 + 17);
    hour_usage = 0.0;
    for (j = 0; j < 4; j++) {
      hour_usage += *(interval_usage + (i * 4) + j);
    }
    sprintf(buff, "%5.1f     ", hour_usage);
    text(buff, BG_WHT + FG_BLK);
    total_kwh += round(hour_usage, 1);

    position(9 + row, col * 40 + 25);
    hour_cost = 0.0;
    for (j = 0; j < 4; j++) {
      hour_cost += *(interval_usage + (i * 4) + j + 96);
    }
    sprintf(buff, "%5.2f     ", hour_cost);
    text(buff, BG_WHT + FG_BLK);
    total_cost += round(hour_cost, 2);
  }

  position(22, 26);
  text("TOTAL:", BG_WHT + FG_BLK);

  position(22, 26 + 8);
  sprintf(buff, "%5.1f kWh     $%-5.2f     ", total_kwh, total_cost);
  text(buff, BG_WHT + FG_BLK);

  time(&ltime);
  tod = localtime(&ltime);
  NotBusy();
  unhide_mouse();
}

void mhBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = du_do_day + du_do_mon * 100L + du_do_year * 10000L;

  if (d1 != d2) {
    --du_do_day;
    if ((du_do_mon == 3) && ((du_do_year % 4) == 0) && (du_do_day == 0)) // leap year, March 0
    {
      du_do_mon = 2;
      du_do_day = 29;
    } else if (du_do_day == 0) {
      --du_do_mon;
      du_do_day = day_tab[0][du_do_mon];
    }
    if (du_do_mon == 0) {
      du_do_mon = 12;
      du_do_day = day_tab[0][du_do_mon];
      --du_do_year;
    }
    MeterHistoryDraw();
  }
}

void mhNextBtnClick(void) {
  if ((du_do_day == today) &&
      (du_do_mon == this_month) &&
      (du_do_year == this_year)) {
    return;
  }
  ++du_do_day;
  if ((du_do_mon == 2) && ((du_do_year % 4) == 0) && (du_do_day == 29)) {
  } else if (du_do_day > day_tab[0][du_do_mon]) {
    ++du_do_mon;
    du_do_day = 1;
  }
  if (du_do_mon > 12) {
    du_do_mon = 1;
    ++du_do_year;
  }
  MeterHistoryDraw();
}

//
//
//
//   ShowDaysRates section
//
//

static void drBackBtnClick(void);
static void drNextBtnClick(void);

static int dr_mon;
static int dr_day;
static int dr_year;

PUSH_BUTTON_T drBtnList[3] = {419, 347, 89, 33,
                              FALSE,            // focus
                              PUSH_BUTTON,      // type
                              TRUE,             // visible
                              BtnMouseDown,     // OnMouseDown()
                              BtnMouseUp,       // OnMouseUp()
                              BtnClick,         // OnClick()
                              BtnKeyPress,      // OnKeyPress()
                              PutButton,        // DisplayObject()
                              FALSE,            // default button
                              "\x9E Back \x9E", // text
                              push,             // type
                              up,               // state
                              NULL,             // icon
                              drBackBtnClick,   // function
                              60,

                              419, 446, 89, 33,
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "\x9F Next \x9F",
                              push,
                              up,
                              NULL,
                              drNextBtnClick,
                              61,

                              419, 545, 89, 33,
                              FALSE,
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              TRUE,
                              "Est.|Bill",
                              push,
                              up,
                              icon[12],
                              ReturnClick,
                              62};

SCREEN_OBJECT_T* drSO[3];

LWINDOW_T drWindow[1] = {
    35, 0, 640, 445, //window position
    drSO,            //screen object
    NULL,
    3,                  //item count
    0,                  //cursor position 0-79 left to right
    0,                  //element that has current focus
    fcsBtn,             //focus type
    "Displaying Rates", //title
    DefaultOnKeyPress   //OnKeyPress();
};

static void DateOkClick(void);
static void DateCancelClick(void);

static PUSH_BUTTON_T getdatebutton[2] =
    {
        300, 247, 70, 30,
        FALSE,
        PUSH_BUTTON,
        TRUE, // visible
        BtnMouseDown,
        BtnMouseUp,
        BtnClick,
        BtnKeyPress,
        PutButton,
        FALSE,
        "Ok", //text
        push,
        up,
        NULL,
        DateOkClick,
        63,

        300, 247 + 75, 70, 30,
        FALSE,
        PUSH_BUTTON,
        TRUE, // visible
        BtnMouseDown,
        BtnMouseUp,
        BtnClick,
        BtnKeyPress,
        PutButton,
        FALSE,
        "Cancel", //text
        push,
        up,
        NULL,
        DateCancelClick,
        64};

static TIME_ENTRY_T getdateentry =
    {
        220, 268, 0, 0,     // location
        FALSE,              // focus
        TIME_ENTRY,         // type
        TRUE,               // visible
        TimeEntryMouseDown, // OnMouseDown()
        TimeEntryMouseUp,   // OnMouseUp()
        TimeEntryClick,     // OnClick()
        TimeEntryKeyPress,  // OnKeyPress()
        DisplayTimeEntry,   // DisplayObject()
        date_ent            // type
};

static void DateOkClick(void) {
  int i;

  dr_mon = getdateentry.month;
  dr_day = getdateentry.day;
  dr_year = getdateentry.year;
  glbWindow = drWindow;
  for (i = 0; i < glbWindow->item_count; i++) {
    drSO[i] = &drBtnList[i].so;
  }
  PutWindow(glbWindow);
  DaysRatesDisplay();
}

void BillingMenu(void);
static void DateCancelClick(void) {
  BillingMenu();
}

static SCREEN_OBJECT_T* getdateso[3];

static LWINDOW_T getdatewindow =
    {
        120, 150, 340, 220, //window position
        getdateso,          //screen object
        NULL,
        3,                    //item count
        0,                    //cursor position 0-79 left to right
        0,                    //element that has current focus
        fcsBtn,               //focus type
        "Display Days Rates", //title
        DefaultOnKeyPress     //OnKeyPress();
};

void ShowDaysRates(void) {
  time_t ltime;
  struct tm* tod;

  getdateso[0] = &getdatebutton[0].so;
  getdateso[1] = &getdatebutton[1].so;
  getdateso[2] = &getdateentry.so;

  time(&ltime);
  tod = localtime(&ltime);
  getdateentry.month = tod->tm_mon + 1;
  getdateentry.day = tod->tm_mday;
  getdateentry.year = tod->tm_year;

  glbWindow = &getdatewindow;
  PutWindow(&getdatewindow);
  ptext("Select a date to display.", 220 - 20, 224, BG_TRANSPARENT + FG_BLK);
}

void drBackBtnClick(void) {
  long int d1;
  long int d2;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = dr_day + dr_mon * 100L + dr_year * 10000L;

  if (d1 != d2) {
    --dr_day;
    if ((dr_mon == 3) && ((dr_year % 4) == 0) && (dr_day == 0)) // leap year, March 0
    {
      dr_mon = 2;
      dr_day = 29;
    } else if (dr_day == 0) {
      --dr_mon;
      dr_day = day_tab[0][dr_mon];
    }
    if (dr_mon == 0) {
      dr_mon = 12;
      dr_day = day_tab[0][dr_mon];
      --dr_year;
    }
    DaysRatesDisplay();
  }
}

void drNextBtnClick(void) {
  ++dr_day;
  if ((dr_mon == 2) && ((dr_year % 4) == 0) && (dr_day == 29)) {
  } else if (dr_day > day_tab[0][dr_mon]) {
    ++dr_mon;
    dr_day = 1;
  }
  if (dr_mon > 12) {
    dr_mon = 1;
    ++dr_year;
  }
  DaysRatesDisplay();
}

void DaysRatesDisplay(void) {
  UCHAR buff[80];
  int i;
  int hour;
  int next_hour;
  DAYS_RATES days_rates;

  hide_mouse();
  Busy();
  sprintf(buff, "Displaying Rates for %3s %02d/%02d/%4d", dayofweek[day_of_week(dr_mon, dr_day, dr_year)],
          dr_mon, dr_day, dr_year + 1900);
  ptext(buff, 60, 19 * 8, BG_WHT + FG_BLK);

  read_rates(dr_mon, dr_day, dr_year, &days_rates);

  position(7, 3);
  text("Standard =", BG_WHT + FG_BLK);
  position(7, 14);
  sprintf(buff, "%7.3f", (double)days_rates.rs_rate / 1000.0);
  text(buff, BG_WHT + FG_BLU);

  position(7, 27);
  text("Load Control - Credit =", BG_WHT + FG_BLK);
  position(7, 51);
  sprintf(buff, "%7.3f", (double)days_rates.credit_rate / 1000.0);
  text(buff, BG_WHT + FG_BLU);

  position(7, 61);
  text("Penalty =", BG_WHT + FG_BLK);
  position(7, 71);
  sprintf(buff, "%7.3f", (double)days_rates.penalty_rate / 1000.0);
  text(buff, BG_WHT + FG_BLU);

  position(9, 2);
  text("Hour (AM)         TOD        Variable", BG_WHT + FG_BLK);
  position(9, 42);
  text("Hour (PM)         TOD        Variable", BG_WHT + FG_BLK);

  for (i = 0; i < 12; i++) {
    if (i == 0) {
      hour = 12;
      next_hour = 1;
    } else {
      hour = i;
      next_hour = i + 1;
    }

    position(10 + i, 2);
    sprintf(buff, "%02d:00 - %02d:00     %7.3f     ", hour, next_hour,
            (double)(days_rates.rtod_rate[i]) / 1000.0);
    text(buff, BG_WHT + FG_BLU);

    sprintf(buff, "%7.3f",
            (double)(days_rates.rsv_rate[i]) / 1000.0);
    text(buff, BG_WHT + FG_BLU);

    position(10 + i, 42);
    sprintf(buff, "%02d:00 - %02d:00     %7.3f     ", hour, next_hour,
            (double)(days_rates.rtod_rate[i + 12]) / 1000.0);
    text(buff, BG_WHT + FG_BLU);

    sprintf(buff, "%7.3f",
            (double)(days_rates.rsv_rate[i + 12]) / 1000.0);
    text(buff, BG_WHT + FG_BLU);
  }
  NotBusy();
  unhide_mouse();
}

void ReturnClick(void) {
  (*Sender)();
}

double GetDeviceDollars(USHORT device_id) {
  char found_key[30];
  char id_string[30];
  UCHAR buff[80];
  int i;
  int j;
  int key_len;
  long ltime;
  double* interval_usage;
  double work_sum;
  double total;
  DAYS_RATES days_rates;
  struct tm* tod;

  interval_usage = (double*)f_buffer;

  time(&ltime);
  tod = localtime(&ltime);

  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }
  if ((uu_selected_month == tod->tm_mon + 1) && (uu_selected_day == tod->tm_mday) && (uu_selected_year == tod->tm_year)) {
    days_rates = todays_rates;
  } else {
    switch (read_rates(uu_selected_month, uu_selected_day, uu_selected_year, &days_rates)) {
    case 1:
    case 2:
    case 5:
      sprintf(buff,
              "The rate information for|%02d/%02d/%4d is not available.",
              uu_selected_month, uu_selected_day, uu_selected_year + 1900);
      msgbox(buff, "Information", MB_OK);
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      return 0.0;
    case -1:
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      return 0.0;
    }
  }

  for (i = 0; i < 96 * 2; ++i) {
    *(interval_usage + i) = 0.0;
  }

  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", device_id, uu_selected_year, uu_selected_month, uu_selected_day);
  strcpy(id_string, found_key);
  if (next_history(found_key) == 0) {
    do {
      if (memcmp(found_key, id_string, key_len) != 0) {
        break;
      }
      i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;
      *(interval_usage + i) += hist_tran.ht_kwh;
      *(interval_usage + i + 96) += hist_tran.ht_kwh_cost;
    } while (!next_history(found_key));
  }

  total = 0.0;
  for (i = 0; i < 24; ++i) {
    // Sum the hour
    for (j = 0, work_sum = 0.0; j < 4; ++j) {
      work_sum += *(interval_usage + i * 4 + j);
    }
    switch (cust_info.rate_type) {
    case 0:
      total += work_sum * ((double)days_rates.rs_rate) / 100000.0;
      break;
    case 1:
      total += work_sum * ((double)days_rates.rtod_rate[i]) / 100000.0;
      break;
    case 2:
      total += work_sum * ((double)days_rates.rsv_rate[i]) / 100000.0;
      break;
    }
  }
  close_history();
  time(&ltime);
  tod = localtime(&ltime);

  return total;
}

double GetDevicekW(USHORT device_id) {
  char found_key[30];
  char id_string[30];
  int i;
  int key_len;
  long ltime;
  struct tm* tod;
  double total;

  i = day_of_week(uu_selected_month, uu_selected_day, uu_selected_year);

  if (open_history()) {
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  total = 0.0;
  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", device_id, uu_selected_year, uu_selected_month, uu_selected_day);
  strcpy(id_string, found_key);
  while (next_history(found_key) == 0) {
    if (memcmp(found_key, id_string, key_len) != 0) {
      break;
    }
    total += hist_tran.ht_kwh;
  }

  time(&ltime);
  tod = localtime(&ltime);

  close_history();

  return total;
}

double GetTotalkW(void) {
  char found_key[30];
  char id_string[30];
  int key_len;
  long ltime;
  struct tm* tod;
  double total;

  if (open_history()) {
    msgbox("HISTORY file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  total = 0.0;
  key_len = 11;
  sprintf(found_key, "%04X%3d%2d%2d    ", 0x0FFFF, uu_selected_year, uu_selected_month, uu_selected_day);
  strcpy(id_string, found_key);
  while (next_history(found_key) == 0) {
    if (memcmp(found_key, id_string, key_len) != 0) {
      break;
    }
    total += hist_tran.ht_kwh;
  }

  time(&ltime);
  tod = localtime(&ltime);

  close_history();

  return total;
}

double GetTotalDollars(void) {
  char found_key[30];
  char id_string[30];

  UCHAR buff[80];

  int i;
  int j;
  int key_len;

  long ltime;

  double total;
  double* interval_usage;
  double work_sum;

  DAYS_RATES days_rates;

  struct tm* tod;

  USHORT device_id = 0x0FFFF;

  time(&ltime);
  tod = localtime(&ltime);
  interval_usage = (double*)f_buffer;
  if (open_history()) {
    msgbox("History file not opened.", "Warning", MB_OK);
    lv_stop();
  }

  if ((uu_selected_month == tod->tm_mon + 1) && (uu_selected_day == tod->tm_mday) && (uu_selected_year == tod->tm_year)) {
    days_rates = todays_rates;
  } else {
    switch (read_rates(uu_selected_month, uu_selected_day, uu_selected_year, &days_rates)) {
    case 1:
    case 2:
    case 5:
      sprintf(buff,
              "The rate information for|%02d/%02d/%4d is not available.",
              uu_selected_month, uu_selected_day, uu_selected_year + 1900);
      msgbox(buff, "Information", MB_OK);
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      return 0.0;
    case -1:
      close_history();
      time(&ltime);
      tod = localtime(&ltime);
      return 0.0;
    }
  }

  for (i = 0; i < 96 * 2; ++i) {
    *(interval_usage + i) = 0.0;
  }

  key_len = 11;
  total = 0.0;

  sprintf(found_key, "%04X%3d%2d%2d    ", device_id, uu_selected_year, uu_selected_month, uu_selected_day);
  strcpy(id_string, found_key);
  if (next_history(found_key) == 0) {
    do {
      if (memcmp(found_key, id_string, key_len) != 0) {
        break;
      }
      i = hist_tran.ht_hr * 4 + hist_tran.ht_qthr;
      *(interval_usage + i) += hist_tran.ht_kwh;
      *(interval_usage + i + 96) += hist_tran.ht_kwh_cost;
    } while (!next_history(found_key));
  }
  for (i = 0; i < 24; ++i) {
    // Sum the hour
    for (j = 0, work_sum = 0.0; j < 4; ++j) {
      work_sum += *(interval_usage + i * 4 + j);
    }
    // calculate dollars for that hour
    switch (cust_info.rate_type) {
    case 0:
      total += work_sum * ((double)days_rates.rs_rate) / 100000.0;
      break;
    case 1:
      total += work_sum * ((double)days_rates.rtod_rate[i]) / 100000.0;
      break;
    case 2:
      total += work_sum * ((double)days_rates.rsv_rate[i]) / 100000.0;
      break;
    }
  }

  close_history();
  time(&ltime);
  tod = localtime(&ltime);

  return total;
}

static void MeterHistorySwitchClick(void) {
  int i;

  du_device_id = 0x0FFFF;
  for (i = 0; i < mon_devices_used; i++) {
    if (device_btn[i].state == down) {
      du_device_id = dev_list.devices[monitored_devices[i]].device_slot;
      break;
    }
  }
  MeterHistoryDraw();
}

static void USAGEDetailClick(void) {
  long ltime;
  struct tm* tod;

  detail_caller = USAGE;

  time(&ltime);
  tod = localtime(&ltime);

  if ((du_do_mon == tod->tm_mon + 1) &&
      (du_do_day == tod->tm_mday) &&
      (du_do_year == tod->tm_year)) {
    device_detail(LookupDev(du_device_id));
  } else {
    MeterHistory();
  }
}

static void COSTDetailClick(void) {
  long ltime;
  struct tm* tod;

  detail_caller = COST;

  time(&ltime);
  tod = localtime(&ltime);

  if ((du_do_mon == tod->tm_mon + 1) &&
      (du_do_day == tod->tm_mday) &&
      (du_do_year == tod->tm_year)) {
    device_detail(LookupDev(du_device_id));
  } else {
    MeterHistory();
  }
}
