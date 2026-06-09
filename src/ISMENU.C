#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

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
#include "uumenu.h"
#include "copyrite.h"
#include "insubs.h"
#include "calendar.h"
#include "fnsmsg.h"
#include "moodmenu.h"
#include "ctrlschd.h"
#include "dvc_evnt.h"
#include "scrtypnl.h"
#include "inutil.h"
#include "graphs.h"
#include "trigger.h"
#include "emmenu.h"
#include "billing.h"
#include "frame_3d.h"
#include "msgbox.h"
#include "ismenu.h"
#include "event.h"
#include "viewmail.h"

////////////////////////////////
#include "scrolbar.h"
////////////////////////////////

#define MAIN_MENU_BUTTONS 17

extern int DEBUG;
extern int LVLITE;
extern int glb_cat_index;
extern int kb_stat(void);
extern int read_devices(int days_back);
extern int SendTable(int table_no, void* request_data, int data_length);

static void ViewRateClick(void);
static void CostEffectiveClick(void);
static void TodaysEnergyUsage(void);
static void ExitLaneView(void);
static void AboutClick(void);
static void LogInNow(void);
static void ScheduleSetup(void);
static void MainViewMailClick(void);
static void AdvancedViewMailClick(void);

int NEW_MAIL;

static void TriggerSetupClick(void);
static void DeviceStatusNControls(void) {
  Device_Event_Menu(glb_cat_index); // 0 = all
}

