#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const int ID_EDIT1 = 1;
const int ID_EDIT2 = 2;
const int ID_EDIT3 = 3;
const int ID_BUTTON = 4;
const int ID_LABEL = 5;
const int ID_CHECKBOX = 6;

HWND hEdit1, hEdit2, hEdit3, hLabel, hCheckBox;
HFONT hFont;

void compileFile(const string& inputFile, const string& outputFile, const string& compileFlags, string& errorMessage) {
    string command = "g++ " + inputFile + " -o " + outputFile + " " + compileFlags + " 2>&1";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        errorMessage = "Failed to execute command.";
        return;
    }

    char buffer[128];
    errorMessage = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        errorMessage += buffer;
    }

    _pclose(pipe);
}

LRESULT CALLBACK ErrorWindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HWND hErrorBox = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 10, 10, 360, 280, hwnd, NULL, NULL, NULL);
            string* errorMessage = (string*)(((CREATESTRUCT*)lp)->lpCreateParams);
            SetWindowText(hErrorBox, errorMessage->c_str());
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, msg, wp, lp);
        }
    }
    return 0;
}

void ShowErrorWindow(HINSTANCE hInstance, const string& errorMessage) {
    WNDCLASS errorClass = {0};
    errorClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    errorClass.hInstance = hInstance;
    errorClass.lpszClassName = "ErrorWindowClass";
    errorClass.lpfnWndProc = ErrorWindowProcedure;
    errorClass.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&errorClass)) {
        return;
    }

    CreateWindow("ErrorWindowClass", "Compilation Errors", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 200, 200, 400, 340, NULL, NULL, hInstance, (LPVOID)&errorMessage);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            // Create Font
            hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
            
            // Create Edit Boxes
            hEdit1 = CreateWindow("EDIT", "file.cpp", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 20, 200, 25, hwnd, (HMENU)ID_EDIT1, NULL, NULL);
            SendMessage(hEdit1, WM_SETFONT, WPARAM(hFont), TRUE);

            hEdit2 = CreateWindow("EDIT", "file.exe", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 60, 200, 25, hwnd, (HMENU)ID_EDIT2, NULL, NULL);
            SendMessage(hEdit2, WM_SETFONT, WPARAM(hFont), TRUE);

            // Create Label
            hLabel = CreateWindow("STATIC", "Extras", WS_VISIBLE | WS_CHILD, 20, 100, 200, 20, hwnd, (HMENU)ID_LABEL, NULL, NULL);
            SendMessage(hLabel, WM_SETFONT, WPARAM(hFont), TRUE);
            SetBkMode(GetDC(hLabel), TRANSPARENT);

            hEdit3 = CreateWindow("EDIT", "-lwinmm -mwindows", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 130, 200, 25, hwnd, (HMENU)ID_EDIT3, NULL, NULL);
            SendMessage(hEdit3, WM_SETFONT, WPARAM(hFont), TRUE);

            // Create Checkbox
            hCheckBox = CreateWindow("BUTTON", "Show Errors", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 20, 170, 200, 20, hwnd, (HMENU)ID_CHECKBOX, NULL, NULL);
            SendMessage(hCheckBox, WM_SETFONT, WPARAM(hFont), TRUE);
            SetBkMode(GetDC(hCheckBox), TRANSPARENT);

            // Create Button
            CreateWindow("BUTTON", "Compile", WS_VISIBLE | WS_CHILD, 20, 200, 200, 25, hwnd, (HMENU)ID_BUTTON, NULL, NULL);
            SendMessage(GetDlgItem(hwnd, ID_BUTTON), WM_SETFONT, WPARAM(hFont), TRUE);

            break;
        }
        case WM_CTLCOLORSTATIC: // Change background of static controls
        case WM_CTLCOLORBTN: {  // Change background of button controls
            HDC hdc = (HDC)wp;
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }
        case WM_COMMAND: {
            if (LOWORD(wp) == ID_BUTTON) {
                char inputFile[256];
                char outputFile[256];
                char compileFlags[256];
                string errorMessage;

                GetWindowText(hEdit1, inputFile, 256);
                GetWindowText(hEdit2, outputFile, 256);
                GetWindowText(hEdit3, compileFlags, 256);

                compileFile(inputFile, outputFile, compileFlags, errorMessage);

                if (IsDlgButtonChecked(hwnd, ID_CHECKBOX) == BST_CHECKED) {
                    ShowErrorWindow(GetModuleHandle(NULL), errorMessage);
                }

                MessageBox(hwnd, "Compilation Finished", "Info", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case WM_DESTROY: {
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, msg, wp, lp);
        }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hInstance = hInstance;
    wc.lpszClassName = "myWindowClass";
    wc.lpfnWndProc = WindowProcedure;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        return -1;
    }

    CreateWindow("myWindowClass", "C++ Compiler", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 260, 300, NULL, NULL, hInstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
