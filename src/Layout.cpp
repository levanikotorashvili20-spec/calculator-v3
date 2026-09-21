#include "Layout.h"

/* ============================================
   ბადის უჯრის კოორდინატები
   ============================================ */
static int ColX(int col) { return BTN_START_X + col * (BTN_W + BTN_GAP); }
static int RowY(int row) { return BTN_START_Y + row * (BTN_H + BTN_GAP); }

/* ღილაკი ბადის უჯრაში; rowSpan > 1 — რამდენიმე რიგზე გადაჭიმული */
static Button Cell(const wchar_t* text, int id, int col, int row, int rowSpan = 1)
{
    return Button{
        text, id,
        ColX(col), RowY(row),
        BTN_W,
        BTN_H * rowSpan + BTN_GAP * (rowSpan - 1)
    };
}

/* ============================================
   ღილაკების განლაგება

      %     C     ⌫     ÷
      √     !     ±     ×
      7     8     9     −
      4     5     6     +
      1     2     3     =
      ?     0     .     =    ← = ორ რიგზეა

   ⌫ და ისტორიის გასუფთავება — ხატულებია
   Segoe MDL2 Assets ფონტიდან (Windows 10/11-ის
   სისტემური ხატულები, იგივე რაც ნამდვილ კალკულატორში)
   ============================================ */
const std::vector<Button> g_buttons = {
    Cell(L"%",      ID_PERCENT,   0, 0),
    Cell(L"C",      ID_CLEAR,     1, 0),
    Cell(L"\uE750", ID_BACKSPACE, 2, 0),    /* ⌫  (ხატულა) */
    Cell(L"\u00F7", ID_DIV,       3, 0),    /* ÷ */

    Cell(L"\u221A", ID_SQRT,      0, 1),    /* √ */
    Cell(L"!",      ID_FACT,      1, 1),
    Cell(L"\u00B1", ID_SIGN,      2, 1),    /* ± */
    Cell(L"\u00D7", ID_MUL,       3, 1),    /* × */

    Cell(L"7",      ID_7,         0, 2),
    Cell(L"8",      ID_8,         1, 2),
    Cell(L"9",      ID_9,         2, 2),
    Cell(L"\u2212", ID_SUB,       3, 2),    /* − */

    Cell(L"4",      ID_4,         0, 3),
    Cell(L"5",      ID_5,         1, 3),
    Cell(L"6",      ID_6,         2, 3),
    Cell(L"+",      ID_ADD,       3, 3),

    Cell(L"1",      ID_1,         0, 4),
    Cell(L"2",      ID_2,         1, 4),
    Cell(L"3",      ID_3,         2, 4),
    Cell(L"=",      ID_EQUAL,     3, 4, 2), /* ორმაგი სიმაღლე */

    Cell(L"?",      ID_HELP,      0, 5),
    Cell(L"0",      ID_0,         1, 5),
    Cell(L".",      ID_DOT,       2, 5),

    /* ისტორიის პანელის ქვედა მარჯვენა კუთხეში */
    Button{ L"\uE74D", ID_CLEAR_HISTORY,    /* 🗑 (ხატულა) */
            HIST_X + HIST_W - MARGIN - 40, WIN_H - MARGIN - 40, 40, 40 },
};

/* ============================================
   რომელ ღილაკს ხვდება წერტილი
   ============================================ */
int HitTest(int x, int y)
{
    for (const Button& b : g_buttons) {
        if (x >= b.x && x < b.x + b.w &&
            y >= b.y && y < b.y + b.h)
            return b.id;
    }
    return ID_NONE;
}

const Button* FindButton(int id)
{
    for (const Button& b : g_buttons) {
        if (b.id == id)
            return &b;
    }
    return nullptr;
}