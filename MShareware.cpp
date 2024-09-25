// MShareware.cpp -- The shareware maker for Win32
////////////////////////////////////////////////////////////////////////////

#ifndef NO_SHAREWARE

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <cassert>
#include <vector>
#include <cctype>

#include "MSha256.h"
#include "MShareware.h"

#define sw_shareware_max_password 256

////////////////////////////////////////////////////////////////////////////
// Load resource string

LPTSTR SwLoadStringDx1(HINSTANCE hInstance, UINT uID)
{
    static TCHAR s_sz[2048];
    s_sz[0] = 0;
    ::LoadString(hInstance, uID, s_sz, 2048);
    assert(::lstrlen(s_sz) < 2048);
    return s_sz;
}

LPTSTR SwLoadStringDx2(HINSTANCE hInstance, UINT uID)
{
    static TCHAR s_sz[2048];
    s_sz[0] = 0;
    ::LoadString(hInstance, uID, s_sz, 2048);
    assert(::lstrlen(s_sz) < 2048);
    return s_sz;
}

////////////////////////////////////////////////////////////////////////////
// Centering dialog

void SwCenterDialog(HWND hwnd)
{
    assert(hwnd);
    assert(::IsWindow(hwnd));
    BOOL bChild = !!(::GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD);

    HWND hwndOwner = (bChild ? ::GetParent(hwnd) : ::GetWindow(hwnd, GW_OWNER));

    RECT rc, rcOwner;
    if (hwndOwner)
        ::GetWindowRect(hwndOwner, &rcOwner);
    else
        ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcOwner, 0);

    ::GetWindowRect(hwnd, &rc);

    POINT pt;
    pt.x = rcOwner.left + ((rcOwner.right - rcOwner.left) - (rc.right - rc.left)) / 2;
    pt.y = rcOwner.top + ((rcOwner.bottom - rcOwner.top) - (rc.bottom - rc.top)) / 2;

    if (bChild && hwndOwner != NULL)
        ::ScreenToClient(hwndOwner, &pt);

    ::SetWindowPos(hwnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    ::SendMessage(hwnd, DM_REPOSITION, 0, 0);
}

////////////////////////////////////////////////////////////////////////////
// Centering message box

static LRESULT CALLBACK
SwMsgBoxCbtProc_(int nCode, WPARAM wParam, LPARAM /*lParam*/)
{
    if (nCode == HCBT_ACTIVATE)
    {
        HWND hwnd = (HWND)wParam;
        TCHAR szClassName[MAX_PATH];
        ::GetClassName(hwnd, szClassName, MAX_PATH);
        if (lstrcmpi(szClassName, TEXT("#32770")) == 0)
        {
            SwCenterDialog(hwnd);
        }
    }
    return 0;
}

int SwCenterMessageBox(HWND hwndParent, LPCTSTR pszText, LPCTSTR pszCaption, UINT uMB_)
{
    static HHOOK s_hMsgBoxHook = NULL;

    if (s_hMsgBoxHook != NULL && ::UnhookWindowsHookEx(s_hMsgBoxHook))
        s_hMsgBoxHook = NULL;

    HINSTANCE hInst = reinterpret_cast<HINSTANCE>(
        ::GetWindowLongPtr(hwndParent, GWLP_HINSTANCE));

    DWORD dwThreadID = ::GetCurrentThreadId();
    s_hMsgBoxHook = ::SetWindowsHookEx(WH_CBT, SwMsgBoxCbtProc_, hInst, dwThreadID);

    int nID = ::MessageBox(hwndParent, pszText, pszCaption, uMB_);

    if (s_hMsgBoxHook != NULL && ::UnhookWindowsHookEx(s_hMsgBoxHook))
        s_hMsgBoxHook = NULL;

    return nID;
}

////////////////////////////////////////////////////////////////////////////
// MShareware class

DWORD MShareware::GetTrialDays() const
{
    return m_dwTrialDays;
}

bool MShareware::IsRegistered() const
{
    return m_status == MShareware::REGD;
}

bool MShareware::IsInTrial() const
{
    return m_status == MShareware::IN_TRIAL ||
           m_status == MShareware::IN_TRIAL_FIRST_TIME;
}

bool MShareware::IsOutOfTrial() const
{
    return m_status == MShareware::OUT_OF_TRIAL;
}

