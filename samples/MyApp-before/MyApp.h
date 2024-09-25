// MyApp.h --- 私のアプリのヘッダー -*- C++ -*-

#pragma once // インクルード ガード

#define _CRT_SECURE_NO_WARNINGS // Visual C++におけるfopenなどの警告を無視する
#define _USE_MATH_DEFINES // Visual C++でも円周率定数 M_PI を使いたい

#include "targetver.h" // アプリの対象OSバージョンを選ぶ

// メモリーリークを検出（Visual C++のみ）
#if defined(_MSC_VER) && !defined(NDEBUG) && !defined(_CRTDBG_MAP_ALLOC)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

// Unicodeサポートの一貫性を要求
#if defined(UNICODE) != defined(_UNICODE)
    #error You lose.
#endif

// ヘッダをインクルードする。必要のないものはコンパイルにかかる時間に影響するので削ってもいい。
#include <windows.h> // ウィンドウズ 標準ヘッダ
#include <windowsx.h> // ウィンドウズ ヘルパーマクロ ヘッダ
#include <commctrl.h> // コモン コントロール ヘッダ
#include <commdlg.h> // コモン ダイアログ ヘッダ
#include <shellapi.h> // シェルAPI ヘッダ
#include <shlobj.h> // シェル オブジェクト群 ヘッダ
#include <shlwapi.h> // シェル軽量API ヘッダ
#include <tchar.h> // ジェネリックテキストマッピング
#include <string> // std::string, std::wstring など
#include <vector> // std::vector
#include <memory> // std::shared_ptr, std::make_shared など
#include <algorithm> // std::sort, std::move などの基本アルゴリズム
#include <cstdlib> // C標準ライブラリ
#include <cstdio> // C標準入出力ライブラリ
#include <cmath> // C数学ライブラリ
#include <cassert> // assertマクロ
#define STRSAFE_NO_DEPRECATE // <strsafe.h> でdeprecate扱いのものも使えるようにする
//#include <strsafe.h> // 文字列バッファを安全に扱うStringCchPrintfなどを使えるようにする
#include "MCenterWindow.h" // ウィンドウを中央揃えにする
#include "mdbg.h" // デバッグ時出力
#include "AntiRE.h" // 逆工学対策
#include "resource.h" // リソースID群

// 配列の要素数を得るマクロ
#ifndef _countof
    #define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// DLLをインポートライブラリで自動リンク（MinGWでは無効）
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

// アプリのインスタンス
extern HINSTANCE g_hInst;
// メインウィンドウのハンドル
extern HWND g_hMainWnd;

// リソース文字列を読み込むヘルパー関数
inline LPCTSTR LoadStringDx(INT nID)
{
    // バッファサイズ。ちょっと大きいが対象変数がstaticなのでまあまあ大丈夫
    const INT c_buf_size = 1024;
    // リングバッファで３回連続で呼ばれても大丈夫
    static TCHAR s_szBuff[3][c_buf_size]; // staticなのでこれを戻り値にしても大丈夫
    static INT s_iBuff = 0; // リングバッファでのインデックス位置をstatic変数で覚えておく
    LPTSTR ret = s_szBuff[s_iBuff]; // バッファを選ぶ
    ret[0] = 0; // LoadString失敗時の安全策
    LoadString(g_hInst, nID, ret, c_buf_size);
    // 次の位置へ移動。余りを使って、くるくる回転
    s_iBuff = (s_iBuff + 1) % _countof(s_szBuff);
    // 格納したバッファへのポインタを返す
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// MObject クラス。このアプリで使う動的に確保するオブジェクトは基本的にこれを継承しようかな、どうしようかな

class MObject
{
public:
    static LONG s_cAliveObjects;
    MObject()
    {
        ++s_cAliveObjects;
    }
    virtual ~MObject()
    {
        --s_cAliveObjects;
    }
};
