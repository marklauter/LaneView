// ==========================================================================
// $Workfile:   UUMENU.C  $
// $Revision:   1.19  $
//
// HISTORY:
// Author            Date      Description
// --------------------------------------------------------------------------
//
//
//
// DESCRIPTION
// ==========================================================================
//   Copyright 1998 TeCom Inc. All rights reserved
// ==========================================================================

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#include "cebus.h"
#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "whmenu.h"
//#include "dsdmenu.h"
#include "ismenu.h"
#include "device.h"
#include "insubs.h"
#include "inutil.h"
#include "graphs.h"
#include "msgbox.h"
#include "inhist.h"
#include "tmentry.h"
#include "checkbox.h"

extern METER meter;
extern int IsSubmeter(DEVICE* dev);

extern int du_do_mon;
extern int du_do_day;
extern int du_do_year;

extern int f_month;
extern int f_year;
extern int f_day;
extern void FindFirstHistory(void);

int uuDateSelection;
int date_clicked = FALSE;
int uu_selected_month;
int uu_selected_day;
int uu_selected_year;

void GetLastLogOn(int* month, int* day, int* year);
static void TodayClick(void);
static void DateClick(void);
static void MonthClick(void);
static void LastMonthClick(void);
static void YTDClick(void);
static void ReturnClick(void);
static void TodayDetailClick(void);
static void DateDetailClick(void);
static void MonthDetailClick(void);
static void LastMonthDetailClick(void);
static void PickMonthClick(void);

void (*DU_addr)() = daily_usage;

PUSH_BUTTON_T uuBtnList[14] = {418, 50, 89, 17,
                               TRUE,         //focus
                               PUSH_BUTTON,  //type
                               TRUE,         // visible
                               BtnMouseDown, //OnMouseDown()
                               BtnMouseUp,   //OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,        //default button
                               "Today",      //text
                               set,          //type
                               down,         //state
                               NULL,         //icon
                               TodayClick,   //function
                               155,

                               438, 50, 89, 17,
                               FALSE,            //focus
                               PUSH_BUTTON,      //type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               FALSE,            //default button
                               "Detail",         //text
                               push,             //type
                               up,               //state
                               NULL,             //icon
                               TodayDetailClick, //function
                               156,

                               418, 149, 89, 17,
                               FALSE,        //focus
                               PUSH_BUTTON,  //type
                               TRUE,         // visible
                               BtnMouseDown, //OnMouseDown()
                               BtnMouseUp,   //OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,        //default button
                               "By Date",    //text
                               set,          //type
                               up,           //state
                               NULL,         //icon
                               DateClick,    //function
                               157,

                               438, 149, 89, 17,
                               FALSE,           //focus
                               PUSH_BUTTON,     //type
                               TRUE,            // visible
                               BtnMouseDown,    //OnMouseDown()
                               BtnMouseUp,      //OnMouseUp()
                               BtnClick,        // OnClick()
                               BtnKeyPress,     // OnKeyPress()
                               PutButton,       // DisplayObject()
                               FALSE,           //default button
                               "Detail",        //text
                               push,            //type
                               up,              //state
                               NULL,            //icon
                               DateDetailClick, //function
                               158,

                               418, 248, 89, 17,
                               FALSE,        //focus
                               PUSH_BUTTON,  //type
                               TRUE,         // visible
                               BtnMouseDown, //OnMouseDown()
                               BtnMouseUp,   //OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,        //default button
                               "This Month", //text
                               set,          //type
                               up,           //state
                               NULL,         //icon
                               MonthClick,   //function
                               159,

                               438, 248, 89, 17,
                               FALSE,            //focus
                               PUSH_BUTTON,      //type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               FALSE,            //default button
                               "Detail",         //text
                               push,             //type
                               up,               //state
                               NULL,             //icon
                               MonthDetailClick, //function
                               160,

                               418, 347, 89, 17,
                               FALSE,          //focus
                               PUSH_BUTTON,    //type
                               TRUE,           // visible
                               BtnMouseDown,   //OnMouseDown()
                               BtnMouseUp,     //OnMouseUp()
                               BtnClick,       // OnClick()
                               BtnKeyPress,    // OnKeyPress()
                               PutButton,      // DisplayObject()
                               FALSE,          //default button
                               "Last Month",   //text
                               set,            //type
                               up,             //state
                               NULL,           //icon
                               LastMonthClick, //function
                               161,

                               438, 347, 89, 17,
                               FALSE,                //focus
                               PUSH_BUTTON,          //type
                               TRUE,                 // visible
                               BtnMouseDown,         //OnMouseDown()
                               BtnMouseUp,           //OnMouseUp()
                               BtnClick,             // OnClick()
                               BtnKeyPress,          // OnKeyPress()
                               PutButton,            // DisplayObject()
                               FALSE,                //default button
                               "Detail",             //text
                               push,                 //type
                               up,                   //state
                               NULL,                 //icon
                               LastMonthDetailClick, //function
                               162,

                               418, 446, 89, 17,
                               FALSE,        //focus
                               PUSH_BUTTON,  //type
                               TRUE,         // visible
                               BtnMouseDown, //OnMouseDown()
                               BtnMouseUp,   //OnMouseUp()
                               BtnClick,     // OnClick()
                               BtnKeyPress,  // OnKeyPress()
                               PutButton,    // DisplayObject()
                               FALSE,        //default button
                               "This Year",  //text
                               set,          //type
                               up,           //state
                               NULL,         //icon
                               YTDClick,     //function
                               163,

                               438, 446, 89, 17,
                               FALSE,          //focus
                               PUSH_BUTTON,    //type
                               TRUE,           // visible
                               BtnMouseDown,   //OnMouseDown()
                               BtnMouseUp,     //OnMouseUp()
                               BtnClick,       // OnClick()
                               BtnKeyPress,    // OnKeyPress()
                               PutButton,      // DisplayObject()
                               FALSE,          //default button
                               "Pick Month",   //text
                               push,           //type
                               up,             //state
                               NULL,           //icon
                               PickMonthClick, //function
                               164,

                               419, 545, 89, 33, // position
                               FALSE,            //focus
                               PUSH_BUTTON,      //type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               TRUE,             //default button
                               "Main|Menu",      //text
                               push,             //type
                               up,               //state
                               icon[12],         //icon
                               ReturnClick,      //function
                               165};