void MShareware::OnTrialFirstTime(HWND hwndParent)
{
}

void MShareware::OnTrial(HWND hwndParent)
{
    UrgeRegister(hwndParent);
}

bool MShareware::OnOutOfTrial(HWND hwndParent)
{
    return UrgeRegister(hwndParent);
}

void MShareware::ThisCommandRequiresRegistering(HWND hwndParent)
{
    ShowErrorMessage(hwndParent, 32737);
}

void MShareware::ShowErrorMessage(HWND hwndParent, UINT uStringID)
{
    SwCenterMessageBox(hwndParent, SwLoadStringDx2(m_hInstance, uStringID), NULL, MB_ICONERROR);
}

MShareware::MShareware(
    LPCTSTR pszAppKey,
    const char *pszSha256HashHexString,
    DWORD dwTrialDays/* = 15*/,
    const char *salt/* = ""*/,
    const char *new_version/* = ""*/)
    : m_hInstance(::GetModuleHandle(NULL))
    , m_dwTrialDays(dwTrialDays)
    , m_status(MShareware::IN_TRIAL)
    , m_strAppKey(pszAppKey)
    , m_strSha256HashHexString(pszSha256HashHexString)
    , m_strSalt(salt)
    , m_strNewVersion(new_version)
    , m_strOldVersion()
{
}

MShareware::MShareware(
    LPCTSTR pszAppKey,
    const BYTE *pbHash32Bytes,
    DWORD dwTrialDays/* = 15*/,
    const char *salt/* = ""*/,
    const char *new_version/* = ""*/)
    : m_hInstance(::GetModuleHandle(NULL))
    , m_dwTrialDays(dwTrialDays)
    , m_status(MShareware::IN_TRIAL)
    , m_strAppKey(pszAppKey)
    , m_strSha256HashHexString()
    , m_strSalt(salt)
    , m_strNewVersion(new_version)
    , m_strOldVersion()
{
    mzc_hex_string_from_bytes(m_strSha256HashHexString, &pbHash32Bytes[0], &pbHash32Bytes[32]);
}

MShareware::~MShareware()
{
}

bool MShareware::Start(HWND hwndParent/* = NULL*/)
{
    CheckRegistry(hwndParent);

    switch (m_status)
    {
    case MShareware::IN_TRIAL_FIRST_TIME:
        OnTrialFirstTime(hwndParent);
        return true;

    case MShareware::IN_TRIAL:
        OnTrial(hwndParent);
        return true;

    case MShareware::REGD:
        return true;

    case MShareware::OUT_OF_TRIAL:
        return OnOutOfTrial(hwndParent);

    default:
        return false;
    }
}

void SwUrgeRegisterDlg_OnInit(HWND hwnd, MShareware *pShareware)
{
    LPTSTR pszMsg;
    TCHAR szMsg[MAX_PATH * 3];
    pShareware->CheckDate();

    switch (pShareware->m_status)
    {
    case MShareware::IN_TRIAL_FIRST_TIME:
        pszMsg = SwLoadStringDx1(pShareware->m_hInstance, 32732);
        ::wsprintf(szMsg, pszMsg, pShareware->m_dwTrialDays);
        ::SetDlgItemText(hwnd, edt1, szMsg);
        break;

    case MShareware::IN_TRIAL:
        if (pShareware->m_dwlTotalMinutesRemains >= 60 * 24)
        {
            pszMsg = SwLoadStringDx1(pShareware->m_hInstance, 32732);
            ::wsprintf(szMsg, pszMsg, static_cast<DWORD>(pShareware->m_dwlTotalMinutesRemains / (60 * 24)));
            ::SetDlgItemText(hwnd, edt1, szMsg);
        }
        else
        {
            pszMsg = SwLoadStringDx1(pShareware->m_hInstance, 32733);
            ::wsprintf(szMsg, pszMsg, static_cast<DWORD>(pShareware->m_dwlTotalMinutesRemains / 60),
                                      static_cast<DWORD>(pShareware->m_dwlTotalMinutesRemains % 60));
            ::SetDlgItemText(hwnd, edt1, szMsg);
        }
        break;

    case MShareware::OUT_OF_TRIAL:
        pszMsg = SwLoadStringDx1(pShareware->m_hInstance, 32734);
        ::SetDlgItemText(hwnd, edt1, pszMsg);
        break;

    case MShareware::REGD:
        ::EnableWindow(::GetDlgItem(hwnd, edt2), FALSE);
        ::EnableWindow(::GetDlgItem(hwnd, IDOK), FALSE);
        pszMsg = SwLoadStringDx1(pShareware->m_hInstance, 32735);
        ::SetDlgItemText(hwnd, edt1, pszMsg);
        ::SetFocus(::GetDlgItem(hwnd, IDCANCEL));
        break;
    }
}

