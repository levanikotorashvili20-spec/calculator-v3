#include <windows.h>
#include <dwmapi.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

/* ============================================
   ყველა მოდულის ჩასმა (unity build)
   ============================================ */
#include "History.cpp"
#include "Calculator.cpp"
#include "Layout.cpp"
#include "Renderer.cpp"

   /* ============================================
      GDI+-ის ჩართვა/გამორთვა — RAII-ით

      კონსტრუქტორი: GdiplusStartup
      დესტრუქტორი:  GdiplusShutdown
      ============================================ */
class GdiplusSession {
public:
    GdiplusSession()
    {
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&m_token, &input, nullptr);
    }
    ~GdiplusSession()
    {
        Gdiplus::GdiplusShutdown(m_token);
    }

    /* კოპირება აკრძალულია — ორჯერ გამორთვა არ უნდა მოხდეს */
    GdiplusSession(const GdiplusSession&) = delete;
    GdiplusSession& operator=(const GdiplusSession&) = delete;

private:
    ULONG_PTR m_token = 0;
};

/* ============================================
   აპლიკაციის მდგომარეობა
   ============================================ */
static Calculator g_calc;
static Renderer* g_renderer = nullptr;   /* WinMain-ში შეიქმნება */

static int  g_hoveredId = ID_NONE;        /* რომელ ღილაკზეა მაუსი */
static int  g_pressedId = ID_NONE;        /* რომელ ღილაკზე დააჭირეს (ჯერ არ აუშვეს) */
static bool g_tracking = false;          /* WM_MOUSELEAVE გამოწერილია? */

/* ============================================
   დახმარების ფანჯარა
   ============================================ */
static void ShowHelp(HWND hwnd)
{
    MessageBoxW(hwnd,
        L"Operations:\n"
        L"   +   \u2212   \u00D7   \u00F7\n"
        L"   %   percent          \u221A   square root\n"
        L"   !   factorial         \u00B1   change sign\n"
        L"   C   clear              Backspace — delete digit\n\n"
        L"Keyboard:\n"
        L"   0-9  .  +  -  *  /\n"
        L"   Enter or =      result\n"
        L"   Esc or Del       clear\n"
        L"   @   square root     !   factorial     %   percent\n"
        L"   F9   change sign    F1   help\n\n"
        L"Author: Levan Kotorashvili",
        L"Calculator \u2014 Help",
        MB_OK | MB_ICONINFORMATION);
}

/* ============================================
   ღილაკის "გააქტიურება" — მაუსით თუ კლავიატურით
   ============================================ */
static void Activate(HWND hwnd, int id)
{
    switch (id) {
    case ID_NONE:          return;
    case ID_HELP:          ShowHelp(hwnd);       break;
    case ID_CLEAR_HISTORY: g_calc.ClearHistory(); break;
    default:               g_calc.HandleInput(id); break;
    }
    InvalidateRect(hwnd, nullptr, FALSE);
}

/* ============================================
   კლავიატურა: სიმბოლო → ღილაკის ID
   ============================================ */
static int CharToId(wchar_t ch)
{
    if (ch >= L'0' && ch <= L'9')
        return ID_0 + (ch - L'0');

    switch (ch) {
    case L'.': case L',': return ID_DOT;
    case L'+':            return ID_ADD;
    case L'-':            return ID_SUB;
    case L'*':            return ID_MUL;
    case L'/':            return ID_DIV;
    case L'=': case L'\r':return ID_EQUAL;       /* \r = Enter */
    case L'%':            return ID_PERCENT;
    case L'!':            return ID_FACT;
    case L'@':            return ID_SQRT;
    case L'\b':           return ID_BACKSPACE;   /* \b = Backspace */
    case 27:                                      /* Esc */
    case L'c': case L'C': return ID_CLEAR;
    default:              return ID_NONE;
    }
}

