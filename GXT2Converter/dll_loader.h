#pragma once
#include <windows.h>

struct Gxt2Dll {
    HMODULE hModule = nullptr;

    using CreateFunc = void* (*)();
    using DestroyFunc = void (*)(void*);
    using LoadFunc = bool (*)(void*, const wchar_t*);
    using SaveFunc = bool (*)(void*, const wchar_t*);

    CreateFunc create = nullptr;
    DestroyFunc destroy = nullptr;
    LoadFunc load = nullptr;       // GXT2読み込み関数ポインタ
    LoadFunc load_txt = nullptr;   // TXT読み込み関数ポインタ
    SaveFunc save = nullptr;       // GXT2書き込み関数ポインタ
    SaveFunc save_txt = nullptr;   // TXT書き込み関数ポインタ

    // メンバ関数名を loadDll に変更（load と被るため）
    bool loadDll(const wchar_t* dllPath);
    void unload();
};
