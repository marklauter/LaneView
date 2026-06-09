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
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "uumenu.h"
#include "insubs.h"
#include "inio.h"
//#include "dsdmenu.h"
#include "graphs.h"
#include "ismenu.h"
#include "msgbox.h"
#include "inutil.h"
#include "inhist.h"
#include "calendar.h"
#include "device.h"
#include "dvc_evnt.h"
#include "whmenu.h"

#if LOAD_CONTROL_TECO
static void LoadMgmtStatusBtnClick(void);
static void LoadMgmtHistoryBtnClick(void);
static void ShowLoadMgmtHistory(void);
#endif

static void lmGetTime(void);
static void lmdateBtnOkClick(void);

void RateComparison(void);

extern int glb_cat_index;
static void LMReturn(void) {
  Device_Event_Menu(glb_cat_index);
}

//
//
//
// Rate Comparison Section
//
//
//

static void rcToggleBtnClick(void);

static int rcToggle;

PUSH_BUTTON_T rcBtnList[2] = {
    400, 429, 102, 54,
    FALSE,
    PUSH_BUTTON,  // type
    TRUE,         // visible
    BtnMouseDown, // OnMouseDown()
    BtnMouseUp,   // OnMouseUp()
    BtnClick,     // OnClick()
    BtnKeyPress,  // OnKeyPress()
    PutButton,    // DisplayObject()
    FALSE,
    "Today/Month|Toggle",
    push,
    up,
    NULL,
    rcToggleBtnClick,
    185,

    400, 535, 102, 54,
    FALSE,
    PUSH_BUTTON,  // type
    TRUE,         // visible
    BtnMouseDown, // OnMouseDown()
    BtnMouseUp,   // OnMouseUp()
    BtnClick,     // OnClick()
    BtnKeyPress,  // OnKeyPress()
    PutButton,    // DisplayObject()
    TRUE,
    "Main Menu",
    push,
    up,
    icon[12],
    IntroScreen,
    186
    //device_status_display_menu
};

SCREEN_OBJECT_T* rcSO[2];

static void RateDraw(void);
LWINDOW_T rcWindow[1] = {
    35, 0, 640, 445, //window position
    rcSO,            //screen object
    RateDraw,
    2,                 //item count
    0,                 //cursor position 0-79 left to right
    0,                 //element that has current focus
    fcsBtn,            //focus type
    "Rate Comparison", //title
    DefaultOnKeyPress  //OnKeyPress();
};

static void RateDraw(void) {
  char buff[80];
  int i;
  int row;
  int col;
  double diff;
  double cur_total;
  double flat_total;
  double tod_total;
  double var_total;
  double total_cost;

  vline(318, 8 * 14 - 5, 20 * 14 + 5, FG_BLU);
  vline(319, 8 * 14 - 5, 20 * 14 + 5, FG_BLU);
  vline(320, 8 * 14 - 5, 20 * 14 + 5, FG_HWHT);
  vline(321, 8 * 14 - 5, 20 * 14 + 5, FG_BLU);
  vline(322, 8 * 14 - 5, 20 * 14 + 5, FG_BLU);

  cur_total = flat_total = tod_total = var_total = 0.0;
  position(7, 2);
  text("  HOUR    KWH  $-Std    $-Tod    $-Var", BG_WHT + FG_BLU);
  position(7, 42);
  text("  HOUR    KWH  $-Std    $-Tod    $-Var", BG_WHT + FG_BLU);

  for (i = 0; i < 24; ++i) {
    if (rcToggle == 0) // Today
    {
      diff = (double)(meter.current_day[i]) / 10000.0;
      cur_total += round(diff, 1);
      flat_total += round(diff * ((double)(todays_rates.rs_rate) / 100000.0), 2);
      tod_total += round(((double)(meter.tod_current_day$[i]) / 100000.0), 2);
      var_total += round(((double)(meter.var_current_day$[i]) / 100000.0), 2);
    } else // Month To Date
    {
      diff = (double)(meter.mtd[i]) / 10000.0;
      cur_total += round(diff, 1);
      flat_total += (round(diff, 1) * ((double)(todays_rates.rs_rate) / 100000.0));
      tod_total += round(((double)(meter.tod_mtd$[i]) / 100000.0), 2);
      var_total += round(((double)(meter.var_mtd$[i]) / 100000.0), 2);
    }
  }
  position(22, 2);
  sprintf(buff, "Current Hour: %7.4f KWH", (double)(meter.current_day[meter_hour]) / 10000.0);
  text(buff, BG_WHT + FG_BLU);

  position(25, 61);
  if (rcToggle == 0) {
    text("Today", BG_WHT + FG_RED);
  } else {
    text("This Month", BG_WHT + FG_RED);
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
    text(buff, BG_WHT + FG_BLK);

    if (rcToggle) {
      total_cost = (double)(meter.mtd[i]) / 10000.0;
    } else {
      total_cost = (double)(meter.current_day[i]) / 10000.0;
    }

    position(9 + row, col * 40 + 10);
    sprintf(buff, "%6.1f", total_cost);
    text(buff, BG_WHT + FG_BLK);

    position(9 + row, col * 40 + 16);
    sprintf(buff, "%7.2f", (double)(total_cost * todays_rates.rs_rate) / 100000.0);
    text(buff, BG_WHT + FG_BLK);

    position(9 + row, col * 40 + 24);
    if (rcToggle) {
      sprintf(buff, "%7.2f", (double)(meter.tod_mtd$[i]) / 100000.0);
    } else {
      sprintf(buff, "%7.2f", (double)(meter.tod_current_day$[i]) / 100000.0);
    }
    text(buff, BG_WHT + FG_BLK);

    position(9 + row, col * 40 + 32);
    if (rcToggle) {
      sprintf(buff, "%7.2f", (double)(meter.var_mtd$[i]) / 100000.0);
    } else {
      sprintf(buff, "%7.2f", (double)(meter.var_current_day$[i]) / 100000.0);
    }
    text(buff, BG_WHT + FG_BLK);
  }

  position(24, 2);
  text("TOTALS:", BG_WHT + FG_BLK);

  position(24, 10);
  sprintf(buff, "%6.1f", cur_total);
  text(buff, BG_WHT + FG_BLK);

  position(24, 16);
  sprintf(buff, "%7.2f", flat_total);
  text(buff, BG_WHT + FG_BLK);

  position(24, 24);
  sprintf(buff, "%7.2f", tod_total);
  text(buff, BG_WHT + FG_BLK);

  position(24, 32);
  sprintf(buff, "%7.2f", var_total);
  text(buff, BG_WHT + FG_BLK);

  return;
}

