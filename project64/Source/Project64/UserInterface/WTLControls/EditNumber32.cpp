#include "stdafx.h"
#include "EditNumber32.h"

CEditNumber32::CEditNumber32(void) :
    m_DisplayType(DisplayDec)
{
}

CEditNumber32::~CEditNumber32(void)
{
}

bool CEditNumber32::IsHexConvertableText(LPTSTR _text)
{
    int start, end;
    GetSel(start, end);

    std::string WindowText = GetCWindowText(*this);
    bool bPaste = true;
    size_t Len = WindowText.size();
    char head = Len > 0 ? WindowText[0] : 0;
    char second = Len > 1 ? WindowText[1] : 0;

    if (second == 'X' || second == 'x')
    {
        if (end <= 1)
        {
            bPaste = false;
        }
    }
    if (!bPaste) { return bPaste; }
    // Check
    unsigned int i = 0;
    if (wcslen(_text) >= 2)
    {
        if (_text[0] == L'0' && (_text[1] == L'x' || _text[1] == L'X'))
        {
            if ((second == L'x' || second == L'X') && (!(start == 0 && end >= 2)))
            {
                bPaste = false;
            }
            else if (start > 0)
            {
                bPaste = false;
            }
            else
            {
                i += 2;
            }
        }
    }
    if (!bPaste) return bPaste;
    if (wcslen(_text) >= 1)
    {
        if (head == L'0' && (_text[0] == L'x' || _text[0] == L'X'))
        {
            i++;
        }
        if ((_text[0] == L'x' || _text[0] == L'X'))
        {
            if (head != L'0' && start == 0)
            {
                bPaste = false;
            }
            else if (!(start == 1 && end >= 1 && head == L'0'))
            {
                bPaste = false;
            }
        }
    }
    if (!bPaste) return bPaste;
    for (; i < wcslen(_text); i++)
    {
        wchar_t c = _text[i];
        if (!(c >= 48 && c <= 57 || c >= L'A'&&c <= L'F' || c >= L'a'&&c <= L'f' || c == L' '))
        {
            bPaste = false;
            break;
        }
    }
    return bPaste;
}

void CEditNumber32::FormatClipboard()
{
    LPTSTR  lptstr, lptstrCopy;
    HGLOBAL hglb;
    if (!this->OpenClipboard() || !IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        return;
    }
    hglb = GetClipboardData(CF_UNICODETEXT);
    if (hglb != nullptr)
    {
        lptstr = (LPTSTR)GlobalLock(hglb);
        for (unsigned int i = 0; i < wcslen(lptstr); i++)
        {
            if (lptstr[i] != L'X'&&lptstr[i] != L'x')
            {
                lptstr[i] = (char)toupper(lptstr[i]);
            }
            if (lptstr[i] == L'X')
            {
                lptstr[i] = L'x';
            }
            if (lptstr[i] == ' ' && (i < wcslen(lptstr)))
            {
                wcscpy(&lptstr[i], &lptstr[i + 1]);
            }
        }
        hglb = GlobalAlloc(GMEM_MOVEABLE, (wcslen(lptstr) + 1) * sizeof(TCHAR));
        if (hglb == nullptr)
        {
            CloseClipboard();
            return;
        }
        lptstrCopy = (LPTSTR)GlobalLock(hglb);
        memcpy(lptstrCopy, lptstr, (wcslen(lptstr) + 1) * sizeof(TCHAR));
        GlobalUnlock(lptstr);
        GlobalUnlock(hglb);
        SetClipboardData(CF_UNICODETEXT, hglb);
        CloseClipboard();
    }
}

LRESULT CEditNumber32::OnValidateValue(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = true;
    return true;
}

LRESULT CEditNumber32::OnPaste(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    // Paste
    bHandled = false;

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        bHandled = true;
        return true;
    }
    if (!OpenClipboard())
    {
        bHandled = true;
        return true;
    }

    HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
    if (hglb != nullptr)
    {
        LPTSTR  lptstr = (LPTSTR)GlobalLock(hglb);
        // Check invalid hex string
        if (!IsHexConvertableText(lptstr))
        {
            bHandled = true;
        }
        GlobalUnlock(lptstr);
    }
    CloseClipboard();
    if (!bHandled)
    {
        FormatClipboard();
    }
    return true;
}

