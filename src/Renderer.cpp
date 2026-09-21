#include "Renderer.h"
#include "Layout.h"
#include "colors.h"

using namespace Gdiplus;

/* ============================================
   ფერები
   ============================================ */
static Color Rgb(int r, int g, int b, int a = 255)
{
    return Color((BYTE)a, (BYTE)r, (BYTE)g, (BYTE)b);
}

static const Color kBg = Rgb(CLR_BG_R, CLR_BG_G, CLR_BG_B);
static const Color kHistory = Rgb(CLR_HISTORY_R, CLR_HISTORY_G, CLR_HISTORY_B);
static const Color kNum = Rgb(CLR_NUM_R, CLR_NUM_G, CLR_NUM_B);
static const Color kOp = Rgb(CLR_OP_R, CLR_OP_G, CLR_OP_B);
static const Color kEq = Rgb(CLR_EQ_R, CLR_EQ_G, CLR_EQ_B);
static const Color kText = Rgb(CLR_TEXT_R, CLR_TEXT_G, CLR_TEXT_B);
static const Color kTextDim = Rgb(CLR_TEXT_DIM_R, CLR_TEXT_DIM_G, CLR_TEXT_DIM_B);
static const Color kEqText = Rgb(CLR_EQ_TEXT_R, CLR_EQ_TEXT_G, CLR_EQ_TEXT_B);

/* ფერის განათება (+) ან დამუქება (−) პროცენტით */
static BYTE ShadeChannel(BYTE v, int percent)
{
    int target = (percent > 0) ? 255 : 0;
    int p = (percent > 0) ? percent : -percent;
    return (BYTE)(v + (target - v) * p / 100);
}

static Color Shade(const Color& c, int percent)
{
    return Color(c.GetA(),
        ShadeChannel(c.GetR(), percent),
        ShadeChannel(c.GetG(), percent),
        ShadeChannel(c.GetB(), percent));
}

/* ============================================
   ღილაკის ტიპი → ფერი
   ============================================ */
static bool IsIcon(int id)
{
    return id == ID_BACKSPACE || id == ID_CLEAR_HISTORY;
}

static Color FaceColor(int id)
{
    if ((id >= ID_0 && id <= ID_9) || id == ID_DOT) return kNum;
    if (id == ID_EQUAL)                             return kEq;
    return kOp;
}

/* ============================================
   მომრგვალებული მართკუთხედი
   GDI+-ში მზა ფუნქცია არ არის — ოთხი რკალით ვაწყობთ:

      ╭──────╮   ზედა-მარცხენა  → 180°-დან 90°
      │      │   ზედა-მარჯვენა  → 270°-დან 90°
      ╰──────╯   ქვედა-მარჯვენა → 0°-დან 90°
                 ქვედა-მარცხენა → 90°-დან 90°
   რკალებს შორის ხაზებს GDI+ თვითონ აერთებს.
   ============================================ */
static void AddRoundRect(GraphicsPath& path, const RectF& r, REAL radius)
{
    REAL d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90);
    path.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    path.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    path.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    path.CloseFigure();
}

/* ============================================
   ფონტები — member initializer list-ით
   ============================================ */
Renderer::Renderer()
    : m_fontButton(L"Segoe UI", 20.0f, FontStyleRegular, UnitPixel)
    , m_fontIcon(L"Segoe MDL2 Assets", 16.0f, FontStyleRegular, UnitPixel)
    , m_fontExpression(L"Segoe UI", 16.0f, FontStyleRegular, UnitPixel)
    , m_fontTitle(L"Segoe UI Semibold", 16.0f, FontStyleRegular, UnitPixel)
    , m_fontHistExpr(L"Segoe UI", 14.0f, FontStyleRegular, UnitPixel)
    , m_fontHistResult(L"Segoe UI Semibold", 22.0f, FontStyleRegular, UnitPixel)
{
}

/* ============================================
   მთელი ფანჯარა
   ============================================ */