void RateComparison(void) {
  int i;
  glbWindow = rcWindow;
  for (i = 0; i < glbWindow->item_count; i++) {
    rcSO[i] = &rcBtnList[i].so;
  }
  PutWindow(glbWindow);
}

void rcToggleBtnClick(void) {
  if (rcToggle == FALSE) {
    rcToggle = TRUE;
  } else {
    rcToggle = FALSE;
  }
  RateComparison();
}

#if LOAD_CONTROL_TECO
//
//
//
// Load Management Section
//
//
//

USHORT lctrl_devices[MAX_DEVICES];
int lctrl_devices_used;

PUSH_BUTTON_T lmBtnList[3] = {210, 171, 89, 33,
                              FALSE,        // focus
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "Status",
                              push,
                              up,
                              NULL,
                              LoadMgmtStatusBtnClick, //LMStatus,
                              146,

                              210, 270, 89, 33,
                              FALSE,        // focus
                              PUSH_BUTTON,  // type
                              TRUE,         // visible
                              BtnMouseDown, // OnMouseDown()
                              BtnMouseUp,   // OnMouseUp()
                              BtnClick,     // OnClick()
                              BtnKeyPress,  // OnKeyPress()
                              PutButton,    // DisplayObject()
                              FALSE,
                              "History",
                              push,
                              up,
                              NULL,
                              LoadMgmtHistoryBtnClick,
                              147,

                              210, 369, 89, 33,
                              FALSE,        // focus
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
                              LMReturn,
                              148};

SCREEN_OBJECT_T* lmSO[3];

LWINDOW_T lmWindow[1] = {
    155, 155, 330, 122, //window position
    lmSO,               //screen object
    NULL,
    3,                 //item count
    0,                 //cursor position 0-79 left to right
    0,                 //element that has current focus
    fcsBtn,            //focus type
    "Load Management", //title
    DefaultOnKeyPress  //OnKeyPress();
};

void LoadMgmtHistoryMenu(void);
void LoadMgmtStatusMenu(void);

void LoadMgmtMenu(void) {
  int i;

  glbWindow = lmWindow;
  for (i = 0; i < glbWindow->item_count; i++) {
    lmSO[i] = &lmBtnList[i].so;
  }
  PutWindow(glbWindow);
}

void LoadMgmtStatusBtnClick(void) {
  LoadMgmtStatusMenu();
}

void LoadMgmtHistoryBtnClick(void) {
  LoadMgmtHistoryMenu();
}

#if LOAD_CONTROL_TECO
//
//
//
// Load Management History Section
//
//
//

void lmhPriorMonBtnClk(void);
void lmhNextMonBtnClk(void);
void lmhExitBtnClk(void);
void lmhPgUpBtnClk(void);
void lmhPgDnBtnClk(void);

