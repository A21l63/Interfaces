#include "main.h"

//Функция обновления координат для вывода текста
void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

//Вывод текста с версткой
void PaintTextWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //Текст на вывод
	  int strLen = 0; //Отрисовываемая длина строки
    metric_t paint;  //Рабочая область вывода
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar); //минимум
    int yStr = 0;
    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //обновили координаты выводимой области

        //Будем выводить эту строку
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]));

        //длина выводимой строки
        strLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - 1;
        if(strIndex < text->strCount)
        {
            //Если вся строка помещается в одной строке на экране
            if (strLen <= view->width)
                    TextOut(hdc, paint.x, paint.y, printNow, strLen);
            else {
                    int k = 0;
                    while(strLen > view->width) {
                        k++;
                        //Выводим что влезает
                        TextOut(hdc, paint.x - 5, paint.y, printNow, view->width);
                        strIndex++;
                        UpdatePaint(&paint, view->sym, strIndex);
                        //Все, что не влезло, перенесли в новую строку, и выводим.
                        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]) + sizeof(char) * k * view->width);
                        strLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - k * view->width - 1;
                    }
                    if (strLen <= view->width) {
                        TextOut(hdc, paint.x, paint.y, printNow, strLen);
                    }
                }
            yStr++; //увеличиваем число строк
        }
    }
}

//Отображения текста в режиме без вёрстки

void PaintTextNoWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //Текст на вывод
	int strLen = 0;    //Отрисовываемая длина строки
    metric_t paint;     //Рабочая область вывода
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar);

    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //обновили координаты выводимой области

        //Выводим эту строку
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[strIndex + scroll->yPos]) + sizeof(char) * scroll->xPos);
        //Длина этой строки
        strLen = text->strStart[strIndex + 1 + scroll->yPos] - text->strStart[strIndex + scroll->yPos] - scroll->xPos - 1;

        if(strIndex < text->strCount)
            TextOut(hdc, paint.x, paint.y, printNow, strLen);

    }
}
/*  This function is called by the Windows function DispatchMessage()  */