void Renderer::Paint(HDC hdc, const Calculator& calc, int hoveredId, int pressedId)
{
    /* 1. მეხსიერებაში "ტილო" — ეკრანზე ჯერ არაფერი ჩანს */
    Bitmap   buffer(WIN_W, WIN_H, PixelFormat32bppPARGB);
    Graphics g(&buffer);

    g.SetSmoothingMode(SmoothingModeAntiAlias);                 /* გლუვი კიდეები */
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);  /* გლუვი ტექსტი */

    /* 2. კალკულატორის ნაწილის ფონი */
    SolidBrush bg(kBg);
    g.FillRectangle(&bg, 0, 0, CALC_W, WIN_H);

    /* 3. დისპლეი */
    DrawDisplay(g, calc);

    /* 4. ღილაკები (ისტორიის გასუფთავებას ისტორიის პანელი ხატავს) */
    for (const Button& b : g_buttons) {
        if (b.id == ID_CLEAR_HISTORY) continue;

        bool hovered = (b.id == hoveredId);
        bool pressed = (b.id == pressedId) && hovered;   /* მაუსი გაიტანე — "აიწევა" */
        DrawButton(g, b, hovered, pressed);
    }

    /* 5. ისტორიის პანელი */
    DrawHistory(g, calc, hoveredId, pressedId);

    /* 6. მზა სურათი ეკრანზე — ერთი ოპერაციით */
    Graphics screen(hdc);
    screen.DrawImage(&buffer, 0, 0, WIN_W, WIN_H);
}

/* ============================================
   დისპლეი: ზემოთ გამოსახულება, ქვემოთ რიცხვი
   ============================================ */
void Renderer::DrawDisplay(Graphics& g, const Calculator& calc)
{
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentFar);            /* მარჯვნივ */
    fmt.SetLineAlignment(StringAlignmentCenter);
    fmt.SetFormatFlags(StringFormatFlagsNoWrap);
    fmt.SetTrimming(StringTrimmingEllipsisCharacter);

    /* ზედა ხაზი: "15 + 12 =" */
    RectF      exprRect((REAL)DISPLAY_X, (REAL)DISPLAY_Y + 4, (REAL)DISPLAY_W, 24.0f);
    SolidBrush dim(kTextDim);
    g.DrawString(calc.GetExpression().c_str(), -1, &m_fontExpression, exprRect, &fmt, &dim);

    /* მთავარი რიცხვი — ფონტი მცირდება, სანამ არ ჩაეტევა */
    const std::wstring& text = calc.GetDisplay();
    RectF numRect((REAL)DISPLAY_X, (REAL)DISPLAY_Y + 30,
        (REAL)DISPLAY_W, (REAL)DISPLAY_H - 30);

    REAL size = 48.0f;
    for (; size > 20.0f; size -= 2.0f) {
        Font  probe(L"Segoe UI Semibold", size, FontStyleRegular, UnitPixel);
        RectF bounds;
        g.MeasureString(text.c_str(), -1, &probe, PointF(0, 0), &bounds);
        if (bounds.Width <= numRect.Width)
            break;
    }

    Font       numFont(L"Segoe UI Semibold", size, FontStyleRegular, UnitPixel);
    SolidBrush white(kText);
    g.DrawString(text.c_str(), -1, &numFont, numRect, &fmt, &white);
}

/* ============================================
   ერთი ღილაკი
   ============================================ */
void Renderer::DrawButton(Graphics& g, const Button& b, bool hovered, bool pressed)
{
    RectF r((REAL)b.x, (REAL)b.y, (REAL)b.w, (REAL)b.h);

    Color face = FaceColor(b.id);
    if (pressed)      face = Shade(face, SHADE_PRESS);
    else if (hovered) face = Shade(face, SHADE_HOVER);

    /* 1. ჩრდილი: იგივე ფორმა, 2px ქვემოთ, ნახევრად გამჭვირვალე შავი.
          დაჭერისას ქრება — ღილაკი "დაწოლილი" ჩანს */
    if (!pressed) {
        GraphicsPath shadow;
        AddRoundRect(shadow, RectF(r.X, r.Y + 2, r.Width, r.Height), (REAL)BTN_RADIUS);
        SolidBrush shadowBrush(Rgb(0, 0, 0, 60));
        g.FillPath(&shadowBrush, &shadow);
    }

    /* 2. თვითონ ღილაკი — ზემოდან ქვემოთ გრადიენტი (ოდნავ ღია → ძირითადი) */
    GraphicsPath path;
    AddRoundRect(path, r, (REAL)BTN_RADIUS);

    RectF gradRect = r;
    gradRect.Inflate(0, 1);   /* GDI+-ის ხარვეზი: ზუსტ ზომაზე კიდეზე ზოლს ტოვებს */
    LinearGradientBrush grad(gradRect, Shade(face, 6), face, LinearGradientModeVertical);
    g.FillPath(&grad, &path);

    /* 3. თხელი ნათელი კონტური — Windows 11-ის სტილი */
    Pen edge(Rgb(255, 255, 255, pressed ? 8 : 18), 1.0f);
    g.DrawPath(&edge, &path);

    /* 4. წარწერა */
    Color tc = (b.id == ID_EQUAL) ? kEqText : kText;
    SolidBrush textBrush(Color(pressed ? 190 : 255, tc.GetR(), tc.GetG(), tc.GetB()));

    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentCenter);
    fmt.SetLineAlignment(StringAlignmentCenter);

    Font* font = IsIcon(b.id) ? &m_fontIcon : &m_fontButton;
    g.DrawString(b.text, -1, font, r, &fmt, &textBrush);
}