PUSH_BUTTON_T lmhBtnList[5] = {405, 115, 100, 50,
                               FALSE,        // focus
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,
                               "Next|Page",
                               push,
                               up,
                               NULL,
                               lmhPgDnBtnClk,
                               149,

                               405, 220, 100, 50,
                               FALSE,        // focus
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,
                               "Previous|Page",
                               push,
                               up,
                               NULL,
                               lmhPgUpBtnClk, //LMStatus,
                               166,

                               405, 325, 100, 50,
                               FALSE,        // focus
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,
                               "Prior|Month",
                               push,
                               up,
                               NULL,
                               lmhPriorMonBtnClk, //LMStatus,
                               167,

                               405, 430, 100, 50,
                               FALSE,        // focus
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,
                               "Next|Month",
                               push,
                               up,
                               NULL,
                               lmhNextMonBtnClk,
                               168,

                               405, 535, 100, 50,
                               FALSE,        // focus
                               PUSH_BUTTON,  // type
                               TRUE,         // visible
                               BtnMouseDown, // OnMouseDown()
                               BtnMouseUp,   // OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               TRUE,
                               "Load|Management|Menu",
                               push,
                               up,
                               icon[12],
                               lmhExitBtnClk,
                               169};

SCREEN_OBJECT_T* lmhSO[5];
static void lmhDraw(void);
LWINDOW_T lmhWindow[1] = {
    35, 0, 640, 445, //window position
    lmhSO,           //screen object
    lmhDraw,
    5,                         //item count
    0,                         //cursor position 0-79 left to right
    0,                         //element that has current focus
    fcsBtn,                    //focus type
    "Load Management History", //title
    DefaultOnKeyPress          //OnKeyPress();
};

static int lmh_do_year;
static int lmh_do_mon;
static DEVICE_LOAD_CTRL* load_ctrl_history;

static int max_rows_on_page;
static int rows_on_page;
static int lmh_first_record;

void ShowPageOfLoadMgmtHistory(int month, int year, int first_record);

void LoadMgmtHistoryMenu(void) {
  int i;

  glbWindow = lmhWindow;
  for (i = 0; i < glbWindow->item_count; i++) {
    lmhSO[i] = &lmhBtnList[i].so;
  }
  PutWindow(glbWindow);
}

void lmhDraw(void) {
  time(&ltime);
  tod = localtime(&ltime);
  lmh_do_year = tod->tm_year;
  lmh_do_mon = tod->tm_mon + 1;

  load_ctrl_history = (void*)f_buffer;

  max_rows_on_page = 10;
  lmh_first_record = 0;
  if (BuildLoadCtrlHistory(lmh_do_year, lmh_do_mon, load_ctrl_history)) {
    return; // need to set a flag here, eh?
  }

  if (load_ctrl_hist_count < max_rows_on_page) {
    rows_on_page = load_ctrl_hist_count;
  } else {
    rows_on_page = max_rows_on_page;
  }
  ShowLoadMgmtHistory();
}

void lmhPgUpBtnClk(void) {
  lmh_first_record -= max_rows_on_page;
  if (lmh_first_record < 0) {
    lmh_first_record = 0;
  }
  rows_on_page = max_rows_on_page;
  ShowLoadMgmtHistory();
}

void lmhPgDnBtnClk(void) {
  if (rows_on_page == max_rows_on_page) {
    lmh_first_record += max_rows_on_page;
    rows_on_page = max_rows_on_page;
  }
  ShowLoadMgmtHistory();
}

void lmhPriorMonBtnClk(void) {
  lmh_first_record = 0;
  lmh_do_mon--;
  if (lmh_do_mon < 1) {
    lmh_do_year--;
    lmh_do_mon = 12;
  }

  BuildLoadCtrlHistory(lmh_do_year, lmh_do_mon, load_ctrl_history);

  if (load_ctrl_hist_count < max_rows_on_page) {
    rows_on_page = load_ctrl_hist_count;
  } else {
    rows_on_page = max_rows_on_page;
  }
  ShowLoadMgmtHistory();
}

void lmhNextMonBtnClk(void) {
  lmh_first_record = 0;
  lmh_do_mon++;
  if (lmh_do_mon > 12) {
    lmh_do_year++;
    lmh_do_mon = 1;
  }
  BuildLoadCtrlHistory(lmh_do_year, lmh_do_mon, load_ctrl_history);

  if (load_ctrl_hist_count < max_rows_on_page) {
    rows_on_page = load_ctrl_hist_count;
  } else {
    rows_on_page = max_rows_on_page;
  }
  ShowLoadMgmtHistory();
}

void lmhExitBtnClk(void) {
  LoadMgmtMenu();
}

void ShowLoadMgmtHistory(void) {
  ShowPageOfLoadMgmtHistory(lmh_do_mon, lmh_do_year, lmh_first_record);
  return;
}

