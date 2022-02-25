
#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include "main.h"
#include "scroll.c"
#include "text.c"
#include "paint.c"



/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("Reader");

TCHAR szMenuName[ ] = _T("Menu");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;               /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);

    wincl.lpszMenuName = szMenuName;                 /*Menu */

    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); /*белый цвет экрана*/

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("My Reader"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           480,                 /* The programs width */
           360,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument                 /* Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm; //Основная информация о физическом шрифте
    RECT rect; //Клиентское окно

    static HMENU hMenu;

    static filename_t filename;
    static text_t text; //Модель работы с текстом
    static view_t view;
    static scroll_t scroll;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {

        //Инициализация статических переменных
        view.newStrCount = 0;
        view.yOneList = 0;
        view.xOneList = 0;
        scroll.sizeCoef = 1.0;
        view.selectedMode = IDM_WRAP_OFF;

        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        view.sym.xChar = tm.tmAveCharWidth; //Cредняя ширина символов
        view.sym.xCaps =(tm.tmPitchAndFamily & 1 ? 3 : 2) * view.sym.xChar / 2;
        view.sym.yChar = tm.tmHeight + tm.tmExternalLeading; //Dысота + межстрочное пространство
        ReleaseDC(hwnd, hdc); //Jсвобождает контекст устройства для использования другими приложениями

        CREATESTRUCT* p = (CREATESTRUCT*) lParam;
        FileInit(hwnd, &filename);

        //Если файл был передан через аргумент командной строки
        char* file = (char*)p->lpCreateParams;
         if (ReadText(&text, file, &view) == TRUE) {
            hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED); //Устанавливает атрибут "галочки" заданного пункта меню в выбранное состояние
        }
        return 0;
    }
        case WM_PAINT: {
            hdc = BeginPaint(hwnd, &ps);

            //Текст существует, тогда выводим его
            if(text.str != NULL) {
                if (view.selectedMode == IDM_WRAP_ON)
                    PaintTextWrap(hdc, &text, &view, &scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    PaintTextNoWrap(hdc, &text, &view, &scroll);
            }
	    	EndPaint(hwnd, &ps);
	    	return 0;
        }

        //Сообщение на изменение окна
        case WM_SIZE : {
            view.clientScreen.x = LOWORD(lParam);
            view.clientScreen.y = HIWORD(lParam);

            GetClientRect(hwnd, &rect);
            GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

            //Обновление вертикального скролла
		    if (view.selectedMode == IDM_WRAP_ON)
               VertScrollWrap(&text, &view, rect, &scroll);
            else if(view.selectedMode == IDM_WRAP_OFF)
                VertScrollNoWrap(&text, &view,  rect, &scroll);
            SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

            //Обновление горизонтального скролла
            if (view.selectedMode == IDM_WRAP_ON)
                HertScrollWrap(&scroll);
            else if(view.selectedMode == IDM_WRAP_OFF)
                HertScrollNoWrap(&text, &view, &scroll);
            SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

		    InvalidateRect(hwnd, NULL, TRUE); //Указывает прямоугольник для перерисовки окна
		    UpdateWindow(hwnd); //Обновление окна

            return 0;
        }

        //Горизонтальная прокрутка
        case WM_HSCROLL:
	    {
		    scroll.scroll.cbSize = sizeof(scroll.scroll);
		    scroll.scroll.fMask = SIF_ALL; //SIF_PAGE, SIF_POS, SIF_RANGE, SIF_TRACKPOS

            //Получает параметры для горизонтальной полосы прокрутки у окна
		    GetScrollInfo(hwnd, SB_HORZ, &(scroll.scroll));
		    scroll.xPos = scroll.scroll.nPos; //Позиция ползунка

		    switch (LOWORD(wParam))
		    {
		        case SB_LINERIGHT:
		        	scroll.scroll.nPos++;
		        	break;
		        case SB_LINELEFT:
		        	scroll.scroll.nPos--;
		        	break;
                case SB_PAGERIGHT: //Сдвиг вправо на ширину окна
                    scroll.scroll.nPos += view.xOneList;
                    break;
	    	    case SB_PAGELEFT: //Сдвиг влево на ширину окна
		     	    scroll.scroll.nPos -= view.xOneList;
		     	    break;
	    	    case SB_THUMBTRACK: //Если работа происходит мышкой
	    	    	scroll.scroll.nPos = scroll.scroll.nTrackPos;
	     	    	break;
	    	    }
	    	    scroll.scroll.fMask = SIF_POS;

	    	    if(scroll.scroll.nPos <= 0)
                    scroll.scroll.nPos = 0;
                else if(scroll.scroll.nPos >= scroll.scroll.nMax*scroll.sizeCoef)
                    scroll.scroll.nPos = scroll.scroll.nMax * scroll.sizeCoef;

		        //Устанавливает параметры горизонтальной полосы прокрутки
		        SetScrollInfo(hwnd, SB_HORZ, &(scroll.scroll), TRUE);

		        //Обновить координаты
	    	    if (scroll.scroll.nPos != scroll.xPos){
		        	scroll.xPos = scroll.scroll.nPos;
		        	InvalidateRect(hwnd, NULL, TRUE);
		        	UpdateWindow(hwnd);
	    	    }
		    return 0;
	    }
    //Вертикальная прокрутка
	case WM_VSCROLL:
	    {

		    scroll.scroll.cbSize = sizeof(scroll.scroll);
		    scroll.scroll.fMask = SIF_ALL;

		    GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));
            scroll.prevPos = scroll.yPos;

		    switch (LOWORD(wParam))
            {
		        case SB_TOP:
			        scroll.yPos = 0;
			        scroll.scroll.nPos = scroll.scroll.nMin;
			        break;
		        case SB_BOTTOM:
		    	    scroll.scroll.nPos = scroll.scroll.nMax;
		    	    scroll.yPos = scroll.scroll.nMax * scroll.sizeCoef; //новая позиция с учетом изменения
                    break;
		        case SB_LINEUP:
			        scroll.yPos--;
			        scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
			        break;
	    	    case SB_LINEDOWN:
		    	    scroll.yPos++;
		    	    scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
		    	    break;
		        case SB_PAGEUP: //Скролл вверх на высоту окна
			        scroll.yPos -= view.yOneList;
			        scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
			        break;
		        case SB_PAGEDOWN: //Скролл вниз на высоту окна
		        	scroll.yPos += view.yOneList;
		        	scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
	    	    	break;
	    	    case SB_THUMBTRACK: //Пользователь дергает прокрутку
		    	    scroll.scroll.nPos = (scroll.scroll.nTrackPos);
		    	    scroll.yPos = scroll.scroll.nPos * scroll.sizeCoef;
		    	    break;
            }

            if(scroll.yPos <= 0)
                scroll.yPos = 0;
            else if(scroll.yPos >= scroll.scroll.nMax*scroll.sizeCoef)
                scroll.yPos = scroll.scroll.nMax * scroll.sizeCoef;

            scroll.scroll.fMask = SIF_POS;
		    SetScrollInfo(hwnd, SB_VERT, &(scroll.scroll), TRUE);

		    if (scroll.yPos != scroll.prevPos){
			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
            }
		    return 0;
	    }

        //Обработка нажатий клавиатруры
	    case WM_KEYDOWN:
	    {
	        switch (wParam)
	        {
		        case VK_UP:
			        SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
		        case VK_DOWN:
		        	SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			        break;
		        case VK_LEFT:
		        	SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0);
		        	break;
		        case VK_RIGHT:
			        SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0);
			        break;
                case VK_PRIOR: //PAGE UP key
			        SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			        break;
		        case VK_NEXT: //PAGE DOWN key
		        	SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			        break;
                case VK_HOME: //HOME key
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END: //END key
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
	    	}
		    return 0;
	    }

        //Обработка пунктов меню
	    case WM_COMMAND:
        {
		    hMenu = GetMenu(hwnd);
		    switch (LOWORD(wParam))
            {
            //Открытие текстового файла
		    case IDM_OPEN:


                if(GetOpenFileName(&(filename.ofn)))
                {
                    //Очиста предыдущего текста, если он был
			        FreeText(&text);

			        //Устанавливает атрибут "галочки" заданного пункта меню в выбранное состояние
			        CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                    //Чтение текста
                    if (ReadText(&text, filename.szFileName, &view) == FALSE)
                        break;

			        scroll.xPos = 0;
			        scroll.yPos = 0;
			        view.iMaxWidth = text.maxStrLen * view.sym.xCaps; //максимальная длина
			        GetClientRect(hwnd, &rect);
                    GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                    //Установка вертикального скролла
                    if (view.selectedMode == IDM_WRAP_ON)
                        VertScrollWrap(&text, &view, rect, &scroll);
                    else if(view.selectedMode == IDM_WRAP_OFF)
                        VertScrollNoWrap(&text, &view,  rect, &scroll);
                    SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                    //Установка горизонтального скролла
                    if (view.selectedMode == IDM_WRAP_ON)
                        HertScrollWrap(&scroll);
                    else if(view.selectedMode == IDM_WRAP_OFF)
                        HertScrollNoWrap(&text, &view,  &scroll);
                    SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);
                }

			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
			    break;

            //Закрытие текстового файла, не программы, и удаление скролла
            case IDM_CLOSE:
                FreeText(&text);

                //Устанавливает атрибут "галочки" заданного пункта меню в выбранное состояние
                CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                scroll.xPos = 0;
                scroll.yPos = 0;
                view.iMaxWidth = 0; //Максимальная длина
                view.newStrCount = 0;
                GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                //Очистка вертикального скролла
                if (view.selectedMode == IDM_WRAP_ON)
                   VertScrollWrap(&text, &view, rect, &scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    VertScrollNoWrap(&text, &view,  rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, 0, FALSE);
                SetScrollPos(hwnd, SB_VERT, 0, TRUE);

                //Очистка горизонтального скролла
                if (view.selectedMode == IDM_WRAP_ON)
                    HertScrollWrap(&scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    HertScrollNoWrap(&text, &view,  &scroll);
                SetScrollRange(hwnd, SB_HORZ, 0, 0, FALSE);
                SetScrollPos(hwnd, SB_HORZ, 0, TRUE);
			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
                break;

            //Закрыть всю программу
            case IDM_EXIT:
                SendMessage(hwnd, WM_CLOSE,0,0);
                break;

		    case IDM_WRAP_OFF:
                //Убрать галочку с выбранного режима
		        CheckMenuItem(hMenu, view.selectedMode, MF_UNCHECKED);
			    view.selectedMode = LOWORD(wParam);

			    //Поставить галочку на новый режим
			    CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

			    GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                //Установка вертикального скролла
                VertScrollNoWrap(&text, &view,  rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                //Установка горизонтального скролла
                HertScrollNoWrap(&text, &view,  &scroll);
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);

		    	return 0;

            case IDM_WRAP_ON:

		        //Убрать галочку с выбранного режима
			    CheckMenuItem(hMenu, view.selectedMode, MF_UNCHECKED);
			    view.selectedMode = LOWORD(wParam);

			    //Поставить галочку на новый режим
			    CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                //Вертикальный скролл
                VertScrollWrap(&text, &view, rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                //Горизонтальный скролл: поставить на ноль в случае верстки
                HertScrollWrap(&scroll);
                scroll.xPos = 0;
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

		    	InvalidateRect(hwnd, NULL, TRUE);
		    	UpdateWindow(hwnd);
		    	return 0;
	    	}
	      	return 0;
        }

        case WM_DESTROY: {
            FreeText(&text);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