static INT_PTR CALLBACK
SwUrgeRegisterDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static MShareware *s_pShareware = NULL;
    char szPassword[sw_shareware_max_password];

    switch (uMsg)
    {
    case WM_INITDIALOG:
        SwCenterDialog(hwnd);
        s_pShareware = reinterpret_cast<MShareware *>(lParam);
        SwUrgeRegisterDlg_OnInit(hwnd, s_pShareware);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            szPassword[0] = 0;
            ::GetDlgItemTextA(hwnd, edt2, szPassword, sw_shareware_max_password);
            if (s_pShareware->RegisterPassword(hwnd, szPassword))
            {
                SwUrgeRegisterDlg_OnInit(hwnd, s_pShareware);
                TCHAR szTitle[256];
                ::GetWindowText(hwnd, szTitle, 256);
                SwCenterMessageBox(hwnd, SwLoadStringDx1(s_pShareware->m_hInstance, 32735),
                                   szTitle, MB_ICONINFORMATION);
            }
            else
            {
                s_pShareware->ShowErrorMessage(hwnd, 32731);
            }
            break;

        case IDCANCEL:
            ::EndDialog(hwnd, IDCANCEL);
            break;
        }
        break;

    case WM_NOTIFY:
        {
            NMHDR *pnmhdr = reinterpret_cast<NMHDR *>(lParam);
            switch (pnmhdr->code)
            {
            case NM_CLICK:
                if (pnmhdr->idFrom == stc1)
                {
                    // Extract URL from stc1 text
                    char szText[MAX_PATH];
                    GetDlgItemTextA(hwnd, stc1, szText, _countof(szText));
                    auto ptr0 = strstr(szText, "<a>");
                    ptr0 += strlen("<a>");
                    auto ptr1 = strstr(szText, "</a>");
                    *ptr1 = 0;
                    // Open the URL
                    ShellExecuteA(hwnd, NULL, ptr0, NULL, NULL, SW_SHOWNORMAL);
                }
                break;
            }
        }
    }
    return 0;
}

bool MShareware::UrgeRegister(HWND hwndParent/* = NULL*/)
{
    ::DialogBoxParam(m_hInstance, MAKEINTRESOURCE(32731), hwndParent, SwUrgeRegisterDlgProc,
                     reinterpret_cast<LPARAM>(this));
    return IsRegistered();
}

bool MShareware::CheckRegistry(HWND hwndParent)
{
    LONG error;
    HKEY hkeyApp;
    DWORD dwDisp;
    bool bHasAppKey = false, bCanUse = false;

    error = ::RegCreateKeyEx(HKEY_CURRENT_USER, m_strAppKey.data(), 0, NULL, 0,
                             KEY_READ | KEY_WRITE, NULL, &hkeyApp, &dwDisp);
    if (!error)
    {
        bHasAppKey = (dwDisp == REG_OPENED_EXISTING_KEY);
        bCanUse = CheckAppKey(hwndParent, hkeyApp);
        ::RegCloseKey(hkeyApp);
    }

    if (!bHasAppKey)
    {
        SetRegistryFirstTime(hwndParent);
        m_status = MShareware::IN_TRIAL_FIRST_TIME;
        bCanUse = true;
    }

    return bCanUse;
}