static void PercentClick(void);
CHECK_BOX_T view_percent =
    {
        61, 5, 0, 0,         // position
        FALSE,               // got_focus
        CHECK_BOX,           // type
        TRUE,                // visible
        CheckBoxOnMouseDown, // OnMouseDown()
        CheckBoxOnMouseUp,   // OnMouseUp()
        CheckBoxOnClick,     // OnClick()
        CheckBoxOnKeyPress,  // OnKeyPress()
        DisplayCheckBox,     // DisplayObject()
        "View Percents",     // Text...
        FALSE,               // Checked - TRUE or FALSE
        PercentClick};

SCREEN_OBJECT_T* uuSO[15];
void UUMDraw(void);
LWINDOW_T uuWindow[1] = {
    35, 0, 640, 445, //window position
    uuSO,            //screen object
    UUMDraw,
    15,     //item count
    0,      //cursor position 0-79 left to right
    0,      //element that has current focus
    fcsBtn, //focus type
    "Utility History",
    DefaultOnKeyPress //OnKeyPress();
};

static UCHAR uuBtnTxtList[10][14];

void UtilityUsageMenu(void) {
  int i;

  FindFirstHistory();

  if (uuDateSelection == 1) {
    in_bar_graph = FALSE;
  } else {
    in_bar_graph = TRUE;
  }

  glbWindow = uuWindow;
  memset(uuBtnTxtList, 0, sizeof(char) * 10 * 14);

  for (i = 0; i < 14; i++) {
    uuSO[i] = &uuBtnList[i].so;
  }
  uuSO[14] = &view_percent.so;
  PutWindow(glbWindow);
}

void UUMDraw(void) {
  char work[80];
  int i;
  int j;

  ptext("Whole House", 92, 10, BG_WHT + FG_BLK);
  for (i = 0, j = 1, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(work, dev_list.devices[i].desc, 13);
      ptext(work, 92 + (28 * j++), 10, BG_WHT + FG_BLK);
      monitored_devices[mon_devices_used++] = i;
    }
  }

  if (DEMO == TRUE) {
    GetLastLogOn(&uu_selected_month, &uu_selected_day, &uu_selected_year);
    uuDateSelection = 0;
    in_bar_graph = FALSE;
    horizontal_graph(uuDateSelection);
    Status("DEMO DATA");
  } else {
    horizontal_graph(uuDateSelection);
  }
  if (uuDateSelection == 1) {
    sprintf(work, "Date Selected: %d/%02d/%02d",
            uu_selected_month, uu_selected_day, uu_selected_year);
    Status(work);
  }
}

