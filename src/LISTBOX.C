#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "2box.h"
#include "vga.h"
#include "frame_3d.h"
#include "scrnobjt.h"
#include "pshbtn.h"
#include "listbox.h"
#include "mouse.h"

void ListBoxScroll(LISTBOX_T* lb) {
  int i;
  int j;
  int t;
  int l;
  int w;
  int h;

  if (lb->so.visible == FALSE) {
    return;
  }

  t = lb->so.position.top;
  l = lb->so.position.left;
  h = lb->so.position.height;
  w = lb->so.position.width;

  lb->display_count = h / 14;
  if (lb->display_count > lb->item_count) {
    lb->display_count = lb->item_count;
  }
  if (lb->item_count > 0) {
    if (lb->item_selected > (lb->item_count - 1)) {
      lb->item_selected = lb->item_count - 1;
    }
    if (lb->item_selected < 0) {
      lb->item_selected = 0;
    }
    if (lb->top_item > (lb->item_count - 1)) {
      lb->top_item = lb->item_count - lb->display_count;
    }
    if ((lb->display_count + lb->top_item) > (lb->item_count)) {
      lb->top_item = lb->item_count - lb->display_count;
    }
    if (lb->item_selected >= (lb->top_item + lb->display_count)) {
      lb->top_item = (lb->item_selected - lb->display_count) + 1;
    }
    if (lb->item_selected < lb->top_item) {
      lb->top_item = lb->item_selected;
    }
  }

  hide_mouse();

  if (lb->item_count > 0) {
    for (j = lb->top_item, i = 0; j < (lb->top_item + lb->display_count); j++) {
      if (lb->item_selected == j) {
        if (lb->so.got_focus == TRUE) {
          area_clear(t + (i * 14) + 2,      //top
                     t + (i * 14) + 1 + 14, //bottom
                     l + 2,                 //left
                     l + w - 2,             //right
                     BG_BLU + FG_BLU);
          (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_BLU + FG_HWHT);
        } else {
          area_clear(t + (i * 14) + 2,
                     t + (i * 14) + 1 + 14,
                     l + 2,
                     l + w - 2,
                     BG_GRY + FG_GRY);
          (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_GRY + FG_HWHT);
        }
      } else {
        area_clear(t + (i * 14) + 2,
                   t + (i * 14) + 1 + 14,
                   l + 2,
                   l + w - 2,
                   BG_HWHT + FG_HWHT);
        (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_HWHT + FG_BLK);
      }
      i++;
    }
  }
  unhide_mouse();
}

void PutListBox(LISTBOX_T* lb) {
  int i;
  int j;
  int t;
  int l;
  int w;
  int h;

  if (lb->so.visible == FALSE) {
    return;
  }

  t = lb->so.position.top;
  l = lb->so.position.left;
  h = lb->so.position.height;
  w = lb->so.position.width;

  lb->display_count = h / 14;
  if (lb->display_count > lb->item_count) {
    lb->display_count = lb->item_count;
  }
  if (lb->item_count > 0) {
    if (lb->item_selected > (lb->item_count - 1)) {
      lb->item_selected = lb->item_count - 1;
    }
    if (lb->item_selected < 0) {
      lb->item_selected = 0;
    }
    if (lb->top_item > (lb->item_count - 1)) {
      lb->top_item = lb->item_count - lb->display_count;
    }
    if ((lb->display_count + lb->top_item) > (lb->item_count)) {
      lb->top_item = lb->item_count - lb->display_count;
    }
    if (lb->item_selected >= (lb->top_item + lb->display_count)) {
      lb->top_item = (lb->item_selected - lb->display_count) + 1;
    }
    if (lb->item_selected < lb->top_item) {
      lb->top_item = lb->item_selected;
    }
  }

  hide_mouse();

  pbutton(t, l, h, w, PB_DN, FG_HWHT);

  if (lb->item_count > 0) {
    for (j = lb->top_item, i = 0; j < (lb->top_item + lb->display_count); j++) {
      if (lb->item_selected == j) {
        if (lb->so.got_focus == TRUE) {
          area_clear(t + (i * 14) + 2,      //top
                     t + (i * 14) + 1 + 14, //bottom
                     l + 2,                 //left
                     l + w - 2,             //right
                     BG_BLU + FG_BLU);
          (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_BLU + FG_HWHT);
        } else {
          area_clear(t + (i * 14) + 2,
                     t + (i * 14) + 1 + 14,
                     l + 2,
                     l + w - 2,
                     BG_GRY + FG_GRY);
          (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_GRY + FG_HWHT);
        }
      } else {
        (*lb->DisplayItem)(lb->sub_list[j], t + (i * 14) + 2, l + 3, BG_HWHT + FG_BLK);
      }
      i++;
    }
  }
  unhide_mouse();
}

void ListBoxMouseDown(LISTBOX_T* lb) {
  COORDINATE_T mfield;
  int i;

  mfield.left = lb->so.position.left;
  mfield.width = lb->so.position.width;
  mfield.height = 14;

  for (i = 0; i < lb->item_count; i++) {
    mfield.top = lb->so.position.top + (i * 14) + 2;
    if (MouseOver(&mfield) == TRUE) {
      lb->item_selected = i + lb->top_item;
      if (lb->item_selected >= lb->top_item + lb->display_count) {
        lb->item_selected -= 1;
      }

      break;
    }
  }
  lb->so.got_focus = TRUE;
  ListBoxScroll(lb);
  //PutListBox(lb);
  if (lb->OnChange != NULL) {
    (*lb->OnChange)();
  }
}

void DownArrow(LISTBOX_T* lb) {
  if (lb->item_selected < (lb->item_count - 1)) {
    ++lb->item_selected;
  }
}

void UpArrow(LISTBOX_T* lb) {
  if (lb->item_selected > 0) {
    --lb->item_selected;
  }
}

void PageUp(LISTBOX_T* lb) {
  lb->item_selected -= (lb->display_count - 1);
  if (lb->item_selected < 0) {
    lb->item_selected = 0;
  }
}

void PageDown(LISTBOX_T* lb) {
  lb->item_selected += (lb->display_count - 1);
  if (lb->item_selected > (lb->item_count - 1)) {
    lb->item_selected = lb->item_count - 1;
  }
}

void ListBoxKeyPress(LISTBOX_T* lb, int c) {
  if (c == 0) {
    c = kbchar();
    switch (c) {
    case 77: // right arrow
    case 80: // down arrow
      DownArrow(lb);
      break;
    case 75: // left arrow
    case 72: // up arrow
      UpArrow(lb);
      break;
    case 71: // home
      lb->item_selected = 0;
      break;
    case 79: // end
      lb->item_selected = lb->item_count - 1;
      break;
    case 73: // page up
      PageUp(lb);
      break;
    case 81: // page down
      PageDown(lb);
      break;
    }
    if (lb->OnChange != NULL) {
      (*lb->OnChange)();
    }
    ListBoxScroll(lb);
    //PutListBox(lb);
  }
}

void ListBoxMouseUp(LISTBOX_T* lb) {
  ;
}

void ListBoxClick(LISTBOX_T* lb) {
  ;
}
