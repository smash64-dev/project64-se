#pragma once
#include "DebuggerUI.h"

#define STACKTRACE_MAX_ENTRIES 100

typedef struct {
    uint32_t routineAddress;
    uint32_t callingAddress;
} STACKTRACE_ENTRY;

class CDebugStackTrace :
    public CDebugDialog<CDebugStackTrace>,
    public CDialogResize<CDebugStackTrace>,
    private CDebugSettings
{
public:
    enum { IDD = IDD_Debugger_StackTrace };

    CDebugStackTrace(CDebuggerUI * debugger);
    virtual ~CDebugStackTrace(void);

    void Refresh();

    inline void PushEntry(uint32_t routineAddress, uint32_t callingAddress)
    {
        if (m_EntriesIndex < STACKTRACE_MAX_ENTRIES)
        {
            m_Entries[m_EntriesIndex] = { routineAddress, callingAddress };
            m_EntriesIndex++;
        }
    }

    inline void PopEntry()
    {
        if (m_EntriesIndex > 0)
        {
            m_EntriesIndex--;
        }
    }

    inline void ClearEntries()
    {
        m_EntriesIndex = 0;
    }

private:

    STACKTRACE_ENTRY m_Entries[STACKTRACE_MAX_ENTRIES];
    int m_EntriesIndex;

    CListViewCtrl m_List;

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnListDblClicked(NMHDR* pNMHDR);
    LRESULT OnDestroy(void);
    void OnExitSizeMove(void);

    BEGIN_MSG_MAP_EX(CDebugStackTrace)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        COMMAND_CODE_HANDLER(BN_CLICKED, OnClicked)
        NOTIFY_HANDLER_EX(IDC_STACKTRACE_LIST, NM_DBLCLK, OnListDblClicked)
        CHAIN_MSG_MAP(CDialogResize<CDebugStackTrace>)
        MSG_WM_EXITSIZEMOVE(OnExitSizeMove)
        END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(CDebugStackTrace)
        DLGRESIZE_CONTROL(IDC_STACKTRACE_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()
};
