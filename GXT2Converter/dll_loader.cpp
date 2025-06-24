#include "dll_loader.h"
#include <iostream>

bool Gxt2Dll::loadDll(const wchar_t* dllPath) {
    hModule = LoadLibraryW(dllPath);
    if (!hModule) {
        wprintf(L"Failed to load DLL: %s\n", dllPath);
        return false;
    }

    create = (CreateFunc)GetProcAddress(hModule, "createGxt2File");
    destroy = (DestroyFunc)GetProcAddress(hModule, "destroyGxt2File");
    load = (LoadFunc)GetProcAddress(hModule, "gxt2_load");
    load_txt = (LoadFunc)GetProcAddress(hModule, "gxt2_load_txt");
    save = (SaveFunc)GetProcAddress(hModule, "gxt2_save");
    save_txt = (SaveFunc)GetProcAddress(hModule, "gxt2_save_txt");

    if (!create || !destroy || !load || !save) {
        wprintf(L"Failed to get required functions from DLL.\n");
        FreeLibrary(hModule);
        hModule = nullptr;
        return false;
    }
    return true;
}

void Gxt2Dll::unload() {
    if (hModule) {
        FreeLibrary(hModule);
        hModule = nullptr;
    }
}
