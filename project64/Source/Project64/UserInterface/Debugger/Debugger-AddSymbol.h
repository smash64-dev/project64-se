#pragma once

class CAddSymbolDlg : public CDialogImpl<CAddSymbolDlg>
{
public:
    enum { IDD = IDD_Debugger_AddSymbol };

    INT_PTR DoModal(CDebuggerUI* debugger);
    INT_PTR DoModal(CDebuggerUI* debugger, uint32_t initAddress);
    INT_PTR DoModal(CDebuggerUI* debugger, uint32_t initAddress, int initType);

private:
    CDebuggerUI* m_Debugger;

    bool m_bHaveAddress;
    bool m_bHaveType;
    uint32_t m_InitAddress;
    int m_InitType;

    CEditNumber32 m_AddressEdit;
    CComboBox   m_TypeComboBox;
    CEdit       m_NameEdit;
    CEdit       m_DescriptionEdit;

    LRESULT OnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(void)
    {
        return 0;
    }

    BEGIN_MSG_MAP_EX(CAddSymbolDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_CODE_HANDLER(BN_CLICKED, OnClicked)
        MSG_WM_DESTROY(OnDestroy)
        END_MSG_MAP()
};
