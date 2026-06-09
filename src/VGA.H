//   Copyright 1996 TeCom Inc. All rights reserved
//  File creation date 22 OCT 96
//  Created by Mark Lauter : TMMSL
//   Last modified:
#ifndef _VGA_H
#define _VGA_H

#define fcsBtn 0
#define fcsTxt 1

typedef struct
{
  char current;
  char type; //type:  0=push_button, 1=text_box
} FOCUS_T;

//graphics functions
extern void vga_pallet(int);
extern void frame_3d(int top, int left, int height, int width, char type);
extern void pbutton(int top, int left, int height, int width, char type, int fill_color);
extern void hline(int top, int left, int right, int color);
extern void vline(int left, int top, int bottom, int color);
extern void area_clear(int top, int bottom, int left, int right, int color);
extern void clear(void);
extern void button(int, int, int, int);
extern void show_logo(int top_pixel, int left_column, int width_in_bytes, int height_in_pixels, char far* pointer_to_icon);
extern void rbar_graph_item(int trow, int lcol, int brow, int rcol, int color);
extern UCHAR far* get_window(USHORT top_row, USHORT left_col, USHORT width, USHORT height);
extern void scroll_window_down(int top_row, int left_col, int bot_row, int right_col);
extern void line(int x1, int y1, int x2, int y2, int color);
extern void setdisplaymode(int);

// bold & light font defs
#define BOLD 1
#define LGHT 0
// font space...
extern UCHAR font_array[2][4096];
// Text and cursor functions
extern void get_font_loc(char* font_array);
extern void position(int row, int col);
extern int get_cursor(void);
extern void set_cursor(int);
extern void rev(char*);
extern void hide_cursor(void);
extern void show_cursor(void);
extern void norm(char*);
extern void attrib(unsigned);
extern void bold(char*);
extern void rev(char*);
extern int trim(char*);
extern void ptext(char* text, int top, int left, int color);

// mouse cordinate used in MouseOver();
typedef struct
{
  int top, left, width, height;
} COORDINATE_T;
// mouse functions
extern void mouse_cursor(void* mouse_def); //change the mouse cursor
extern void hide_mouse(void);
extern void unhide_mouse(void);
extern int find_mouse(void);
extern void mouse_on(void);
extern void mouse_off(void);
extern void position_mouse(int top, int left);

// keyboard functions
extern void unkbchar(int c);
extern int kbchar(void);

// other...
extern void lv_stop(void);
extern void beep(void);
extern int exit_no;
extern int MainLoopOpt;
extern int debug;

#endif
