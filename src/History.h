#pragma once
#include "calc.h"

/* ============================================
   History — გამოთვლების ისტორია
   ინახავს მეხსიერებაში (პანელისთვის) და
   პარალელურად წერს history.txt-ში
   ============================================ */
class History {
public:
    static constexpr size_t MaxEntries = 50;   /* მეხსიერებაში მაქსიმუმ */

    void Add(const std::wstring& expression, const std::wstring& result);
    void Clear();

    const std::vector<HistoryEntry>& Entries() const { return m_entries; }

private:
    void AppendToFile(const std::wstring& expression, const std::wstring& result);

    std::vector<HistoryEntry> m_entries;       /* უახლესი — პირველი */
};