void TodayClick(void) {
  if (DEMO == TRUE) {
    GetLastLogOn(&uu_selected_month, &uu_selected_day, &uu_selected_year);
    in_bar_graph = FALSE;
    Status("DEMO DATA");
  } else {
    in_bar_graph = TRUE;
  }
  uuDateSelection = 0;
  horizontal_graph(uuDateSelection);
}

#if 0 // Unused
void ParseDate(char* buf, int* month, int* day, int* year)
{
   int         i;
   int         j;

   for (i = 1; i < 8; ++i)
   {
      if (buf[i] == '/')
      {
         break;
      }
      if (buf[i] == '-')
      {
         break;
      }
   }
   buf[i++] = 0;
   *month = atoi(buf);

   for (j = i; j < 8; ++j)
   {
      if (buf[j] == '/')
      {
         break;
      }
      if (buf[j] == '-')
      {
         break;
      }
   }
   buf[j++] = 0;
   *day = atoi(buf + i);

   *year = atoi(buf + j);
}
#endif

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
        107,

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
        108};

static void TMOnChange(void);
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
        date_ent,           // type
        0, 0,
        0, 0, 0,
        0,
        0,
        TMOnChange};

static void TMOnChange(void) {
  ULONG d1;
  ULONG d2;
  struct tm* tod;
  int this_month;
  int this_day;
  int this_year;

  extern int day_tab[2][13];

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  this_day = tod->tm_mday;
  this_year = tod->tm_year;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = getdateentry.day + getdateentry.month * 100L + getdateentry.year * 10000L;

  if (d1 > d2) {
    getdateentry.day = f_day;
    getdateentry.year = f_year;
    getdateentry.month = f_month;
  }

  d1 = this_day + this_month * 100L + this_year * 10000L;

  if (d1 < d2) {
    getdateentry.day = this_day;
    getdateentry.year = this_year;
    getdateentry.month = this_month;
  }
}

static void DateOkClick(void) {
  date_clicked = TRUE;
  uu_selected_month = getdateentry.month;
  uu_selected_day = getdateentry.day;
  uu_selected_year = getdateentry.year;

  uuBtnList[uuDateSelection * 2].state = up;
  uuDateSelection = 1;
  uuBtnList[uuDateSelection * 2].state = down;
  //UtilityUsageMenu();
  if (Sender == UtilityUsageMenu) {
    (*Sender)();
  } else {
    uuDateSelection = 1;
    du_do_mon = uu_selected_month;
    du_do_day = uu_selected_day;
    du_do_year = uu_selected_year;
    Sender = UtilityUsageMenu;
    daily_usage();
  }
}

static void DateCancelClick(void) {
  if ((uuDateSelection != 1) &&
      (date_clicked != TRUE)) {
    uuBtnList[0].state = down;
    uuBtnList[2].state = up;
    uuDateSelection = 0;
  }
  UtilityUsageMenu();
}

static SCREEN_OBJECT_T* getdateso[3];
static void DTClickDraw(void);
static LWINDOW_T getdatewindow =
    {
        120, 150, 340, 220, //window position
        getdateso,          //screen object
        DTClickDraw,
        3,                    //item count
        0,                    //cursor position 0-79 left to right
        0,                    //element that has current focus
        fcsBtn,               //focus type
        "Display Days Rates", //title
        DefaultOnKeyPress     //OnKeyPress();
};

void DateClick(void) {
  time_t ltime;
  struct tm* tod;

  getdateso[0] = &getdatebutton[0].so;
  getdateso[1] = &getdatebutton[1].so;
  getdateso[2] = &getdateentry.so;

  if (date_clicked == TRUE) {
    getdateentry.month = uu_selected_month;
    getdateentry.day = uu_selected_day;
    getdateentry.year = uu_selected_year;
  } else {
    time(&ltime);
    tod = localtime(&ltime);
    getdateentry.month = tod->tm_mon + 1;
    getdateentry.day = tod->tm_mday;
    getdateentry.year = tod->tm_year;
  }
  glbWindow = &getdatewindow;
  Sender = UtilityUsageMenu;
  PutWindow(&getdatewindow);
  in_bar_graph = FALSE;
}

