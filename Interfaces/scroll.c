#include "main.h"

//Обновление вертикального скролла и его позиции в режиме с версткой

void VertScrollWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll){
    view->width = (rect.right - rect.left) / view->sym.xCaps; //Ширина окна в символах
    view->yOneList = (rect.bottom - rect.top) / view->sym.yChar; //Высота окна в символах
    scroll->scroll.cbSize = sizeof((scroll->scroll));
    scroll->scroll.fMask = SIF_ALL;
    scroll->scroll.nMin = 0; //Нижняя граница скролла
    scroll->scroll.nPage = (rect.bottom - rect.top) / view->sym.yChar; //Размер страницы

    int strIndex = 0, paintBegin=0;
    int paintEnd = text->strCount; //Сколько строк поместится в рабочую область
    int yStr = 0;
    int extraCountStr = 0; //Для переноса того, что не влезло
    long int textLen = 0; //Длина одной строки

	if (text->str != NULL) {
		for (strIndex = paintBegin; strIndex < paintEnd; strIndex++)
		{
			textLen = text->strStart[yStr+1] - text->strStart[yStr]; //Длина данной строки в символах
			if (strIndex < text->strCount + extraCountStr)
			{
				int k = 0;
				if (textLen > view->width) {
					while (textLen > view->width) {
						k++;
						strIndex++;
						//Длина строки подстраивается под размер рабочей области, остальное - переносится на следующую строку
						textLen = text->strStart[yStr + 1] - text->strStart[yStr] - k * view->width;
						paintEnd++;
						extraCountStr++;
					}
				}
				yStr++; //Увеличиваем число строк
			}
		}
		view->newStrCount = paintEnd;
	}

	//Если весь текст влезет в один лист, то скролла не надо
	if (view->yOneList >= view->newStrCount) {
		scroll->scroll.cbSize = sizeof(scroll->scroll);
		scroll->scroll.fMask = SIF_ALL;
		scroll->scroll.nMin = 0;
		scroll->scroll.nMax = 0;
		scroll->scroll.nPage = 0;
		scroll->scroll.nPos = 0;
	}

	//Иначе скролл нужен
	else {
		if (view->newStrCount <= LONG_MAX)
        {
			scroll->scroll.nMax = text->strCount - 1;
			scroll->scroll.nPos = scroll->yPos;
			scroll->sizeCoef = 1.0;
		}
		else
		{
			scroll->scroll.nMax = LONG_MAX;
			scroll->sizeCoef = (double)(fabs((double)(text->strCount - 1)) / (double)LONG_MAX);
			scroll->scroll.nPos = (int)(scroll->yPos / scroll->sizeCoef);
			scroll->scroll.nPage = (int)((rect.bottom - rect.top) / view->sym.yChar) / scroll->sizeCoef;
		}
	}
}

//Обновление вертикального скролла и его позиции в режиме без верстки

void VertScrollNoWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll)
{
     view->yOneList = (rect.bottom - rect.top) /  view->sym.yChar; //Высота всего окна в символах
     view->xOneList = (rect.right - rect.left) /  view->sym.xChar; //Ширина всего окна в символах

    //Если все влезает, то скролла не нужно
    if ( view->yOneList >= text->strCount) {
        scroll->scroll.cbSize = sizeof(scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nMax = 0;
        scroll->scroll.nPage = 0;
        scroll->scroll.nPos = 0;
    }

    //Иначе устанавливаем скролл
    else {
        scroll->scroll.cbSize = sizeof(scroll->scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nPage = (rect.bottom - rect.top) /  view->sym.yChar;

        if (text->strCount <= LONG_MAX)
        {
            scroll->scroll.nMax = text->strCount -  view->clientScreen.y /  view->sym.yChar;
            scroll->scroll.nPos = scroll->yPos;
            scroll->sizeCoef = 1.0;
        }
        else
        {
            scroll->scroll.nMax = LONG_MAX;
            scroll->sizeCoef = (double)((double)(text->strCount - 1) / (double)LONG_MAX);
            scroll->scroll.nPos = (int)(scroll->yPos/scroll->sizeCoef);
            scroll->scroll.nPage = (int)((rect.bottom - rect.top) /  view->sym.yChar) / scroll->sizeCoef;
        }
    }
}

//Обновление горизонтального скролла и его позиции в режиме с версткой

void HertScrollWrap(scroll_t* scroll)
{
    scroll-> scroll.cbSize = sizeof(scroll->scroll);
    scroll-> scroll.fMask = SIF_ALL;
    scroll-> scroll.nMin = 0;
    scroll-> scroll.nMax = 0;
    scroll-> scroll.nPage = 0;
    scroll-> scroll.nPos = 0;
}


//Обновление горизонтального скролла и его позиции в режиме без верстки

void HertScrollNoWrap(text_t* text, view_t* view, scroll_t* scroll)
{
    //Если все влезает, то скролл не нужен
    if (view->xOneList >= text->maxStrLen) {
        scroll->scroll.cbSize = sizeof(scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nMax = 0;
        scroll->scroll.nPage = 0;
        scroll->scroll.nPos = 0;
    }
    else {

        //Устанавливаем горизонтальный скролл, зная длину самой длинной строки
        scroll->scroll.nMax = 2 + (view->iMaxWidth - view->clientScreen.x) / view->sym.xChar;
        if (scroll->scroll.nMax < 0) {
            scroll->scroll.nMax = 0;
        }
        scroll->scroll.nPage = view->clientScreen.x / view->sym.xChar;
        scroll->scroll.nPos = scroll->xPos;
    }
}
