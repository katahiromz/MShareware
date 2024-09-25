(Japanese, UTF-8)

# MShareware

## これは何？

MShareware は、C++/Win32 のシェアウェアを楽に作るためのフレームワークです。

## 使い方

1. 次の 5 つのファイルをプロジェクトフォルダに追加して下さい。

    - MShareware.cpp
    - MShareware.h
    - MShareware.rc
    - MSha256.cpp
    - MSha256.h

2. MShareware.cpp、MShareware.rc、MSha256.cppの3つのファイルをプロジェクトに追加して下さい。
   CMakeでプロジェクトを管理しているのであれば、CMakeLists.txt の add_executable にこの3つのファイルを追加して下さい。
   Visual C++でプロジェクトを管理しているのであれば、プロジェクトに既存の項目としてこの3つのファイルを追加して下さい。

3. ファイル MShareware.rc の中の"TODO:"項目を修正して下さい。
4. salt文字列と 8 文字以上のパスワード文字列を思い浮かべて下さい。どちらもなるべく不規則な文字列にして下さい。

※ salt文字列は、パスワード以外にハッシュのパターンを変化させる調味料のようなものです。パスワードが少し短くても、saltが充分複雑で長ければ、解析が難しくなります。

5. MSha256.exe により、salt 文字列に対して、パスワードに対応するハッシュ文字列を取得して下さい。

例えば、salt 文字列が空文字列で、パスワードが "test" ならば、ハッシュ文字列は

    "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"

になります。

    (MSha256.exeの実行例)
    Enter 'exit' to exit.
    Enter salt string:
    Enter password: test
    9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08
    {0x9f, 0x86, 0xd0, 0x81, 0x88, 0x4c, 0x7d, 0x65, 0x9a, 0x2f, 0xea, 0xa0, 0xc5, 0
    x5a, 0xd0, 0x15, 0xa3, 0xbf, 0x4f, 0x1b, 0x2b, 0x0b, 0x82, 0x2c, 0xd1, 0x5d, 0x6
    c, 0x15, 0xb0, 0xf0, 0x0a, 0x08, }
    Enter password: exit

6. #include "MShareware.h"をメインのソースファイルに追加して下さい。

    #ifndef NO_SHAREWARE
    #include "MShareware.h"
    #endif

7. クラス MShareware の g_shareware という名前のグローバル変数の定義を次のように追加して下さい：

    #ifndef NO_SHAREWARE
    MShareware g_shareware(
        /* application registry path */ TEXT("SOFTWARE\\MyCompany\\MyApp"),
        /* password hash */             "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08",
        /* trial days */                15,
        /* salt string */               "",
        /* version string */            "");
    #endif

- 第1引数は、アプリが使用するレジストリキーのパスです。MyCompanyに英語のあなたの会社名、MyAppに英語のあなたのアプリ名を指定します。
- 第2引数は、パスワードのハッシュ文字列です。
- 第3引数は、試用期間の日数です（省略可）。
- 第4引数は、salt文字列です（省略可）。
- 第5引数は、アプリケーションの現在のバージョンを表す文字列です（省略可）。

バージョンは、バージョンアップ時の試用期間の延長に使われます。延長しないときは、常に空文字列を指定して下さい。

8. 次のようなコードをアプリケーションの始まりに追加して下さい。ウィンドウ ハンドル hwnd がなければ NULL でも構いません。

    #ifndef NO_SHAREWARE
        if (!g_shareware.Start(hwnd))
        {
            // 失敗。アプリケーションを終了する。
            ...
        }
    #endif

9. 機能制限したい機能の実行の直前に次のようなコードを追加して下さい。

    #ifndef NO_SHAREWARE
    if (!g_shareware.IsRegistered())
    {
        g_shareware.ThisCommandRequiresRegistering(hwnd);
        if (!g_shareware.UrgeRegister(hwnd))
        {
            // 機能を実行しない。
            return;
        }
    }
    #endif

10. ビルドして下さい。あなたのシェアウェアが出来上がります。実際に使ってみて試してみて下さい。
11. 好きなように MShareware.rc を改良して下さい。
12. アプリ内で保護すべきデータ資産を付属のMBlowfishとパスワードなどを使って暗号化・復号して保護して下さい。

## 注意

- 最初の起動時には、「ライセンス キーの登録」ダイアログが表示されません（デフォルト）。
- デバッグ版（Debug）を配布してはいけません。リリース版（Release）を配布して下さい。
- 配布前に、ちゃんと実機か仮想マシンでテストしましょう。
- 販売促進のため、幅広い顧客の目を引き付け、購入へ誘導する工夫をしましょう。

## 連絡先

何かございましたら、片山博文MZ（katahiromz）までご連絡下さい。

- メール katayama.hirofumi.mz@gmail.com
- ホームページ http://katahiromz.fc2.page/
