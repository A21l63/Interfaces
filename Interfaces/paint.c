/*‘ункци€ дл€ обновлени€ координат дл€ вывода текста
 //PARAM[paint] - указатель на структуру координат при рисовке в рабочей облсти
 //PARAM[sym] - структура представлени€ символа
 //PARAM[str] - номер выводимой строки строки
 //¬ызываетс€ из функции PaintText...
 */

 #include "main.h"

void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

 /*ќтображени€ текста в режиме с вЄрсткой
 //PARAM[hdc] - описатель контекста устройства (ƒл€ функции TextOut)
 //PARAM[text] - модель представлени€ текста
 //PARAM[view] - модель представлени€ изображени€
 //PARAM[scroll] - модель представлени€ скролла
 */
void PaintTextWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //выводима€ строка
	  int textLen = 0; //длина строки при отрисовке
    metric_t paint;  //размер области, где строка выводитс€
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar); //минимум
    int yStr = 0;
    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //обновили координаты выводимой области

        //Ѕудем выводить эту строку
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]));

        //длина выводимой строки
        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - 1;
        if(strIndex < text->strCount)
        {
            //≈сли вс€ строка помещаетс€ в одной строке на экране
            if (textLen <= view->width)
                    TextOut(hdc, paint.x, paint.y, printNow, textLen);
            else {
                    int k = 0;
                    while(textLen > view->width) {
                        k++;
                        //¬ыводим что влезает
                        TextOut(hdc, paint.x - 5, paint.y, printNow, view->width);
                        strIndex++;
                        UpdatePaint(&paint, view->sym, strIndex);
                        //¬се, что не влезло, перенесли в новую строку, и выводим.
                        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]) + sizeof(char) * k * view->width);
                        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - k * view->width - 1;
                    }
                    if (textLen <= view->width) {
                        TextOut(hdc, paint.x, paint.y, printNow, textLen);
                    }
                }
            yStr++; //увеличиваем число строк
        }
    }
}

 /*ќтображени€ текста в режиме без вЄрстки
 //PARAM[hdc] - описатель контекста устройства (ƒл€ функции TextOut)
 //PARAM[text] - модель представлени€ текста
 //PARAM[view] - модель представлени€ изображени€
 //PARAM[scroll] - модель представлени€ скролла
 */
void PaintTextNoWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //строчка, котора€ будет выводитьс€
	int textLen = 0; //длина строки при отрисовке
    metric_t paint;  //размер области, где выводить
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar);

    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //обновили координаты выводимой области

        //¬ыводим эту строку
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[strIndex + scroll->yPos]) + sizeof(char) * scroll->xPos);
        //ƒлина этой строки
        textLen = text->strStart[strIndex + 1 + scroll->yPos] - text->strStart[strIndex + scroll->yPos] - scroll->xPos - 1;

        if(strIndex < text->strCount)
            TextOut(hdc, paint.x, paint.y, printNow, textLen);

    }
}
/*  This function is called by the Windows function DispatchMessage()  */
