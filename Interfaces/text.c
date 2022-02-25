#include "main.h"

//����� ����� ����������� ������� ������ ��� ������ ��� �������


uint FindMaxStrLen(char* str) {
    uint i = 0, max = 0, len = 0; //max - ����� ���������� ������
    while(str[i] != '\0') {
        len = 0;
        while(str[i] != '\0' && str[i] != '\n'){
            ++len;
            ++i;
        }
        max= len > max? len:max;
        ++i;
    }
    return max;
}

//������� ��������� ������ ��� ���������� ��������� ��� �������� �����
void FreeText(text_t* text) {

    if (text->strStart){
        free(text->strStart);
        text->strStart = NULL;
    }
    if (text->str){
        free(text->str);
        text->str = NULL;
    }

    text->maxStrLen = 0;
    text->strCount = 0;
}

//������������� ��������� ����������� ����

void FileInit(HWND hwnd, filename_t* filename)
{
    static char szTitleName[_MAX_FNAME + _MAX_EXT]; //������ - �������� �����
    filename->ofn.lStructSize       = sizeof(OPENFILENAME);
    filename->ofn.hwndOwner         = hwnd;
    filename->ofn.lpstrFilter       = "text(*.TXT)\0*.txt\0\0"; //��������� ��� �������� ������ ��������� ������;
    filename->ofn.lpstrDefExt       = "txt";
    filename->ofn.nMaxFile          = _MAX_PATH;
    filename->ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT;
    filename->ofn.nMaxCustFilter    = 0;
    filename->ofn.nFilterIndex      = 0;
    filename->ofn.Flags             = OFN_HIDEREADONLY; //Hides the Read Only check box.
    filename->ofn.nFileOffset       = 0;
    filename->ofn.nFileExtension    = 0;
    filename->ofn.lCustData         = 0L;
    filename->ofn.lpfnHook          = NULL;
    filename->ofn.lpTemplateName    = NULL;
    filename->ofn.lpstrInitialDir   = NULL;
    filename->ofn.lpstrTitle        = NULL;
    filename->ofn.lpstrFileTitle    = szTitleName;//�������� �����
    filename->ofn.lpstrFile         = filename->szFileName; //�������������� �����
    filename->ofn.lpstrCustomFilter = NULL;
    filename->ofn.hInstance         = NULL;
}


//������ ������ �� �����
BOOL ReadText(text_t* text, char* fileName, view_t* view)
{
    FILE* file = fopen(fileName, "r");
    if(!file)
        return FALSE;

    uint strCount=0;
    uint i=0, sizeText=0;

    fseek(file, 0, SEEK_END);
    sizeText = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (sizeText > 0) {
        //��������� ������ ��� ���� ��������� ������
        text->str = (char*)calloc((sizeText + 1), sizeof(char));
        text->strStart = (uint*)calloc(sizeText, sizeof(uint));
    }
    else { //���� ����
        fclose(file);
        return FALSE;
    }

    if (text->str && text->strStart) {
        text->str[sizeText] = '\0';
        fread(text->str, sizeof(char), sizeText, file); //��������� ���� (����)
        for (i = 0; i < sizeText; i++) {
            if (text->str[i] == '\n'){//�������� ����� ������
                strCount++;
                text->strStart[strCount] = i + 1; //����� ������ ������
                sizeText--;
            }
        }
        text->strStart[strCount + 1] = i;
        text->strCount = strCount + 1;

        //���������� ����� ������ � ������
        text->maxStrLen = FindMaxStrLen(text->str);

        //������������ ����� �������� � ������ ��� ������ ��� �������
        view->iMaxWidth = text->maxStrLen * view->sym.xCaps;

    }

    //������ ��������� ������ ��� str ��� strStart
    else {
            FreeText(text);
            fclose(file);
            return FALSE;
        }
    fclose(file);
    return TRUE;
}
