#pragma once
#include "calc.h"
#include "Calculator.h"
#include <gdiplus.h>

/* ============================================
   Renderer — მთელი ფანჯრის ხატვა GDI+-ით

   ყოველ ჯერზე სრულ სურათს ხატავს მეხსიერებაში
   (double buffering) და მერე ერთბაშად გადააქვს
   ეკრანზე — ასე მოციმციმეობა არ ხდება.

   ⚠ უნდა შეიქმნას GdiplusStartup-ის შემდეგ!
   ============================================ */
class Renderer {
public:
    Renderer();

    void Paint(HDC hdc, const Calculator& calc, int hoveredId, int pressedId);

private:
    void DrawDisplay(Gdiplus::Graphics& g, const Calculator& calc);
    void DrawButton(Gdiplus::Graphics& g, const Button& b, bool hovered, bool pressed);
    void DrawHistory(Gdiplus::Graphics& g, const Calculator& calc, int hoveredId, int pressedId);

    /* ფონტები — ერთხელ იქმნება, ყოველ ხატვაზე გამოიყენება */
    Gdiplus::Font m_fontButton;
    Gdiplus::Font m_fontIcon;
    Gdiplus::Font m_fontExpression;
    Gdiplus::Font m_fontTitle;
    Gdiplus::Font m_fontHistExpr;
    Gdiplus::Font m_fontHistResult;
};