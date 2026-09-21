#pragma once
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <vector>

/* ============================================
   ღილაკების ID-ები
   ============================================ */
enum ButtonId {
    ID_NONE = 0,

    ID_0 = 100, ID_1, ID_2, ID_3, ID_4,
    ID_5, ID_6, ID_7, ID_8, ID_9,

    ID_DOT = 110,
    ID_CLEAR,
    ID_EQUAL,
    ID_DIV,
    ID_MUL,
    ID_SUB,
    ID_ADD,
    ID_HELP,
    ID_PERCENT,
    ID_SQRT,
    ID_FACT,
    ID_SIGN,        /* ± */
    ID_BACKSPACE,   /* ⌫ */
    ID_CLEAR_HISTORY
};

/* ============================================
   ოპერაციები
   ============================================ */
enum Operation {
    OP_NONE = 0,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

/* ============================================
   განლაგების ზომები
   ============================================ */
const int WIN_W = 620;
const int WIN_H = 560;

const int CALC_W = 340;      /* კალკულატორის ნაწილი */
const int HIST_X = 340;      /* ისტორიის პანელი აქედან */
const int HIST_W = 280;

const int MARGIN = 14;

const int DISPLAY_X = MARGIN;
const int DISPLAY_Y = MARGIN;
const int DISPLAY_W = CALC_W - MARGIN * 2;
const int DISPLAY_H = 110;

const int BTN_W = 74;
const int BTN_H = 60;
const int BTN_GAP = 6;
const int BTN_START_X = MARGIN;
const int BTN_START_Y = DISPLAY_Y + DISPLAY_H + 14;

const int BTN_RADIUS = 8;

/* ============================================
   ღილაკის აღწერა
   ============================================ */
struct Button {
    const wchar_t* text;   /* Unicode — √ ± ⌫ სიმბოლოებისთვის */
    int id;
    int x, y, w, h;
};

extern const std::vector<Button> g_buttons;

/* ============================================
   ისტორიის ერთი ჩანაწერი
   ============================================ */
struct HistoryEntry {
    std::wstring expression;   /* "15 + 12 =" */
    std::wstring result;       /* "27"        */
};