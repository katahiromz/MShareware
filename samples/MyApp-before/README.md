# MyApp by katahiromz

## これは何？

katahiromz（片山博文MZ）氏が作ったWin32アプリのテンプレートです。

## ファイル群

- `res` ― リソースフォルダ
- `.gitignore` ― Gitが無視すべきファイル群を記載
- `AntiRE.h` ― 逆工学対策
- `CMakeLists.txt` ― ビルドツール CMake用の設定ファイル
- `LICENSE.txt` ― ライセンス（使用許諾）情報
- `MCenterWindow.h` ― ウィンドウやメッセージボックスを中央寄せする
- `mdbg.h` ― デバッグ時出力
- `mt19937ar.c` と `mt19937ar.h` ― メルセンヌツイスター乱数生成
- `MyApp.cpp` ― 私のアプリのソース
- `MyApp.h` ― 私のアプリのヘッダ
- `MyApp_res.rc` ― 私のアプリのリソースデータ
- `README.md` ― このファイル
- `resource.h` ― リソースID群
- `targetver.h` ― ターゲットOSバージョン

## ビルド方法

Git コマンドがインストールされていれば、

```bash
git clone https://github.com/katahiromz/MyApp
```

でMyAppのソースコードをダウンロードできます。ダウンロードが終わったら、普通に CMake と C++11/Win32 コンパイラを使ってビルドしてください。

C++11/Win32 コンパイラで迷ったら、WinXP互換の RosBE (ReactOS Build Environment) でも使ってやってください。
RosBEでのビルド方法は次の通りです。

```cmd
cd (MyAppのフォルダへのパス)
cmake -G Ninja .
ninja
```

`MSYS2`を使う場合は、必要なパッケージを`MSYS2`内部の`pacman`でインストールした後で、
`cd`コマンド、`cmake -G "MinGW Makefiles" .`、そして`mingw32-make.exe` でできます。
`pacman`によるパッケージのインストール方法はここでは説明しません。検索エンジンや人工知能にでもお伺い下さい。

Visual C++を使うのでしたら、まずはC++開発ができるように開発環境を整備し、Hello worldから始めてください。
それが終わったら、スタンドアロンのCMake をインストールして、CMake GUI または CMake コマンドを使って
CMakeからビルドしてください。Visual C++でのビルドはいろんなやり方があるので、ここではあまり説明しません。
CMakeを使ったビルドは、ソリューションファイルを使う方法よりも遠回りに見えますが、
ソリューションファイルは人間がマネージできるファイルではないので、後々が厳しいです。

せっかくアプリをビルドしても「WinXPで動作しない」とか「x86で動かない」、
「DLLファイルが足りなくて動作しない」なんてことがあり得ます。
マシンやOSのアーキテクチャを理解し、実機や仮想マシンで実際に動作するか確かめましょう。
もちろん、セキュリティチェックを忘れずに。

## 改造のヒント

- ボタン、メニューバーなどを追加してみましょう。
- ボタンを押したときに動作するC/C++のコードを書いてみてください。
- アプリ名やリソースデータを変更・追加してみましょう。
- 要らないコードを消してみましょう。
- 似たような処理は、引数付きの「ヘルパー関数」でまとめましょう。
- `MsgCrack`を使って `WM_DROPFILES (OnDropFiles)` などのイベントハンドラを追加してみましょう。
- `ShellExecute`シェル関数でメモ帳（`notepad.exe`）を起動してみましょう。
- `GetModuleFileName`関数でEXEファイルの場所が分かります。
- ビットマップ画像やWAVE音声などを追加してみましょう。
- パスファイル名を操作したい場合は`<shlwapi.h>`の`PathFindExtension`、`PathFindFileName`、`PathRemoveExtension`などの関数を使ってみましょう。

## すごいアプリが出来たら...

- Vector（ベクター）に登録してみましょう。
- マイクロソフトのストアに登録してみましょう。
- GitHubでソースコードを公開してみましょう。
- そのアプリのすごさをウェブで宣伝しましょう。
- シェアウェアでお金儲けに挑戦してみましょう。

## でも自分にはアプリのアイデアがない...

- 解決すべき課題は、現実世界に転がっている!!
- ワードやエクセルが作れなくても、解決できる課題はあるはずだ
- 有益なIT技術には名前が付いている。文字認識、画像処理、画像認識、QRコード、人工知能 etc.
- 現場と図書館をめぐり、課題を探しに出かけよう！

## 使用許諾と著作権

- License: MIT
- Copyright (C) 2024 katahiromz <katayama.hirofumi.mz@gmail.com>

## 参照

- Win32 Programmer's Reference (ご自分で自己責任でお探しください)
- C++日本語リファレンス ― https://cpprefjp.github.io/
- テキストエディタ「サクラエディタ」 ― https://sakura-editor.github.io/
- リソース編集ツール「リソーエディタ」 ― https://katahiromz.fc2.page/risoheditor/
- Win32開発のお供に「MsgCrack」 ― https://katahiromz.fc2.page/msgcrack/
- EXE圧縮ツール「UPX」 ― https://upx.github.io/
