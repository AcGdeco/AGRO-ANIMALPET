// Pet.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "Pet.h"
#include "sqlite3.h"
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <locale.h>
#include "Select.h"
#include "MenuUniversal.h"
#include <Add.h>
#include <Consultar.h>
#include <regex>
#include <Edit.h>

int g_scrollX;      // Posição horizontal do scroll
int g_scrollY;      // Posição vertical do scroll
int g_contentWidth;   // Largura total do conteúdo
int g_contentHeight;   // Altura total do conteúdo
int g_clientWidth;       // Largura da área cliente
int g_clientHeight;      // Altura da área cliente

std::vector<HWND> g_editControls; // Array global para armazenar handles dos controles de edição
std::vector<std::vector<std::wstring>> g_tableData;
std::vector<std::vector<std::wstring>> g_tableDataFull;
LONG_PTR idRecord;

std::vector<HWND> g_buttons;

// Substitua o macro por constexpr conforme sugerido pelo VCR101
constexpr int MAX_LOADSTRING = 100;

int windowsNumber = 1;

// Instância da aplicação (global para uso em outros arquivos)
HINSTANCE hInstMenu;

// Variáveis Globais:
const wchar_t title[] = L"AGRO ANIMALPET";
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal
const wchar_t szSelectClass[] = L"HomeClass"; // Nova classe de janela
HWND g_hButton = NULL;
HBRUSH hBrushTransparent = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    setlocale(LC_ALL, "");

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Coloque o código aqui.

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PET, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realize a inicialização do aplicativo:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PET));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PET);
    wcex.lpszClassName  = L"HomeClass";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void createInputsFilters(HDC hdc, HWND hWnd) {
    g_tableDataFull.clear();

    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);

    if (rc == SQLITE_OK) {
        const char* sqlSelect = "SELECT * FROM Pets LIMIT 1;";
        rc = sqlite3_exec(db, sqlSelect, sqlite_callback, &g_tableDataFull, &errMsg);

        if (rc != SQLITE_OK) {
            if (errMsg) {
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                g_tableDataFull.push_back({ L"Erro", wErrMsg });
                sqlite3_free(errMsg);
            }
        }
        sqlite3_close(db);
    }
    else {
        g_tableDataFull.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // LIMPAR a área de desenho primeiro
    HBRUSH hBgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdc, &rect, hBgBrush);
    DeleteObject(hBgBrush);

    // Desenhar fundos alternados para as linhas
    HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));
    HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

    // Configurar a tabela
    int cellHeight = 32;
    int numColumns = g_tableDataFull.empty() ? 0 : g_tableDataFull[0].size();
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 80 - g_scrollY;  // Posição Y com scroll
    int startX = 22 - g_scrollX;  // Posição X com scroll

    // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários
    for (size_t row = 0; row < 1; row++) {
        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);

        HBRUSH hCurrentBrush = (row % 2 == 0) ? hBrushGray : hBrushWhite;

        if (row == 0) {
            hCurrentBrush = hBrushHeader;
            fonte(L"Header", RGB(255, 255, 255), hdc);
        }
        else {
            fonte(L"Font", RGB(0, 0, 0), hdc);
        }

        // Desenhar o fundo da linha
        RECT rowRect = {
            startX,
            startY + static_cast<int>(row) * cellHeight,
            startX + width,
            startY + (static_cast<int>(row) + 1) * cellHeight
        };
        FillRect(hdc, &rowRect, hCurrentBrush);

        // Desenhar as células de dados
        for (size_t col = 0; col < g_tableDataFull[row].size(); col++) {
            int xPos = startX + col * cellWidth + 10;
            int yPos = startY + row * cellHeight + 7;

            std::wstring displayText = g_tableDataFull[row][col];

            // Traduzir cabeçalhos se necessário
            if (row == 0) {
                if (displayText == L"Nome_do_Pet") displayText = L"Nome do Pet";
                else if (displayText == L"Nome_do_Tutor") displayText = L"Nome do Tutor";
                else if (displayText == L"Raca") displayText = L"Raça";
                else if (displayText == L"Appointment_Date") displayText = L"Data";
                else if (displayText == L"Appointment_Hour") displayText = L"Hora";
                else if (displayText == L"Date") displayText = L"Data Registro";
                else if (displayText == L"Hour") displayText = L"Hora Registro";
            }

            TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(displayText.length()));
        }

        // Desenhar cabeçalhos dos botões apenas na linha do cabeçalho
        // REMOVER o código duplicado de desenho de botões que estava criando tabelas sobrepostas
    }
}

