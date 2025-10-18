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
#include <sstream>
#include <tuple>

int g_scrollX;      // Posição horizontal do scroll
int g_scrollY;      // Posição vertical do scroll
int g_contentWidth;   // Largura total do conteúdo
int g_contentHeight;   // Altura total do conteúdo
int g_clientWidth;       // Largura da área cliente
int g_clientHeight;      // Altura da área cliente
int rowsNumber;

std::vector<HWND> g_editControls; // Array global para armazenar handles dos controles de edição
std::vector<HWND> g_editControlsFilters;
std::vector<HWND> g_editControlsOrder;
std::vector<std::vector<std::wstring>> g_tableData;
std::vector<std::vector<std::wstring>> g_tableDataFull;
LONG_PTR idRecord;
std::string orderColumn = "ID";
std::string orderAscDesc = "DESC";

std::vector<std::wstring> dados(24);
std::wstring dataAte;
std::wstring dataRegistroAte;

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

void AtualizarPosicoesOrder(HWND hWnd) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = g_tableData.empty() ? 0 : 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - g_scrollY;  // Posição Y com scroll
    int startX = 22 - g_scrollX;  // Posição X com scroll

    for (int col = 0; col < 7; col++) {
        int xPos = startX + col * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        SetWindowPos(g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
        
        SetWindowPos(g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void createOrderBtn(HWND hWnd){
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = g_tableData.empty() ? 0 : 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - g_scrollY;  // Posição Y com scroll
    int startX = 22 - g_scrollX;  // Posição X com scroll

    for (int i = 0; i < 7; i++) {
        int xPos = startX + i * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        // 1. Mude o tipo de HBITMAP para HICON
        HICON hIcon;
        hIcon = (HICON)LoadImageW(
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), // Handle da instância
            MAKEINTRESOURCE(IDB_SETAS), // ID do recurso (definido no resource.h)
            IMAGE_ICON, // <-- CORREÇÃO: Carregue como Ícone (ICON)
            10, 15,
            LR_SHARED
        );

        // 2. Mude o estilo do botão de BS_BITMAP para BS_ICON
        HWND hButton = CreateWindowW(
            L"BUTTON",
            L"", // Remova o texto, pois a imagem será exibida
            WS_VISIBLE | WS_CHILD | BS_ICON, // <-- CORREÇÃO: Use BS_ICON
            xPos - 10, yPos, 10, 15, // Ajuste X, Y, Largura e Altura conforme necessário
            hWnd,
            (HMENU)(ORDENAR), // Seu ID de controle (555 é um valor válido)
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL
        );
        
        SetWindowLongPtr(hButton, GWLP_USERDATA, i);

        // 3. Mude o tipo de objeto em BM_SETIMAGE e use hIcon
        if (hButton && hIcon) {
            // Envia o handle da imagem (hIcon) para o botão
            SendMessage(
                hButton,
                BM_SETIMAGE,
                (WPARAM)IMAGE_ICON, // <-- CORREÇÃO: O tipo de objeto que está sendo definido (Icon)
                (LPARAM)hIcon       // <-- Mude para hIcon
            );
        }

        g_editControlsOrder.push_back(hButton);
    }
}

// Função auxiliar para converter a string "dd/mm/aa" para a struct Data
// Retorna um std::optional<Data> (C++17) ou uma struct com valores -1 em caso de erro.
// Aqui, usaremos um retorno com tupla e flag de sucesso para ser compatível com C++ mais antigo.
std::tuple<Data, bool> parseData(const std::wstring& dataStr) {
    Data data = { 0, 0, 0 };
    bool sucesso = false;

    // Garante que a string tem o tamanho esperado (8 caracteres + 2 separadores = 10)
    if (dataStr.length() == 10 && dataStr[2] == L'/' && dataStr[5] == L'/') {
        try {
            // Extrai as partes. Exemplo: "25/12/23"
            std::wstring diaStr = dataStr.substr(0, 2);
            std::wstring mesStr = dataStr.substr(3, 2);
            std::wstring anoStr = dataStr.substr(6, 4);

            // Converte para inteiro. 'aa' (ex: 23) vira 2023.
            // Nota: Esta conversão de 'aa' para '20aa' assume o século 21.
            data.dia = std::stoi(diaStr);
            data.mes = std::stoi(mesStr);
            data.ano = std::stoi(anoStr); // Assume o século atual

            sucesso = true;
        }
        catch (...) {
            // Lidar com erro de conversão (se o formato não for numérico)
            sucesso = false;
        }
    }

    return std::make_tuple(data, sucesso);
}

/**
 * @brief Verifica se uma data está entre (ou é igual a) duas outras datas.
 * * @param dataIntervalo1 Data inicial do intervalo ("dd/mm/aa").
 * @param dataIntervalo2 Data final do intervalo ("dd/mm/aa").
 * @param dataParaTestar A data a ser testada ("dd/mm/aa").
 * @return true Se a dataParaTestar estiver entre dataIntervalo1 e dataIntervalo2.
 * @return false Caso contrário, ou se houver falha na conversão de formato.
 */
bool estaEntreDatas(const std::wstring& dataIntervalo1,
    const std::wstring& dataIntervalo2,
    const std::wstring& dataParaTestar) {

    Data dataA;
    bool sucessoA;
    std::tie(dataA, sucessoA) = parseData(dataIntervalo1);

    Data dataB;
    bool sucessoB;
    std::tie(dataB, sucessoB) = parseData(dataIntervalo2);

    Data dataTeste;
    bool sucessoTeste;
    std::tie(dataTeste, sucessoTeste) = parseData(dataParaTestar);

    if (dataIntervalo1.empty()) {
        sucessoA = true;
    }

    if (dataIntervalo2.empty()) {
        sucessoB = true;
    }

    // Se a conversão de qualquer data falhar, retorna false
    if (!sucessoA || !sucessoB || !sucessoTeste) {
        // Opcional: imprimir mensagem de erro
        // std::wcerr << L"Erro de formato de data." << std::endl;
        return false;
    }

    else if (dataIntervalo1 != dataIntervalo2 && !dataIntervalo1.empty() && !dataIntervalo2.empty() && dataB <= dataA) {
        return false;
    }

    if (!dataIntervalo1.empty() && !dataIntervalo2.empty()) {
        // 1. Garante que dataA é a data de início e dataB é a data de fim.
        //    Se a ordem estiver invertida, troca.
        const Data& dataInicio = (dataA <= dataB) ? dataA : dataB;
        const Data& dataFim = (dataA <= dataB) ? dataB : dataA;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return (dataInicio <= dataTeste) && (dataTeste <= dataFim);
    }
    else if(!dataIntervalo1.empty() || !dataIntervalo2.empty()){
        if (dataIntervalo1 == dataParaTestar || dataIntervalo2 == dataParaTestar) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return true;
    }
}

std::wstring arrumarNomesColunas(std::wstring displayText) {
    if (displayText == L"Nome_do_Pet") displayText = L"Nome do Pet";
    else if (displayText == L"Nome_do_Tutor") displayText = L"Nome do Tutor";
    else if (displayText == L"Raca") displayText = L"Raça";
    else if (displayText == L"Appointment_Date") displayText = L"Data (de - até)";
    else if (displayText == L"Appointment_Hour") displayText = L"Hora";
    else if (displayText == L"Date") displayText = L"Data Registro (de - até)";
    else if (displayText == L"Hour") displayText = L"Hora Registro";
    else if (displayText == L"Ponto_de_referencia") displayText = L"Ponto de Referência";
    else if (displayText == L"Obs_Tosa") displayText = L"Observação";
    else if (displayText == L"Lesoes") displayText = L"Lesões";
    else if (displayText == L"Obs_Lesoes") displayText = L"Observação";
    else if (displayText == L"Obs_Lesoes") displayText = L"Observação";
    else if (displayText == L"Endereco") displayText = L"Endereço";

	return displayText;
}

void AtualizarPosicoesInputs(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumnsFull = 10;
    int cellWidthFull = width / numColumnsFull;
    int cellHeight = 32;
    int startXFull = 22 - g_scrollX;
    int startYFull = 80 - g_scrollY;
    int row = 1;
    int xPos;
    int yPos;

    for (int col = 0; col < 10; col++) {
        xPos = startXFull + col * cellWidthFull + 10;
        yPos = startYFull + row * cellHeight + 7;
        SetWindowPos(g_editControlsFilters[col], NULL, xPos, yPos,
            cellWidthFull - 15, 25, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    int colNumber = 10;
    int colFinalNumber = colNumber + 10;

    for (int col = 10; col < 20; col++) {
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - colNumber) * cellWidthFull + 10;
        SetWindowPos(g_editControlsFilters[col], NULL, xPos, yPos,
            cellWidthFull - 15, 25, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    colNumber = 20;
    colFinalNumber = colNumber + 10;

    for (int col = 20; col < g_editControlsFilters.size(); col++) {
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - colNumber) * cellWidthFull + 10;
        int widthDate = 68;

        if (col == 20) {
            SetWindowPos(g_editControlsFilters[col], NULL, xPos, yPos,
                widthDate, 25, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (col == 21) {
            xPos = startXFull + (col - 1 - colNumber) * cellWidthFull + 10;
            SetWindowPos(g_editControlsFilters[col], NULL, xPos + widthDate, yPos,
                widthDate, 25, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (col == 23) {
            xPos = startXFull + (col - 1 - colNumber) * cellWidthFull + 10;
            SetWindowPos(g_editControlsFilters[col], NULL, xPos, yPos,
                widthDate, 25, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (col == 24) {
            xPos = startXFull + (col - 2 - colNumber) * cellWidthFull + 10;
            SetWindowPos(g_editControlsFilters[col], NULL, xPos + widthDate, yPos,
                widthDate, 25, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else {
            int column;
            if (col == 22) {
                xPos = startXFull + (col - 1 - colNumber) * cellWidthFull + 10;
            }
            else if (col == 25) {
                xPos = startXFull + (col - 2 - colNumber) * cellWidthFull + 10;
            }
           
            SetWindowPos(g_editControlsFilters[col], NULL, xPos, yPos,
                cellWidthFull - 15, 25, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        }
    }

    startXFull = 22 - g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;
    SetWindowPos(g_editControlsFilters[26], NULL, xPos, yPos,
        70, 30, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
}

void criarInputsFilters(HWND hWnd) {

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int cellHeight = 32;
    int numColumnsFull = 10;
    int cellWidthFull = width / (numColumnsFull > 0 ? numColumnsFull : 1); // +3 para os botões

    // Configurar a linha do header
    int startYFull = 80 - g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - g_scrollX;  // Posição X com scroll

    int xPos;
    int yPos;
    int row = 1;

    for (int col = 0; col < 10; col++) {
        int controlID = col + 100000;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + col * cellWidthFull + 10;

        if (col == 9) {
            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, cellWidthFull, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Traga o ComboBox para a frente, acima de todos os outros controles irmãos (siblings)
            SetWindowPos(hComboBox, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sim");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Não");

            g_editControlsFilters.push_back(hComboBox);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            g_editControlsFilters.push_back(hEdit);
        }
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    int colNumber = 10;
    int colFinalNumber = colNumber + 10;

    for (int col = colNumber; col < colFinalNumber; col++) {
        int controlID = col + 100000;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - colNumber) * cellWidthFull + 10;

        if (col == 12) {
            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, cellWidthFull, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Traga o ComboBox para a frente, acima de todos os outros controles irmãos (siblings)
            SetWindowPos(hComboBox, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Padrão");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Hidratação");
            // Adicionar a opção C
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Nenhum");

            g_editControlsFilters.push_back(hComboBox);
        }
        else if (col == 13) {
            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, cellWidthFull, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Traga o ComboBox para a frente, acima de todos os outros controles irmãos (siblings)
            SetWindowPos(hComboBox, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Tesoura");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Máquina");
            // Adicionar a opção C
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Higiênica");
            // Adicionar a opção C
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Tosa da Raça");
            // Adicionar a opção C
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Nenhum");

            g_editControlsFilters.push_back(hComboBox);
        }
        else if (col == 15) {
            // 1. Defina a largura desejada para a lista suspensa (ex: 300 pixels)
            int desiredDroppedWidth = 170;

            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, cellWidthFull, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // independentemente da largura do controle principal.
            SendMessageW(
                hComboBox,
                CB_SETDROPPEDWIDTH,
                (WPARAM)desiredDroppedWidth, // Novo valor de largura
                0
            );

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pulgas");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Carrapatos");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pulgas e Carrapatos");

            g_editControlsFilters.push_back(hComboBox);
        }
        else if (col == 16) {
            // 1. Defina a largura desejada para a lista suspensa (ex: 300 pixels)
            int desiredDroppedWidth = 250;

            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, cellWidthFull, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // independentemente da largura do controle principal.
            SendMessageW(
                hComboBox,
                CB_SETDROPPEDWIDTH,
                (WPARAM)desiredDroppedWidth, // Novo valor de largura
                0
            );

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Olhos");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Secreção");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele e Olhos");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele e Secreção");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Olhos e Secreção");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Olhos e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Secreção e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Olhos, Secreção e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele, Secreção e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele, Olhos e Ouvido");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele, Olhos e Secreção");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Pele, Olhos, Secreção e Ouvido");

            g_editControlsFilters.push_back(hComboBox);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            g_editControlsFilters.push_back(hEdit);
        }
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    colNumber = 20;
    colFinalNumber = 24;
    int controlID;

    for (int col = colNumber; col < colFinalNumber; col++) {
        controlID = col + 100000;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - colNumber) * cellWidthFull + 10;

        if (col == 20) {
            HFONT hFont;
            hFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

            g_editControlsFilters.push_back(hEdit);

            HWND hEdit2 = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos + 75, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID + 100000), NULL, NULL
            );

            SendMessageW(hEdit2, WM_SETFONT, (WPARAM)hFont, TRUE);

            g_editControlsFilters.push_back(hEdit2);
        }
        else if (col == 22) {
            HFONT hFont;
            hFont = CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            
            SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

            g_editControlsFilters.push_back(hEdit);

            HWND hEdit2 = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos + 75, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID + 100000), NULL, NULL
            );

            SendMessageW(hEdit2, WM_SETFONT, (WPARAM)hFont, TRUE);

            g_editControlsFilters.push_back(hEdit2);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                xPos, yPos, cellWidthFull, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            g_editControlsFilters.push_back(hEdit);
        }
    }

    startXFull = 22 - g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;
    
    //Criar botão para filtrar
    HWND hButton = CreateWindowW(
        L"BUTTON", L"Filtrar",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        xPos, yPos, 70, 30,
        hWnd, (HMENU)(FILTRAR),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    if (hButton) {
        //SetWindowLongPtr(hButton, GWLP_USERDATA, controlID + 1);
        g_editControlsFilters.push_back(hButton);
    }
}

void criarHeaderLineFilter(HDC hdc, HWND hWnd, int startYFull, int startXFull, int colNumber, int colFinalNumber) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Desenhar fundos alternados para as linhas
    HBRUSH hBrushHeader = CreateSolidBrush(RGB(200, 200, 200));
    HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

    // Configurar a tabela
    int cellHeight = 32;
    int numColumnsFull = 10;
    int cellWidthFull = width / (numColumnsFull > 0 ? numColumnsFull : 1); // +3 para os botões
    startYFull = startYFull;  // Posição Y com scroll
    startXFull = startXFull;  // Posição X com scroll

    // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários
    for (size_t row = 0; row < 1; row++) {
        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);

        HBRUSH hCurrentBrush = (row % 2 == 0) ? hBrushGray : hBrushWhite;

        if (row == 0) {
            hCurrentBrush = hBrushHeader;
            fonte(L"Header", RGB(0, 0, 0), hdc);
        }
        else {
            fonte(L"Font", RGB(0, 0, 0), hdc);
        }

        // Desenhar o fundo da linha
        RECT rowRect = {
            startXFull,
            startYFull + static_cast<int>(row) * cellHeight,
            startXFull + width,
            startYFull + (static_cast<int>(row) + 1) * cellHeight
        };
        FillRect(hdc, &rowRect, hCurrentBrush);

        // Desenhar as células de dados
        for (size_t col = colNumber; col < colFinalNumber; col++) {
            int xPos;
            int yPos;
            yPos = startYFull + row * cellHeight + 7;
            xPos = startXFull + (col - colNumber) * cellWidthFull + 10;

            std::wstring displayText = g_tableDataFull[row][col];

            // Traduzir cabeçalhos se necessário
            if (row == 0) {
                displayText = arrumarNomesColunas(displayText);
            }

            TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(displayText.length()));
        }
    }
}

void createHeaderFilters(HDC hdc, HWND hWnd) {
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
    
    // Configurar a linha do header
    int cellHeight = 32;
    int startYFull = 80 - g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - g_scrollX;  // Posição X com scroll
    int colNumber = 0;
	int colFinalNumber = colNumber + 10;

    criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);

    // Configurar a linha do header
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    colNumber = 10;
    colFinalNumber = colNumber + 10;

    criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);

    // Configurar a linha do header
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - g_scrollX;  // Posição X com scroll
    colNumber = 20;
    colFinalNumber = g_tableDataFull[1].size();

    criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);
}

