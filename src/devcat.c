//DeviceMenuWindow
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "2box.h"
#include "inside.h"
#include "vga.h"
#include "scrnobjt.h"
#include "textbox.h"
#include "pshbtn.h"
#include "listbox.h"
#include "mouse.h"
#include "lwindow.h"
#include "ismenu.h"
#include "moodmenu.h"
#include "frame_3d.h"
#include "ctrlschd.h"
#include "devcat.h"

////////////////////////////////////////////////
// Device Categories
// From 2box.h
////////////////////////////////////////////////
//#define ALL_CATEGORIES         0
//#define LIGHTING_CATEGORY       1
//#define ENVIRONMENTAL_CATEGORY  2
//#define POOL_SPA_CATEGORY       3
//#define SPRINKLER_CATEGORY      4
//#define ALERT_DEVICE_CATEGORY   5
//#define SECURITY            6

DEVICE_CATEGORY_LIST_T dev_cat = {5,
                                  "ALL",
                                  "Lighting",
                                  "Comfort Control",
                                  "Pool/Spa",
                                  "Sprinkler"};
//                           "Alert",
//                           "Security"

void DisplayDeviceCategory(UCHAR* desc, int top, int left, UCHAR color) {
  ptext(desc, top, left, color);
}
