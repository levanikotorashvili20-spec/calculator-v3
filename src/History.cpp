#include "History.h"
#include <fstream>

/* ============================================
   wstring → UTF-8 (ფაილში სწორად ჩასაწერად)
   √ და ± სიმბოლოები ჩვეულებრივ char-ში არ ეტევა
   ============================================ */
static std::string ToUtf8(const std::wstring& w)
{
    if (w.empty()) return {};

    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
        nullptr, 0, nullptr, nullptr);
    std::string s(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
        &s[0], len, nullptr, nullptr);
    return s;
}

/* ============================================
   ახალი ჩანაწერის დამატება
   ============================================ */
void History::Add(const std::wstring& expression, const std::wstring& result)
{
    /* თავში ვამატებთ — პანელში უახლესი ზემოთ გამოჩნდება */
    m_entries.insert(m_entries.begin(), HistoryEntry{ expression, result });

    if (m_entries.size() > MaxEntries)
        m_entries.pop_back();                  /* ყველაზე ძველი იშლება */

    AppendToFile(expression, result);
}

void History::Clear()
{
    m_entries.clear();
}

/* ============================================
   ფაილში ჩაწერა:  [15:24:44] 15 + 12 = 27
   ============================================ */
void History::AppendToFile(const std::wstring& expression, const std::wstring& result)
{
    std::ofstream file("history.txt", std::ios::app);
    if (!file) return;

    SYSTEMTIME st;
    GetLocalTime(&st);

    char time[16];
    sprintf_s(time, "[%02d:%02d:%02d] ", st.wHour, st.wMinute, st.wSecond);

    file << time << ToUtf8(expression) << " " << ToUtf8(result) << "\n";

}   /* ← აქ file ავტომატურად იხურება */