//Intro Screen
PUSH_BUTTON_T isBtnList[MAIN_MENU_BUTTONS] =
    {
        100, 67, 115, 76,            // position
        FALSE,                       // focus
        PUSH_BUTTON,                 // type
        TRUE,                        // visible
        BtnMouseDown,                // OnMouseDown()
        BtnMouseUp,                  // OnMouseUp()
        BtnClick,                    // OnClick()
        BtnKeyPress,                 // OnKeyPress()
        PutButton,                   // DisplayObject()
        FALSE,                       //default button
        "Utility|History &|Graphs ", //"Utility|Usage",   //text
        push,                        //type
        up,                          //state
        icon[0],                     //icon
        UtilityUsageMenu,            //function
        1,

        196, 67, 115, 76, // position
        FALSE,            //focus
        PUSH_BUTTON,      //type
        TRUE,             // visible
        BtnMouseDown,     //OnMouseDown()
        BtnMouseUp,       //OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            //default button
        "Today's|Energy|Graphs",
        push,              //type
        up,                //state
        icon[2],           //icon
        TodaysEnergyUsage, //function void daily_usage(USHORT device_id, int sender)
        2,

        292, 67, 115, 76,             // position
        FALSE,                        //focus
        PUSH_BUTTON,                  //type
        TRUE,                         // visible
        BtnMouseDown,                 //OnMouseDown()
        BtnMouseUp,                   //OnMouseUp()
        BtnClick,                     // OnClick()
        BtnKeyPress,                  // OnKeyPress()
        PutButton,                    // DisplayObject()
        FALSE,                        //default button
        "Monthly Cost|Effectiveness", //"Rate|Comparison",   //text
        push,                         //type
        up,                           //state
        icon[11],                     //icon
        CostEffectiveClick,           //function
        3,

        100, 197, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Energy|Monitor",  //"Master Controls \& Schedules",   //text
        push,              //type
        up,                //state
        icon[9],           //icon
        EnergyMonitorMenu, //function
        4,

        196, 197, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Estimated|Bill",  //text
        push,              //type
        up,                //state
        icon[4],           //icon
        BillingMenu,       //function
        5,

        292, 197, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Remote|Control",  //text
        push,              //type
        up,                //state
        icon[1],           //icon
        TriggerSetupClick,
        6,

        100, 327, 115, 76,         // position
        FALSE,                     //focus
        PUSH_BUTTON,               //type
        TRUE,                      // visible
        BtnMouseDown,              //OnMouseDown()
        BtnMouseUp,                //OnMouseUp()
        BtnClick,                  // OnClick()
        BtnKeyPress,               // OnKeyPress()
        PutButton,                 // DisplayObject()
        FALSE,                     //default button
        "Device|Status &|Control", //"Controls|& Schedules",   //text
        push,                      //type
        up,                        //state
        icon[6],                   //icon
        DeviceStatusNControls,     //monitor_device_menu, //function
        7,

        196, 327, 115, 76,       // position
        FALSE,                   //focus
        PUSH_BUTTON,             //type
        TRUE,                    // visible
        BtnMouseDown,            //OnMouseDown()
        BtnMouseUp,              //OnMouseUp()
        BtnClick,                // OnClick()
        BtnKeyPress,             // OnKeyPress()
        PutButton,               // DisplayObject()
        FALSE,                   //default button
        "Moods|Setup &|Control", //text
        push,                    //type
        up,                      //state
        icon[3],                 //icon
        MoodMenu,                //function
        8,

        292, 327, 115, 76,        // position
        FALSE,                    //focus
        PUSH_BUTTON,              //type
        TRUE,                     // visible
        BtnMouseDown,             //OnMouseDown()
        BtnMouseUp,               //OnMouseUp()
        BtnClick,                 // OnClick()
        BtnKeyPress,              // OnKeyPress()
        PutButton,                // DisplayObject()
        FALSE,                    //default button
        "System|Setup &|Control", //text
        push,                     //type
        up,                       //state
        icon[7],                  //icon
        MasterControls,           //function
        9,

        100, 457, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Schedule|Events",
        push,          //type
        up,            //state
        icon[8],       //icon
        ScheduleSetup, //function
        10,

        196, 457, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Security|System", //text
        push,              //type
        up,                //state
        icon[5],           //icon
        SecurityPanel,     //function
        11,

        292, 457, 115, 76, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        FALSE,             //default button
        "Advanced",        //text
        push,              //type
        up,                //state
        icon[10],          //icon
        AdvancedFeatures,  //function
        12,

        400, 540, 90, 50, // position
        FALSE,            //focus
        PUSH_BUTTON,      //type
        TRUE,             // visible
        BtnMouseDown,     //OnMouseDown()
        BtnMouseUp,       //OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        FALSE,            //default button
        "Exit",           //text
        push,             //type
        up,               //state
        icon[12],         //icon
        ExitLaneView,     //function
        9999,

        400, 540 - 99, 90, 50, // position
        FALSE,                 //focus
        PUSH_BUTTON,           //type
        TRUE,                  // visible
        BtnMouseDown,          //OnMouseDown()
        BtnMouseUp,            //OnMouseUp()
        BtnClick,              // OnClick()
        BtnKeyPress,           // OnKeyPress()
        PutButton,             // DisplayObject()
        FALSE,                 //default button
        "About",               //text
        push,                  //type
        up,                    //state
        icon[13],              //icon
        AboutClick,            //function
        13,

        419, 10, 150, 33, // position
        FALSE,            // focus
        PUSH_BUTTON,      // type
        FALSE,            // visible
        BtnMouseDown,     // OnMouseDown()
        BtnMouseUp,       // OnMouseUp()
        BtnClick,         // OnClick()
        BtnKeyPress,      // OnKeyPress()
        PutButton,        // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,            //default button
        "Update History", //text
        push,             //type
        up,               //state
        NULL,             //icon
        LogInNow,         //function
        14,

        419, 120, 100, 33, // position
        FALSE,             // focus
        PUSH_BUTTON,       // type
        FALSE,             // visible
        BtnMouseDown,      // OnMouseDown()
        BtnMouseUp,        // OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,  //default button
        "help", //text
        push,   //type
        up,     //state
        NULL,   //icon
        NULL,   //function
        15,

        72, 67, 505, 20, // position
        FALSE,           // focus
        PUSH_BUTTON,     // type
        FALSE,           // visible
        BtnMouseDown,    // OnMouseDown()
        BtnMouseUp,      // OnMouseUp()
        BtnClick,        // OnClick()
        BtnKeyPress,     // OnKeyPress()
        PutButton,       // DisplayObject()
        // END SCREEN OBJECT DEFINITION
        FALSE,                               // default button
        "Click here to view your new mail.", // text
        push,                                // type
        up,                                  // state
        NULL,                                // icon
        MainViewMailClick,                   // function
        600};
