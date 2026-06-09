#ifndef _device_category_h
#define _device_category_h

#define DEVICE_CATEGORY_DESC_LEN 26
typedef struct
{
  UCHAR count;
  UCHAR* desc[15];
} DEVICE_CATEGORY_LIST_T;

extern DEVICE_CATEGORY_LIST_T dev_cat;

extern void AddDeviceCategory(void);
extern void SaveDeviceCategory(void);
extern int EditDeviceCategory(DEVICE_CATEGORY_LIST_T* dc);
extern void DisplayDeviceCategory(UCHAR* desc, int top, int left, UCHAR color);
extern void CancelDeviceCategory(void);

#endif
