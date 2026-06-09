// ==========================================================================
// $Workfile:   emmenu.c  $
// $Revision:   1.11  $
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
#include "device.h"

void emonLast30BtnClk(void);
void emonCurHourBtnClk(void);
void SevenMinBtnClk(void);
void emonWHBtnClick(void);
void emonDeviceBtnClick(void);
void emon_horizontal_graph(void);
void emon7MinuteGraph(void);
void emonExitBtnClk(void);

int in_energy_monitor;
int in_7_min_monitor;
int emon_device_idx;

static int emon_selection;

PUSH_BUTTON_T emonBtnList[15] = {
    419, 248, 89, 33,
    FALSE,             //focus
    PUSH_BUTTON,       //type
    TRUE,              // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "Last 30|Seconds", //text
    push,              //type
    up,                //state
    NULL,              //icon
    emonLast30BtnClk,  //function
    96,

    419, 347, 89, 33,
    FALSE,             //focus
    PUSH_BUTTON,       //type
    TRUE,              // visible
    BtnMouseDown,      //OnMouseDown()
    BtnMouseUp,        //OnMouseUp()
    BtnClick,          // OnClick()
    BtnKeyPress,       // OnKeyPress()
    PutButton,         // DisplayObject()
    FALSE,             //default button
    "Current|Hour",    //text
    push,              //type
    up,                //state
    NULL,              //icon
    emonCurHourBtnClk, //function
    97,

    419, 446, 89, 33,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    TRUE,               // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "7 Minute|History", //text
    push,               //type
    up,                 //state
    NULL,               //icon
    SevenMinBtnClk,     //function
    98,

    419, 545, 89, 33,
    FALSE,          //focus
    PUSH_BUTTON,    //type
    TRUE,           // visible
    BtnMouseDown,   //OnMouseDown()
    BtnMouseUp,     //OnMouseUp()
    BtnClick,       // OnClick()
    BtnKeyPress,    // OnKeyPress()
    PutButton,      // DisplayObject()
    TRUE,           //default button
    "Main|Menu",    //text
    push,           //type
    up,             //state
    icon[12],       //icon
    emonExitBtnClk, //function
    99,

    84, 5, 109, 26,
    FALSE,          //focus
    PUSH_BUTTON,    //type
    TRUE,           // visible
    BtnMouseDown,   //OnMouseDown()
    BtnMouseUp,     //OnMouseUp()
    BtnClick,       // OnClick()
    BtnKeyPress,    // OnKeyPress()
    PutButton,      // DisplayObject()
    TRUE,           //default button
    "Whole House",  //text
    set,            //type
    down,           //state
    NULL,           //icon
    emonWHBtnClick, //function
    100,

    112, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    101,

    140, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    102,

    168, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    103,

    196, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    104,

    224, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    105,

    252, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    106,

    280, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    107,

    308, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    108,

    336, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    109,

    364, 5, 109, 26,
    FALSE,              //focus
    PUSH_BUTTON,        //type
    FALSE,              // visible
    BtnMouseDown,       //OnMouseDown()
    BtnMouseUp,         //OnMouseUp()
    BtnClick,           // OnClick()
    BtnKeyPress,        // OnKeyPress()
    PutButton,          // DisplayObject()
    FALSE,              //default button
    "blank",            //text
    set,                //type
    up,                 //state
    NULL,               //icon
    emonDeviceBtnClick, //function
    110};

SCREEN_OBJECT_T* emonSO[15];
static void emonWindowDraw(void);
LWINDOW_T emonWindow[1] = {
    35, 0, 640, 445, //window position
    emonSO,          //screen object
    emonWindowDraw,
    15,     //item count
    0,      //cursor position 0-79 left to right
    0,      //element that has current focus
    fcsBtn, //focus type
    "Energy Monitor",
    DefaultOnKeyPress //OnKeyPress();
};

static UCHAR emonBtnTxtList[10][14];

