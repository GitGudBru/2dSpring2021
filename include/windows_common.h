#ifndef __WINDOWS_COMMON_H__
#define __WINDOWS_COMMON_H__

#include "gf2d_windows.h"

Window *window_yes_no(char *text, void(*onYes)(void *), void(*onNo)(void *), void *yesData, void *noData);
Window* window_menu(char* text, void(*onYes)(void*), void(*onNo)(void*), void* yesData, void* noData);
Window* window_menu2(char* text, void(*onYes)(void*), void(*onYes2)(void*), void(*onNo)(void*), void* yesData, void* yes2Data, void* noData);
Window *window_text_entry(char *question, char *defaultText, size_t length, void(*onOk)(void *), void(*onCancel)(void *));

#endif