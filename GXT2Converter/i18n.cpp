#include "i18n.h"
#include <windows.h>
#include <unordered_map>

I18n::I18n() {
    LANGID lang = GetUserDefaultUILanguage();
    if (PRIMARYLANGID(lang) == LANG_JAPANESE)
        m_lang = Language::Japanese;
    else
        m_lang = Language::English;
}

std::wstring I18n::get(const std::wstring& key) const {
    static const std::unordered_map<std::wstring, std::wstring> en = {
        {L"err_load_gxt2", L"Failed to load GXT2 file: "},
        {L"err_load_txt",  L"Failed to load TXT file: "},
        {L"err_save_gxt2", L"Failed to save GXT2 file: "},
        {L"err_save_txt",  L"Failed to save TXT file: "},
        {L"usage", L"Usage: gxt2tool [options] <file_or_folder> [...]"},
        {L"help", L"Options:\n  -h, --help     Show this help message\n"},
        {L"processing_file", L"Processing file: "},
        {L"processing_dir", L"Processing directory: "},
        {L"saved_gxt2", L"Saved GXT2 file: "},
        {L"saved_txt",  L"Saved TXT file: "},
        {L"unsupported_ext", L"Unsupported file extension: "}
    };

    static const std::unordered_map<std::wstring, std::wstring> ja = {
        {L"err_load_gxt2", L"GXT2ファイルの読み込みに失敗しました: "},
        {L"err_load_txt",  L"TXTファイルの読み込みに失敗しました: "},
        {L"err_save_gxt2", L"GXT2ファイルの保存に失敗しました: "},
        {L"err_save_txt",  L"TXTファイルの保存に失敗しました: "},
        {L"usage", L"使い方: gxt2tool [オプション] <ファイルまたはフォルダ> [...]"},
        {L"help", L"オプション:\n  -h, --help     ヘルプ表示\n"},
        {L"processing_file", L"ファイルを処理中: "},
        {L"processing_dir", L"フォルダを処理中: "},
        {L"saved_gxt2", L"GXT2ファイルを保存しました: "},
        {L"saved_txt",  L"TXTファイルを保存しました: "},
        {L"unsupported_ext", L"対応していないファイル拡張子です: "}
    };

    const auto& dict = (m_lang == Language::Japanese) ? ja : en;
    auto it = dict.find(key);
    if (it != dict.end())
        return it->second;
    return L"[Undefined i18n key]";
}
