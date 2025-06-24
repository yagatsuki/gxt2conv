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
    LoadFunc load = nullptr;       // GXT2�ǂݍ��݊֐��|�C���^
    LoadFunc load_txt = nullptr;   // TXT�ǂݍ��݊֐��|�C���^
    SaveFunc save = nullptr;       // GXT2�������݊֐��|�C���^
    SaveFunc save_txt = nullptr;   // TXT�������݊֐��|�C���^

    // �����o�֐����� loadDll �ɕύX�iload �Ɣ�邽�߁j
    bool loadDll(const wchar_t* dllPath);
    void unload();
};
