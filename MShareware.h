// MShareware.h -- The shareware maker for Win32
////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef NO_SHAREWARE

#include <string> // for std::string and std::wstring

////////////////////////////////////////////////////////////////////////////
// useful functions

LPTSTR SwLoadStringDx1(HINSTANCE hInstance, UINT uID);
LPTSTR SwLoadStringDx2(HINSTANCE hInstance, UINT uID);
void SwCenterDialog(HWND hwnd);
int SwCenterMessageBox(HWND hwndParent, LPCTSTR pszText, LPCTSTR pszCaption, UINT uMB_);

////////////////////////////////////////////////////////////////////////////
// MShareware class

class MShareware
{
public:
    // NOTE: pszAppKey is the path of the registry key of the application.
    //       For example, "SOFTWARE\\MyCompany\\MyApp".
    // NOTE: dwTrialDays is the trial period in days.
    // NOTE: salt is the salt string.
    // NOTE: new_version is the current version of this software.
    MShareware(LPCTSTR pszAppKey,
               const char *pszSha256HashHexString,
               DWORD dwTrialDays = 15,
               const char *salt = "",
               const char *new_version = "");
    MShareware(LPCTSTR pszAppKey,
               const BYTE *pbHash32Bytes,
               DWORD dwTrialDays = 15,
               const char *salt = "",
               const char *new_version = "");
    virtual ~MShareware();

    // NOTE: MShareware::Start must be called on start-up of the MShareware shareware.
    // NOTE: MShareware::Start returns false if the application cannot be used.
    virtual bool Start(HWND hwndParent = NULL);

    bool IsRegistered() const;
    bool IsInTrial() const;
    bool IsOutOfTrial() const;

    DWORD GetTrialDays() const;
    bool IsPasswordValid(const char *pszPassword) const;

    // NOTE: UrgeRegister show a dialog and returns true if registered.
    virtual bool UrgeRegister(HWND hwndParent = NULL);
    bool RegisterPassword(HWND hwndParent, const char *pszPassword);
    virtual void ShowErrorMessage(HWND hwndParent, UINT uStringID);
    virtual void ThisCommandRequiresRegistering(HWND hwndParent);
    bool CheckDate();

    virtual int CompareVersion(const char *old_ver, const char *new_ver);

public:
    HINSTANCE           m_hInstance;
    DWORDLONG           m_dwlTotalMinutesRemains;
    DWORD               m_dwTrialDays;
    FILETIME            m_ftStart;

    enum SHAREWARE_STATUS
    {
        IN_TRIAL_FIRST_TIME, IN_TRIAL, OUT_OF_TRIAL, REGD
    };
    SHAREWARE_STATUS    m_status;

public:
    #ifdef UNICODE
        typedef std::wstring tstring;
    #else
        typedef std::string tstring;
    #endif

protected:
    tstring             m_strAppKey;
    std::string         m_strSha256HashHexString;
    std::string         m_strSalt;
    std::string         m_strNewVersion;
    std::string         m_strOldVersion;

    bool CheckRegistry(HWND hwndParent);
    bool CheckAppKey(HWND hwndParent, HKEY hkeyApp);

    bool SetRegistryFirstTime(HWND hwndParent);

    DWORD GetUserCheckSum() const;
    virtual void OnTrialFirstTime(HWND hwndParent);
    virtual void OnTrial(HWND hwndParent);
    virtual bool OnOutOfTrial(HWND hwndParent);
    virtual void EncodePassword(void *pass, DWORD size) const;
    virtual void DecodePassword(void *pass, DWORD size) const;

private:
    MShareware();
    // NOTE: MShareware is not copyable.
    MShareware(const MShareware&) = delete;
    MShareware& operator=(const MShareware&) = delete;
};

////////////////////////////////////////////////////////////////////////////

#endif // ndef NO_SHAREWARE