bool MShareware::SetRegistryFirstTime(HWND hwndParent)
{
    LONG error;
    HKEY hkeyApp;
    DWORD dwDisp;
    bool bOK = false;

    error = ::RegCreateKeyEx(HKEY_CURRENT_USER, m_strAppKey.data(), 0, NULL, 0,
                             KEY_READ | KEY_WRITE, NULL, &hkeyApp, &dwDisp);
    if (!error)
    {
        FILETIME ft;
        ::GetSystemTimeAsFileTime(&ft);

        DWORD cb = static_cast<DWORD>(sizeof(ft));
        error = ::RegSetValueEx(hkeyApp, TEXT("SW_StartUse"), 0, REG_BINARY, reinterpret_cast<const BYTE *>(&ft), cb);
        if (!error)
        {
            error = ::RegQueryValueEx(hkeyApp, TEXT("SW_StartUse"), NULL, NULL, NULL, NULL);
            if (!error)
                bOK = true;
            else
                ShowErrorMessage(hwndParent, 32736);
        }

        ::RegDeleteValueA(hkeyApp, "SW_EncodedPassword");
        ::RegCloseKey(hkeyApp);
    }

    return bOK;
}

bool MShareware::RegisterPassword(HWND hwndParent, const char *pszPassword)
{
    LONG error;
    HKEY hkeyApp;
    DWORD dwDisp;
    bool bOK = false;

    // check password
    if (!IsPasswordValid(pszPassword))
    {
        ::Sleep(750);
        return false;
    }

    // duplicate and encode the password
#ifdef _MSC_VER
    char *pszEncodedPassword = _strdup(pszPassword);
#else
    char *pszEncodedPassword = strdup(pszPassword);
#endif
    DWORD size = static_cast<DWORD>(::lstrlenA(pszEncodedPassword));
    EncodePassword(pszEncodedPassword, size);

#ifndef NDEBUG
    DecodePassword(pszEncodedPassword, size);
    assert(memcmp(pszEncodedPassword, pszPassword, size) == 0);
    EncodePassword(pszEncodedPassword, size);
#endif

    error = ::RegCreateKeyEx(HKEY_CURRENT_USER, m_strAppKey.c_str(), 0, NULL, 0,
                             KEY_READ | KEY_WRITE, NULL, &hkeyApp, &dwDisp);
    if (!error)
    {
        bOK = true;

        // set password
        DWORD cb = size;
        error = ::RegSetValueExA(hkeyApp, "SW_EncodedPassword", 0, REG_BINARY,
                                 reinterpret_cast<const BYTE *>(pszEncodedPassword), cb);
        if (error)
        {
            bOK = false;
        }
        else
        {
            error = ::RegQueryValueExA(hkeyApp, "SW_EncodedPassword", NULL, NULL, NULL, NULL);
            if (error)
            {
                ShowErrorMessage(hwndParent, 32736);
                bOK = false;
            }
        }

        if (bOK)
        {
            // set check sum
            DWORD dwCheckSum = GetUserCheckSum();
            cb = static_cast<DWORD>(sizeof(DWORD));
            error = ::RegSetValueEx(hkeyApp, TEXT("SW_CheckSum"), 0, REG_DWORD,
                                    reinterpret_cast<const BYTE *>(&dwCheckSum), cb);
            if (error)
            {
                bOK = false;
            }
            else
            {
                error = ::RegQueryValueEx(hkeyApp, TEXT("SW_CheckSum"), NULL, NULL, NULL, NULL);
                if (error)
                {
                    ShowErrorMessage(hwndParent, 32736);
                    bOK = false;
                }
            }
        }

        ::RegCloseKey(hkeyApp);
    }

    if (bOK)
        m_status = MShareware::REGD;

    free(pszEncodedPassword);

    return bOK;
}

