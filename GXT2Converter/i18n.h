#pragma once
#include <string>

enum class Language {
    English,
    Japanese
};

class I18n {
public:
    I18n();
    std::wstring get(const std::wstring& key) const;

private:
    Language m_lang;
};
