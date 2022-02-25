#include "main.h"

//поиск длины максимально длинной строки для вывода без верстки


uint FindMaxStrLen(char* str) {
    uint i = 0, max = 0, len = 0; //max - длина наибольшей строки
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

//Очистка структуры текста для завершения программы или закрытия файла
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

//Инициализация структуры диалогового окна

void FileInit(HWND hwnd, filename_t* filename)
{
    static char szTitleName[_MAX_FNAME + _MAX_EXT]; //Массив - названия файла
    filename->ofn.lStructSize       = sizeof(OPENFILENAME);
    filename->ofn.hwndOwner         = hwnd;
    filename->ofn.lpstrFilter       = "text(*.TXT)\0*.txt\0\0"; //параметры для открытия только текстовых файлов;
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
    filename->ofn.lpstrFileTitle    = szTitleName;//Название файла
    filename->ofn.lpstrFile         = filename->szFileName; //Местоположение файла
    filename->ofn.lpstrCustomFilter = NULL;
    filename->ofn.hInstance         = NULL;
}


//Чтение данных из файла
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
        //выделение памяти под поля структуры текста
        text->str = (char*)calloc((sizeText + 1), sizeof(char));
        text->strStart = (uint*)calloc(sizeText, sizeof(uint));
    }
    else { //файл пуст
        fclose(file);
        return FALSE;
    }

    if (text->str && text->strStart) {
        text->str[sizeText] = '\0';
        fread(text->str, sizeof(char), sizeText, file); //Считываем файл (весь)
        for (i = 0; i < sizeText; i++) {
            if (text->str[i] == '\n'){//Проверка конца строки
                strCount++;
                text->strStart[strCount] = i + 1; //Новый отступ строки
                sizeText--;
            }
        }
        text->strStart[strCount + 1] = i;
        text->strCount = strCount + 1;

        //Наибольшая длина строки в тексте
        text->maxStrLen = FindMaxStrLen(text->str);

        //Максимальное число пикселей в строке для вывода без верстки
        view->iMaxWidth = text->maxStrLen * view->sym.xCaps;

    }

    //Ошибка выделения памяти под str или strStart
    else {
            FreeText(text);
            fclose(file);
            return FALSE;
        }
    fclose(file);
    return TRUE;
}