bool MShareware::CheckAppKey(HWND hwndParent, HKEY hkeyApp)
{
    LONG error;
    DWORD cb;
    FILETIME ft;

    m_dwlTotalMinutesRemains = 0;

    // check password
    char szPassword[sw_shareware_max_password];
    cb = sw_shareware_max_password - 1;
    error = ::RegQueryValueExA(hkeyApp, "SW_EncodedPassword", NULL, NULL,
                               reinterpret_cast<BYTE*>(&szPassword), &cb);
    if (!error)
    {
        szPassword[cb] = 0;
        DecodePassword(szPassword, cb);
        if (IsPasswordValid(szPassword))
        {
            // check check sum
            DWORD dwValue;
            cb = static_cast<DWORD>(sizeof(DWORD));
            error = ::RegQueryValueEx(hkeyApp, TEXT("SW_CheckSum"), NULL, NULL,
                                      reinterpret_cast<BYTE*>(&dwValue), &cb);
            if (error == ERROR_SUCCESS && dwValue == GetUserCheckSum())
            {
                m_status = MShareware::REGD;
                return true;
            }
        }
    }

    // check version
    char szOldVersion[64];
    cb = 64 - 1;
    error = ::RegQueryValueExA(hkeyApp, "SW_OldVersion", NULL, NULL,
                               reinterpret_cast<BYTE*>(&szOldVersion), &cb);
    if (!error)
    {
        m_strOldVersion = szOldVersion;

        // compare version
        if (CompareVersion(m_strOldVersion.data(), m_strNewVersion.data()) < 0)
        {
            m_status = MShareware::IN_TRIAL;

            // update version
            cb = static_cast<DWORD>(m_strNewVersion.size() + 1);
            error = ::RegSetValueExA(hkeyApp, "SW_OldVersion", 0, REG_SZ,
                reinterpret_cast<const BYTE *>(m_strNewVersion.data()), cb);
            assert(!error);

            // update date
            ::GetSystemTimeAsFileTime(&ft);
            m_ftStart = ft;
            cb = static_cast<DWORD>(sizeof(ft));
            error = ::RegSetValueEx(hkeyApp, TEXT("SW_StartUse"), 0, REG_BINARY,
                reinterpret_cast<const BYTE *>(&ft), cb);
            assert(!error);

            return true;
        }
    }
    else
    {
        // set version
        cb = static_cast<DWORD>(m_strNewVersion.size() + 1);
        error = ::RegSetValueExA(hkeyApp, "SW_OldVersion", 0, REG_SZ,
                                 reinterpret_cast<const BYTE *>(m_strNewVersion.data()), cb);
        assert(!error);
    }

    // check date
    cb = static_cast<DWORD>(sizeof(ft));
    error = ::RegQueryValueEx(hkeyApp, TEXT("SW_StartUse"), NULL, NULL,
                              reinterpret_cast<BYTE*>(&ft), &cb);
    if (!error)
    {
        m_ftStart = ft;
        if (!CheckDate())
        {
            m_status = MShareware::OUT_OF_TRIAL;
            return false;
        }
    }
    else
    {
        // set date
        ::GetSystemTimeAsFileTime(&ft);
        m_ftStart = ft;
        cb = static_cast<DWORD>(sizeof(ft));
        ::RegSetValueEx(hkeyApp, TEXT("SW_StartUse"), 0, REG_BINARY,
                        reinterpret_cast<BYTE *>(&ft), cb);

        // query date for registry check
        LONG error;
        error = ::RegQueryValueEx(hkeyApp, TEXT("SW_StartUse"), NULL, NULL, NULL, NULL);
        if (error)
        {
            ShowErrorMessage(hwndParent, 32736);
            return false;
        }
    }

    m_status = MShareware::IN_TRIAL;
    return true;
}

bool MShareware::IsPasswordValid(const char *pszPassword) const
{
    std::string str;
    MSha256::hex_from_passwd_and_salt(str, pszPassword, m_strSalt.data());
    return (str == m_strSha256HashHexString);
}

bool MShareware::CheckDate()
{
    ULARGE_INTEGER uli;

    uli.LowPart = m_ftStart.dwLowDateTime;
    uli.HighPart = m_ftStart.dwHighDateTime;
    DWORDLONG dwlStart = uli.QuadPart;

    FILETIME ftNow;
    ::GetSystemTimeAsFileTime(&ftNow);
    uli.LowPart = ftNow.dwLowDateTime;
    uli.HighPart = ftNow.dwHighDateTime;
    DWORDLONG dwlNow = uli.QuadPart;

    LONGLONG delta = GetTrialDays();
    delta *= 24 * 60 * 60;
    delta *= 10000000;

    if (dwlStart + delta > dwlNow)
    {
        m_dwlTotalMinutesRemains = (dwlStart + delta) - dwlNow;
        m_dwlTotalMinutesRemains /= 10000000;
        m_dwlTotalMinutesRemains /= 60;
        return true;
    }

    m_dwlTotalMinutesRemains = 0;
    return false;
}

DWORD MShareware::GetUserCheckSum() const
{
    DWORD dwCheckSum = 0xDeadFace;
    TCHAR szUser[64];
    DWORD dwSize = 64;
    if (::GetUserName(szUser, &dwSize))
    {
        LPTSTR pch = szUser;
        while (*pch)
        {
            dwCheckSum += *pch;
            ++pch;
        }
    }
    return dwCheckSum;
}

