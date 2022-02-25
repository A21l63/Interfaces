#include "main.h"

//������� ���������� ��������� ��� ������ ������
void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

//����� ������ � ��������
void PaintTextWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //����� �� �����
	  int strLen = 0; //�������������� ����� ������
    metric_t paint;  //������� ������� ������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar); //�������
    int yStr = 0;
    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //����� �������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]));

        //����� ��������� ������
        strLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - 1;
        if(strIndex < text->strCount)
        {
            //���� ��� ������ ���������� � ����� ������ �� ������
            if (strLen <= view->width)
                    TextOut(hdc, paint.x, paint.y, printNow, strLen);
            else {
                    int k = 0;
                    while(strLen > view->width) {
                        k++;
                        //������� ��� �������
                        TextOut(hdc, paint.x - 5, paint.y, printNow, view->width);
                        strIndex++;
                        UpdatePaint(&paint, view->sym, strIndex);
                        //���, ��� �� ������, ��������� � ����� ������, � �������.
                        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]) + sizeof(char) * k * view->width);
                        strLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - k * view->width - 1;
                    }
                    if (strLen <= view->width) {
                        TextOut(hdc, paint.x, paint.y, printNow, strLen);
                    }
                }
            yStr++; //����������� ����� �����
        }
    }
}

//����������� ������ � ������ ��� ������

void PaintTextNoWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //����� �� �����
	int strLen = 0;    //�������������� ����� ������
    metric_t paint;     //������� ������� ������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar);

    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[strIndex + scroll->yPos]) + sizeof(char) * scroll->xPos);
        //����� ���� ������
        strLen = text->strStart[strIndex + 1 + scroll->yPos] - text->strStart[strIndex + scroll->yPos] - scroll->xPos - 1;

        if(strIndex < text->strCount)
            TextOut(hdc, paint.x, paint.y, printNow, strLen);

    }
}
/*  This function is called by the Windows function DispatchMessage()  */
