#ifndef _device_h
#define _device_h

typedef struct
{
  int devices_used;
  DEVICE devices[MAX_DEVICES];
} DEVICE_LIST_T;

extern DEVICE_LIST_T dev_list;
extern int IsSubmeter(DEVICE* dev);
extern DEVICE_HISTORY device_history[MAX_HISTORY];

extern UCHAR* x10_ltr[16];
extern UCHAR* x10_nmr[16];

extern void DisplayDeviceShort(DEVICE* dev, int top, int left, UCHAR color);
extern void DisplayDeviceLong(DEVICE* dev, int top, int left, UCHAR color);
extern void EditDevice(DEVICE* dev, int option, int category);
extern int send_device_body(PKT_INFO* pkt_info, UCHAR* cp);
extern int SendDevice(DEVICE* dev, int option);
extern int IsDimmer(DEVICE* dev);
extern int LookupDev(USHORT device_id);
extern int IsTstat(DEVICE* dev);

#endif