static void DTClickDraw(void) {
  ptext("Select a date to display.", 220 - 20, 224, BG_TRANSPARENT + FG_BLK);
}

void MonthClick(void) {
  if (DEMO == TRUE) {
    GetLastLogOn(&uu_selected_month, &uu_selected_day, &uu_selected_year);
    in_bar_graph = FALSE;
    Status("DEMO DATA");
  } else {
    in_bar_graph = TRUE;
  }
  uuDateSelection = 2;
  horizontal_graph(uuDateSelection);
}

void LastMonthClick(void) {
  if (DEMO == TRUE) {
    GetLastLogOn(&uu_selected_month, &uu_selected_day, &uu_selected_year);
    in_bar_graph = FALSE;
    Status("DEMO DATA");
  } else {
    in_bar_graph = TRUE;
  }
  uuDateSelection = 3;
  horizontal_graph(uuDateSelection);
}

void YTDClick(void) {
  uuDateSelection = 4;
  if (DEMO == TRUE) {
    GetLastLogOn(&uu_selected_month, &uu_selected_day, &uu_selected_year);
    in_bar_graph = FALSE;
    Status("DEMO DATA");
  } else {
    in_bar_graph = TRUE;
  }
  horizontal_graph(uuDateSelection);
}

void ReturnClick(void) {
  in_bar_graph = FALSE;
  IntroScreen();
}

void TodayDetailClick(void) {
  long ltime;
  struct tm* tod;

  in_bar_graph = FALSE;

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;

  du_do_mon = this_month;
  du_do_day = today;
  du_do_year = this_year;

  Sender = UtilityUsageMenu;
  uuBtnList[uuDateSelection * 2].state = up;
  uuDateSelection = 0;
  uuBtnList[uuDateSelection * 2].state = down;
  daily_usage();
}

void DateDetailClick(void) {
  time_t ltime;
  struct tm* tod;

  if (date_clicked == FALSE) {
    getdateso[0] = &getdatebutton[0].so;
    getdateso[1] = &getdatebutton[1].so;
    getdateso[2] = &getdateentry.so;

    if (date_clicked == TRUE) {
      getdateentry.month = uu_selected_month;
      getdateentry.day = uu_selected_day;
      getdateentry.year = uu_selected_year;
    } else {
      time(&ltime);
      tod = localtime(&ltime);
      getdateentry.month = tod->tm_mon + 1;
      getdateentry.day = tod->tm_mday;
      getdateentry.year = tod->tm_year;
    }
    in_bar_graph = FALSE;
    glbWindow = &getdatewindow;
    Sender = DU_addr;
    PutWindow(&getdatewindow);
  } else {
    uuBtnList[uuDateSelection * 2].state = up;
    uuDateSelection = 1;
    uuBtnList[uuDateSelection * 2].state = down;
    in_bar_graph = FALSE;
    du_do_mon = uu_selected_month;
    du_do_day = uu_selected_day;
    du_do_year = uu_selected_year;
    Sender = UtilityUsageMenu;
    daily_usage();
  }
}

extern int mc_do_mon;
extern int mc_do_day;
extern int mc_do_year;

void MonthDetailClick(void) {
  long ltime;
  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;
  mc_do_mon = this_month;
  mc_do_day = today;
  mc_do_year = this_year;

  in_bar_graph = FALSE;
  Sender = UtilityUsageMenu;
  if (uuDateSelection == 1) {
    uuBtnList[uuDateSelection * 2].state = up;
    uuDateSelection = 2;
    uuBtnList[uuDateSelection * 2].state = down;
  }
  MonthlyUsage();
}

void LastMonthDetailClick(void) {
  long ltime;
  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;
  if (this_month > 1) {
    mc_do_mon = this_month - 1;
    mc_do_year = this_year;
  } else {
    mc_do_mon = 12;
    mc_do_year = this_year - 1;
  }
  mc_do_day = today;

  in_bar_graph = FALSE;
  Sender = UtilityUsageMenu;
  if (uuDateSelection == 1) {
    uuBtnList[uuDateSelection * 2].state = up;
    uuDateSelection = 3; // "Last Month"
    uuBtnList[uuDateSelection * 2].state = down;
  }
  MonthlyUsage();
}