void ShowPageOfLoadMgmtHistory(int month, int year, int first_record) {
  char buff[80];
  int i;
  int hour;
  char zone;
  struct tm tm_work;
  DEVICE_LOAD_CTRL* current_history;

  area_clear(6 * 14, 349, 8, 630, FG_WHT);
  sprintf(buff, "Load Management History for %s %d      ", mname[month],
          year + 1900);

  ptext(buff, 60, 152, BG_WHT + FG_BLK);

  position(7, 2);
  text("          Start   End                                         Credit   Penalty",
       BG_WHT + FG_YEL);
  position(8, 2);
  text(" Date     Time    Time    Device        Pty  Override  KWH    Amount   Amount",
       BG_WHT + FG_YEL);

  for (i = 0; i < rows_on_page; i++) {
    if ((first_record + i) >= load_ctrl_hist_count) {
      rows_on_page = i;
      break;
    }

    current_history = load_ctrl_history + first_record + i;

    if (current_history > load_ctrl_history + MAX_LOAD_CTRL_HISTORY) {
      msgbox("History record overflow.", "Error", MB_OK);
      return;
    }

    position(10 + i, 2);

    tm_work = *localtime(&current_history->start_time);
    sprintf(buff, "%2d/%02d/%02d  ", tm_work.tm_mon + 1, tm_work.tm_mday, tm_work.tm_year);
    text(buff, BG_WHT + FG_BLK);

    hour = tm_work.tm_hour;

    zone = 'a';
    if (hour > 11) {
      hour -= 12;
      zone = 'p';
    }
    if (hour == 0) {
      hour = 12;
    }

    sprintf(buff, "%2d:%02d%c  ", hour, tm_work.tm_min, zone);
    text(buff, BG_WHT + FG_BLK);

    tm_work.tm_min += current_history->duration;
    mktime(&tm_work);
    hour = tm_work.tm_hour;

    zone = 'a';
    if (hour > 11) {
      hour -= 12;
      zone = 'p';
    }
    if (hour == 0) {
      hour = 12;
    }

    sprintf(buff, "%2d:%02d%c  ", hour, tm_work.tm_min, zone);
    text(buff, BG_WHT + FG_BLK);

    sprintf(buff, "%.13s  %1d      %c     ", dev_list.devices[current_history->device_id].desc,
            current_history->level, current_history->override);
    text(buff, BG_WHT + FG_BLK);

    sprintf(buff, "%6.2f  $%6.2f  $%6.2f", current_history->kwh,
            current_history->credit, current_history->penalty);
    text(buff, BG_WHT + FG_BLK);
  }

  position(11 + max_rows_on_page, 53);
  sprintf(buff, "Total     $%6.2f  $%6.2f", monthly_total_credit,
          monthly_total_penalty);
  text(buff, BG_WHT + FG_BLK);

  return;
}

//
//
//
// Load Management Status Section
//
//
//

void lmsOverrideBtnClk(void);
void lmsClearORBtnClk(void);
void lmsExitBtnClk(void);
void ShowLoadMgmtStatus(void);
void lmsDevBtnClk(void);

PUSH_BUTTON_T lmsBtnList[13] = {405, 205, 140, 50,
                                FALSE,        // focus
                                PUSH_BUTTON,  // type
                                TRUE,         // visible
                                BtnMouseDown, // OnMouseDown()
                                BtnMouseUp,   // OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,
                                "Override",
                                push,
                                up,
                                NULL,
                                lmsOverrideBtnClk, //LMStatus,
                                170,

                                405, 350, 140, 50,
                                FALSE,        // focus
                                PUSH_BUTTON,  // type
                                TRUE,         // visible
                                BtnMouseDown, // OnMouseDown()
                                BtnMouseUp,   // OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,
                                "Clear|Override",
                                push,
                                up,
                                NULL,
                                lmsClearORBtnClk,
                                171,

                                405, 495, 140, 50,
                                FALSE,        // focus
                                PUSH_BUTTON,  // type
                                TRUE,         // visible
                                BtnMouseDown, // OnMouseDown()
                                BtnMouseUp,   // OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,
                                "Load|Management|Menu",
                                push,
                                up,
                                NULL,
                                lmsExitBtnClk,
                                172,

                                112, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                173,

                                140, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                174,

                                168, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                175,

                                196, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                176,

                                224, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                177,

                                252, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                178,

                                280, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                179,

                                308, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                180,

                                336, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                181,

                                364, 5, 120, 26,
                                FALSE,        //focus
                                PUSH_BUTTON,  //type
                                FALSE,        // visible
                                BtnMouseDown, //OnMouseDown()
                                BtnMouseUp,   //OnMouseUp()
                                BtnClick,     // OnClick()
                                BtnKeyPress,  // OnKeyPress()
                                PutButton,    // DisplayObject()
                                FALSE,        //default button
                                "blank",      //text
                                set,          //type
                                up,           //state
                                NULL,         //icon
                                lmsDevBtnClk, //function
                                182};

