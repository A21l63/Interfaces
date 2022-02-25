#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <math.h>
#include <limits.h>

#include "menu.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif

typedef unsigned int uint;

//Структура модели текста
typedef struct text_t {
	char* str; //Сам текст
	uint* strStart; //Индексы символов - начал строк
	uint maxStrLen; //Наибольшая длина строки
	uint strCount; //Количество строк
}text_t;

//Размеры окна
typedef struct metric_t {
  uint x;
  uint y;
}metric_t;

//Инициализация структуры происходит с помощью TEXTMETRIC и GetTextMetrics()
typedef struct symSize_t {
    uint xChar; //Средняя ширина символа
    uint xCaps; //Средняя ширина капса
    uint yChar; //Средняя высота символа
} symSize_t;

//Структура модели представления
typedef struct view_t{
    int selectedMode; //Режим верстки
    uint newStrCount; //Количество строк для режима с вёрсткой
    uint yOneList, xOneList; //Размер рабочей области в символах (высота и ширина)
    uint iMaxWidth;   //Наибольшая ширина строки (режим без верстки)
    metric_t clientScreen; //Размеры рабочего окна (клиентской области)
    symSize_t sym; //Размеры символа
    int width; //Ширина окна в символах
}view_t;

typedef struct scroll_t{
    SCROLLINFO scroll; //Параметры скролла (устанавливаются SetScrollInfo)
    int xPos, yPos, prevPos; //Сдвиг по х ползунка прокрутки, сдвиг по у, предыдущий двиг по у ползунка прокрутки
    double sizeCoef;   //Коэффициент изменения  (для больших файлов)
} scroll_t;

typedef struct fileName_t {
    OPENFILENAME ofn; //Информация для инициализации диалогового окна
    char szFileName[_MAX_PATH]; //Максимально доступная длина названия файла
}filename_t;

void VertScrollWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);
void VertScrollNoWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);

void HertScrollWrap(scroll_t* scroll);
void HertScrollNoWrap(text_t* text, view_t* view, scroll_t* scroll);


#endif // HEADER_H_INCLUDED
