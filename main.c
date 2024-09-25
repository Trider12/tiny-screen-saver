#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <ntsecapi.h>
#include "dialog.h"

#define NAMEOF(var) #var

#define APP_NAME "tss.scr"
#define BRUSH_COUNT 16 // must be a power of two
#define DEFAULT_TILE_SIZE 20
#define TIMER_DELTA_MS 50

int srcWindowWidth;
int srcWindowHeight;
int dstWindowWidth;
int dstWindowHeight;
unsigned int gridSizeX;
unsigned int gridSizeY;
int tileSize;
int tileCount;
char *tiles;
unsigned int bufferIndex; // 0 or 1

HDC windowHdc;
HDC drawHdc;

LRESULT WINAPI WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // see https://learn.microsoft.com/en-us/windows/win32/lwef/screen-saver-library#about-screen-savers
    switch (message)
    {
    case WM_TIMER:
        char *readBuffer = tiles + bufferIndex * tileCount;
        bufferIndex ^= 1;
        char *writeBuffer = tiles + bufferIndex * tileCount;

        for (unsigned int i = 0; i < gridSizeY; i++)
        {
            for (unsigned int j = 0; j < gridSizeX; j++)
            {
                unsigned int curr = i * gridSizeX + j;
                char srcColor = readBuffer[curr] & (BRUSH_COUNT - 1);
                char nextColor = (srcColor + 1) & (BRUSH_COUNT - 1);
                char count = 0;

                for (int k = -1; k < 2; k++)
                {
                    for (int l = -1; l < 2; l++)
                    {
                        unsigned int neighbour = ((gridSizeY + i + k) % gridSizeY) * gridSizeX + (gridSizeX + j + l) % gridSizeX;
                        count += readBuffer[neighbour] == nextColor;
                    }
                }

                char dstColor = count ? nextColor : srcColor;
                writeBuffer[curr] = dstColor;
                SetDCBrushColor(drawHdc, PALETTEINDEX(dstColor));
                SetDCPenColor(drawHdc, PALETTEINDEX(dstColor));
                Rectangle(drawHdc, j * tileSize, i * tileSize, (j + 1) * tileSize, (i + 1) * tileSize);
            }
        }

        StretchBlt(windowHdc, 0, 0, dstWindowWidth, dstWindowHeight, drawHdc, 0, 0, srcWindowWidth, srcWindowHeight, SRCCOPY);

        break;
        //case WM_PAINT: // works fine without this
        //    RECT rect = { 0, 0, srcWindowWidth, srcWindowHeight };
        //    ValidateRect(hWnd, &rect);
        //    break;
    case WM_SETCURSOR:
        SetCursor(NULL);
        break;
    case WM_ACTIVATE:
        if (wParam)
            break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_KEYDOWN:
        //case WM_MOUSEMOVE: // enabling this exits the preview immediately
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    (void)lParam;
    // see https://stackoverflow.com/questions/23751216/where-to-find-definition-of-window-message-wm-uahdrawmenuitem-0x92
    switch (message)
    {
    case WM_INITDIALOG:
        CheckDlgButton(hWnd, tileSize, BST_CHECKED);
        return TRUE;
    case WM_COMMAND:
        switch (wParam)
        {
        case ID_BUTTON_10:
        case ID_BUTTON_20:
        case ID_BUTTON_40:
            tileSize = wParam;
            break;
        case IDOK:
            char buffer[3]; // two digit number and 0
            wnsprintfA(buffer, sizeof(buffer), "%d", tileSize);
            WriteProfileStringA(APP_NAME, NAMEOF(tileSize), buffer);
        case IDCANCEL:
            EndDialog(hWnd, wParam);
            break;
        }
        return TRUE;
    }

    return FALSE;
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    tileSize = GetProfileIntA(APP_NAME, NAMEOF(tileSize), DEFAULT_TILE_SIZE);

    int argc;
    WCHAR **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // see https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ms686421(v=vs.85)#command-line-options
    if (argc == 1 || !StrCmpNW(argv[1], L"/c", 2))
    {
#ifdef USE_CRINKLER
        // see https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgtemplate
        // and https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-dlgitemtemplate
        // and https://learn.microsoft.com/en-us/windows/win32/dlgbox/using-dialog-boxes#creating-a-template-in-memory
#define ITEM_TEMPLATE(varName, titleSize) _Alignas(sizeof(DWORD)) DLGITEMTEMPLATE varName; WORD varName##Class; WORD varName##Ordinal; WCHAR varName##Title[titleSize]; WORD varName##CreationData

        static struct
        {
            _Alignas(sizeof(DWORD)) DLGTEMPLATE dialog;
            WORD menu;
            WORD windowClass;
            WCHAR  dialogTitle[_countof(DIALOG_TITLE)];
            short  pointSize;
            WCHAR  dialogFont[_countof(DIALOG_FONT)];
            ITEM_TEMPLATE(groupBox, _countof(DIALOG_GROUPBOX_TITLE));
            ITEM_TEMPLATE(button10, _countof(DIALOG_BUTTON_10_TITLE));
            ITEM_TEMPLATE(button20, _countof(DIALOG_BUTTON_20_TITLE));
            ITEM_TEMPLATE(button40, _countof(DIALOG_BUTTON_40_TITLE));
            ITEM_TEMPLATE(buttonOk, _countof(DIALOG_BUTTON_OK_TITLE));
            ITEM_TEMPLATE(buttonCancel, _countof(DIALOG_BUTTON_CANCEL_TITLE));
        } dialogTemplate =
        {
            DIALOG_STYLE, DIALOG_EX_STYLE, 6, DIALOG_RECT, 0, 0, DIALOG_TITLE, DIALOG_FONT_SIZE, DIALOG_FONT,
            DIALOG_GROUPBOX_STYLE,      WS_EX_NOPARENTNOTIFY, DIALOG_GROUPBOX_RECT,      ID_GROUPBOX,  0xFFFF, 0x0080, DIALOG_GROUPBOX_TITLE,      0,
            DIALOG_BUTTON_10_STYLE,     WS_EX_NOPARENTNOTIFY, DIALOG_BUTTON_10_RECT,     ID_BUTTON_10, 0xFFFF, 0x0080, DIALOG_BUTTON_10_TITLE,     0,
            DIALOG_BUTTON_20_STYLE,     WS_EX_NOPARENTNOTIFY, DIALOG_BUTTON_20_RECT,     ID_BUTTON_20, 0xFFFF, 0x0080, DIALOG_BUTTON_20_TITLE,     0,
            DIALOG_BUTTON_40_STYLE,     WS_EX_NOPARENTNOTIFY, DIALOG_BUTTON_40_RECT,     ID_BUTTON_40, 0xFFFF, 0x0080, DIALOG_BUTTON_40_TITLE,     0,
            DIALOG_BUTTON_OK_STYLE,     WS_EX_NOPARENTNOTIFY, DIALOG_BUTTON_OK_RECT,     IDOK,         0xFFFF, 0x0080, DIALOG_BUTTON_OK_TITLE,     0,
            DIALOG_BUTTON_CANCEL_STYLE, WS_EX_NOPARENTNOTIFY, DIALOG_BUTTON_CANCEL_RECT, IDCANCEL,     0xFFFF, 0x0080, DIALOG_BUTTON_CANCEL_TITLE, 0
        };

        DialogBoxIndirectParamW(hInstance, (LPCDLGTEMPLATEW)&dialogTemplate, NULL, DlgProc, 0);
#else
        DialogBoxA(NULL, MAKEINTRESOURCE(ID_DIALOG), NULL, DlgProc); // this requires compiling with the .rc file... that crinkler doesn't support
#endif // USE_CRINKLER
        return 0;
    }

    srcWindowWidth = GetSystemMetrics(SM_CXSCREEN);
    srcWindowHeight = GetSystemMetrics(SM_CYSCREEN);
    dstWindowWidth = srcWindowWidth;
    dstWindowHeight = srcWindowHeight;
    RECT dstWindowRect;
    dstWindowRect.left = 0;
    dstWindowRect.top = 0;

    if (!StrCmpNW(argv[1], L"/p", 2))
    {
        GetWindowRect((HWND)StrToIntW(argv[2]), &dstWindowRect);
        dstWindowWidth = dstWindowRect.right - dstWindowRect.left;
        dstWindowHeight = dstWindowRect.bottom - dstWindowRect.top;
    }

    const char *windowClassName = "TSS";
    WNDCLASS windowClass;
    windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS | CS_DBLCLKS;
    windowClass.lpfnWndProc = (WNDPROC)WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = windowClassName;
    RegisterClass(&windowClass);

    HWND hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, windowClassName, windowClassName, WS_POPUP | WS_VISIBLE, dstWindowRect.left, dstWindowRect.top, dstWindowWidth, dstWindowHeight, NULL, NULL, hInstance, NULL);

    gridSizeX = (srcWindowWidth + tileSize - 1) / tileSize;
    gridSizeY = (srcWindowHeight + tileSize - 1) / tileSize;
    tileCount = gridSizeX * gridSizeY;

    tiles = GlobalAlloc(0, tileCount * 2); // double buffer
    RtlGenRandom(tiles, tileCount);

    windowHdc = GetDC(hWnd);
    drawHdc = CreateCompatibleDC(windowHdc);
    HBITMAP bitmap = CreateCompatibleBitmap(windowHdc, srcWindowWidth, srcWindowHeight);
    SelectObject(drawHdc, bitmap);
    SelectObject(drawHdc, GetStockObject(DC_BRUSH));
    SelectObject(drawHdc, GetStockObject(DC_PEN));
    SetStretchBltMode(windowHdc, COLORONCOLOR);
    SetTimer(hWnd, 1, TIMER_DELTA_MS, NULL);

    for (MSG msg; GetMessage(&msg, NULL, 0, 0);)
    {
        DispatchMessage(&msg);
    }

    // who frees memory anyway ;)
    //LocalFree(argv);
    //GlobalFree(tiles);
    //ReleaseDC(windowHdc);
    //DeleteDC(drawHdc);
    //DeleteObject(bitmap);
    //KillTimer(hWnd, 1);

    return 0;
}