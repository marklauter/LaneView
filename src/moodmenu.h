#ifndef _mood_menu_h
#define _mood_menu_h

#define MD_PROT 1
#define MD_UNPROT 0

#define MD_OFF 0x00
#define MD_ON 0x01
#define MD_DIM0 0x02
#define MD_DIM1 0x12
#define MD_DIM2 0x22
#define MD_DIM3 0x32
#define MD_DIM4 0x42
#define MD_DIM5 0x52
#define MD_DIM6 0x62
#define MD_DIM7 0x72
#define MD_DIM8 0x82
#define MD_DIM9 0x92
#define MD_DIM10 0xA2
#define MD_DIM11 0xB2
#define MD_DIM12 0xC2
#define MD_DIM13 0xD2
#define MD_DIM14 0xE2
#define MD_DIM15 0xF2

extern int active_moods;
extern int moods_used;
extern MOOD_T mood_list[MAX_MOODS];

extern int LookupMoodIndex(USHORT mood_id);
extern void MoodMenu(void);
extern void AdjustMoods(USHORT which_id);

#endif