void updateWindow(LPCWSTR className) {
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        selectDB();
        RecarregarDadosTabela(hSelectWnd);
    }
}

// Função para atualizar posições dos botões com scroll
void AtualizarPosicoesBotoes(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = g_tableData.empty() ? 0 : 7 + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 350;
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

void selectDB() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    g_tableData.clear();

    // Consultar o banco apenas se a tabela estiver vazia
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        std::string sqlSelect;
        if (orderColumn == "Appointment_Hour") {

            // Expressão para converter 'H:MM' ou 'HH:MM' para o formato ordenável 'HH:MM'
            std::string hourSorting =
                // 1. Pega a hora (antes do ':'), converte para INT, e preenche com zero (ex: '4' -> '04')
                "PRINTF('%02d', CAST(SUBSTR(Appointment_Hour, 1, INSTR(Appointment_Hour, ':') - 1) AS INTEGER))"
                // 2. Concatena com os minutos (incluindo o ':')
                " || SUBSTR(Appointment_Hour, INSTR(Appointment_Hour, ':'))";

            sqlSelect = "SELECT * FROM Pets ORDER BY " + hourSorting + " " + orderAscDesc + ";";
        }
        else if (orderColumn == "Appointment_Date") {

            // Define a string de ordenação complexa para a data DD/MM/YYYY
            std::string dataSorting =
                "SUBSTR(Appointment_Date, 7, 4) || SUBSTR(Appointment_Date, 4, 2) || SUBSTR(Appointment_Date, 1, 2)";
            sqlSelect = "SELECT * FROM Pets ORDER BY " + dataSorting + " " + orderAscDesc + ";";

        }
        else {
            //const char* sqlSelect = "SELECT ID, Nome_do_Pet, Nome_do_Tutor, Banho, Tosa, Appointment_Date, Appointment_Hour FROM Pets;";
            sqlSelect = "SELECT * FROM Pets ORDER BY " + orderColumn + " COLLATE NOCASE " + orderAscDesc + ";";
        }
        rc = sqlite3_exec(db, sqlSelect.c_str(), sqlite_callback, &g_tableData, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                g_tableData.push_back({ L"Erro", wErrMsg });
            }
            else {
                g_tableData.push_back({ L"Erro", L"Desconhecido" });
            }
            if (errMsg) sqlite3_free(errMsg);
        }
        sqlite3_close(db);
        naoDesenhar.resize(g_tableData.size());
    }
    else {
        g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

void verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern){
    // Colunas que não serão desenhadas na tabela
    // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários

    rowsNumber = 0;
    std::wstring filtro;
    std::wstring dadoTable;
    for (size_t row = 0; row < g_tableData.size(); row++) {

        naoDesenharIntern[row] = 0;
        for (size_t col = 0; col < g_tableData[row].size(); col++) {
            std::wstring displayText = g_tableData[row][col];

            if (row != 0 && !dados[col].empty() && (col == 0 || col == 6 || col == 7 || col == 9 || col == 12 || col == 13 || col == 15 || col == 16)) {
                filtro = dados[col];
                dadoTable = g_tableData[row][col];
                if (filtro != dadoTable) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
            else if (row != 0 && (!dados[col].empty() || !dataAte.empty()) && col == 20) {
                bool estaEntre = estaEntreDatas(dados[col], dataAte, g_tableData[row][col]);

                if (!estaEntre) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
            else if (row != 0 && (!dados[col].empty() || !dataRegistroAte.empty()) && col == 22) {
                bool estaEntre = estaEntreDatas(dados[col], dataRegistroAte, g_tableData[row][col]);

                if (!estaEntre) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
            else if (row != 0 && !dados[col].empty() && g_tableData[row][col].find(dados[col]) == std::wstring::npos) {
                naoDesenharIntern[row] = 1;
                break;
            }
        }
    }

    for (size_t row = 0; row < g_tableData.size(); row++) {
        if (naoDesenharIntern[row] != 1) {
            rowsNumber++;
        }
    }

    naoDesenhar = naoDesenharIntern;
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
        if (naoDesenhar[row] != 1) {
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
}

// Função para configurar scroll bars
void ConfigurarScrollBars(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    g_clientHeight = rect.bottom - rect.top;

    int cellHeight = 32;
    int filtersHeight = 6 * cellHeight + 6 * 7;

    g_contentHeight = static_cast<int>(rowsNumber) * cellHeight + 160 + filtersHeight;

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
    selectDB();

    //Verificar filtros
    verificarFiltro(dados, naoDesenhar);

    // Criar botões após carregar os dados
    CriarBotoesTabela(hWnd);

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

void checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData) {

    // 1. Variáveis que você quer exibir (col precisa ser convertido)
    std::wstring displayText = tableData; // Seu valor do BD

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
            CW_USEDEFAULT, CW_USEDEFAULT, // Tamanho inicial (pode ser ignorado ao maximizar)
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
            ShowWindow(hNewWnd, SW_SHOWMAXIMIZED);
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