/* ============================================
   ისტორიის პანელი
   ============================================ */
void Renderer::DrawHistory(Graphics& g, const Calculator& calc, int hoveredId, int pressedId)
{
    const REAL left = (REAL)(HIST_X + MARGIN);
    const REAL width = (REAL)(HIST_W - MARGIN * 2);

    /* ფონი */
    SolidBrush bg(kHistory);
    g.FillRectangle(&bg, HIST_X, 0, HIST_W, WIN_H);

    /* სათაური და ლურჯი ზოლი ქვემოთ (როგორც Windows-ის ჩანართი) */
    SolidBrush white(kText);
    SolidBrush dim(kTextDim);
    SolidBrush accent(kEq);

    g.DrawString(L"History", -1, &m_fontTitle, PointF(left, (REAL)MARGIN), &white);
    g.FillRectangle(&accent, left, (REAL)MARGIN + 26, 24.0f, 3.0f);

    const std::vector<HistoryEntry>& entries = calc.GetHistory();

    if (entries.empty()) {
        g.DrawString(L"There's no history yet", -1, &m_fontHistExpr,
            PointF(left, (REAL)MARGIN + 50), &dim);
        return;   /* ცარიელზე გასუფთავების ღილაკიც არ ჩანს */
    }

    /* ჩანაწერები — უახლესი ზემოთ */
    StringFormat fmt;
    fmt.SetAlignment(StringAlignmentFar);
    fmt.SetFormatFlags(StringFormatFlagsNoWrap);
    fmt.SetTrimming(StringTrimmingEllipsisCharacter);

    REAL       y = (REAL)MARGIN + 50;
    const REAL bottom = (REAL)(WIN_H - MARGIN - 50);   /* ქვემოთ ადგილი ღილაკისთვის */

    for (const HistoryEntry& e : entries) {
        if (y + 52 > bottom) break;                     /* აღარ ეტევა */

        g.DrawString(e.expression.c_str(), -1, &m_fontHistExpr,
            RectF(left, y, width, 20), &fmt, &dim);
        g.DrawString(e.result.c_str(), -1, &m_fontHistResult,
            RectF(left, y + 20, width, 30), &fmt, &white);
        y += 60;
    }

    /* ისტორიის გასუფთავება — ფონი მხოლოდ hover-ზე ჩანს */
    if (const Button* b = FindButton(ID_CLEAR_HISTORY)) {
        bool  hovered = (hoveredId == b->id);
        bool  pressed = (pressedId == b->id) && hovered;
        RectF r((REAL)b->x, (REAL)b->y, (REAL)b->w, (REAL)b->h);

        if (hovered) {
            GraphicsPath path;
            AddRoundRect(path, r, (REAL)BTN_RADIUS);
            SolidBrush hb(pressed ? kNum : kOp);
            g.FillPath(&hb, &path);
        }

        StringFormat center;
        center.SetAlignment(StringAlignmentCenter);
        center.SetLineAlignment(StringAlignmentCenter);

        SolidBrush iconBrush(hovered ? kText : kTextDim);
        g.DrawString(b->text, -1, &m_fontIcon, r, &center, &iconBrush);
    }
}