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
        {L"err_load_gxt2", L"GXT2�t�@�C���̓ǂݍ��݂Ɏ��s���܂���: "},
        {L"err_load_txt",  L"TXT�t�@�C���̓ǂݍ��݂Ɏ��s���܂���: "},
        {L"err_save_gxt2", L"GXT2�t�@�C���̕ۑ��Ɏ��s���܂���: "},
        {L"err_save_txt",  L"TXT�t�@�C���̕ۑ��Ɏ��s���܂���: "},
        {L"usage", L"�g����: gxt2tool [�I�v�V����] <�t�@�C���܂��̓t�H���_> [...]"},
        {L"help", L"�I�v�V����:\n  -h, --help     �w���v�\��\n"},
        {L"processing_file", L"�t�@�C����������: "},
        {L"processing_dir", L"�t�H���_��������: "},
        {L"saved_gxt2", L"GXT2�t�@�C����ۑ����܂���: "},
        {L"saved_txt",  L"TXT�t�@�C����ۑ����܂���: "},
        {L"unsupported_ext", L"�Ή����Ă��Ȃ��t�@�C���g���q�ł�: "}
    };

    const auto& dict = (m_lang == Language::Japanese) ? ja : en;
    auto it = dict.find(key);
    if (it != dict.end())
        return it->second;
    return L"[Undefined i18n key]";
}
