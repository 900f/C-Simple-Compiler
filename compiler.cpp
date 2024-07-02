#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const int ID_EDIT1 = 1;
const int ID_EDIT2 = 2;
const int ID_BUTTON = 3;

HWND hEdit1, hEdit2;

void compileFile(const string& inputFile, const string& outputFile) {
    string command = "g++ " + inputFile + " -o " + outputFile + " -lwinmm -mwindows";
    system(command.c_str());
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            // Create Edit Boxes
            hEdit1 = CreateWindow("EDIT", "file.cpp", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 20, 200, 25, hwnd, (HMENU)ID_EDIT1, NULL, NULL);
            hEdit2 = CreateWindow("EDIT", "file.exe", WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 60, 200, 25, hwnd, (HMENU)ID_EDIT2, NULL, NULL);
            
            // Create Button
            CreateWindow("BUTTON", "Compile", WS_VISIBLE | WS_CHILD, 20, 100, 200, 25, hwnd, (HMENU)ID_BUTTON, NULL, NULL);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wp) == ID_BUTTON) {
                char inputFile[256];
                char outputFile[256];

                GetWindowText(hEdit1, inputFile, 256);
                GetWindowText(hEdit2, outputFile, 256);

                compileFile(inputFile, outputFile);

                MessageBox(hwnd, "Compilation Finished", "Info", MB_OK | MB_ICONINFORMATION);
            }
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hInstance = hInstance;
    wc.lpszClassName = "myWindowClass";
    wc.lpfnWndProc = WindowProcedure;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        return -1;
    }

    CreateWindow("myWindowClass", "C++ Compiler", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 260, 200, NULL, NULL, hInstance, NULL);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