void EnergyMonitorMenu(void) {
  int i;
  int j;

  in_energy_monitor = TRUE;
  in_7_min_monitor = FALSE;

  memset(emonBtnTxtList, 0, sizeof(char) * 10 * 14);

  for (i = 0; i < emonWindow[0].item_count; i++) {
    emonSO[i] = &emonBtnList[i].so;
  }
  for (i = 5; i < 15; i++) {
    emonBtnList[i].so.visible = FALSE;
  }

  for (i = 0, j = 0, mon_devices_used = 0; i < dev_list.devices_used; i++) {
    if (IsSubmeter(&dev_list.devices[i]) == TRUE) {
      movez(emonBtnTxtList[j], dev_list.devices[i].desc, 13);
      trim(emonBtnTxtList[j]);
      emonBtnList[j + 5].text = emonBtnTxtList[j];
      emonBtnList[j + 5].so.visible = TRUE;
      j++;
      monitored_devices[mon_devices_used++] = i;
    }
  }

  for (i = 4; i < 15; i++) {
    if (emonBtnList[i].state == down) {
      if (i != 4) {
        emon_device_idx = monitored_devices[i - 5];
        break;
      } else {
        emon_device_idx = 0x0FFFF;
        break;
      }
    }
  }

  glbWindow = emonWindow;
  PutWindow(glbWindow);
}

static void emonWindowDraw(void) {
  emon_horizontal_graph();
  switch (emon_selection) {
  case 0: //30 seconds
    Status("Viewing last 30 seconds");
    break;
  case 1: //hour
    Status("Viewing current hour");
    break;
  }
}

void emonLast30BtnClk(void) {
  Status("Viewing last 30 seconds");
  emon_selection = 0;
  emon_horizontal_graph();
  in_energy_monitor = TRUE;
  in_7_min_monitor = FALSE;
}

void emonCurHourBtnClk(void) {
  Status("Viewing current hour");
  emon_selection = 1;
  emon_horizontal_graph();
  in_energy_monitor = TRUE;
  in_7_min_monitor = FALSE;
}

void SevenMinBtnClk(void) {
  in_energy_monitor = FALSE;
  in_7_min_monitor = TRUE;
  emon7MinuteGraph();
}

void emonExitBtnClk(void) {
  emon_selection = 0;
  in_energy_monitor = FALSE;
  in_7_min_monitor = FALSE;
  IntroScreen();
}

void emonWHBtnClick(void) {
  emon_device_idx = 0x0FFFF;
  //in_energy_monitor = FALSE;
  //in_7_min_monitor = TRUE;
  //emon7MinuteGraph();
}

void emonDeviceBtnClick(void) {
  emon_device_idx = monitored_devices[glbWindow->focus.current - 5];
  //in_energy_monitor = FALSE;
  //in_7_min_monitor = TRUE;
  //emon7MinuteGraph();
}

void emon_horizontal_graph(void) {
  int i;
  int history_index; //history bucket for monitored device
  double total_kw;
  double device_kw;
  double total$;
  double device$;

  area_clear(75, 398, 248, 637, FG_WHT);

  switch (emon_selection) {
  case 0: // Last 30 Seconds
    ptext("kW ", 70, 152, BG_WHT + FG_LGRN);
    ptext("$/Hr", 70, 206, BG_WHT + FG_YEL);
    total_kw = monitor_kwh[0];
    total$ = (double)monitor_cents[0] / 100.0;
    break;
  case 1: // Current Hour
    ptext("kW", 70, 152, BG_WHT + FG_LGRN);
    ptext("$/Hr", 70, 206, BG_WHT + FG_YEL);
    total_kw = (double)meter.current_day[meter_hour] / 10000.0;
    total$ = (double)meter.current_day$[meter_hour] / 100000.0;
    break;
  }

  horiz_house_graph(0, total_kw, total$, total_kw, total$);

  for (i = 0; i < mon_devices_used; ++i) //mon_devices_used
  {
    history_index = dev_list.devices[monitored_devices[i]].history_bucket - 1;

    switch (emon_selection) {
    case 0: // Last 30 Seconds
      device_kw = dmonitor_kwh[history_index][0];
      device$ = (double)dmonitor_cents[history_index][0] / 100.0;
      break;
    case 1: // Current Hour
      device_kw = (double)device_history[history_index].current_day[meter_hour] / 10000.0;
      device$ = (double)device_history[history_index].current_day$[meter_hour] / 100000.0;
      break;
    }
    horiz_house_graph(28 * (i + 1), device_kw, device$, total_kw, total$);
  }
}

//
//
//
//
//  7 Minute Monitor section
//
//
//
//

