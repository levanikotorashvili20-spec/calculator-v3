#pragma once
#include "calc.h"
#include "History.h"

/* ============================================
   Calculator — კალკულატორის მთელი ლოგიკა

   Windows-ის შესახებ არაფერი იცის: არც ფანჯარა,
   არც ღილაკები, არც ხატვა. მხოლოდ ID-ს იღებს
   და ეკრანისთვის ტექსტს აბრუნებს.
   ============================================ */
class Calculator {
public:
    void HandleInput(int id);

    const std::wstring& GetDisplay()    const { return m_input; }
    const std::wstring& GetExpression() const { return m_expression; }
    const std::vector<HistoryEntry>& GetHistory() const { return m_history.Entries(); }

    void ClearHistory() { m_history.Clear(); }

private:
    /* --- რიცხვის აკრეფა --- */
    void InputDigit(int digit);
    void InputDot();
    void Backspace();
    void ToggleSign();

    /* --- ბინარული ოპერაციები (a op b) --- */
    void SetOperator(Operation op);
    void Equal();
    bool Compute();

    /* --- უნარული ოპერაციები (ერთ რიცხვზე) --- */
    void Percent();
    void SquareRoot();
    void Factorial();
    void ApplyUnary(const std::wstring& expr, double result);

    /* --- დამხმარე --- */
    void   Reset();
    void   SetError(const std::wstring& reason);
    void   BeginNewEntry();
    double CurrentValue() const;
    static std::wstring FormatNumber(double v);

    static constexpr size_t MaxInputLength = 16;

    /* --- მდგომარეობა (საწყისი მნიშვნელობებით) --- */
    std::wstring m_input = L"0";     /* ეკრანზე დიდად */
    std::wstring m_expression;            /* ეკრანზე წვრილად: "15 + 12 =" */
    double       m_stored = 0.0;      /* პირველი რიცხვი (ოპერატორამდე) */
    Operation    m_op = OP_NONE;
    bool         m_newNumber = true;     /* შემდეგი ციფრი ახალ რიცხვს იწყებს */
    bool         m_hasOperand = false;    /* ოპერატორის შემდეგ მეორე რიცხვი შეიყვანეს */
    bool         m_error = false;

    History      m_history;
};