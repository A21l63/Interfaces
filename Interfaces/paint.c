/*������� ��� ���������� ��������� ��� ������ ������
 //PARAM[paint] - ��������� �� ��������� ��������� ��� ������� � ������� ������
 //PARAM[sym] - ��������� ������������� �������
 //PARAM[str] - ����� ��������� ������ ������
 //���������� �� ������� PaintText...
 */

 #include "main.h"

void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

 /*����������� ������ � ������ � �������
 //PARAM[hdc] - ��������� ��������� ���������� (��� ������� TextOut)
 //PARAM[text] - ������ ������������� ������
 //PARAM[view] - ������ ������������� �����������
 //PARAM[scroll] - ������ ������������� �������
 */
void PaintTextWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //��������� ������
	  int textLen = 0; //����� ������ ��� ���������
    metric_t paint;  //������ �������, ��� ������ ���������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar); //�������
    int yStr = 0;
    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //����� �������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]));

        //����� ��������� ������
        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - 1;
        if(strIndex < text->strCount)
        {
            //���� ��� ������ ���������� � ����� ������ �� ������
            if (textLen <= view->width)
                    TextOut(hdc, paint.x, paint.y, printNow, textLen);
            else {
                    int k = 0;
                    while(textLen > view->width) {
                        k++;
                        //������� ��� �������
                        TextOut(hdc, paint.x - 5, paint.y, printNow, view->width);
                        strIndex++;
                        UpdatePaint(&paint, view->sym, strIndex);
                        //���, ��� �� ������, ��������� � ����� ������, � �������.
                        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]) + sizeof(char) * k * view->width);
                        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - k * view->width - 1;
                    }
                    if (textLen <= view->width) {
                        TextOut(hdc, paint.x, paint.y, printNow, textLen);
                    }
                }
            yStr++; //����������� ����� �����
        }
    }
}

 /*����������� ������ � ������ ��� ������
 //PARAM[hdc] - ��������� ��������� ���������� (��� ������� TextOut)
 //PARAM[text] - ������ ������������� ������
 //PARAM[view] - ������ ������������� �����������
 //PARAM[scroll] - ������ ������������� �������
 */
void PaintTextNoWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //�������, ������� ����� ����������
	int textLen = 0; //����� ������ ��� ���������
    metric_t paint;  //������ �������, ��� ��������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar);

    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[strIndex + scroll->yPos]) + sizeof(char) * scroll->xPos);
        //����� ���� ������
        textLen = text->strStart[strIndex + 1 + scroll->yPos] - text->strStart[strIndex + scroll->yPos] - scroll->xPos - 1;

        if(strIndex < text->strCount)
            TextOut(hdc, paint.x, paint.y, printNow, textLen);

    }
}
/*  This function is called by the Windows function DispatchMessage()  */
