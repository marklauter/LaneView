#ifndef event_h
#define event_h

#define EVNT_OFF 0x00
#define EVNT_ON 0x01
#define EVNT_DIM0 0x02
#define EVNT_DIM1 0x12
#define EVNT_DIM2 0x22
#define EVNT_DIM3 0x32
#define EVNT_DIM4 0x42
#define EVNT_DIM5 0x52
#define EVNT_DIM6 0x62
#define EVNT_DIM7 0x72
#define EVNT_DIM8 0x82
#define EVNT_DIM9 0x92
#define EVNT_DIM10 0xA2
#define EVNT_DIM11 0xB2
#define EVNT_DIM12 0xC2
#define EVNT_DIM13 0xD2
#define EVNT_DIM14 0xE2
#define EVNT_DIM15 0xF2

#define TSTAT_OFF 0
#define TSTAT_HEAT 1
#define TSTAT_COOL 2
#define TSTAT_AUTO 3
#define TSTAT_40 40
#define TSTAT_60 60
#define TSTAT_62 62
#define TSTAT_63 63
#define TSTAT_64 64
#define TSTAT_65 65
#define TSTAT_66 66
#define TSTAT_67 67
#define TSTAT_68 68
#define TSTAT_69 69
#define TSTAT_70 70
#define TSTAT_71 71
#define TSTAT_72 72
#define TSTAT_73 73
#define TSTAT_74 74
#define TSTAT_75 75
#define TSTAT_76 76
#define TSTAT_77 77
#define TSTAT_78 78
#define TSTAT_79 79
#define TSTAT_80 80
#define TSTAT_81 81
#define TSTAT_82 82
#define TSTAT_83 83
#define TSTAT_84 84
#define TSTAT_86 86
#define TSTAT_88 88
#define TSTAT_90 90

// centigrade defines:
#define TSTAT_05 5
#define TSTAT_06 6
#define TSTAT_07 7
#define TSTAT_08 8
#define TSTAT_09 9
#define TSTAT_10 10
#define TSTAT_11 11
#define TSTAT_12 12
#define TSTAT_13 13
#define TSTAT_14 14
#define TSTAT_15 15
#define TSTAT_16 16
#define TSTAT_17 17
#define TSTAT_18 18
#define TSTAT_19 19
#define TSTAT_20 20
#define TSTAT_21 21
#define TSTAT_22 22
#define TSTAT_23 23
#define TSTAT_24 24
#define TSTAT_25 25
#define TSTAT_26 26
#define TSTAT_27 27
#define TSTAT_28 28
#define TSTAT_29 29
#define TSTAT_30 30
#define TSTAT_31 31
#define TSTAT_32 32

//day type mask
#define DT_SUNDAY 0x001
#define DT_MONDAY 0x002
#define DT_TUESDAY 0x004
#define DT_WEDNESDAY 0x008
#define DT_THURSDAY 0x010
#define DT_FRIDAY 0x020
#define DT_SATURDAY 0x040

//mask1
#define MODE_VACATION 0x080
#define MODE_NORMAL 0x040
#define MODE_HEAT 0x020
#define MODE_COOL 0x010
#define MODE_SPRING 0x008
#define MODE_SUMMER 0x004
#define MODE_AUTUMN 0x002
#define MODE_WINTER 0x001

//mask2
#define MODE_CRITICAL_COST 0x080
#define MODE_HIGH_COST 0x040
#define MODE_MEDIUM_COST 0x020
#define MODE_LOW_COST 0x010
#define MODE_HOME 0x008
#define MODE_AWAY 0x004
#define NA1 0x002
#define NA2 0x001

//mask3
// mask 3 is for future expansion

extern EVENT events[MAX_EVENTS];
extern int events_used;
extern int tstat_table[32];
extern int centi_tstat_table[32];
extern char* tstat_strings[5];

extern void ScheduleEvents(DEVICE* dev, int cat_index, int sender);
extern void AdjustEvents(int device_slot);
extern void InsertEvent(EVENT* event);
extern int LookupEvent(USHORT event_id);
extern int RemoveEvent(EVENT* event);

#endif
