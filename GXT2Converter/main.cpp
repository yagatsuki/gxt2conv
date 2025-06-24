#include "dll_loader.h"
#include "i18n.h"
#include "cli.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>

void setupUtf16Console()
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
}

int main(int argc, char* argv[]) {
    I18n i18n;
    Gxt2Dll dll;

    setupUtf16Console();

    if (!dll.loadDll(L"gxt2conv.dll")) {
        std::wcerr << L"Failed to load gxt2conv.dll" << std::endl;
        return 1;
    }

    CLI cli(dll, i18n);
    int ret = cli.run(argc, argv);

    dll.unload();
    return ret;
}
