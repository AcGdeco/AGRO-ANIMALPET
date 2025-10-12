#include "Consultar.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <string>
#include "Select.h"
#include "sqlite3.h"
#include <vector>

std::vector<std::vector<std::wstring>> g_tableDataConsulta;

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring utf8_to_wstring_consulta(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

int sqlite_callback_consulta(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? utf8_to_wstring_consulta(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? utf8_to_wstring_consulta(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterReadClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcRead;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaReadClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Read
LRESULT CALLBACK WndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

        // Consultar o banco apenas se a tabela estiver vazia
        if (g_tableDataConsulta.empty()) {
            sqlite3* db;
            char* errMsg = 0;
            int rc = sqlite3_open("pet.db", &db);
            if (rc == SQLITE_OK) {
                const char* sqlSelectConsulta = "SELECT * FROM Pets;";
                rc = sqlite3_exec(db, sqlSelectConsulta, sqlite_callback_consulta, &g_tableDataConsulta, &errMsg);
                if (rc != SQLITE_OK) {
                    if (errMsg) {
                        // Converte char* para wchar_t* corretamente
                        size_t len = strlen(errMsg) + 1;
                        std::wstring wErrMsg(len, L'\0');
                        mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                        // Remove o caractere nulo extra do final
                        wErrMsg.resize(wcslen(wErrMsg.c_str()));
                        g_tableDataConsulta.push_back({ L"Erro", wErrMsg });
                    }
                    else {
                        g_tableDataConsulta.push_back({ L"Erro", L"Desconhecido" });
                    }
                    if (errMsg) sqlite3_free(errMsg);
                }
                sqlite3_close(db);
            }
            else {
                g_tableDataConsulta.push_back({ L"Erro", L"Não foi possível abrir o banco" });
            }
        }

        // Obter dimensões da janela
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = g_tableDataConsulta.empty() ? 0 : g_tableDataConsulta[0].size() + 3;  // Número de colunas baseado nos cabeçalhos
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);  // Evitar divisão por zero
        int startY = 100;  // Centralizar verticalmente
        int startX = 22;

        // Desenhar a grade
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));  // Caneta preta
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        //for (int i = 0; i <= static_cast<int>(g_tableDataConsulta.size()); i++) {  // Linhas horizontais
            //MoveToEx(hdc, startX, startY + i * cellHeight, NULL);
            //LineTo(hdc, startX + width, startY + i * cellHeight);
        //}
        //for (int i = 0; i < numColumns + 1; i++) {  // Linhas verticais, corrigido para numColumns
            //MoveToEx(hdc, startX + i * cellWidth, startY, NULL);
            //LineTo(hdc, startX + i * cellWidth, startY + g_tableDataConsulta.size() * cellHeight);
        //}
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        // Desenhar fundos alternados para as linhas
        HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150)); // Fundo header
        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255)); // Fundo branco
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));  // Fundo cinza claro
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushWhite); // Pincel inicial

        // Criar fonte header
        HFONT hFontHeader = CreateFont(
            16,                        // Altura da fonte (ajuste conforme necessário)
            0,                         // Largura (0 para proporção automática)
            0,                         // Escapamento
            0,                         // Orientação
            FW_EXTRABOLD,              // Peso (700 para bold, 800 para extra bold)
            FALSE,                     // Itálico
            FALSE,                     // Sublinhado
            FALSE,                     // Tachado
            DEFAULT_CHARSET,           // Conjunto de caracteres
            OUT_DEFAULT_PRECIS,        // Precisão de saída
            CLIP_DEFAULT_PRECIS,       // Precisão de recorte
            ANTIALIASED_QUALITY,       // Qualidade
            DEFAULT_PITCH | FF_DONTCARE, // Tipo de pitch e família
            L"Arial"                   // Nome da fonte (pode mudar, ex.: "Times New Roman")
        );

        // Criar fonte default
        HFONT hFont = CreateFont(
            16,                        // Altura da fonte (ajuste conforme necessário)
            0,                         // Largura (0 para proporção automática)
            0,                         // Escapamento
            0,                         // Orientação
            FW_BOLD,                 // Peso (700 para bold, 800 para extra bold)
            FALSE,                     // Itálico
            FALSE,                     // Sublinhado
            FALSE,                     // Tachado
            DEFAULT_CHARSET,           // Conjunto de caracteres
            OUT_DEFAULT_PRECIS,        // Precisão de saída
            CLIP_DEFAULT_PRECIS,       // Precisão de recorte
            ANTIALIASED_QUALITY,       // Qualidade
            DEFAULT_PITCH | FF_DONTCARE, // Tipo de pitch e família
            L"Arial"                   // Nome da fonte (pode mudar, ex.: "Times New Roman")
        );

        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);  // Texto sem fundo
        int colNumber = 0;
		int rowNumber = 0;
        for (size_t col = 0; col < 22; col++) {
            colNumber++;

            HBRUSH hCurrentBrush = (col % 2 == 0) ? hBrushGray : hBrushWhite;
            SelectObject(hdc, hCurrentBrush);

            // Desenhar o fundo da linha
            RECT rowRect = { startX, startY + static_cast<int>(colNumber) * cellHeight,
                            startX + width, startY + (static_cast<int>(colNumber) + 1) * cellHeight };
            FillRect(hdc, &rowRect, hCurrentBrush);

            for (size_t row = 0; row < g_tableDataConsulta.size(); row++) {
                if (g_tableDataConsulta[row][0] == std::to_wstring(idRecord) || row == 0) {
                    
                    int xPos;
                    int yPos;

                    if (row == 0) {
                        xPos = startX;  // Pequeno deslocamento para margem
                        yPos = startY + colNumber * cellHeight + 7;  // Ajuste vertical
                    }
                    else {
                        xPos = startX + cellWidth + 2;  // Pequeno deslocamento para margem
                        yPos = startY + colNumber * cellHeight + 7;  // Ajuste vertical
                    }
                    

                    if (g_tableDataConsulta[row][col] == L"Nome_do_Pet") {
                        TextOut(hdc, xPos, yPos, L"Nome do Pet", static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                    else if (g_tableDataConsulta[row][col] == L"Nome_do_Tutor") {
                        TextOut(hdc, xPos, yPos, L"Nome do Tutor", static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                    else if (g_tableDataConsulta[row][col] == L"Raca") {
                        TextOut(hdc, xPos, yPos, L"Raça", static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                    else if (g_tableDataConsulta[row][col] == L"Date") {
                        TextOut(hdc, xPos, yPos, L"Data", static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                    else if (g_tableDataConsulta[row][col] == L"Hour") {
                        TextOut(hdc, xPos, yPos, L"Hora", static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                    else {
                        TextOut(hdc, xPos, yPos, g_tableDataConsulta[row][col].c_str(), static_cast<int>(g_tableDataConsulta[row][col].length()));
                    }
                }
                rowNumber++;
            }
        }

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
LRESULT CALLBACK NewWndProcRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela Read!", 21);
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
BOOL InitRead(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcRead;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ReadClass";
    return RegisterClassW(&wc);
}