void MShareware::EncodePassword(void *pass, DWORD size) const
{
    // TODO:
    BYTE *pb = reinterpret_cast<BYTE *>(pass);
    while (size--)
    {
        *pb ^= 0xFF;
        pb++;
    }
}

void MShareware::DecodePassword(void *pass, DWORD size) const
{
    // TODO:
    BYTE *pb = reinterpret_cast<BYTE *>(pass);
    while (size--)
    {
        *pb ^= 0xFF;
        pb++;
    }
}

static void sw_explode(std::vector<std::string>& v, const std::string& separators, const std::string& s)
{
    size_t i = s.find_first_not_of(separators);
    size_t n = s.size();

    v.clear();
    while (i < n)
    {
        size_t stop = s.find_first_of(separators, i);
        if (stop > n) stop = n;
        v.push_back(s.substr(i, stop - i));
        i = s.find_first_not_of(separators, stop + 1);
    }
}

int
MShareware::CompareVersion(const char *old_ver, const char *new_ver)
{
    using namespace std;
    if (old_ver[0] == 0 && new_ver[0] == 0)
        return 0;

    std::vector<string> o, n;
    sw_explode(o, " .", old_ver);
    sw_explode(n, " .", new_ver);

    size_t osiz = o.size(), nsiz = n.size();
    size_t m = (osiz > nsiz ? nsiz : osiz);
    for (size_t i = 0; i < m; ++i)
    {
        if (o[i].size() == 0 && n[i].size() == 0) continue;
        if (o[i].size() == 0) return -1;
        if (n[i].size() == 0) return -1;

        if (isdigit(o[i][0]) && isdigit(n[i][0]))
        {
            char *op, *np;
            int on = strtol(o[i].data(), &op, 10);
            int nn = strtol(n[i].data(), &np, 10);
            if (on < nn) return -1;
            if (on > nn) return 1;

            int cmp = strcmp(op, np);
            if (cmp < 0) return -1;
            if (cmp > 0) return 1;
        }
        else
        {
            int cmp = strcmp(o[i].data(), n[i].data());
            if (cmp < 0) return -1;
            if (cmp > 0) return 1;
        }
    }
    if (osiz < nsiz) return -1;
    if (osiz > nsiz) return 1;
    return 0;
}

////////////////////////////////////////////////////////////////////////////

#ifdef SHAREWARE_UNITTEST
    extern "C"
    int main(void)
    {
        MShareware s(
            TEXT("Katayama Hirofumi MZ"),
            TEXT("SharewareTest"),
            "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08");
        assert(s.IsPasswordValid("test"));

        assert(s.CompareVersion("", "") == 0);
        assert(s.CompareVersion("1", "1") == 0);
        assert(s.CompareVersion("2.1", "2.1") == 0);

        assert(s.CompareVersion("1", "2") < 0);
        assert(s.CompareVersion("1.1", "2.1") < 0);
        assert(s.CompareVersion("1.1", "1.2") < 0);
        assert(s.CompareVersion("1.1", "1.1.1") < 0);
        assert(s.CompareVersion("1.1", "1.2.1") < 0);
        assert(s.CompareVersion("1.1", "1.1a") < 0);
        assert(s.CompareVersion("1.1a", "1.1b") < 0);
        assert(s.CompareVersion("1.1", "1.1 b") < 0);
        assert(s.CompareVersion("1.1 a", "1.1 b") < 0);

        assert(s.CompareVersion("2", "1") > 0);
        assert(s.CompareVersion("2.1", "1.1") > 0);
        assert(s.CompareVersion("1.2", "1.1") > 0);
        assert(s.CompareVersion("1.1.1", "1.1") > 0);
        assert(s.CompareVersion("1.2.1", "1.1") > 0);
        assert(s.CompareVersion("1.1a", "1.1") > 0);
        assert(s.CompareVersion("1.1b", "1.1a") > 0);
        assert(s.CompareVersion("1.1 b", "1.1") > 0);
        assert(s.CompareVersion("1.1 b", "1.1 a") > 0);
        return 0;
    }
#endif  // def SHAREWARE_UNITTEST

////////////////////////////////////////////////////////////////////////////

#endif  // ndef NO_SHAREWARE