LRESULT CEditNumber32::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    int start, end;
    GetSel(start, end);
    std::string WindowText = GetCWindowText(*this);
    char Head = WindowText.length() > 0 ? WindowText[0] : 0;
    char Second = WindowText.length() > 1 ? WindowText[1] : 0;

    if (uMsg == WM_CHAR)
    {
        size_t MaxLen = 30;

        if (m_DisplayType == DisplayHex)
        {
            MaxLen = 8;
            if (Second == L'x' || Second == L'X')
            {
                MaxLen += 2;
            }
        }
        wchar_t c = (wchar_t)wParam;
        if (wParam < 32)
        {
            if (wParam == 8 && (Second == 'x' || Second == 'X') && Head == '0' && end == 1)
            {
                // Does not allow to delete '0' before x
                bHandled = true;
            }
            else {
                bHandled = false;
            }
            return TRUE;
        }

        if (Second == 'x' || Second == 'X')
        {
            // Does not allow to change head except select includes first and second
            if (start <= 1 && end <= 1)
            {
                bHandled = true;
                return TRUE;
            }
        }
        if (start == 1 && (c == 'X' || c == 'x') && Head == '0')
        {
            if (c == 'X')
            {
                SendMessage(uMsg, L'x', lParam);
                bHandled = true;
            }
            else {
                bHandled = false;
            }
            return true;
        }
        else if (c >= L'0' && c <= L'9' || c >= L'A' && c <= L'F')
        {
            if (WindowText.length() >= MaxLen && start == end)
            {
                bHandled = true;
                return true;
            }
            bHandled = false;
            return true;
        }
        else if (c >= L'a' && c <= L'f')
        {
            if (WindowText.length() >= MaxLen && start == end)
            {
                bHandled = true;
                return true;
            }
            SendMessage(uMsg, wParam - 32, lParam);
            bHandled = true;
            return true;
        }
        bHandled = true;
        return true;
    }

    bHandled = false;
    return false;
}

BOOL CEditNumber32::Attach(HWND hWndNew)
{
    return SubclassWindow(hWndNew);
}

BOOL CEditNumber32::AttachToDlgItem(HWND parent, UINT dlgID)
{
    return SubclassWindow(::GetDlgItem(parent, dlgID));
}

void CEditNumber32::SetDisplayType(DisplayType Type)
{
    DWORD lCurrentValue = GetValue();
    m_DisplayType = Type;
    SetValue(lCurrentValue);
}

uint32_t CEditNumber32::GetValue(void)
{
    std::string Text = GetCWindowText(*this);
    if (m_DisplayType == DisplayDec)
    {
        return atoi(Text.c_str());
    }

    size_t Finish = Text.length();
    wchar_t Second = Finish > 1 ? Text[1] : 0;
    size_t Start = (Second == 'x' || Second == 'X') ? 2 : 0;

    if (Finish > 8 + Start) { Finish = 8 + Start; }

    DWORD Value = 0;
    for (size_t i = Start; i < Finish; i++)
    {
        Value = (Value << 4);
        switch (Text[i])
        {
        case '0': break;
        case '1': Value += 1; break;
        case '2': Value += 2; break;
        case '3': Value += 3; break;
        case '4': Value += 4; break;
        case '5': Value += 5; break;
        case '6': Value += 6; break;
        case '7': Value += 7; break;
        case '8': Value += 8; break;
        case '9': Value += 9; break;
        case 'A': Value += 10; break;
        case 'a': Value += 10; break;
        case 'B': Value += 11; break;
        case 'b': Value += 11; break;
        case 'C': Value += 12; break;
        case 'c': Value += 12; break;
        case 'D': Value += 13; break;
        case 'd': Value += 13; break;
        case 'E': Value += 14; break;
        case 'e': Value += 14; break;
        case 'F': Value += 15; break;
        case 'f': Value += 15; break;
        default:
            Value = (Value >> 4);
            i = Finish;
        }
    }
    return Value;
}

void CEditNumber32::SetValue(uint32_t Value, DisplayMode Display)
{
    stdstr text;
    if (m_DisplayType == DisplayDec)
    {
        text.Format("%d", Value);
    }
    else
    {
        text.Format("%s%0*X",(Display & DisplayMode::ShowHexIdent) == DisplayMode::ShowHexIdent ? "0x" : "", (Display & DisplayMode::ZeroExtend) == DisplayMode::ZeroExtend ? 8 : 0, Value);
    }
    SetWindowText(text.ToUTF16().c_str());
}