PUSH_BUTTON_T min7BtnList[1] = {400, 535, 100, 54,
                                TRUE,                  //focus
                                PUSH_BUTTON,           //type
                                TRUE,                  // visible
                                BtnMouseDown,          //OnMouseDown()
                                BtnMouseUp,            //OnMouseUp()
                                BtnClick,              // OnClick()
                                BtnKeyPress,           // OnKeyPress()
                                PutButton,             // DisplayObject()
                                TRUE,                  //default button
                                "Energy|Monitor|Menu", //text
                                push,                  //type
                                up,                    //state
                                icon[12],              //icon
                                EnergyMonitorMenu,     //function
                                111};

SCREEN_OBJECT_T* min7SO[1];
static void min7WindowDraw(void);
LWINDOW_T min7Window[1] =
    {
        35, 0, 640, 445, //window position
        min7SO,          //screen object
        min7WindowDraw,
        1,      //item count
        0,      //cursor position 0-79 left to right
        0,      //element that has current focus
        fcsBtn, //focus type
        "7 Minute Monitor",
        DefaultOnKeyPress //OnKeyPress();
};

void emon7MinuteGraph(void) {
  int i;

  glbWindow = min7Window;
  for (i = 0; i < glbWindow->item_count; i++) {
    min7SO[i] = &min7BtnList[i].so;
  }
  PutWindow(glbWindow);
}

static void min7WindowDraw(void) {
  char work[100];
  int i;
  int row;
  int col;
  int ecol;
  double avg;
  int hist_idx;

  if (emon_device_idx == -1) // it's the house's meter
  {
    position(7, 7);
    text("-- Whole House Power Consumption in 30 Second Intervals (kw) --", BG_WHT + FG_BLU);

    for (i = monitor_count - 1; i >= 0; --i) {
      scroll_window_down(8, 2, 21, 79);
      area_clear(7 * 14, 7 * 14 + 13, 8, 79 * 8, FG_WHT);
      avg = monitor_kwh[i];
      sprintf(work, "%7.1f", avg);
      position(8, 2);
      text(work + 2, BG_WHT + FG_BLU);
      row = 7 * 14 + 3;
      col = 6 * 8 + 4;
      ecol = (int)(col + avg * 35 + 0.5); // times extra 7 to make bar longer
      if (ecol > 631) {
        ecol = 631; // sanity check //
      }
      if (ecol < col) {
        ecol = 631; // sanity check //
      }
      hline(row + 0, col, ecol, FG_YEL);
      hline(row + 1, col, ecol, FG_YEL);
      hline(row + 2, col, ecol, FG_YEL);
      hline(row + 3, col, ecol, FG_YEL);
      hline(row + 4, col, ecol, FG_YEL);
      hline(row + 5, col, ecol, FG_YEL);
    }
  } else // it's another device
  {
    position(7, 7);

    sprintf(work,
            "-- %.13s Inst. Power Consumption in 30 Second Intervals (kw) --",
            dev_list.devices[emon_device_idx].desc);

    text(work, BG_WHT + FG_BLU);
    hist_idx = dev_list.devices[emon_device_idx].history_bucket - 1;

    for (i = dmonitor_count[hist_idx] - 1; i >= 0; --i) {
      scroll_window_down(7, 2, 21, 79);
      area_clear(7 * 14, 7 * 14 + 13, 8, 79 * 8, FG_WHT);
      avg = dmonitor_kwh[hist_idx][i];
      sprintf(work, "%7.1f", avg);
      position(8, 2);
      text(work + 2, BG_WHT + FG_BLU);
      row = 7 * 14 + 3;
      col = 6 * 8 + 4;
      ecol = (int)(col + avg * 35 + 0.5); // times an extra 7 to make bar longer
      if (ecol > 631) {
        ecol = 631; // sanity check //
      }
      if (ecol < col) {
        ecol = 631; // sanity check //
      }
      hline(row + 0, col, ecol, FG_YEL);
      hline(row + 1, col, ecol, FG_YEL);
      hline(row + 2, col, ecol, FG_YEL);
      hline(row + 3, col, ecol, FG_YEL);
      hline(row + 4, col, ecol, FG_YEL);
      hline(row + 5, col, ecol, FG_YEL);
    }
  }
}