SCREEN_OBJECT_T* lmsSO[13];
static void lmDraw(void);
LWINDOW_T lmsWindow[1] = {
    35, 0, 640, 445, //window position
    lmsSO,           //screen object
    lmDraw,
    13,                       //item count
    0,                        //cursor position 0-79 left to right
    0,                        //element that has current focus
    fcsBtn,                   //focus type
    "Load Management Status", //title
    DefaultOnKeyPress         //OnKeyPress();
};

static UCHAR lmsBtnTxtList[10][14];
static int lms_index;

void LoadMgmtStatusMenu(void) {
  int i;
  int j;

  glbWindow = lmsWindow;
  memset(lmsBtnTxtList, 0, sizeof(char) * 10 * 14);

  max_rows_on_page = 12;
  for (i = 0; i < glbWindow->item_count; i++) {
    lmsSO[i] = &lmsBtnList[i].so;
    if (i > 2) {
      lmsBtnList[i].so.visible = FALSE;
    }
  }

  for (i = 0, j = 0, lctrl_devices_used = 0; i < dev_list.devices_used; i++) {
    if ((dev_list.devices[i].priority > 0) && !(IsSubmeter(&dev_list.devices[i]))) {
      movez(lmsBtnTxtList[j], dev_list.devices[i].desc, 13);
      trim(lmsBtnTxtList[j]);
      lmsBtnList[j + 3].text = lmsBtnTxtList[j];
      lmsBtnList[j + 3].so.visible = TRUE;
      if (j == 0) {
        lmsBtnList[j + 3].state = down;
        lms_index = 0;
      } else {
        lmsBtnList[j + 3].state = up;
      }
      j++;
      lctrl_devices[lctrl_devices_used++] = i;
    }
  }

  if (lctrl_devices_used < max_rows_on_page) {
    rows_on_page = mon_devices_used;
  } else {
    rows_on_page = max_rows_on_page;
  }

  PutWindow(glbWindow);
}

static void lmDraw(void) {
  read_load_control_info();
  ShowLoadMgmtStatus();
}

void lmsExitBtnClk(void) {
  LoadMgmtMenu();
}

void lmsClearORBtnClk(void) {
  if (send_load_ctrl_override(
          dev_list.devices[lctrl_devices[lms_index]].device_slot, 0L, 0L) != 0) {
    msgbox("Unable to access ICU|Unable to clear Override",
           "Clearing Override", MB_OK);
  } else {
    msgbox("Credit for current load|control will start in|the next 15 minute period.",
           "Clearing Override", MB_OK);
    dev_list.devices[lctrl_devices[lms_index]].override_start = 0;
    dev_list.devices[lctrl_devices[lms_index]].override_end = 0;
  }
  LoadMgmtStatusMenu();
}

void lmsOverrideBtnClk(void) {
  if (load_ctrl_tbl[0].level == 4) {
    msgbox("This is an emergency load|control.  It cannot be overridden.",
           "Load Control Override", MB_OK);
    ShowLoadMgmtStatus();
  } else {
    // GET START/STOP TIMES
    lmGetTime();
  }
}

void lmsDevBtnClk(void) {
  lms_index = glbWindow->focus.current - 3; // 3 for buttons ahead of us in list
}