static void MainViewMailClick(void) {
  Sender = IntroScreen;
  ViewMail();
}
SCREEN_OBJECT_T* isSO[MAIN_MENU_BUTTONS];

LWINDOW_T IntroWindow[1] = {
    35, 0, 640, 445, // window position
    isSO,            // screen object
    NULL,
    MAIN_MENU_BUTTONS, // item count
    0,                 // cursor position 0-79 left to right
    0,                 // element that has current focus
    fcsBtn,            // focus type
    "InterLane^ Home Manager Main Menu",
    DefaultOnKeyPress //OnKeyPress();
};

void IntroScreen(void) {
  int i = 0, j = 0;
  int x = 0, y = 0;

  glbWindow = IntroWindow;

  in_bar_graph = FALSE; // not in bar graph screen
  in_monitor = FALSE;   // not in monitor screen

  DrawTop();

  Busy();

  MainLoopOpt = 1;

  for (i = 0; i < MAIN_MENU_BUTTONS; ++i) {
    isSO[i] = &isBtnList[i].so;
  }

  if (LVLITE == TRUE) {
    isBtnList[14].so.visible = TRUE;
  } else {
    isBtnList[14].so.visible = FALSE;
  }
  if (NEW_MAIL == TRUE) {
    isBtnList[16].so.visible = TRUE;
  } else {
    isBtnList[16].so.visible = FALSE;
  }

  PutWindow(glbWindow);
  //   GroupBox("Utility Applications", 84, 8, 265, 300);
  //   GroupBox("Home Automation & Advanced Features", 84, 332, 265, 300);
  if (secured_user) {
    Status("SECURED USER ACCESS ACTIVE");
  }
  NotBusy();
} //end of intro screen

//===========================================================================================
// NAME:   AdvancedFeatures()
//
// HISTORY:
// Author         Date         Description
// M. Lauter      27 Jan 97      Initial Version
//
//
//
// INPUTS:
// Name            Type         Description
// N/A            N/A            N/A
//
// OUTPUTS:
// Name            Type         Description
// N/A            N/A            N/A
//
// DESCRIPTION:
//
// AdvancedFeatures() displays a small menu window
// containing features that are not implemented
// yet.  It is only called from the Advanced button
// located on the Main Menu
//
//===========================================================================================
PUSH_BUTTON_T advanced_buttons[6] =
    {
        80, 220, 200, 40,      // position
        FALSE,                 //focus
        PUSH_BUTTON,           //type
        TRUE,                  // visible
        BtnMouseDown,          //OnMouseDown()
        BtnMouseUp,            //OnMouseUp()
        BtnClick,              // OnClick()
        BtnKeyPress,           // OnKeyPress()
        PutButton,             // DisplayObject()
        TRUE,                  //default button
        "  E-Mail",            //text
        push,                  //type
        up,                    //state
        icon[15],              //icon
        AdvancedViewMailClick, //function
        20,

        140, 220, 200, 40, // position
        FALSE,             //focus
        PUSH_BUTTON,       //type
        TRUE,              // visible
        BtnMouseDown,      //OnMouseDown()
        BtnMouseUp,        //OnMouseUp()
        BtnClick,          // OnClick()
        BtnKeyPress,       // OnKeyPress()
        PutButton,         // DisplayObject()
        TRUE,              //default button
        "  Calendar",      //text
        push,              //type
        up,                //state
        icon[16],          //icon
        calendar,          //function
        16,

        200, 220, 200, 40,      // position
        FALSE,                  //focus
        PUSH_BUTTON,            //type
        TRUE,                   // visible
        BtnMouseDown,           //OnMouseDown()
        BtnMouseUp,             //OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  //default button
        "News",                 //text
        push,                   //type
        up,                     //state
        NULL,                   //icon
        function_not_supported, //function
        17,

        260, 220, 200, 40,      // position
        FALSE,                  //focus
        PUSH_BUTTON,            //type
        TRUE,                   // visible
        BtnMouseDown,           //OnMouseDown()
        BtnMouseUp,             //OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  //default button
        "Weather",              //text
        push,                   //type
        up,                     //state
        NULL,                   //icon
        function_not_supported, //function
        18,

        320, 220, 200, 40,      // position
        FALSE,                  //focus
        PUSH_BUTTON,            //type
        TRUE,                   // visible
        BtnMouseDown,           //OnMouseDown()
        BtnMouseUp,             //OnMouseUp()
        BtnClick,               // OnClick()
        BtnKeyPress,            // OnKeyPress()
        PutButton,              // DisplayObject()
        FALSE,                  //default button
        "Video",                //text
        push,                   //type
        up,                     //state
        NULL,                   //icon
        function_not_supported, //function
        19,

        419, 540, 89, 33, // position
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
        IntroScreen,      //function
        21};
