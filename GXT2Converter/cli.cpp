#include "cli.h"
#include "utf8conv.h"
#include <filesystem>
#include <iostream>
#include <windows.h>

namespace fs = std::filesystem;

CLI::CLI(const Gxt2Dll& dll, const I18n& i18n)
    : m_dll(dll), m_i18n(i18n) {
}

bool CLI::isGxt2File(const std::wstring& path) const {
    return fs::path(path).extension() == L".gxt2";
}

bool CLI::isTxtFile(const std::wstring& path) const {
    return fs::path(path).extension() == L".txt";
}

void CLI::printHelp() const {
    std::wcout << m_i18n.get(L"usage") << L"\n" << m_i18n.get(L"help");
}

bool CLI::processFile(const std::wstring& filePath) {
    void* instance = m_dll.create();
    if (!instance) {
        std::wcerr << L"Failed to create Gxt2File instance." << std::endl;
        return false;
    }

    std::wcout << m_i18n.get(L"processing_file") << filePath << std::endl;

    bool success = false;
    if (isGxt2File(filePath)) {
        if (!m_dll.load(instance, filePath.c_str())) { // 引数2つ渡している
            std::wcerr << m_i18n.get(L"err_load_gxt2") << filePath << std::endl;
        }
        else {
            auto txtPath = fs::path(filePath).replace_extension(".txt").wstring();
            if (!m_dll.save_txt(instance, txtPath.c_str())) {
                std::wcerr << m_i18n.get(L"err_save_txt") << txtPath << std::endl;
            }
            else {
                std::wcout << m_i18n.get(L"saved_txt") << txtPath << std::endl;
                success = true;
            }
        }
    }
    else if (isTxtFile(filePath)) {
        if (!m_dll.load_txt(instance, filePath.c_str())) {
            std::wcerr << m_i18n.get(L"err_load_txt") << filePath << std::endl;
        }
        else {
            auto gxtPath = fs::path(filePath).replace_extension(".gxt2").wstring();
            if (!m_dll.save(instance, gxtPath.c_str())) {
                std::wcerr << m_i18n.get(L"err_save_gxt2") << gxtPath << std::endl;
            }
            else {
                std::wcout << m_i18n.get(L"saved_gxt2") << gxtPath << std::endl;
                success = true;
            }
        }
    }
    else {
        std::wcerr << m_i18n.get(L"unsupported_ext") << filePath << std::endl;
    }

    m_dll.destroy(instance);
    return success;
}

bool CLI::processDirectory(const std::wstring& dirPath) {
    bool allSuccess = true;

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (!entry.is_regular_file()) continue;

        auto ext = entry.path().extension();
        std::wstring filePath = entry.path().wstring();

        if (ext == L".txt" || ext == L".gxt2") {
            if (!processFile(filePath)) {
                allSuccess = false;
            }
        }
        else {
            // 他拡張子はスキップ
        }
    }

    return allSuccess;
}

int CLI::run(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }

        std::wstring warg = utf8_to_wstring(arg);

        DWORD attr = GetFileAttributesW(warg.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES) {
            std::wcerr << L"Invalid path: " << warg << std::endl;
            continue;
        }
        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            std::wcout << m_i18n.get(L"processing_dir") << warg << std::endl;
            if (!processDirectory(warg)) return 1;
        }
        else {
            if (!processFile(warg)) return 1;
        }
    }
    return 0;
}