void updateWindow(LPCWSTR className) {
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        RecarregarDadosTabela(hSelectWnd);
    }
}

// Função para atualizar posições dos botões com scroll
void AtualizarPosicoesBotoes(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = g_tableData.empty() ? 0 : g_tableData[0].size() + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 80;
    int cellHeight = 32;

    // Desabilitar redesenho durante a atualização
    SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

    for (size_t i = 0; i < g_buttons.size(); i++) {
        size_t row = (i / 3) + 1;
        int buttonType = i % 3;

        int yPos = startY + row * cellHeight + 2 - g_scrollY;
        int xPos = startX + (7 + buttonType) * cellWidth + 2;

        // Verificar se o botão está visível na área da janela
        BOOL isVisible = (yPos >= -cellHeight && yPos <= rect.bottom);

        if (isVisible) {
            SetWindowPos(g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        else {
            // Esconder botões que estão fora da área visível
            SetWindowPos(g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
        }
    }

    // Reabilitar redesenho e forçar atualização
    SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);

    RedrawWindow(hWnd, NULL, NULL,
        RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

bool deleteRecordById(const std::string& databasePath, int id, HWND hWnd) {
    sqlite3* db;
    char* errMsg = 0;

    // Abrir conexão com o banco
    int rc = sqlite3_open(databasePath.c_str(), &db);
    if (rc != SQLITE_OK) {
        MessageBoxW(hWnd, L"Erro ao abrir banco de dados", L"Erro", MB_ICONERROR);
        return false;
    }

    // Preparar a query SQL
    std::string sql = "DELETE FROM Pets WHERE ID = " + std::to_string(id) + ";";

    // Executar a query
    rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &errMsg);

    bool success = true;
    if (rc != SQLITE_OK) {
        std::wstring errorMsg = L"Erro ao deletar registro: " +
            std::wstring(errMsg, errMsg + strlen(errMsg));
        MessageBoxW(hWnd, errorMsg.c_str(), L"Erro", MB_ICONERROR);
        sqlite3_free(errMsg);
        success = false;
    }
    else {
        int changes = sqlite3_changes(db);
        if (changes > 0) {
            MessageBoxW(hWnd, L"Registro deletado com sucesso!", L"Sucesso", MB_ICONINFORMATION);
        }
        else {
            MessageBoxW(hWnd, L"Nenhum registro encontrado com este ID", L"Aviso", MB_ICONWARNING);
            success = false;
        }
    }

    // Fechar conexão
    sqlite3_close(db);
    return success;
}

void CriarBotoesTabela(HWND hWnd)
{
    // Limpar botões existentes
    for (HWND hButton : g_buttons) {
        DestroyWindow(hButton);
    }
    g_buttons.clear();

    if (g_tableData.size() <= 1) return;

    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = g_tableData.empty() ? 0 : g_tableData[0].size() + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 80;
    int cellHeight = 32;

    for (size_t row = 1; row < g_tableData.size(); row++) {
        LONG_PTR recordId = _wtoi(g_tableData[row][0].c_str());
        int yPos = startY + row * cellHeight + 2;

        // Botão Consultar
        int xPos = startX + 7 * cellWidth + 2;
        HWND hButton = CreateWindowW(
            L"BUTTON", L"Consultar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(CONSULTAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            g_buttons.push_back(hButton);
        }

        // Botão Editar
        xPos = startX + 8 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Editar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(EDITAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            g_buttons.push_back(hButton);
        }

        // Botão Deletar
        xPos = startX + 9 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Deletar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(DELETAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            g_buttons.push_back(hButton);
        }
    }
}

// Função para configurar scroll bars
void ConfigurarScrollBars(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_clientHeight = rect.bottom - rect.top;

    int cellHeight = 32;
    g_contentHeight = static_cast<int>(g_tableData.size()) * cellHeight + 130;

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = g_contentHeight;
    si.nPage = g_clientHeight;
    si.nPos = g_scrollY;

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

// Função para recarregar dados do banco
void RecarregarDadosTabela(HWND hWnd) {
    // Limpar dados antigos
    g_tableData.clear();

    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);

    if (rc == SQLITE_OK) {
        const char* sqlSelect = "SELECT ID, Nome_do_Pet, Nome_do_Tutor, Banho, Tosa, Appointment_Date, Appointment_Hour FROM Pets;";
        rc = sqlite3_exec(db, sqlSelect, sqlite_callback, &g_tableData, &errMsg);

        if (rc != SQLITE_OK) {
            if (errMsg) {
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                g_tableData.push_back({ L"Erro", wErrMsg });
                sqlite3_free(errMsg);
            }
        }
        sqlite3_close(db);
    }
    else {
        g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }

    // Recriar botões com os novos dados
    CriarBotoesTabela(hWnd);

    // Reconfigurar scroll bars
    ConfigurarScrollBars(hWnd);

    //Atualizar posição dos botões
    AtualizarPosicoesBotoes(hWnd);

    // Forçar redesenho da janela
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
}

void checarInput(HWND hinput, int col, std::wstring word) {

    // 1. Variáveis que você quer exibir (col precisa ser convertido)
    std::wstring displayText = g_tableDataEditar[1][col + 1]; // Seu valor do BD

    // 2. Converta o inteiro 'col' para uma std::wstring
    std::wstring col_str = std::to_wstring(col);

    // 3. Concatene todas as partes em uma única std::wstring
    // Inclua quebras de linha (L"\n") para melhor formatação visual
    std::wstring mensagemCompleta =
        L"Valor da Coluna (Índice): " + col_str + L"\n" +
        L"Conteúdo do Banco de Dados: " + displayText;

    // 4. Exiba a mensagem usando MessageBox
    //MessageBox(
        //hinput,                           // Handle da Janela (use NULL se não tiver)
        //mensagemCompleta.c_str(),       // A string concatenada convertida para LPCWSTR
        //L"Depuração de Dados",          // Título da caixa de mensagem
        //MB_OK | MB_ICONINFORMATION      // Estilos
    //);

    //std::wstring displayText = g_tableDataEditar[1][col + 1];
    if (displayText.find(word) != std::wstring::npos) {
        SendMessage(hinput, BM_SETCHECK, BST_CHECKED, 0);
    }
}

int sqlite_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? utf8_to_wstring(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? utf8_to_wstring(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Função para atualizar posição dos controles com scroll
void AtualizarPosicoesControlesAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
    int startY = 40 - g_scrollY;
    int startX = 22 - g_scrollX;
    int xPos = 0;
    int yPos = 0;
    int colNumber;
    int countRow = 0;

    // Atualizar posição dos campos de entrada
    for (size_t i = 0; i < g_editControls.size(); i++) {
        colNumber = countRow + 1;

        if (i == 11) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 12) {
            SetWindowPos(g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 13) {
            SetWindowPos(g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 14) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 15) {
            SetWindowPos(g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 16) {
            SetWindowPos(g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 17) {
            SetWindowPos(g_editControls[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 18) {
            SetWindowPos(g_editControls[i], NULL, xPos + 600, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 8) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 25, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 20) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 21) {
            SetWindowPos(g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 22) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 23) {
            SetWindowPos(g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 24) {
            SetWindowPos(g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 25) {
            SetWindowPos(g_editControls[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(g_editControls[i], NULL, xPos, yPos, 700, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
    }

    // Atualizar posição do botão
    if (g_hButton) {
        int buttonY = startY + 22 * cellHeight + 3;
        SetWindowPos(g_hButton, NULL, startX, buttonY, 150, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Função para configurar scroll bars
void ConfigurarScrollBarsAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_clientHeight = rect.bottom - rect.top;
    g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo (19 linhas + título + botão)
    int cellHeight = 32;
    g_contentHeight = 22 * cellHeight + 100; // 19 campos + título + botão + margem
    g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = g_contentHeight;
    si.nPage = g_clientHeight;
    si.nPos = g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = g_contentWidth;
    si.nPage = g_clientWidth;
    si.nPos = g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para obter a data atual como string
std::wstring GetCurrentDate()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t dateStr[80];
    swprintf_s(dateStr, L"%02d/%02d/%04d", st.wDay, st.wMonth, st.wYear);

    return std::wstring(dateStr);
}

// Função para obter a hora atual como string
std::wstring GetCurrentHour()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t timeStr[80];
    swprintf_s(timeStr, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

    return std::wstring(timeStr);
}

bool isNumber(const std::wstring& str) {
    if (str.empty()) return false;

    for (wchar_t c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Versão que permite números decimais
bool isDecimalNumber(const std::wstring& str) {
    if (str.empty()) return false;

    bool hasDecimalPoint = false;
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == L',' || str[i] == L'.') {
            if (hasDecimalPoint) return false; // Mais de um ponto decimal
            hasDecimalPoint = true;
        }
        else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool isValidTime(const std::wstring& time) {
    // Remover espaços em branco
    std::wstring hora_limpa = time;
    hora_limpa.erase(std::remove(hora_limpa.begin(), hora_limpa.end(), L' '), hora_limpa.end());

    // Regex para hh:mm
    std::wregex time_pattern(L"^([01]?[0-9]|2[0-3]):([0-5][0-9])$");

    if (!std::regex_match(hora_limpa, time_pattern)) {
        return false;
    }

    // Extrair hora e minuto
    int hora = std::stoi(hora_limpa.substr(0, 2));
    int minuto = std::stoi(hora_limpa.substr(3, 2));

    // Verificações adicionais (redundantes, mas seguras)
    return (hora >= 0 && hora <= 23) && (minuto >= 0 && minuto <= 59);
}

bool isValidDate(const std::wstring& date) {
    // Regex para dd/mm/aaaa
    std::wregex date_pattern(L"^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/(\\d{4})$");

    if (!std::regex_match(date, date_pattern)) {
        return false;
    }

    // Extrair dia, mês e ano
    int dia = std::stoi(date.substr(0, 2));
    int mes = std::stoi(date.substr(3, 2));
    int ano = std::stoi(date.substr(6, 4));

    // Verificar meses com 30 dias
    if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) {
        return false;
    }

    // Verificar fevereiro
    if (mes == 2) {
        // Verificar ano bissexto
        bool isBissexto = (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
        if (isBissexto && dia > 29) {
            return false;
        }
        if (!isBissexto && dia > 28) {
            return false;
        }
    }

    // Verificar ano (opcional - ajuste conforme necessidade)
    if (ano < 1900 || ano > 2100) {
        return false;
    }

    return true;
}

LPCWSTR error = L"0";
std::wstring mensagem = L"";
LPCWSTR msg = L"";

std::wstring treatDataAppointment(std::wstring dado, int number) {
    std::wstring dado_escaped = dado;
    size_t pos = 0;

    if (number == 2 && dado.empty()) {
        error = L"1";
        mensagem = L"Insira: 'Nome do Pet'.\n" + mensagem;
    }
    else if (number == 4 && dado.empty()) {
        error = L"1";
        mensagem = L"Insira: 'Nome do Tutor'.\n" + mensagem;
    }
    else if (number == 13 && dado.empty()) {
        error = L"1";
        mensagem = L"Insira: 'Banho'.\n" + mensagem;
    }
    else if (number == 14 && dado.empty()) {
        error = L"1";
        mensagem = L"Insira: 'Tosa'.\n" + mensagem;
    }
    else if (number == 21) {
        if (dado.empty()) {
            error = L"1";
            mensagem = L"Insira: 'Data'.\n" + mensagem;
        }
        else if (!isValidDate(dado)) {
            error = L"1";
            mensagem = L"Insira: 'Data' no formato: dd/mm/aaaa.\n" + mensagem;
        }
    }
    else if (number == 22) {
        if (dado.empty()) {
            error = L"1";
            mensagem = L"Insira: 'Hora'.\n" + mensagem;
        }
        else if (!isValidTime(dado)) {
            error = L"1";
            mensagem = L"Insira: 'Hora' no formato: hh:mm.\n" + mensagem;
        }
    }
    else if (number == 5 && !dado.empty()) {
        if (!isNumber(dado)) {
            error = L"1";
            mensagem = L"Insira: Apenas números em 'CEP'.\n" + mensagem;
        }
    }
    else if (number == 7 && !dado.empty()) {
        if (!isNumber(dado)) {
            error = L"1";
            mensagem = L"Insira: Apenas números em 'Idade'.\n" + mensagem;
        }
    }
    else if (number == 19 && !dado.empty()) {
        if (!isNumber(dado)) {
            error = L"1";
            mensagem = L"Insira: Apenas números em 'Telefone'.\n" + mensagem;
        }
    }
    else if (number == 20 && !dado.empty()) {
        if (!isNumber(dado)) {
            error = L"1";
            mensagem = L"Insira: Apenas números em 'CPF'.\n" + mensagem;
        }
    }
    else if (number == 8 && !dado.empty()) {
        if (!isDecimalNumber(dado)) {
            error = L"1";
            mensagem = L"Insira: Apenas números decimais ou inteiros em 'Peso'.\n" + mensagem;
        }
    }
    else if (dado.empty()) {
        dado_escaped = L"";
    }

    while ((pos = dado_escaped.find(L"'", pos)) != std::wstring::npos) {
        dado_escaped.replace(pos, 1, L"''");
        pos += 2;
    }

    msg = mensagem.c_str();

    return dado_escaped;
}

// Converter std::wstring para std::string UTF-8
std::string WideToUTF8(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Converter std::string UTF-8 para std::wstring
std::wstring UTF8ToWide(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

BOOL fonte(LPCWSTR fonte, COLORREF color, HDC hdc) {
    HFONT hFont;
    if (fonte == L"Header") {
        hFont = CreateFont(16, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    }
    else if (fonte == L"Title") {
        hFont = CreateFont(35, 0, 0, 0, FW_EXTRABOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    }
    else {
        hFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    }
    COLORREF textColor = color;
    SelectObject(hdc, hFont);
    SetTextColor(hdc, textColor);

    //EXCLUIR A FONTE CRIADA PARA LIBERAR O RECURSO GDI, O MÁXIMO É PROVAVELMENTE 10000
    DeleteObject(hFont);

    return 0;
}

BOOL windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size) {
    fonte(L"Title", RGB(0, 0, 0), hdc);
    TextOut(hdc, startX, startY, tit, size);

    return 0;
}

BOOL scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight)
{
    // Variáveis de scroll
    g_scrollX = scrollX;      // Posição horizontal do scroll
    g_scrollY = scrollY;      // Posição vertical do scroll
    g_contentWidth = contentWidth;   // Largura total do conteúdo
    g_contentHeight = contentHeight;   // Altura total do conteúdo
    g_clientWidth = clientWidth;       // Largura da área cliente
    g_clientHeight = clientHeight;      // Altura da área cliente

    // Obter dimensões da área cliente
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_clientWidth = rect.right - rect.left;
    g_clientHeight = rect.bottom - rect.top;

    // Configurar scroll bars
    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = g_contentHeight;
    si.nPage = g_clientHeight;
    si.nPos = g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = g_contentWidth;
    si.nPage = g_clientWidth;
    si.nPos = g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

    return 0;
}

BOOL windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int windowsNumberString = windowsNumber;

    //Converte o número para string e exibe
    //wchar_t buffer[32];
    //swprintf_s(buffer, L"Número de janelas: %d", windowsNumberString);
    //MessageBoxW(hWnd, buffer, L"Info", MB_OK | MB_ICONINFORMATION);

    if (windowsNumber == 1) {
        PostQuitMessage(0);
    }

    windowsNumber -= 1;
    
    return 0;
}

BOOL Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    #ifndef VK_N
    #define VK_N 0x4E // Código virtual para a tecla 'N'
    #endif

    #ifndef VK_O
    #define VK_O 0x4F // Código virtual para a tecla 'O'
    #endif

    #ifndef VK_E
    #define VK_E 0x45 // Código virtual para a tecla 'E'
    #endif

    #ifndef VK_D
    #define VK_D 0x44 // Código virtual para a tecla 'D'
    #endif

    #ifndef VK_A
    #define VK_A 0x41 // Código virtual para a tecla 'A'
    #endif

    #ifndef VK_I
    #define VK_I 0x49 // Código virtual para a tecla 'I'
    #endif

    if (GetKeyState(VK_CONTROL) & 0x8000) // Verifica se Ctrl está pressionado
    {
        switch (wParam)
        {
        case VK_I: SendMessage(hWnd, WM_COMMAND, IDM_HOME_INICIO, 0); break;
        case VK_N: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_NOVO, 0); break;
        case VK_O: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_E: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_D: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_A: SendMessage(hWnd, WM_COMMAND, IDM_AJUDA_SOBRE, 0); break;
        }
    }

    return 0;
}

BOOL ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Primeiro processa os comandos do menu
    LRESULT resultMenuHome = ProcessarMenuHome(hWnd, message, wParam, lParam);
    if (resultMenuHome != 0)
    {
        return resultMenuHome;
    }

    LRESULT resultMenu = ProcessarMenuArquivo(hWnd, message, wParam, lParam);
    if (resultMenu != 0)
    {
        return resultMenu;
    }

    LRESULT resultMenuAjuda = ProcessarMenuAjuda(hWnd, message, wParam, lParam);
    if (resultMenuAjuda != 0)
    {
        return resultMenuAjuda;
    }
}

// Função para criar a janela nova com menu
BOOL CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle)
{
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        SetForegroundWindow(hSelectWnd);
    }
    else {

        // Na criação da janela, adicione CS_DBLCLKS
        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;  // ← ADICIONE ESTE
        wc.lpfnWndProc = WndProc;

        // Criar a janela Nova
        HWND hNewWnd = CreateWindowW(
            className,
            windowTittle,
            WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1400, 400, // Tamanho inicial (pode ser ignorado ao maximizar)
            NULL,
            NULL,
            hInst,
            NULL
        );

        if (hNewWnd)
        {
            // Criar e associar o menu à nova janela
            HMENU hMenu = CriarMenu();
            if (hMenu)
            {
                SetMenu(hNewWnd, hMenu);
                DrawMenuBar(hNewWnd); // Atualiza a barra de menu
            }
            else
            {
                MessageBoxW(hNewWnd, L"Erro ao criar o menu na nova janela!", L"Erro", MB_OK | MB_ICONERROR);
            }

            windowsNumber += 1;
            int windowsNumberString = windowsNumber;

            // Converte o número para string e exibe
            //wchar_t buffer[32];
            //swprintf_s(buffer, L"Número de janelas: %d", windowsNumberString);
            //MessageBoxW(hWndParent, buffer, L"Info", MB_OK | MB_ICONINFORMATION);

            // Perguntar se deseja fechar a janela
            int response = MessageBoxW(hWndParent,
                L"Deseja fechar a janela atual?",
                L"Confirmação",
                MB_YESNO | MB_ICONQUESTION);

            // Verificar a resposta
            if (response == IDYES)
            {
                DestroyWindow(hWndParent);
            }

            // Maximizar a janela
            ShowWindow(hNewWnd, SW_MAXIMIZE);
            UpdateWindow(hNewWnd);

            return TRUE;
        }
        else
        {
            // Obter o código de erro
            DWORD errorCode = GetLastError();
            wchar_t errorMsg[256];
            if (FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                errorMsg,
                sizeof(errorMsg) / sizeof(wchar_t),
                NULL) == 0)
            {
                // Se FormatMessage falhar, use uma mensagem genérica com o código
                swprintf_s(errorMsg, L"Erro desconhecido. Código: %lu", errorCode);
            }

            // Construir a mensagem completa
            wchar_t fullMsg[512];
            swprintf_s(fullMsg, L"Falha ao criar janela %s! Detalhes: %s", className, errorMsg);
            MessageBoxW(hWndParent, fullMsg, L"Erro", MB_OK | MB_ICONERROR);

            return FALSE;
        }
    }
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

   // Registrar a nova classe de janela
   if (!InitSelect(hInstance)) {
       return FALSE;
   }

   HWND hWnd = CreateWindowW(L"HomeClass", title, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
  
   // Registrar a classe da janela Select, se ainda não registrada
   if (!RegisterSelectClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Select!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   // Registrar a classe da janela Add, se ainda não registrada
   if (!RegisterAddClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Add!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   // Registrar a classe da janela Read, se ainda não registrada
   if (!RegisterReadClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Read!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   // Registrar a classe da janela Edit, se ainda não registrada
   if (!RegisterEditClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Edit!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   HMENU hMenu = CriarMenu();
   if (hMenu)
   {
       SetMenu(hWnd, hMenu);
       DrawMenuBar(hWnd); // Atualiza a barra de menu
   }
   else
   {
       MessageBox(hWnd, L"Erro ao criar o menu na nova janela!", L"Erro", MB_OK | MB_ICONERROR);
   }

   ShowWindow(hWnd, SW_MAXIMIZE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }
    
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analise as seleções do menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            fonte(L"Font", RGB(0, 0, 0), hdc);

            // Obter as dimensões da janela
            RECT rect;
            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Criar uma fonte grande
            HFONT hFont = CreateFont(
                72,                        // Altura da fonte (aumente para fonte maior)
                0,                         // Largura (0 para proporção automática)
                0,                         // Escapamento
                0,                         // Orientação
                FW_BOLD,                   // Peso (negrito)
                FALSE,                     // Itálico
                FALSE,                     // Sublinhado
                FALSE,                     // Tachado
                DEFAULT_CHARSET,           // Conjunto de caracteres
                OUT_DEFAULT_PRECIS,        // Precisão de saída
                CLIP_DEFAULT_PRECIS,       // Precisão de recorte
                ANTIALIASED_QUALITY,       // Qualidade
                DEFAULT_PITCH | FF_DONTCARE, // Tipo de pitch e família
                L"Arial"                   // Nome da fonte (pode mudar para outra, ex.: "Times New Roman")
            );
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            // Configurar texto sem fundo
            SetBkMode(hdc, TRANSPARENT);

            // Calcular posição central para o texto
            const wchar_t* text = L"AGRO ANIMALPET";
            SIZE textSize;
            GetTextExtentPoint32W(hdc, text, wcslen(text), &textSize);
            int x = (width - textSize.cx) / 2;  // Centralizar horizontalmente
            int y = (height - textSize.cy) / 2; // Centralizar verticalmente

            // Desenhar o texto
            TextOutW(hdc, x, y, text, wcslen(text));

            // Restaurar a fonte antiga e liberar recursos
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
    {
        Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    case WM_DESTROY:
        windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