SCREEN_OBJECT_T* advanced_so[6];

LWINDOW_T advanced_window[1] =
    {
        35, 0, 640, 445, //window position
        advanced_so,     //screen object
        NULL,
        6,      //item count
        0,      //cursor position 0-79 left to right
        0,      //element that has current focus
        fcsBtn, //focus type
        "InterLane Advanced Menu",
        DefaultOnKeyPress //OnKeyPress();
};

void AdvancedFeatures(void) {
  int i;

  glbWindow = advanced_window;

  for (i = 0; i < 6; ++i) {
    advanced_so[i] = &advanced_buttons[i].so;
  }
  PutWindow(glbWindow);
}

static void CostEffectiveClick(void) {
  Sender = IntroScreen;
  MonthlyCostEffective();
}

extern int du_do_mon;
extern int du_do_day;
extern int du_do_year;

static void TodaysEnergyUsage(void) {
  long ltime;

  struct tm* tod;

  time(&ltime);
  tod = localtime(&ltime);
  this_month = tod->tm_mon + 1;
  today = tod->tm_mday;
  this_year = tod->tm_year;
  //   du_do_mon = this_month;
  //   du_do_day = today;
  //   du_do_year = this_year;

  Sender = IntroScreen;
  daily_usage();
}

static void ExitLaneView(void) {
  char work[100];
  int rc;
  int times;

  if (MsgsOnIcu != 0) {
    rc = msgbox("Do you want the ICU to use your phone|"
                "to send your E-Mail out now? (If you |"
                "answer no, your messages WILL be     |"
                "uploaded during the next regularly   |"
                "scheduled call-in.)                  ",
                "There are messages to be sent", MB_YES_NO);
    if (rc == MB_RESULT_YES) {
      times = 0;
      work[0] = 0;
      if (SendTable(CALL_ALT_NUM_NOW, work, 1) == SUCCESS) {
        MsgsOnIcu = 0;
      }
    }
  }

  if (msgbox("Are you sure you|want to exit LaneView?",
             "Exit LaneView",
             MB_YES_NO) == MB_RESULT_YES) {
    lv_stop();
  }
}

void GetPassword(void) {
  int date_check;
  struct tm* tod;
  long ltime;
  UCHAR* password;

  time(&ltime);
  tod = localtime(&ltime);

  password = inputbox("Enter the password.", "Secured User Access Logon", "*");

  if (memcmp(password, "TeCom", 5) == 0) {
    date_check = atoi(&password[5]);
    if (date_check == tod->tm_mon + 1 + tod->tm_mday) {
      msgbox("Password Accepted|Security Level: UTILITY", "Information", MB_OK);
      secured_user = TRUE;
    } else {
      msgbox("Access Denied!|Security Level: END USER", "Information", MB_OK);
      secured_user = FALSE;
    }
  } else {
    msgbox("Access Denied!|Security Level: END USER", "Information", MB_OK);
    secured_user = FALSE;
  }
  show_utility_message = 1;
  show_time = TRUE;
  IntroScreen();
}