void ShowControlStatus(int device_index, int color) {
  char buff[80];
  int i;
  int hour;
  int found;
  char zone;
  struct tm tm_work;

  read_device_status_info(dev_list.devices[lctrl_devices[device_index]].device_slot);

#if 0
   position(row, 2);

   movez(buff, dev_list.devices[lctrl_devices[device_index]].desc, 13);
   strcat(buff, "  ");
   text(buff, color);
#else
  //position(row, 18);
#endif

  if (device_status.device_id == dev_list.devices[lctrl_devices[device_index]].device_slot) {
    switch (device_status.under_load_ctrl) {
    case 0:
      ptext("No       ", 116 + 28 * device_index, 136, color);
      break;
    case 1:
      ptext("Override ", 116 + 28 * device_index, 136, color);
      break;
    case 2:
      ptext("Yes      ", 116 + 28 * device_index, 136, color);
      break;
    default:
      ptext("Unknown  ", 116 + 28 * device_index, 136, color);
      break;
    }
  } else {
    ptext("Unknown  ", 116 + 28 * device_index, 136, color);
  }

  found = 0;
  for (i = 0; i < MAX_LOAD_CTRLS; i++) {
    if (load_ctrl_tbl[i].level == 0) {
      break;
    }

    if (load_ctrl_tbl[i].level >= dev_list.devices[lctrl_devices[device_index]].priority) {
      tm_work = *gmtime(&load_ctrl_tbl[i].start_time);
      sprintf(buff, "%2d/%02d/%02d ", tm_work.tm_mon + 1, tm_work.tm_mday, tm_work.tm_year);
      ptext(buff, 116 + 28 * device_index, 208, color);

      hour = tm_work.tm_hour;

      zone = 'a';
      if (hour > 11) {
        hour -= 12;
        zone = 'p';
      }
      if (hour == 0) {
        hour = 12;
      }

      sprintf(buff, "%2d:%02d%c-", hour, tm_work.tm_min, zone);
      ptext(buff, 116 + 28 * device_index, 280, color);

      tm_work.tm_min += load_ctrl_tbl[i].duration;
      tm_work.tm_isdst = _daylight;
      mktime(&tm_work);
      hour = tm_work.tm_hour;

      zone = 'a';
      if (hour > 11) {
        hour -= 12;
        zone = 'p';
      }
      if (hour == 0) {
        hour = 12;
      }

      sprintf(buff, "%2d:%02d%c ", hour, tm_work.tm_min, zone);
      ptext(buff, 116 + 28 * device_index, 336, color);
      found = 1;
      break;
    }
  }

  //if(!found)
  //{
  //   text("                        ", color);
  //}

  if (dev_list.devices[lctrl_devices[device_index]].override_start > 0) {
    tm_work = *gmtime(&dev_list.devices[lctrl_devices[device_index]].override_start);
    sprintf(buff, "%2d/%02d/%02d ", tm_work.tm_mon + 1, tm_work.tm_mday, tm_work.tm_year);
    ptext(buff, 116 + 28 * device_index, 392, color);

    hour = tm_work.tm_hour;
    zone = 'a';
    if (hour > 11) {
      hour -= 12;
      zone = 'p';
    }
    if (hour == 0) {
      hour = 12;
    }

    sprintf(buff, "%2d:%02d%c ", hour, tm_work.tm_min, zone);
    ptext(buff, 116 + 28 * device_index, 464, color);

    tm_work = *gmtime(&dev_list.devices[lctrl_devices[device_index]].override_end);
    sprintf(buff, "%2d/%02d/%02d ", tm_work.tm_mon + 1, tm_work.tm_mday, tm_work.tm_year);
    ptext(buff, 116 + 28 * device_index, 520, color);

    hour = tm_work.tm_hour;
    zone = 'a';
    if (hour > 11) {
      hour -= 12;
      zone = 'p';
    }
    if (hour == 0) {
      hour = 12;
    }

    sprintf(buff, "%2d:%02d%c", hour, tm_work.tm_min, zone);
    ptext(buff, 116 + 28 * device_index, 592, color);
  } else {
    //text("                               ", color);
  }

  tod = localtime(&ltime);
  return;
}

void ShowLoadMgmtStatus(void) {
  int i;

  //area_clear(6*14,349,4,635,FG_WHT);
  ptext("     Load Management Device Status", 60, 160, BG_WHT + FG_BLK);

  position(7, 2);
  text("                Under    Start/Stop              Override        Override", BG_WHT + FG_YEL);
  position(8, 2);
  text(" Device         Control  Time                    Start           Until", BG_WHT + FG_YEL);

  for (i = 0; i < lctrl_devices_used; ++i) {
    //if(i == opt)
    //{
    //   show_control_status(i, i+9, norm_attrib);
    //}
    //else
    //{
    ShowControlStatus(i, BG_WHT + FG_BLK);
    //}
  }
}

//Get Date Screens
PUSH_BUTTON_T lmdateBtnList[2] = {330, 230, 80, 30,
                                  FALSE,
                                  PUSH_BUTTON,  // type
                                  TRUE,         // visible
                                  BtnMouseDown, // OnMouseDown()
                                  BtnMouseUp,   // OnMouseUp()
                                  BtnClick,     // OnClick()
                                  BtnKeyPress,  // OnKeyPress()
                                  PutButton,    // DisplayObject()
                                  TRUE,
                                  "Ok",
                                  push,
                                  up,
                                  NULL,
                                  lmdateBtnOkClick,
                                  183,

                                  330, 330, 80, 30,
                                  FALSE,
                                  PUSH_BUTTON,  // type
                                  TRUE,         // visible
                                  BtnMouseDown, // OnMouseDown()
                                  BtnMouseUp,   // OnMouseUp()
                                  BtnClick,     // OnClick()
                                  BtnKeyPress,  // OnKeyPress()
                                  PutButton,    // DisplayObject()
                                  FALSE,
                                  "Cancel",
                                  push,
                                  up,
                                  NULL,
                                  LoadMgmtStatusMenu,
                                  184};

