#include "Add.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <string>

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterAddClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcAdd;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaAddClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Add
LRESULT CALLBACK WndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ProcessarMenu(hWnd, message, wParam, lParam);

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Texto de exemplo
        RECT rect;
        GetClientRect(hWnd, &rect);

        // Fundo branco
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Obter dimensões da janela
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela com scroll
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = 21;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - g_scrollY;  // Posição Y com scroll
        int startX = 22 - g_scrollX;  // Posição X com scroll

        // Desenhar a grade
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        // Desenhar fundos alternados para as linhas
        HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));
        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushWhite);

        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);
        int colNumber = 0;
        int rowNumber = 0;

        //Título
        windowsTitle(hdc, startX, startY - 20, L"CRIAR REGISTRO", 14);

        for (size_t col = 0; col < 17; col++) {
            colNumber++;

            HBRUSH hCurrentBrush = (col % 2 == 0) ? hBrushGray : hBrushWhite;
            SelectObject(hdc, hCurrentBrush);

            // Desenhar o fundo da linha (com scroll)
            RECT rowRect = {
                startX,
                startY + static_cast<int>(colNumber) * cellHeight,
                startX + width,
                startY + (static_cast<int>(colNumber) + 1) * cellHeight
            };
            FillRect(hdc, &rowRect, hCurrentBrush);

            for (size_t row = 0; row < 2; row++) {
                int xPos;
                int yPos;

                if (row == 0) {
                    xPos = startX;
                    yPos = startY + colNumber * cellHeight + 7;
                    fonte(L"Header", RGB(0, 0, 0), hdc);
                }
                else {
                    xPos = startX + cellWidth + 2;
                    yPos = startY + colNumber * cellHeight + 7;
                    fonte(L"Font", RGB(0, 0, 0), hdc);
                }

                if (row == 0 && col == 0) {
                    TextOut(hdc, xPos, yPos, L"Nome do Pet", 11);
                }
                else if (row == 0 && col == 0) {
                    TextOut(hdc, xPos, yPos, L"Raça", 4);
                }
                else if (row == 0 && col == 1) {
                    TextOut(hdc, xPos, yPos, L"Nome do Tutor", 13);
                }
                else if (row == 0 && col == 2) {
                    TextOut(hdc, xPos, yPos, L"CEP", 3);
                }
                else if (row == 0 && col == 3) {
                    TextOut(hdc, xPos, yPos, L"Cor", 3);
                }
                else if (row == 0 && col == 4) {
                    TextOut(hdc, xPos, yPos, L"Idade", 5);
                }
                else if (row == 0 && col == 5) {
                    TextOut(hdc, xPos, yPos, L"Peso", 4);
                }
                else if (row == 0 && col == 6) {
                    TextOut(hdc, xPos, yPos, L"Sexo", 4);
                }
                else if (row == 0 && col == 7) {
                    TextOut(hdc, xPos, yPos, L"Castrado", 8);
                }
                else if (row == 0 && col == 8) {
                    TextOut(hdc, xPos, yPos, L"Endereço", 8);
                }
                else if (row == 0 && col == 9) {
                    TextOut(hdc, xPos, yPos, L"Ponto de Referência", 19);
                }
                else if (row == 0 && col == 10) {
                    TextOut(hdc, xPos, yPos, L"Banho", 5);
                }
                else if (row == 0 && col == 11) {
                    TextOut(hdc, xPos, yPos, L"Tosa", 4);
                }
                else if (row == 0 && col == 12) {
                    TextOut(hdc, xPos, yPos, L"Observação", 10);
                }
                else if (row == 0 && col == 13) {
                    TextOut(hdc, xPos, yPos, L"Parasitas", 9);
                }
                else if (row == 0 && col == 14) {
                    TextOut(hdc, xPos, yPos, L"Lesões", 6);
                }
                else if (row == 0 && col == 15) {
                    TextOut(hdc, xPos, yPos, L"Observação", 10);
                }
                else if (row == 0 && col == 16) {
                    TextOut(hdc, xPos, yPos, L"Telefone", 8);
                }
                else if (row == 0 && col == 17) {
                    TextOut(hdc, xPos, yPos, L"CPF", 3);
                }
                else {
                    TextOut(hdc, xPos, yPos, L"Input", 5);
                }
            rowNumber++;
            }
        }

        // Limpar recursos
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrushHeader);
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_KEYDOWN:
    {
        Shortcuts(hWnd, message, wParam, lParam);
        break;
    }

    case WM_DESTROY:
    {
        windowClose(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela add!", 21);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitAdd(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcAdd;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AddClass";
    return RegisterClassW(&wc);
}