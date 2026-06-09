#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>

#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "calendar.h"
#include "frame_3d.h"
#include "inio.h"
#include "ismenu.h"

extern struct tm* tod;

static int mon = 0, year = 0;

char* mname[13] = {
    "",
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"};

char* wkdayname3[7] =
    {
        "Sun",
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat"

};

USHORT days_tab[2][13] =
    {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

int day_tab[2][13] =
    {
        {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

int day_of_week(int month, int day, int year) {
  int i, leap;

  day += 1; /* jan 1, 1980 was a Tuesday */
  for (i = 80; i < year; ++i) {
    if ((i % 4) == 0) /* closest year == 0 is 2000, not a leap year) */
    {
      day += 366;
    } else {
      day += 365;
    }
  }
  if ((year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }
  for (i = 1; i < month; ++i) {
    day += day_tab[leap][i];
  }
  return (day % 7);
}

int calendar_direction;

static void display_calendar(void) {
  int i, leap, top, left;
  int weekday, week, last_day;
  UCHAR buffer[50];

  switch (calendar_direction) {
  case CAL_NOW:
    mon = tod->tm_mon + 1; //this month
    year = tod->tm_year;
    break;
  case CAL_NEXT:
    ++mon; //next month (next is relative to currently displayed month)
    if (mon > 12) {
      mon = 1;
      ++year;
    }
    break;
  case CAL_BACK:
    --mon; //prior month (prior is relative to currently displayed month)
    if (mon < 1) {
      mon = 12;
      --year;
    }
    break;
  }

  hide_mouse();
  area_clear(82, 388, 31, 583, BG_WHT + FG_WHT);
  sprintf(buffer, "Calendar:  %s, %4d      ", mname[mon], 1900 + year);
  ShowTitle(buffer);

  weekday = day_of_week(mon, 1, year);
  if ((year % 4) == 0) {
    leap = 1;
  } else {
    leap = 0;
  }
  last_day = day_tab[leap][mon];

  for (i = 0; i < 7; ++i) {
    position(6, 8 + i * 9);
    ptext(wkdayname3[i], 80, 56 + i * 72, BG_WHT + FG_BLK);
  }

  for (i = 1, week = 0; i <= last_day; ++i) {
    top = 8 + 3 * week;
    left = 5 + 9 * weekday;
    frame_3d((top - 1) * 14,
             (left - 1) * 8,
             39, //height
             69, //width
             FT_PRESSED);
    //position(top+1,left+3);
    sprintf(buffer, "%2d", i);

    if ((mon == (tod->tm_mon + 1)) && (i == tod->tm_mday) && (year == tod->tm_year)) {
      area_clear((top - 1) * 14 + 1, (top - 1) * 14 + 38, (left - 1) * 8 + 1, (left - 1) * 8 + 68, FG_LBLU);
      ptext(buffer, (top * 14) - 14, (left * 8) - 4, BG_TRANSPARENT + FG_HWHT);
    } else {
      ptext(buffer, (top * 14) - 14, (left * 8) - 4, BG_TRANSPARENT + FG_BLK);
    }

    if ((++weekday) > 6) {
      ++week;
      weekday = 0;
    }
  }

  if ((mon == (tod->tm_mon + 1)) && (year == tod->tm_year)) {
    if (dawn_dusk_time.dawn_hour == 0) {
      read_dawn_dusk_info((UCHAR)year, (UCHAR)mon, (UCHAR)tod->tm_mday);
    }

    if (dawn_dusk_time.dawn_hour != 0) // if read didn't fail
    {
      sprintf(buffer, "Dawn Today: %2d:%02d am", dawn_dusk_time.dawn_hour,
              dawn_dusk_time.dawn_min);
      ptext(buffer, 360, 240, BG_WHT + FG_BLK);
      sprintf(buffer, "Dusk Today: %2d:%02d pm", dawn_dusk_time.dusk_hour - 12,
              dawn_dusk_time.dusk_min);
      ptext(buffer, 374, 240, BG_WHT + FG_BLK);
    }
  }
  unhide_mouse();
}

void BackClick(void) {
  calendar_direction = CAL_BACK;
  display_calendar();
}

void NextClick(void) {
  calendar_direction = CAL_NEXT;
  display_calendar();
}

void calOnKeyPress(SCREEN_OBJECT_T* so, int c) {
  //if not pageup or page down
  //call focused object's keypress event
  DefaultOnKeyPress(so, c);
}

// function_not_supported
PUSH_BUTTON_T calBtnList[3] = {419, 347, 89, 33, // position
                               FALSE,            //screen object focus
                               PUSH_BUTTON,      //screen object type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               FALSE,            //default button
                               "\x9E Back \x9E", //button text
                               push,             //type
                               up,               //state
                               NULL,             //icon
                               BackClick,        //function
                               143,

                               419, 446, 89, 33, // position
                               FALSE,            //screen object focus
                               PUSH_BUTTON,      //type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               FALSE,            //default button
                               "\x9F Next \x9F", //text
                               push,             //type
                               up,               //state
                               NULL,             //icon
                               NextClick,        //function
                               144,

                               419, 545, 89, 33, // position
                               FALSE,            //screen object focus
                               PUSH_BUTTON,      //type
                               TRUE,             // visible
                               BtnMouseDown,     //OnMouseDown()
                               BtnMouseUp,       //OnMouseUp()
                               BtnClick,         // OnClick()
                               BtnKeyPress,      // OnKeyPress()
                               PutButton,        // DisplayObject()
                               TRUE,             //default button
                               "Return",         //text
                               push,             //type
                               up,               //state
                               icon[12],         //icon
                               AdvancedFeatures, //function
                               145};

SCREEN_OBJECT_T* calSO[3];
LWINDOW_T calWindow[1] = {
    35, 0, 640, 445, //window position
    calSO,
    display_calendar,
    3,            //item_count
    0,            //cursor position 0-79 left to right
    0,            //element that has current focus
    fcsBtn,       //focus type
    NULL,         //title, set & displayed by display_calendar()
    calOnKeyPress //OnKeyPress();
};

void calendar(void) {
  int i = 0;
  glbWindow = calWindow;
  for (i = 0; i < 3; i++) {
    calSO[i] = &calBtnList[i].so;
  }
  calendar_direction = CAL_NOW;
  PutWindow(glbWindow);
  //display_calendar();
}
