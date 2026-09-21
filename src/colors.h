#pragma once
#include <windows.h>

/* ============================================
   ფერების პალიტრა — Windows 11 Calculator სტილი

   GDI+ იყენებს Gdiplus::Color(a, r, g, b) ფორმატს,
   ამიტომ აქ ცალკეულ კომპონენტებს ვინახავთ.
   ============================================ */

   /* --- ფონი --- */
#define CLR_BG_R           23
#define CLR_BG_G           28
#define CLR_BG_B           42

/* --- ისტორიის პანელის ფონი (ოდნავ მუქი) --- */
#define CLR_HISTORY_R      19
#define CLR_HISTORY_G      23
#define CLR_HISTORY_B      35

/* --- ღილაკი: ციფრები --- */
#define CLR_NUM_R          32
#define CLR_NUM_G          38
#define CLR_NUM_B          54

/* --- ღილაკი: ოპერაციები (ოდნავ ღია) --- */
#define CLR_OP_R           40
#define CLR_OP_G           46
#define CLR_OP_B           64

/* --- ღილაკი: = (აქცენტი) --- */
#define CLR_EQ_R           76
#define CLR_EQ_G          194
#define CLR_EQ_B          255

/* --- ტექსტი --- */
#define CLR_TEXT_R        250
#define CLR_TEXT_G        250
#define CLR_TEXT_B        250

#define CLR_TEXT_DIM_R    150
#define CLR_TEXT_DIM_G    158
#define CLR_TEXT_DIM_B    175

/* = ღილაკის ტექსტი — მუქი, ნათელ ფონზე */
#define CLR_EQ_TEXT_R      16
#define CLR_EQ_TEXT_G      20
#define CLR_EQ_TEXT_B      30

/* --- მდგომარეობის კორექცია (%) --- */
#define SHADE_HOVER        14     /* ნათდება */
#define SHADE_PRESS      (-12)    /* მუქდება */