static void AboutOkClick(void) {
  IntroScreen();
}

static PUSH_BUTTON_T aboutokbtn[1] =
    {
        300, 284, 70, 30,
        TRUE, //got focus
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
        AboutOkClick,
        107};

SCREEN_OBJECT_T* aboutso[1];
static void AboutDraw(void);
void AboutOnKeyPress(SCREEN_OBJECT_T* so, int c);
extern UCHAR longversion[];
extern UCHAR lw_ver[8];
extern UCHAR about_title[];

LWINDOW_T aboutwindow =
    {
        120, 150, 340, 220, //window position
        aboutso,            //screen object
        AboutDraw,
        1,              //item count
        0,              //cursor position 0-79 left to right
        0,              //element that has current focus
        fcsBtn,         //focus type
        about_title,    //title
        AboutOnKeyPress //OnKeyPress();
};

void AboutClick(void) {
  aboutso[0] = &aboutokbtn[0].so;
  glbWindow = &aboutwindow;
  PutWindow(&aboutwindow);
}

extern char copyright_text[];
extern char all_rights_reserved[];

static void AboutDraw(void) {
  char work[10];
  char logo[522];
  UCHAR lw[100];

  memcpy(work, lw_ver, 8);
  work[4] = work[3];
  work[3] = '.';
  sprintf(lw, "LaneWare Version %5.5s", work);

  GetResource("LANEVIEW.SLM", logo);

  show_logo(148, 160 / 8, 4, 32, logo + 10);

  ptext(longversion, 200, 198, BG_TRANSPARENT + FG_BLK);
  ptext(lw, 214, 198, BG_TRANSPARENT + FG_BLK);
  ptext(copyright_text, 228, 198, BG_TRANSPARENT + FG_BLK);
  ptext(all_rights_reserved, 242, 198, BG_TRANSPARENT + FG_BLK);
}

void AboutOnKeyPress(SCREEN_OBJECT_T* so, int c) {
  int top = 242 + 14;
  int bottom = 295 - 16;
  int row = 0;
  int i = 0;

  char* array[15] =
      {
          "                           ",
          "        Written by:        ",
          "  Mark \"The King\" Lauter ",
          "            and            ",
          "  Gary \"Mad Man\" Speegle ",
          "                           ",
          " LaneManager(Utility Side) ",
          "Dominick \"The Dominator\" Ginex",
          "                            ",
          "        Tested by:         ",
          "Steve \"Fix It Now\" Amberg",
          "                           ",
          "     Special Thanks to     ",
          " Vivian \"E-Mom\" Fernandez",
          "                           "};

  if (c == 65) {
    row = bottom;
    for (i = 1; i < 15; i++) {
      while (1) {
        ptext(array[0], row + 4, 218, BG_WHT + FG_BLK);
        ptext(array[i], row--, 218, BG_WHT + FG_BLK);
        if (row-- < top) {
          row = bottom;
          break;
        }
        Wait(1L);
        if (kb_stat()) {
          if (kbchar() == 27) {
            return;
          }
        }
      }
    }
  }
}

static void ScheduleSetup(void) {
  //void ScheduleEvents(DEVICE *dev, int cat_index);
  ScheduleEvents(NULL, glb_cat_index, 0);
}

static void LogInNow(void) {
  UCHAR* days_back;

  LVLITE = FALSE;
  if ((secured_user == TRUE) && (DEBUG == TRUE)) {
    days_back = inputbox("How many days back to read?", "Log In Now", "0");
    if (days_back[0] == 0) {
      LVLITE = TRUE;
      return;
    }
    read_devices(atoi(days_back));
  } else {
    read_devices(-1);
  }
  IntroScreen();
}

static void TriggerSetupClick(void) {
  Sender = IntroScreen;
  TriggerMenu();
}

static void AdvancedViewMailClick(void) {
  Sender = AdvancedFeatures;
  ViewMail();
}
