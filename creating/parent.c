#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>

using namespace std;

int main()
{
    STARTUPINFO sti = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    DWORD excode;
 
    // Так как обе программы находятся в одном solution, то запускаем программу в виде команды командной строки.

    wstring CommandLine(L"Child.exe");
    LPWSTR lpwCmdLine= &CommandLine[0];

    cout << "== == PARENT PROCESS START == ==\n";
    if (!CreateProcess(NULL, lpwCmdLine, NULL, NULL, TRUE, NULL, NULL, NULL, &sti, &pi)) {
        // Не удалось запустить процесс.
        cout << "Unable to generate process" << endl;
        return -1;
    }
    _getch();

    // Извлекаем данные о состоянии завершения работы заданного процесса.
    GetExitCodeProcess(pi.hProcess, &excode);
    cout << excode << endl;
    // Процесс завершен.
    if (excode != STILL_ACTIVE) {
        cout << "Done" << endl;
    }
    // Процесс не завершен (код 259).
    else {
        cout << "Process still running" << endl;
    }
   cout << "== == PARENT PROCESS END == ==\n";
    _getch();
}