void GetLastLogOn(int* month, int* day, int* year) {
  FILE* f;

  f = fopen("lastlog.on", "rb");
  if (f == NULL) {
    time(&ltime);
    tod = localtime(&ltime);
    *month = tod->tm_mon + 1;
    *day = tod->tm_mday;
    *year = tod->tm_year;
  } else {
    fread(month, 1, sizeof(int), f);
    fread(day, 1, sizeof(int), f);
    fread(year, 1, sizeof(int), f);
    fclose(f);
  }
}

static void PercentClick(void) {
  UtilityUsageMenu();
}

//==========================================================================
// Month-Year entry routines.
//
// Routine History:
// Author               Date            Description
// GES                  5/20/1998
//
//
//
// Description:
//   These routines collectively allow for entry of Month/Year type dates.
//   this allows selection of a month to display by the operator.
//==========================================================================

static void MoYrOkClick(void);
static void MoYrCancelClick(void);

static SCREEN_OBJECT_T* getMoYrso[3];

static PUSH_BUTTON_T getMoYrbutton[2] =
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
        MoYrOkClick,
        107,

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
        MoYrCancelClick,
        108};

static void MoYrOnChange(void);

static TIME_ENTRY_T getMoYrentry =
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
        mo_yr_ent,          // type
        0, 0,
        0, 0, 0,
        0,
        0,
        MoYrOnChange};

static void MoYrOnChange(void) {
  ULONG d1;
  ULONG d2;
  struct tm* tod;
  int this_month;
  int this_day;
  int this_year;

  extern int day_tab[2][13];

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  this_day = tod->tm_mday;
  this_year = tod->tm_year;

  d1 = f_day + f_month * 100L + f_year * 10000L;

  d2 = 1 + getMoYrentry.month * 100L + getMoYrentry.year * 10000L;

  if (d1 > d2) {
    getMoYrentry.day = f_day;
    getMoYrentry.year = f_year;
    getMoYrentry.month = f_month;
  }

  d1 = this_day + this_month * 100L + this_year * 10000L;

  if (d1 < d2) {
    getMoYrentry.day = this_day;
    getMoYrentry.year = this_year;
    getMoYrentry.month = this_month;
  }
}

static void MoYrOkClick(void) {
  uu_selected_month = getMoYrentry.month;
  uu_selected_day = 1;
  uu_selected_year = getMoYrentry.year;

  // uuDateSelection = 1;
  uuDateSelection = 0;
  du_do_mon = uu_selected_month;
  du_do_day = uu_selected_day;
  du_do_year = uu_selected_year;
  Sender = UtilityUsageMenu;
  mc_do_mon = getMoYrentry.month;
  mc_do_day = 1;
  mc_do_year = getMoYrentry.year;
  MonthlyUsage();
}

static void MoYrCancelClick(void) {
  if ((uuDateSelection != 1) &&
      (date_clicked != TRUE)) {
    uuBtnList[0].state = down;
    uuBtnList[2].state = up;
    uuDateSelection = 0;
  }
  UtilityUsageMenu();
}

static void MoYrClickDraw(void);

static LWINDOW_T getMoYrwindow =
    {
        120, 150, 340, 220, //window position
        getMoYrso,          //screen object
        MoYrClickDraw,
        3,                         //item count
        0,                         //cursor position 0-79 left to right
        0,                         //element that has current focus
        fcsBtn,                    //focus type
        "Select Month to Display", //title
        DefaultOnKeyPress          //OnKeyPress();
};

static void MoYrClickDraw(void) {
  ptext("Select a Month to display.", 220 - 20, 224, BG_TRANSPARENT + FG_BLK);
}

void PickMonthClick(void) {
  long ltime;
  struct tm* tod;

  //////////////
  getMoYrso[0] = &getMoYrbutton[0].so;
  getMoYrso[1] = &getMoYrbutton[1].so;
  getMoYrso[2] = &getMoYrentry.so;

  time(&ltime);
  tod = localtime(&ltime);
  getMoYrentry.month = tod->tm_mon + 1;
  getMoYrentry.day = 1;
  getMoYrentry.year = tod->tm_year;

  in_bar_graph = FALSE;
  glbWindow = &getMoYrwindow;
  //Sender = _addr;
  uuBtnList[uuDateSelection * 2].state = up;
  uuDateSelection = 0;
  uuBtnList[uuDateSelection * 2].state = down;
  PutWindow(&getMoYrwindow);
}
