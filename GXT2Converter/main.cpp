#include "dll_loader.h"
#include "i18n.h"
#include "cli.h"
#include <iostream>
#include <fcntl.h>
#include <io.h>

int main(int argc, char* argv[]) {
    I18n i18n;
    Gxt2Dll dll;

    int originalStdout = _setmode(_fileno(stdout), _O_U16TEXT);
    int originalStderr = _setmode(_fileno(stderr), _O_U16TEXT);
    if (originalStdout == -1 || originalStderr == -1) {
        // エラー処理（モード変更失敗）
        return 1;
    }

    if (!dll.loadDll(L"gxt2conv.dll")) {
        std::wcerr << L"Failed to load gxt2conv.dll" << std::endl;
        return 1;
    }

    CLI cli(dll, i18n);
    int ret = cli.run(argc, argv);

    dll.unload();

    (void)_setmode(_fileno(stdout), originalStdout);
    (void)_setmode(_fileno(stderr), originalStderr);
    return ret;
}
