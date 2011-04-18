/*
 *  The MIT License
 *
 *  Copyright 2008-2011 Andrew James <ephphatha@thelettereph.com>.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */
#include <windows.h>
#include <scrnsave.h>
#include <commctrl.h>
#include <time.h>

#pragma comment(lib, "Scrnsavw.lib")
#pragma comment(lib, "comctl32.lib")

void DrawDigit(HDC hdc, const RECT *boundingRect, int digit);
void DefineRects(const int XANCHOR, const int YANCHOR,
                 const int NUMBERWIDTH,
                 RECT *hoursTens, RECT *hoursOnes,
                 RECT *minutesTens, RECT *minutesOnes,
                 RECT *secondsTens, RECT *secondsOnes);

LRESULT WINAPI ScreenSaverProc(HWND hwnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam)
{
    HDC hdc, hdcMem;
    PAINTSTRUCT ps;
    HBITMAP bitmap;
    HPEN initialPen, pen;
    HBRUSH initialBrush, brush;
    static RECT clientRect, hoursTens, hoursOnes, minutesTens, minutesOnes,
                secondsTens, secondsOnes;

    time_t theTime;
    struct tm local;

    static int numberWidth = 0, xCenter = 0, yCenter = 0;

    switch(message)
    {
        case WM_CREATE:
            SetTimer(hwnd, 1, 500, NULL);
            // ShowCursor(FALSE);
            GetClientRect(hwnd, &clientRect);
            numberWidth = (clientRect.right / 10);
            xCenter = (clientRect.right / 2);
            yCenter = (clientRect.bottom / 2);
            DefineRects(xCenter, yCenter, numberWidth, &hoursTens, &hoursOnes,
                       &minutesTens, &minutesOnes, &secondsTens, &secondsOnes);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            hdcMem = CreateCompatibleDC(hdc);
            bitmap = CreateCompatibleBitmap(hdc, clientRect.right,
                                            clientRect.bottom);
            SelectObject(hdcMem, bitmap);

            pen = CreatePen(PS_SOLID, 1, 0x00000000);
            brush = CreateSolidBrush(0x00005500);

            initialPen = (HPEN)SelectObject(hdcMem, pen);
            initialBrush = (HBRUSH)SelectObject(hdcMem, brush);

            Ellipse(hdcMem,
                    (hoursOnes.right
                    + ((minutesTens.left - hoursOnes.right) / 2))
                    - (numberWidth / 16),
                    (yCenter - (numberWidth / 2)) - (numberWidth / 16),
                    (hoursOnes.right
                    + ((minutesTens.left - hoursOnes.right) / 2))
                    + (numberWidth / 16),
                    (yCenter - (numberWidth / 2)) + (numberWidth / 16));
            Ellipse(hdcMem,
                    (hoursOnes.right
                    + ((minutesTens.left - hoursOnes.right) / 2))
                    - (numberWidth / 16),
                    (yCenter + (numberWidth / 2)) - (numberWidth / 16),
                    (hoursOnes.right
                    + ((minutesTens.left - hoursOnes.right) / 2))
                    + (numberWidth / 16),
                    (yCenter + (numberWidth / 2)) + (numberWidth / 16));

            SelectObject(hdcMem, initialPen);
            SelectObject(hdcMem, initialBrush);

            DeleteObject(pen);
            DeleteObject(brush);

            theTime = time(NULL);
            localtime_s(&local, &theTime);

            DrawDigit(hdcMem, &hoursTens, local.tm_hour / 10);
            DrawDigit(hdcMem, &hoursOnes, local.tm_hour % 10);
            DrawDigit(hdcMem, &minutesTens, local.tm_min / 10);
            DrawDigit(hdcMem, &minutesOnes, local.tm_min % 10);
            DrawDigit(hdcMem, &secondsTens, local.tm_sec / 10);
            DrawDigit(hdcMem, &secondsOnes, local.tm_sec % 10);

            BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom,
                   hdcMem, 0, 0, SRCCOPY);

            DeleteDC(hdcMem);
            DeleteObject(bitmap);
            DeleteObject(initialPen);
            DeleteObject(initialBrush);
            EndPaint(hwnd, &ps);
			return 0;

        case WM_TIMER:
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        // case WM_KEYDOWN:
            // if(wParam == VK_ESCAPE)
            // {
                // PostMessage(hwnd, WM_DESTROY, 0, 0);
            // }
            // return 0;

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            // ShowCursor(TRUE);
            PostQuitMessage (0);
            return 0;

        default:
            return DefScreenSaverProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void DrawDigit(HDC hdc, const RECT *boundingRect, int digit)
{
    HPEN initialPen, pen;
    POINT initialPos;
    int width = boundingRect->right - boundingRect->left;
    int height = boundingRect->bottom - boundingRect->top;
    int leftMark = boundingRect->left + (width / 10);
    int rightMark = boundingRect->right - (width /10);
    int topMark = boundingRect->top + (width / 10);
    int bottomMark = boundingRect->bottom - (width / 10);
    int middleMark = boundingRect->top + height / 2;
    pen = CreatePen(PS_SOLID, width / 15, 0x00005500);
    initialPen = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, leftMark, middleMark, &initialPos);

    if(((digit > 3) && (digit != 7)) || (digit == 0))
    {
        LineTo(hdc, leftMark, topMark);
    }
    else
    {
        MoveToEx(hdc, leftMark, topMark, NULL);
    }

    if((digit != 1 ) && (digit != 4))
    {
        LineTo(hdc, rightMark, topMark);
    }
    else
    {
        MoveToEx(hdc, rightMark, topMark, NULL);
    }

    if((digit != 5) && (digit != 6))
    {
        LineTo(hdc, rightMark, middleMark);
    }
    else
    {
        MoveToEx(hdc, rightMark, middleMark, NULL);
    }

    if(digit != 2)
    {
        LineTo(hdc, rightMark, bottomMark);
    }
    else
    {
        MoveToEx(hdc, rightMark, bottomMark, NULL);
    }

    if(digit % 3 != 1)
    {
        LineTo(hdc, leftMark, bottomMark);
    }
    else
    {
        MoveToEx(hdc, leftMark, bottomMark, NULL);
    }

    if((digit % 2 == 0) && (digit != 4))
    {
        LineTo(hdc, leftMark, middleMark);
    }
    else
    {
        MoveToEx(hdc, leftMark, middleMark, NULL);
    }

    if((digit != 1) && (digit != 7) && (digit != 0))
    {
        LineTo(hdc, rightMark, middleMark);
    }

    MoveToEx(hdc, initialPos.x, initialPos.y, NULL);
    SelectObject(hdc, initialPen);
    DeleteObject(pen);
}