TEXT_BOX_T lmdateTxtList[4] = {
    0, 0, 0, 14,  // position
    TRUE,         // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    15,           // row
    30,           // column
    8,            // box length
    tb_date,      // type
    "",           // text
    NULL,

    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    15,           // row
    44,           // column
    6,            // box length
    tb_time,      // type
    "",           // text
    NULL,

    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    21,           // row
    30,           // column
    8,            // box length
    tb_date,      // type
    "",           // text
    NULL,

    0, 0, 0, 14,  // position
    FALSE,        // got_focus
    TEXT_BOX,     // type
    TRUE,         // visible
    TxtMouseDown, // OnMouseDown()
    TxtMouseUp,   // OnMouseUp()
    TxtClick,     // OnClick()
    TxtKeyPress,  // OnKeyPress()
    PutTxtBox,    // DisplayObject()
    21,           // row
    44,           // column
    6,            // box length
    tb_time,      // type
    "",           // text
    NULL};

SCREEN_OBJECT_T* lmdateSO[6];

static void lmGetTimeDraw(void);

LWINDOW_T lmdateWindow[1] = {
    100, 160, 320, 270, //window position
    lmdateSO,           //screen object
    lmGetTimeDraw,
    6,                         //item count
    29,                        //cursor position 0-79 left to right
    0,                         //element that has current focus
    fcsBtn,                    //focus type
    "Entering override times", //title
    DefaultOnKeyPress,         // OnKeyPress() event handler
    0                          //default object
};

void lmGetTime(void) {
  char am_pm;
  int i;
  int wk_min;
  int wk_hour;
  time_t end_time_t;
  struct tm end_tm;

  glbWindow = lmdateWindow;
  for (i = 0; i < 4; i++) {
    lmdateSO[i] = &lmdateTxtList[i].so;
  }

  for (i = 0; i < 2; i++) {
    lmdateSO[i + 4] = &lmdateBtnList[i].so;
  }

  // calculate best guess for override start time (now)
  sprintf(lmdateTxtList[0].text, "%02d/%02d/%02d", tod->tm_mon + 1, tod->tm_mday, tod->tm_year);

  wk_hour = tod->tm_hour;
  if (wk_hour >= 12) {
    am_pm = 'p';
    wk_hour -= 12;
  } else {
    am_pm = 'a';
  }
  if (wk_hour == 0) {
    wk_hour = 12;
  }

  wk_min = tod->tm_min;
  if (wk_min > 45) {
    wk_min = 45;
  } else if (wk_min > 30) {
    wk_min = 30;
  } else if (wk_min > 15) {
    wk_min = 15;
  } else {
    wk_min = 0;
  }
  sprintf(lmdateTxtList[1].text, "%02d:%02d%c", wk_hour, wk_min, am_pm);

  // calculate best guess for override end time
  //      ...(end of next load control OR if not that, midnight tonight)

  end_time_t = 0;
  for (i = 0; i < MAX_LOAD_CTRLS; ++i) {
    if (load_ctrl_tbl[i].level >= dev_list.devices[lctrl_devices[lms_index]].priority) {
      end_time_t = load_ctrl_tbl[i].start_time + load_ctrl_tbl[i].duration * 60;
      end_tm = *gmtime(&end_time_t);
      break;
    }
  }

  if (end_time_t == 0) // wasn't found in table of known upcoming load controls
  {
    end_time_t = ltime; // now
    end_tm = *gmtime(&end_time_t);
    ++end_tm.tm_mday;
    end_tm.tm_hour = 0;
    end_tm.tm_min = 0;
    end_tm.tm_sec = 0;
  }

  end_tm.tm_isdst = _daylight;
  mktime(&end_tm);
  wk_hour = end_tm.tm_hour;
  wk_min = end_tm.tm_min;

  if (wk_hour >= 12) {
    am_pm = 'p';
    wk_hour -= 12;
  } else {
    am_pm = 'a';
  }

  if (wk_hour == 0) {
    wk_hour = 12;
  }
  sprintf(lmdateTxtList[2].text, "%02d/%02d/%02d", end_tm.tm_mon + 1, end_tm.tm_mday, end_tm.tm_year);
  sprintf(lmdateTxtList[3].text, "%02d:%02d%c", wk_hour, wk_min, am_pm);

  PutWindow(glbWindow);

  time(&ltime);            // put things back
  tod = localtime(&ltime); // put things back
}

static void lmGetTimeDraw(void) {
  ptext("(minutes must be 00,15, 30 or 45)", 129, 176, BG_WHT + FG_RED);
  ptext("Override start:", 156, 176, BG_WHT + FG_BLU);
  ptext("Date", 176, 242, BG_WHT + FG_BLU);
  ptext("Time", 176, 350, BG_WHT + FG_BLU);

  ptext("Override end:", 240, 176, BG_WHT + FG_BLU);
  ptext("Date", 260, 242, BG_WHT + FG_BLU);
  ptext("Time", 260, 350, BG_WHT + FG_BLU);
}