/* კლავიშები, რომლებიც სიმბოლოს არ აგზავნიან */
static int KeyToId(WPARAM vk)
{
    switch (vk) {
    case VK_F1:     return ID_HELP;
    case VK_F9:     return ID_SIGN;
    case VK_DELETE: return ID_CLEAR;
    default:        return ID_NONE;
    }
}

/* ============================================
   მუქი სათაურის ზოლი (Windows 10/11)
   ============================================ */
static void ApplyDarkTitleBar(HWND hwnd)
{
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */,
        &dark, sizeof(dark));

    /* Windows 11: სათაურის ზუსტად იგივე ფერი, რაც ფონს აქვს
       (Windows 10-ზე უბრალოდ იგნორირდება) */
    COLORREF caption = RGB(CLR_BG_R, CLR_BG_G, CLR_BG_B);
    DwmSetWindowAttribute(hwnd, 35 /* DWMWA_CAPTION_COLOR */,
        &caption, sizeof(caption));
}

/* ============================================
   WndProc
   ============================================ */
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

        /* ---- ხატვა ---- */
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_renderer)
            g_renderer->Paint(hdc, g_calc, g_hoveredId, g_pressedId);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;   /* ფონს Renderer ხატავს — Windows-მა არ გაასუფთაოს */

        /* ---- მაუსის მოძრაობა ---- */
    case WM_MOUSEMOVE: {
        if (!g_tracking) {
            /* "შემატყობინე, როცა მაუსი ფანჯრიდან გავა" */
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            g_tracking = true;
        }

        int id = HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        if (id != g_hoveredId) {
            g_hoveredId = id;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_tracking = false;
        g_hoveredId = ID_NONE;
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

        /* ---- დაჭერა: ღილაკი "ჩადის", მაგრამ ჯერ არაფერი ხდება ---- */
    case WM_LBUTTONDOWN: {
        int id = HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        if (id != ID_NONE) {
            g_pressedId = id;
            SetCapture(hwnd);   /* მაუსი ფანჯრის გარეთაც ჩვენი იყოს, სანამ არ აუშვებს */
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

                       /* ---- აშვება: მოქმედება მხოლოდ თუ ისევ იმავე ღილაკზეა ---- */
    case WM_LBUTTONUP: {
        if (g_pressedId != ID_NONE) {
            int pressed = g_pressedId;
            int id = HitTest(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

            g_pressedId = ID_NONE;
            ReleaseCapture();

            if (id == pressed)
                Activate(hwnd, id);
            else
                InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

                     /* capture სხვამ წაართვა (მაგ. Alt+Tab დაჭერის დროს) */
    case WM_CAPTURECHANGED:
        if (g_pressedId != ID_NONE) {
            g_pressedId = ID_NONE;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

        /* ---- კლავიატურა ---- */
    case WM_CHAR:
        Activate(hwnd, CharToId((wchar_t)wParam));
        return 0;

    case WM_KEYDOWN:
        Activate(hwnd, KeyToId(wParam));
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ============================================
   Entry Point
   ============================================ */
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_     LPWSTR    /*lpCmdLine*/,
    _In_     int       nCmdShow)
{
    /* თანმიმდევრობა მნიშვნელოვანია:
       1. GDI+ ირთვება
       2. Renderer იქმნება (მისი ფონტები GDI+-ს საჭიროებს)
       ფუნქციის ბოლოს ნადგურდება საპირისპირო რიგით:
       ჯერ Renderer, მერე GDI+ ითიშება */
    GdiplusSession gdiplus;
    Renderer       renderer;
    g_renderer = &renderer;

    /* ---- Window class ---- */
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"CalculatorV3Window";

    if (!RegisterClassExW(&wc))
        return 0;

    /* ---- ფანჯრის ზომა: კლიენტის არე ზუსტად WIN_W × WIN_H ---- */
    DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    RECT  rc = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&rc, style, FALSE);

    HWND hwnd = CreateWindowExW(
        0, wc.lpszClassName, L"Calculator", style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
        return 0;

    ApplyDarkTitleBar(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* ---- Message loop ---- */
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_renderer = nullptr;
    return (int)msg.wParam;
}