void DefineRects(const int XANCHOR, const int YANCHOR,
                 const int NUMBERWIDTH,
                 RECT *hoursTens, RECT *hoursOnes,
                 RECT *minutesTens, RECT *minutesOnes,
                 RECT *secondsTens, RECT *secondsOnes)
{
    hoursTens->top = YANCHOR - NUMBERWIDTH;
    hoursTens->bottom = YANCHOR + NUMBERWIDTH;
    hoursOnes->top = hoursTens->top;
    hoursOnes->bottom = hoursTens->bottom;

    minutesTens->top = hoursTens->top;
    minutesTens->bottom = hoursTens->bottom;
    minutesOnes->top = hoursTens->top;
    minutesOnes->bottom = hoursTens->bottom;

    secondsTens->top = YANCHOR;
    secondsTens->bottom = hoursTens->bottom;
    secondsOnes->top = YANCHOR;
    secondsOnes->bottom = hoursTens->bottom;

    hoursTens->left =  XANCHOR - (3 * NUMBERWIDTH);
    hoursTens->right = hoursTens->left + NUMBERWIDTH;
    hoursOnes->left = hoursTens->right;
    hoursOnes->right = hoursOnes->left + NUMBERWIDTH;

    minutesTens->left = hoursOnes->right + (NUMBERWIDTH / 2);
    minutesTens->right = minutesTens->left + NUMBERWIDTH;
    minutesOnes->left = minutesTens->right;
    minutesOnes->right = minutesOnes->left + NUMBERWIDTH;

    secondsTens->left = minutesOnes->right;
    secondsTens->right = secondsTens->left + (NUMBERWIDTH / 2);
    secondsOnes->left = secondsTens->right;
    secondsOnes->right = secondsOnes->left + (NUMBERWIDTH / 2);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg,
                                       UINT message,
                                       WPARAM wParam,
                                       LPARAM lParam)
{
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}