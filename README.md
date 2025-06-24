# GXT2Converter

GTA5のGXT2 とテキストファイル（TXT形式）の相互変換を行うツールおよびライブラリです。

---

## 特長

- GXT2 ⇔ TXT の相互変換
- CLIツール `GXT2Converter` による操作
- DLLライブラリとしての利用も可能
- フォルダ内の複数ファイルを一括変換対応
- 文字列キーをそのまま扱う独自仕様対応

---

## 動作環境

- Windows 10/11
- Visual Studio 2022 以降

---

## 使い方

### CLIツール `GXT2Converter`

```
GXT2Converter <入力ファイルまたはディレクトリ>
```

### 主な機能

* GXT2ファイルをテキストファイルに変換
* テキストファイルからGXT2ファイルを生成
* フォルダ単位の一括処理対応

### 使用例

```bash
# GXT2ファイルをTXTに変換
GXT2Converter input.gxt2

# TXTファイルをGXT2に変換
GXT2Converter input.txt

# フォルダ内のすべての対応ファイルを一括変換（サブフォルダは対象外）
GXT2Converter ./locales
```

---

## ライブラリ利用

`gxt2conv.dll` は動的ライブラリとして外部プログラムから読み込み、APIを利用して変換処理が可能です。

詳細はソースコードのヘッダーをご参照ください。

---

## 連絡先

[X @yagatsuki](https://x.com/yagatsuki)

---