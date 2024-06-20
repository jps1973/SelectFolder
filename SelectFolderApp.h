// SelectFolderApp.h

#pragma once

#include <windows.h>
#include <commctrl.h>

#include "Ascii.h"
#include "Common.h"

#include "SelectFolder.h"

#define MAIN_WINDOW_CLASS_NAME													"Main SelectFolder Window Class"

#define MAIN_WINDOW_CLASS_STYLE													0
#define MAIN_WINDOW_CLASS_ICON													LoadIcon( NULL, IDI_APPLICATION )
#define MAIN_WINDOW_CLASS_CURSOR												LoadCursor( NULL, IDC_ARROW )
#define MAIN_WINDOW_CLASS_BACKGROUND											( HBRUSH )( COLOR_WINDOW + 1 )
#define MAIN_WINDOW_CLASS_MENU_NAME												NULL

#define MAIN_WINDOW_EXTENDED_STYLE												WS_EX_CLIENTEDGE
#define MAIN_WINDOW_TITLE														"SelectFolder"
#define MAIN_WINDOW_STYLE														WS_OVERLAPPEDWINDOW

#define MAIN_WINDOW_MINIMUM_WIDTH												500
#define MAIN_WINDOW_MINIMUM_HEIGHT												400

#define UNABLE_TO_CREATE_MAIN_WINDOW_ERROR_MESSAGE								"Unable to create main window"
#define UNABLE_TO_REGISTER_MAIN_WINDOW_CLASS_ERROR_MESSAGE						"Unable to register main window class"
