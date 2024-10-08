#pragma once

#define ID_DIALOG    100
#define ID_GROUPBOX  0
#define ID_BUTTON_10 10
#define ID_BUTTON_20 20
#define ID_BUTTON_40 40

#define DIALOG_STYLE     DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_3DLOOK | DS_SETFONT
#define DIALOG_EX_STYLE  WS_EX_TOOLWINDOW
#define DIALOG_TITLE     L"TSS"
#define DIALOG_FONT      L"MS Shell Dlg"
#define DIALOG_FONT_SIZE 8

#define DIALOG_GROUPBOX_TITLE      L"Cell Size"
#define DIALOG_BUTTON_10_TITLE     L"10"
#define DIALOG_BUTTON_20_TITLE     L"20"
#define DIALOG_BUTTON_40_TITLE     L"40"
#define DIALOG_BUTTON_OK_TITLE     L"Ok"
#define DIALOG_BUTTON_CANCEL_TITLE L"Cancel"

#define DIALOG_GROUPBOX_STYLE      WS_CHILD | WS_VISIBLE | BS_GROUPBOX
#define DIALOG_BUTTON_10_STYLE     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP
#define DIALOG_BUTTON_20_STYLE     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP
#define DIALOG_BUTTON_40_STYLE     WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP
#define DIALOG_BUTTON_OK_STYLE     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP
#define DIALOG_BUTTON_CANCEL_STYLE WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP

#define DIALOG_RECT               100, 100, 90, 110
#define DIALOG_GROUPBOX_RECT      10, 10, 70, 70
#define DIALOG_BUTTON_10_RECT     20, 20, 30, 10
#define DIALOG_BUTTON_20_RECT     20, 40, 30, 10
#define DIALOG_BUTTON_40_RECT     20, 60, 30, 10
#define DIALOG_BUTTON_OK_RECT     10, 85, 30, 15
#define DIALOG_BUTTON_CANCEL_RECT 50, 85, 30, 15
