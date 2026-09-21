#include "Calculator.h"
#include <cmath>
#include <cwchar>

/* ============================================
   ოპერაციის სიმბოლო ეკრანისთვის
   \u — Unicode escape, რომ ფაილის კოდირებაზე
   დამოკიდებული არ ვიყოთ
   ============================================ */
static const wchar_t* OpSymbol(Operation op)
{
    switch (op) {
    case OP_ADD: return L"+";
    case OP_SUB: return L"\u2212";   /* − */
    case OP_MUL: return L"\u00D7";   /* × */
    case OP_DIV: return L"\u00F7";   /* ÷ */
    default:     return L"";
    }
}

/* ============================================
   მთავარი დისპეტჩერი
   ============================================ */
void Calculator::HandleInput(int id)
{
    bool isDigit = (id >= ID_0 && id <= ID_9);

    /* შეცდომის მდგომარეობაში: C ასუფთავებს, ციფრი თავიდან იწყებს,
       დანარჩენი იგნორირდება */
    if (m_error) {
        if (id == ID_CLEAR) { Reset(); return; }
        if (isDigit || id == ID_DOT) Reset();
        else                         return;
    }

    if (isDigit) {
        InputDigit(id - ID_0);
        return;
    }

    switch (id) {
    case ID_DOT:       InputDot();          break;
    case ID_ADD:       SetOperator(OP_ADD); break;
    case ID_SUB:       SetOperator(OP_SUB); break;
    case ID_MUL:       SetOperator(OP_MUL); break;
    case ID_DIV:       SetOperator(OP_DIV); break;
    case ID_EQUAL:     Equal();             break;
    case ID_CLEAR:     Reset();             break;
    case ID_BACKSPACE: Backspace();         break;
    case ID_SIGN:      ToggleSign();        break;
    case ID_PERCENT:   Percent();           break;
    case ID_SQRT:      SquareRoot();        break;
    case ID_FACT:      Factorial();         break;
    }
}

/* ============================================
   რიცხვის აკრეფა
   ============================================ */
void Calculator::BeginNewEntry()
{
    /* თუ ოპერაცია არ მიმდინარეობს, წინა გამოთვლის ზედა ხაზი ქრება */
    if (m_op == OP_NONE)
        m_expression.clear();
    m_newNumber = false;
}

void Calculator::InputDigit(int digit)
{
    wchar_t ch = static_cast<wchar_t>(L'0' + digit);

    if (m_newNumber) {
        BeginNewEntry();
        m_input = ch;
    }
    else if (m_input == L"0") {
        m_input = ch;                          /* "0" → "5", არა "05" */
    }
    else if (m_input == L"-0") {
        m_input = std::wstring(L"-") + ch;
    }
    else {
        if (m_input.size() >= MaxInputLength) return;
        m_input += ch;
    }
    m_hasOperand = true;
}

void Calculator::InputDot()
{
    if (m_newNumber) {
        BeginNewEntry();
        m_input = L"0.";
    }
    else {
        if (m_input.find(L'.') != std::wstring::npos) return;   /* მეორე წერტილი არა */
        if (m_input.size() >= MaxInputLength) return;
        m_input += L'.';
    }
    m_hasOperand = true;
}

void Calculator::Backspace()
{
    if (m_newNumber) return;                   /* შედეგს არ ვარედაქტირებთ */

    m_input.pop_back();
    if (m_input.empty() || m_input == L"-")
        m_input = L"0";
}

void Calculator::ToggleSign()
{
    if (m_input == L"0") return;

    if (m_input[0] == L'-') m_input.erase(0, 1);
    else                    m_input.insert(0, 1, L'-');

    m_hasOperand = true;
}

/* ============================================
   ბინარული ოპერაციები
   ============================================ */
void Calculator::SetOperator(Operation op)
{
    /* ჯაჭვი: 2 + 3 + ...  →  მეორე '+'-ზე ჯერ 2 + 3 სრულდება */
    if (m_op != OP_NONE && m_hasOperand) {
        if (!Compute()) return;
    }

    m_stored = CurrentValue();
    m_op = op;
    m_newNumber = true;
    m_hasOperand = false;
    m_expression = FormatNumber(m_stored) + L" " + OpSymbol(op);
}

