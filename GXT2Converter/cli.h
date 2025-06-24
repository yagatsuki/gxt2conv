#pragma once
#include <string>
#include "dll_loader.h"
#include "i18n.h"

class CLI {
public:
    CLI(const Gxt2Dll& dll, const I18n& i18n);
    int run(int argc, char* argv[]);

private:
    const Gxt2Dll&m_dll;
    const I18n& m_i18n;

    bool processPath(const std::string& path);
    bool processFile(const std::wstring& filePath);
    bool processDirectory(const std::wstring& dirPath);
    bool isGxt2File(const std::wstring& path) const;
    bool isTxtFile(const std::wstring& path) const;
    void printHelp() const;
};