void lmdateBtnOkClick(void) {
  char am_pm;
  char work[50];
  int i;
  int j;
  int wk_min;
  int wk_hour;
  int wk_mon;
  int wk_day;
  int wk_year;
  time_t start_time_t;
  time_t end_time_t;
  struct tm wk_tm;

  memset(&wk_tm, 0, sizeof(struct tm));

#if LOAD_CONTROL_TECO
  if (IsValid(&lmdateTxtList[0]) != TRUE) {
    msgbox("Cannot understand start date",
           "Override start date", MB_OK);
    return;
  }
  if (IsValid(&lmdateTxtList[1]) != TRUE) {
    msgbox("Cannot understand start time",
           "Override start time", MB_OK);
    return;
  }
  if (IsValid(&lmdateTxtList[2]) != TRUE) {
    msgbox("Cannot understand start date",
           "Override end date", MB_OK);
    return;
  }
  if (IsValid(&lmdateTxtList[3]) != TRUE) {
    msgbox("Cannot understand end time",
           "Override end time", MB_OK);
    return;
  }
#endif

  // validate & if OK, send override times to ICU.

  // Start with beginning date & time...
  wk_mon = wk_day = wk_year = 0;
  strcpy(work, lmdateTxtList[0].text);
  for (i = 0; i < (int)strlen(work); ++i) {
    if ((work[i] == '/') || (work[i] == '-')) {
      for (j = i + 1; j < (int)strlen(work); ++j) {
        if ((work[j] == '/') || (work[j] == '-')) {
          wk_mon = atoi(work);
          wk_day = atoi(work + i + 1);
          wk_year = atoi(work + j + 1);
          break;
        }
      }
      break;
    }
  }
  wk_hour = wk_min = 0;
  am_pm = 'a';
  strcpy(work, lmdateTxtList[1].text);
  strupr(work);
  for (i = 0; i < (int)strlen(work); ++i) {
    if (work[i] == ':') {
      wk_hour = atoi(work);
      wk_min = atoi(work + i + 1);
      switch (work[strlen(work) - 1]) {
      case 'A':
        am_pm = 'a';
        break;
      case 'P':
        am_pm = 'p';
        break;
      }
      break;
    }
  }
  if (wk_hour == 12) {
    wk_hour = 0;
  }
  if (am_pm == 'p') {
    wk_hour += 12;
  }

  wk_tm.tm_hour = wk_hour;
  wk_tm.tm_min = wk_min;
  wk_tm.tm_mon = wk_mon - 1;
  wk_tm.tm_mday = wk_day;
  wk_tm.tm_year = wk_year;
  wk_tm.tm_isdst = _daylight;
  start_time_t = mktime(&wk_tm);

  memset(&wk_tm, 0, sizeof(struct tm));

  // Continue with end date & time...
  wk_mon = wk_day = wk_year = 0;
  strcpy(work, lmdateTxtList[2].text);
  for (i = 0; i < (int)strlen(work); ++i) {
    if ((work[i] == '/') || (work[i] == '-')) {
      for (j = i + 1; j < (int)strlen(work); ++j) {
        if ((work[j] == '/') || (work[j] == '-')) {
          wk_mon = atoi(work);
          wk_day = atoi(work + i + 1);
          wk_year = atoi(work + j + 1);
          break;
        }
      }
      break;
    }
  }
  wk_hour = wk_min = 0;
  am_pm = 'a';
  strcpy(work, lmdateTxtList[3].text);
  strupr(work);
  for (i = 0; i < (int)strlen(work); ++i) {
    if (work[i] == ':') {
      wk_hour = atoi(work);
      wk_min = atoi(work + i + 1);
      switch (work[strlen(work) - 1]) {
      case 'A':
        am_pm = 'a';
        break;
      case 'P':
        am_pm = 'p';
        break;
      }
      break;
    }
  }
  if (wk_hour == 12) {
    wk_hour = 0;
  }
  if (am_pm == 'p') {
    wk_hour += 12;
  }
  wk_tm.tm_hour = wk_hour;
  wk_tm.tm_min = wk_min;
  wk_tm.tm_mon = wk_mon - 1;
  wk_tm.tm_mday = wk_day;
  wk_tm.tm_year = wk_year;
  wk_tm.tm_isdst = _daylight;
  end_time_t = mktime(&wk_tm);

  if (end_time_t < start_time_t) {
    msgbox("The override end cannot be|before the override start.",
           "Override time period error", MB_OK);
    return;
  }

  if (send_load_ctrl_override(
          dev_list.devices[lctrl_devices[lms_index]].device_slot,
          start_time_t, end_time_t) != 0) {
    msgbox("Unable to access ICU|Unable to send Override",
           "Setting Override", MB_OK);
  } else {
    msgbox("Override control sent to ICU.",
           "Setting Override Time", MB_OK);
    dev_list.devices[lctrl_devices[lms_index]].override_start = start_time_t;
    dev_list.devices[lctrl_devices[lms_index]].override_end = end_time_t;
  }
  LoadMgmtStatusMenu();
}
#endif

#endif