void Calculator::Equal()
{
    if (m_op == OP_NONE) return;
    if (!Compute()) return;

    m_op = OP_NONE;
    m_newNumber = true;
    m_hasOperand = false;
}

/* a op b — შედეგი m_input-ში, ჩანაწერი ისტორიაში */
bool Calculator::Compute()
{
    double a = m_stored;
    double b = CurrentValue();
    double r = 0.0;

    switch (m_op) {
    case OP_ADD: r = a + b; break;
    case OP_SUB: r = a - b; break;
    case OP_MUL: r = a * b; break;
    case OP_DIV:
        if (b == 0.0) {
            SetError(L"Cannot divide by zero");
            return false;
        }
        r = a / b;
        break;
    default:
        return true;
    }

    if (!std::isfinite(r)) {
        SetError(L"Overflow");
        return false;
    }

    std::wstring expr = FormatNumber(a) + L" " + OpSymbol(m_op) + L" "
        + FormatNumber(b) + L" =";
    std::wstring res = FormatNumber(r);

    m_history.Add(expr, res);
    m_expression = expr;
    m_input = res;
    return true;
}

/* ============================================
   უნარული ოპერაციები
   ============================================ */
void Calculator::Percent()
{
    double v = CurrentValue();

    /* 200 + 10%  →  10% 200-დან = 20
       50 × 10%   →  0.1
       50%        →  0.5 */
    double r = (m_op == OP_ADD || m_op == OP_SUB)
        ? m_stored * v / 100.0
        : v / 100.0;

    ApplyUnary(FormatNumber(v) + L"%", r);
}

void Calculator::SquareRoot()
{
    double v = CurrentValue();
    if (v < 0.0) {
        SetError(L"Invalid input");
        return;
    }
    ApplyUnary(std::wstring(L"\u221A(") + FormatNumber(v) + L")", std::sqrt(v));   /* √ */
}

void Calculator::Factorial()
{
    double v = CurrentValue();

    if (v < 0.0 || v != std::floor(v)) {       /* მხოლოდ მთელი, არაუარყოფითი */
        SetError(L"Invalid input");
        return;
    }
    if (v > 170.0) {                           /* double-ის ზღვარი */
        SetError(L"Overflow");
        return;
    }

    double r = 1.0;
    for (int i = 2; i <= static_cast<int>(v); ++i)
        r *= i;

    ApplyUnary(FormatNumber(v) + L"!", r);
}

/* უნარული შედეგის საერთო დამუშავება */
void Calculator::ApplyUnary(const std::wstring& expr, double result)
{
    if (!std::isfinite(result)) {
        SetError(L"Overflow");
        return;
    }

    std::wstring res = FormatNumber(result);

    if (m_op == OP_NONE) {
        /* დამოუკიდებელი გამოთვლა: √(9) = 3 → ისტორიაში */
        m_expression = expr + L" =";
        m_history.Add(m_expression, res);
    }
    else {
        /* ოპერაციის შუაში: 200 + 10% → ისტორიაში '='-ზე მოხვდება */
        m_expression = FormatNumber(m_stored) + L" " + OpSymbol(m_op) + L" " + expr;
    }

    m_input = res;
    m_newNumber = true;
    m_hasOperand = true;
}

/* ============================================
   დამხმარე
   ============================================ */
void Calculator::Reset()
{
    m_input = L"0";
    m_expression.clear();
    m_stored = 0.0;
    m_op = OP_NONE;
    m_newNumber = true;
    m_hasOperand = false;
    m_error = false;
}

void Calculator::SetError(const std::wstring& reason)
{
    m_error = true;
    m_input = L"Error";
    m_expression = reason;                     /* მიზეზი ზედა, წვრილ ხაზზე */
    m_op = OP_NONE;
    m_newNumber = true;
    m_hasOperand = false;
}

double Calculator::CurrentValue() const
{
    return std::wcstod(m_input.c_str(), nullptr);
}

/* 12 მნიშვნელოვანი ციფრი, ზედმეტი ნულების გარეშე:
   27.0 → "27",  0.1 + 0.2 → "0.3",  20! → "2.43290200818e+18" */
std::wstring Calculator::FormatNumber(double v)
{
    if (v == 0.0) v = 0.0;                     /* -0 → 0 */

    wchar_t buf[64];
    swprintf_s(buf, L"%.12g", v);
    return buf;
}