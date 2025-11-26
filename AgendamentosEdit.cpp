#include "AgendamentosEdit.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "AgendamentosFuncoes.h"
#include <string>
#include "sqlite3.h"
#include <vector>
#include <uxtheme.h>
#include <regex>
#include <cctype>
#pragma comment(lib, "uxtheme.lib")

// IDs dos controles
#define ID_RADIO_EDIT_PADRAO 2015
#define ID_RADIO_EDIT_HIDRATACAO 2016
#define ID_RADIO_EDIT_BANHO_NENHUM 2017

#define ID_RADIO_EDIT_TESOURA 2018
#define ID_RADIO_EDIT_MAQUINA 2019
#define ID_RADIO_EDIT_HIGIENICA 2020
#define ID_RADIO_EDIT_TOSADARACA 2021
#define ID_RADIO_EDIT_TOSA_NENHUM 2022

#define ID_CHECKBOX_EDIT_PULGAS 2023
#define ID_CHECKBOX_EDIT_CARRAPATOS 2024

#define ID_CHECKBOX_EDIT_PELE 2025
#define ID_CHECKBOX_EDIT_OLHOS 2026
#define ID_CHECKBOX_EDIT_SECRECAO 2027
#define ID_CHECKBOX_EDIT_OUVIDO 2028

struct ComboBoxItemData {
    long long idPet;
    long long idTutor;
};

std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableDataEditar;
std::vector<HWND> AgendamentosSelect_g_editControlsEdit;

void AgendamentosSelect_PreencherControlesEdicao(HWND hWnd) {
    // 1. Busca os Dados no Banco de Dados
    AgendamentosSelect_selectBD();

    if (AgendamentosSelect_g_tableDataEditar.empty()) {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return;
    }

    // 2. Preenchimento dos Campos de Entrada
    // O loop deve corresponder ao loop de criação para garantir que 
    // os dados e os controles (controlID = col + 2) estejam sincronizados.
    int column;
    for (int col = 0; col < 10; col++) {
        int controlID = col + 2;
		column = col + 21;
        std::wstring displayText = col == 9 ? AgendamentosSelect_g_tableDataEditar[1][32] : AgendamentosSelect_g_tableDataEditar[1][column];

        // 3. Obter o HWND do controle (pelo ID ou pelo vetor g_editControls)
        // Usar GetDlgItem(hWnd, controlID) ou iterar sobre g_editControls
        HWND hControl = GetDlgItem(hWnd, controlID);

        if (hControl == NULL) {
            // Se hControl é NULL, é porque ele é um Radio Button (que tem IDs diferentes)
            // ou é um dos outros Checkboxes.
            // Para os Checkboxes e Edit Controls, o hControl não será NULL aqui.
        }

        if (col == 0) { // Tutores - Pets (Checkbox)
            std::wstring displayText = AgendamentosSelect_g_tableDataEditar[1][10];
            ChecarOpcaoComboBoxPorID(hControl, displayText);
        }
        else if (col == 1) { // Banho (Radio Buttons)
            // Aqui, precisamos de todos os HWNDs de rádio, 
            // pois eles têm IDs únicos e não o controlID.

            // Padrão
            HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_PADRAO);
            AgendamentosSelect_checarInput(hRadio1, column, L"Padrão", displayText);

            // Hidratação
            HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIDRATACAO);
            AgendamentosSelect_checarInput(hRadio2, column, L"Hidratação", displayText);

            // Nenhum
            HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_BANHO_NENHUM);
            AgendamentosSelect_checarInput(hRadio3, column, L"Nenhum", displayText);
        }
        else if (col == 2) { // Tosa (Radio Buttons)
            // Tesoura
            HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_TESOURA);
            AgendamentosSelect_checarInput(hRadio1, column, L"Tesoura", displayText);

            // Máquina
            HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_MAQUINA);
            AgendamentosSelect_checarInput(hRadio2, column, L"Máquina", displayText);

            // Higiênica
            HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIGIENICA);
            AgendamentosSelect_checarInput(hRadio3, column, L"Higiênica", displayText);

            // Tosa da Raça
            HWND hRadio4 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSADARACA);
            AgendamentosSelect_checarInput(hRadio4, column, L"Tosa da Raça", displayText);

            // Nenhum (Tosa)
            HWND hRadio5 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSA_NENHUM);
            AgendamentosSelect_checarInput(hRadio5, column, L"Nenhum", displayText);
        }
        else if (col == 4) { // Pulgas/Carrapatos (Checkboxes)
            // Pulgas
            HWND hCheckbox1 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PULGAS);
            AgendamentosSelect_checarInput(hCheckbox1, column, L"Pulgas", displayText);

            // Carrapatos
            HWND hCheckbox2 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_CARRAPATOS);
            AgendamentosSelect_checarInput(hCheckbox2, column, L"Carrapatos", displayText);
        }
        else if (col == 5) { // Lesões (Checkboxes)
            // Pele
            HWND hCheckbox1 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PELE);
            AgendamentosSelect_checarInput(hCheckbox1, column, L"Pele", displayText);

            // Olhos
            HWND hCheckbox2 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OLHOS);
            AgendamentosSelect_checarInput(hCheckbox2, column, L"Olhos", displayText);

            // Secreção
            HWND hCheckbox3 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_SECRECAO);
            AgendamentosSelect_checarInput(hCheckbox3, column, L"Secreção", displayText);

            // Ouvido
            HWND hCheckbox4 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OUVIDO);
            AgendamentosSelect_checarInput(hCheckbox4, column, L"Ouvido", displayText);
        }
        else { // Campos de Edição Padrão (EDIT)
            // Para Edit Controls, basta usar SetWindowText
            SetWindowText(hControl, displayText.c_str());
        }
    }
}

void AgendamentosSelect_CriarControlesEdicao(HWND hWnd) {
    // 1. Resetar Scroll e Limpar Controles Antigos
    AgendamentosSelect_g_scrollY = 0;
    AgendamentosSelect_g_scrollX = 0;

    // É crucial DESTRUIR os HWNDs antes de limpar o vetor,
    // para evitar vazamento de recursos.
    // Assumindo que você tem uma função para isso:
    // AgendamentosSelect_DestroyAllControls(); 
    AgendamentosSelect_g_editControlsEdit.clear();

    AgendamentosSelect_ConfigurarScrollBarsAgendamento(hWnd);

    // 2. Cálculo de Dimensões
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);

    // As posições iniciais dependem do scroll, mas aqui elas são 0
    // pois os scrollbars acabaram de ser resetados.
    int startY = 40; // O scroll será aplicado na função de atualização de posição
    int startX = 22;

    // 3. Criação dos Campos de Entrada
    for (int col = 0; col < 10; col++) {
        int colNumber = col + 1;
        int controlID = col + 2; // IDs de 2 a 22
        int xPos = startX + cellWidth + 10;
        int yPos = startY + colNumber * cellHeight + 3;

        // Em AgendamentosSelect_CriarControlesEdicao, não precisamos do displayText

        if (col == 0) {
            HWND hComboBox = CreateWindowW(
                L"COMBOBOX",                         // Classe do controle: MUDAR de "BUTTON" para "COMBOBOX"
                NULL,                                // Texto: NULL para ComboBox
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos IMPORTANTES
                xPos, yPos,                          // Posição X, Y
                200, 200,                            // Largura, Altura (A altura precisa ser maior para exibir a lista)
                hWnd,                                // Janela pai
                (HMENU)(controlID),                  // ID único
                NULL,                                // Instância
                NULL
            );

            AgendamentosSelect_g_editControlsEdit.push_back(hComboBox);

            // Criar botão consultar
            AgendamentosSelect_g_hButton_consultar = CreateWindowW(
                L"BUTTON", L"Tutor",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                xPos + 700, yPos, 40, 25,
                hWnd, (HMENU)(20),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
            );

            // Criar botão Dados do Pet
            AgendamentosSelect_g_hButton_consultar_Pet = CreateWindowW(
                L"BUTTON", L"Pet",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                xPos + 740, yPos, 40, 25,
                hWnd, (HMENU)(21),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
            );
        }
        else if (col == 1) { // Banho (Radio Buttons)
            HWND hRadio;

            // Padrão
            hRadio = CreateWindowW(
                L"BUTTON", L"Padrão", WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_PADRAO), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Hidratação
            hRadio = CreateWindowW(
                L"BUTTON", L"Hidratação", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_HIDRATACAO), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Nenhum
            hRadio = CreateWindowW(
                L"BUTTON", L"Nenhum", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + 2 * cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_BANHO_NENHUM), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);
        }
        else if (col == 2) { // Tosa (Radio Buttons)
            HWND hRadio;

            // Tesoura
            hRadio = CreateWindowW(
                L"BUTTON", L"Tesoura", WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_TESOURA), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Máquina
            hRadio = CreateWindowW(
                L"BUTTON", L"Máquina", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_MAQUINA), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Higiênica
            hRadio = CreateWindowW(
                L"BUTTON", L"Higiênica", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + 2 * cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_HIGIENICA), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Tosa da Raça
            hRadio = CreateWindowW(
                L"BUTTON", L"Tosa da Raça", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + 3 * cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_TOSADARACA), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);

            // Nenhum (Tosa)
            hRadio = CreateWindowW(
                L"BUTTON", L"Nenhum", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_TABSTOP,
                xPos + 4 * cellWidth + 10, yPos, 100, 20, hWnd, (HMENU)(ID_RADIO_EDIT_TOSA_NENHUM), NULL, NULL
            );
            SetWindowTheme(hRadio, L"", L"");
            AgendamentosSelect_g_editControlsEdit.push_back(hRadio);
        }
        else if (col == 4) { // Pulgas/Carrapatos (Checkboxes)
            // Pulgas
            HWND hCheckbox = CreateWindowW(
                L"BUTTON", L"Pulgas", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_PULGAS), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);

            // Carrapatos
            hCheckbox = CreateWindowW(
                L"BUTTON", L"Carrapatos", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos + cellWidth + 10, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_CARRAPATOS), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);
        }
        else if (col == 5) { // Lesões (Checkboxes)
            // Pele
            HWND hCheckbox = CreateWindowW(
                L"BUTTON", L"Pele", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_PELE), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);

            // Olhos
            hCheckbox = CreateWindowW(
                L"BUTTON", L"Olhos", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos + cellWidth + 10, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_OLHOS), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);

            // Secreção
            hCheckbox = CreateWindowW(
                L"BUTTON", L"Secreção", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos + 2 * cellWidth + 10, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_SECRECAO), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);

            // Ouvido
            hCheckbox = CreateWindowW(
                L"BUTTON", L"Ouvido", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP,
                xPos + 3 * cellWidth + 10, yPos, 20, 20, hWnd, (HMENU)(ID_CHECKBOX_EDIT_OUVIDO), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hCheckbox);
        }
        else { // Campos de Edição Padrão (EDIT)
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"", // Cria vazio
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, 200, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            AgendamentosSelect_g_editControlsEdit.push_back(hEdit);
        }
    }

    // 4. Criação do Botão Salvar
    int buttonY = startY + 23 * cellHeight + 3;
    AgendamentosSelect_g_hButton = CreateWindowW(
        L"BUTTON", L"Salvar",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        startX, buttonY, 150, 30,
        hWnd, (HMENU)(1),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
}

void AgendamentosSelect_selectBD() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    AgendamentosSelect_g_tableDataEditar.clear();

    // Abrir ou criar o banco de dados (código original mantido)
    sqlite3* db = nullptr;
    char* errMsg = nullptr;
    int rc;
    OpenDatabase(db);

    if (OpenDatabase(db)) {
        std::string idRecordStr = std::to_string(AgendamentosSelect_idRecord);
        
        // 1. Defina o buffer de destino. Escolha um tamanho adequado.
        char buffer[512];

        // 2. Use snprintf para formatar a string
        snprintf(
            buffer,
            sizeof(buffer),
            "SELECT * "
            "FROM Tutores T INNER JOIN Pets P ON T.ID = P.ID_Tutor_FK "
            "INNER JOIN Agendamentos A ON P.ID = A.ID_Pet_FK "
            "WHERE A.ID = %I64d;", // <-- MUDANÇA AQUI: de %d para %I64d
            AgendamentosSelect_idRecord // O valor que é um __int64 (ou long long)
        );

        // 3. O 'buffer' agora contém a string SQL formatada.
        const char* sqlSelectConsulta = buffer;


        rc = sqlite3_exec(db, sqlSelectConsulta, AgendamentosSelect_sqlite_callback, &AgendamentosSelect_g_tableDataEditar, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                size_t convertedChars = 0;
                mbstowcs_s(&convertedChars, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                if (convertedChars > 0) {
                    wErrMsg.resize(convertedChars - 1);
                }
                AgendamentosSelect_g_tableDataEditar.push_back({ L"Erro", wErrMsg });
                sqlite3_free(errMsg);
            }
            else {
                AgendamentosSelect_g_tableDataEditar.push_back({ L"Erro", L"Erro desconhecido no SQLite: " + std::to_wstring(rc) });
            }
        }
        else {
            // Sucesso - talvez adicionar uma mensagem de confirmação
            if (AgendamentosSelect_g_tableDataEditar.empty()) {
                // AgendamentosSelect_g_tableDataEditar.push_back({ L"Info", L"Nenhum registro encontrado com ID: " + std::to_wstring(AgendamentosSelect_idRecord) });
            }
        }

        sqlite3_close(db);
    }
    else {
        AgendamentosSelect_g_tableDataEditar.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcAgendamentosEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para registrar a classe da janela
BOOL RegisterAgendamentosEditClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcAgendamentosEdit;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"JanelaAgendamentosEditClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Edit COM SCROLL CORRIGIDO
LRESULT CALLBACK WndProcAgendamentosEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (AgendamentosSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        // 1. Cria a interface
        AgendamentosSelect_CriarControlesEdicao(hWnd);
        break;
    }

    case WM_VSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINEUP:        si.nPos -= 10; break;
        case SB_LINEDOWN:      si.nPos += 10; break;
        case SB_PAGEUP:        si.nPos -= si.nPage; break;
        case SB_PAGEDOWN:      si.nPos += si.nPage; break;
        case SB_THUMBTRACK:    si.nPos = si.nTrackPos; break;
        case SB_THUMBPOSITION: si.nPos = si.nTrackPos; break;
        case SB_TOP:           si.nPos = si.nMin; break;
        case SB_BOTTOM:        si.nPos = si.nMax; break;
        default: break;
        }

        si.fMask = SIF_POS;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hWnd, SB_VERT, &si);

        if (si.nPos != oldPos) {
            AgendamentosSelect_g_scrollY = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoEdit(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            AgendamentosSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_HSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_HORZ, &si);

        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINELEFT:      si.nPos -= 10; break;
        case SB_LINERIGHT:     si.nPos += 10; break;
        case SB_PAGELEFT:      si.nPos -= si.nPage; break;
        case SB_PAGERIGHT:     si.nPos += si.nPage; break;
        case SB_THUMBTRACK:    si.nPos = si.nTrackPos; break;
        case SB_THUMBPOSITION: si.nPos = si.nTrackPos; break;
        default: break;
        }

        si.fMask = SIF_POS;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));
        SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
        GetScrollInfo(hWnd, SB_HORZ, &si);

        if (si.nPos != oldPos) {
            AgendamentosSelect_g_scrollX = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoEdit(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            AgendamentosSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        AgendamentosSelect_g_clientWidth = newWidth;
        AgendamentosSelect_g_clientHeight = newHeight;

        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        AgendamentosSelect_ConfigurarScrollBarsAgendamento(hWnd);
        // Atualizar posições dos controles após redimensionamento
        AgendamentosSelect_AtualizarPosicoesControlesAgendamentoEdit(hWnd);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        AgendamentosSelect_invalidateDrawing(hWnd);
        UpdateWindow(hWnd);
        break;
    }

    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        int oldPos = si.nPos;
        int scrollAmount = -zDelta / WHEEL_DELTA * 30;

        si.nPos += scrollAmount;
        si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));

        si.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hWnd, SB_VERT, &si);

        if (si.nPos != oldPos) {
            AgendamentosSelect_g_scrollY = si.nPos;
            SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
            // Atualizar posições dos controles ANTES do redraw
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoEdit(hWnd);
            SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
            AgendamentosSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (AgendamentosSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        if (AgendamentosSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        HBRUSH hbr = CreateSolidBrush(RGB(30, 30, 30));
        RECT rect;
        GetClientRect(hWnd, &rect);
        FillRect(hdc, &rect, hbr);
        DeleteObject(hbr);
        return 1; // Evitar flicker
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        if (wmId == 1) // Botão "Salvar Registro"
        {
            std::wstring dados[12];
            for (int i = 2; i <= 12; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i);

                if (i == 2) {
                    if (input) {
                        LRESULT selectedIndex = SendMessage(input, CB_GETCURSEL, 0, 0);

                        // 2. Recuperar o ponteiro armazenado
                        LPARAM itemData = SendMessage(input, CB_GETITEMDATA, selectedIndex, 0);

                        // 3. Converter o LPARAM de volta para o tipo de ponteiro da sua estrutura
                        ComboBoxItemData* data = reinterpret_cast<ComboBoxItemData*>(itemData);

                        long long petId = 0;
                        long long tutorId = 0;
                        if (data != nullptr && selectedIndex != -1) {
                            // 4. Acessar os valores
                            petId = data->idPet;
                            tutorId = data->idTutor;
                        }

                        if (petId == 0) {
                            dados[i] = L"";
                        }
                        else {
                            // 6. Armazena o texto no seu vetor de dados.
                            dados[i] = std::to_wstring(petId);
                        }
                    }
                }
                else if (i == 3) { // Banho (radio buttons)
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_PADRAO);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIDRATACAO);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_BANHO_NENHUM);
                    if (hRadio1 && SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Padrão";
                    }
                    else if (hRadio2 && SendMessage(hRadio2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Hidratação";
                    }
                    else if (hRadio3 && SendMessage(hRadio3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Nenhum";
                    }
                }
                else if (i == 4) { // Tosa (radio buttons)
                    HWND hRadio1 = GetDlgItem(hWnd, ID_RADIO_EDIT_TESOURA);
                    HWND hRadio2 = GetDlgItem(hWnd, ID_RADIO_EDIT_MAQUINA);
                    HWND hRadio3 = GetDlgItem(hWnd, ID_RADIO_EDIT_HIGIENICA);
                    HWND hRadio4 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSADARACA);
                    HWND hRadio5 = GetDlgItem(hWnd, ID_RADIO_EDIT_TOSA_NENHUM);
                    if (hRadio1 && SendMessage(hRadio1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Tesoura";
                    }
                    else if (hRadio2 && SendMessage(hRadio2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Máquina";
                    }
                    else if (hRadio3 && SendMessage(hRadio3, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Higiênica";
                    }
                    else if (hRadio4 && SendMessage(hRadio4, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Tosa da Raça";
                    }
                    else if (hRadio5 && SendMessage(hRadio5, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Nenhum";
                    }
                }
                else if (i == 6) { // Parasitas (radio buttons)
                    HWND hCheckbox1 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PULGAS);
                    HWND hCheckbox2 = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_CARRAPATOS);

                    if (hCheckbox1 && SendMessage(hCheckbox1, BM_GETCHECK, 0, 0) == BST_CHECKED && hCheckbox2 && SendMessage(hCheckbox2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Pulgas e Carrapatos";
                    }
                    else if (hCheckbox1 && SendMessage(hCheckbox1, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Pulgas";
                    }
                    else if (hCheckbox2 && SendMessage(hCheckbox2, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        dados[i] = L"Carrapatos";
                    }
                }
                else if (i == 7) { // Lesões (checkbox buttons)
                    HWND hCheckboxPele = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_PELE);
                    HWND hCheckboxOlhos = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OLHOS);
                    HWND hCheckboxSecrecao = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_SECRECAO);
                    HWND hCheckboxOuvido = GetDlgItem(hWnd, ID_CHECKBOX_EDIT_OUVIDO);

                    // Verificar estado de cada checkbox
                    bool peleChecked = hCheckboxPele && SendMessage(hCheckboxPele, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool olhosChecked = hCheckboxOlhos && SendMessage(hCheckboxOlhos, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool secrecaoChecked = hCheckboxSecrecao && SendMessage(hCheckboxSecrecao, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    bool ouvidoChecked = hCheckboxOuvido && SendMessage(hCheckboxOuvido, BM_GETCHECK, 0, 0) == BST_CHECKED;

                    // Contar quantos estão selecionados
                    int countChecked = (peleChecked ? 1 : 0) + (olhosChecked ? 1 : 0) +
                        (secrecaoChecked ? 1 : 0) + (ouvidoChecked ? 1 : 0);

                    // Lógica para determinar o texto baseado nas combinações
                    if (countChecked == 0) {
                        dados[i] = L""; // ou L"", dependendo do que você preferir
                    }
                    else if (countChecked == 4) {
                        dados[i] = L"Pele, Olhos, Secreção e Ouvido";
                    }
                    else if (countChecked == 3) {
                        if (!peleChecked) dados[i] = L"Olhos, Secreção e Ouvido";
                        else if (!olhosChecked) dados[i] = L"Pele, Secreção e Ouvido";
                        else if (!secrecaoChecked) dados[i] = L"Pele, Olhos e Ouvido";
                        else dados[i] = L"Pele, Olhos e Secreção";
                    }
                    else if (countChecked == 2) {
                        if (peleChecked && olhosChecked) dados[i] = L"Pele e Olhos";
                        else if (peleChecked && secrecaoChecked) dados[i] = L"Pele e Secreção";
                        else if (peleChecked && ouvidoChecked) dados[i] = L"Pele e Ouvido";
                        else if (olhosChecked && secrecaoChecked) dados[i] = L"Olhos e Secreção";
                        else if (olhosChecked && ouvidoChecked) dados[i] = L"Olhos e Ouvido";
                        else dados[i] = L"Secreção e Ouvido";
                    }
                    else { // countChecked == 1
                        if (peleChecked) dados[i] = L"Pele";
                        else if (olhosChecked) dados[i] = L"Olhos";
                        else if (secrecaoChecked) dados[i] = L"Secreção";
                        else dados[i] = L"Ouvido";
                    }
                }
                else {
                    if (input) {
                        wchar_t buffer[256];
                        GetWindowText(input, buffer, 256);
                        dados[i] = std::wstring(buffer);
                    }
                }
            }

            // Abrir ou criar o banco de dados (código original mantido)
            sqlite3* db = nullptr;
            char* errMsg = nullptr;
            int rc;
            OpenDatabase(db);

            if (!OpenDatabase(db)) {
                MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            }
            else {
                std::wstring currentDate = AgendamentosSelect_GetCurrentDate();
                std::wstring currentHour = AgendamentosSelect_GetCurrentHour();

                std::wstring sqlInsertW = L"UPDATE Agendamentos SET "
                    L"Banho = '" + AgendamentosSelect_treatDataAppointment(dados[3], 3) + L"', "
                    L"Tosa = '" + AgendamentosSelect_treatDataAppointment(dados[4], 4) + L"', "
                    L"Obs_Tosa = '" + AgendamentosSelect_treatDataAppointment(dados[5], 5) + L"', "
                    L"Parasitas = '" + AgendamentosSelect_treatDataAppointment(dados[6], 6) + L"', "
                    L"Lesoes = '" + AgendamentosSelect_treatDataAppointment(dados[7], 7) + L"', "
                    L"Obs_Lesoes = '" + AgendamentosSelect_treatDataAppointment(dados[8], 8) + L"', "
                    L"Appointment_Date = '" + AgendamentosSelect_treatDataAppointment(dados[9], 9) + L"', "
                    L"Appointment_Hour = '" + AgendamentosSelect_treatDataAppointment(dados[10], 10) + L"', "
                    L"Date = '" + currentDate + L"', "
                    L"Hour = '" + currentHour + L"', "
                    L"Price = '" + AgendamentosSelect_treatDataAppointment(dados[11], 11) + L"', "
                    L"ID_Pet_FK = '" + AgendamentosSelect_treatDataAppointment(dados[2], 2) + L"' "
                    L"WHERE ID = " + std::to_wstring(AgendamentosSelect_idRecord) + L";";

                if (AgendamentosSelect_error == L"1") {
                    MessageBox(hWnd, AgendamentosSelect_msg, L"Erro", MB_OK | MB_ICONERROR);
                    AgendamentosSelect_error = L"0";
                    AgendamentosSelect_mensagem.clear();
                }
                else {
                    std::string sqlInsertUtf8 = AgendamentosSelect_WideToUTF8(sqlInsertW);
                    errMsg = nullptr;
                    rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);

                    if (rc != SQLITE_OK && errMsg) {
                        std::wstring wErrMsg = AgendamentosSelect_UTF8ToWide(errMsg);
                        MessageBox(hWnd, wErrMsg.c_str(), L"Erro", MB_OK | MB_ICONERROR);
                        sqlite3_free(errMsg);
                    }
                    else {
                        AtualizarJanelas();
                        MessageBox(hWnd, L"Dados inseridos com sucesso!", L"Sucesso", MB_OK);
                    }
                }
                sqlite3_close(db);
            }
        }
        else if (wmId == 20) { //DADOS DO CLIENTE
            HWND input = GetDlgItem(hWnd, 2);
            LRESULT selectedIndex = SendMessage(input, CB_GETCURSEL, 0, 0);

            // 2. Recuperar o ponteiro armazenado
            LPARAM itemData = SendMessage(input, CB_GETITEMDATA, selectedIndex, 0);

            // 3. Converter o LPARAM de volta para o tipo de ponteiro da sua estrutura
            ComboBoxItemData* data = reinterpret_cast<ComboBoxItemData*>(itemData);

            long long petId = 0;
            long long tutorId = 0;
            if (data != nullptr && selectedIndex != -1) {
                // 4. Acessar os valores
                petId = data->idPet;
                tutorId = data->idTutor;
            }

            if (tutorId == 0) {
                MessageBox(hWnd, L"Selecione o Tutor!", L"Erro", MB_OK | MB_ICONERROR);
                break;
            }

            TutoresSelect_idRecord = tutorId;

            if (!AgendamentosSelect_CreateNewWindow(hWnd, hInst, L"JanelaTutoresReadClasse", L"DADOS DO CLIENTE"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaTutoresReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        else if (wmId == 21) { //DADOS DO PET
            HWND input = GetDlgItem(hWnd, 2);
            LRESULT selectedIndex = SendMessage(input, CB_GETCURSEL, 0, 0);

            // 2. Recuperar o ponteiro armazenado
            LPARAM itemData = SendMessage(input, CB_GETITEMDATA, selectedIndex, 0);

            // 3. Converter o LPARAM de volta para o tipo de ponteiro da sua estrutura
            ComboBoxItemData* data = reinterpret_cast<ComboBoxItemData*>(itemData);

            long long petId = 0;
            long long tutorId = 0;
            if (data != nullptr && selectedIndex != -1) {
                // 4. Acessar os valores
                petId = data->idPet;
                tutorId = data->idTutor;
            }

            if (petId == 0) {
                MessageBox(hWnd, L"Selecione o Pet!", L"Erro", MB_OK | MB_ICONERROR);
                break;
            }

            PetsSelect_idRecord = petId;

            if (!AgendamentosSelect_CreateNewWindow(hWnd, hInst, L"JanelaPetsReadClasse", L"DADOS DO PET"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaPetsReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        break;
    }

    case WM_PAINT:
    {
        TutoresPetsSelect_Global_selectDB();

        HWND hComboBox = AgendamentosSelect_g_editControlsEdit[0];
        SendMessage(hComboBox, CB_RESETCONTENT, 0, 0);
        TutoresPetsSelect_Global_preencherComboBox(hComboBox);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 2. Preenche os campos com os dados do banco de dados
        AgendamentosSelect_PreencherControlesEdicao(hWnd);

        // Double buffering
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, AgendamentosSelect_g_clientWidth, AgendamentosSelect_g_clientHeight);
        HGDIOBJ hOld = SelectObject(hdcMem, hbmMem);

        // Preencher fundo completo com branco
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        FillRect(hdcMem, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

        AgendamentosSelect_fonte(L"Font", RGB(0, 0, 0), hdcMem);

        int width = AgendamentosSelect_g_clientWidth - 44;
        int cellHeight = 32;
        int startY = 40 - AgendamentosSelect_g_scrollY;
        int startX = 22 - AgendamentosSelect_g_scrollX;

        // Título com fundo branco
        RECT titleRect = { startX - 5, startY - 25, startX + 250, startY + 5 };
        FillRect(hdcMem, &titleRect, (HBRUSH)(COLOR_WINDOW + 1));
        AgendamentosSelect_windowsTitle(hdcMem, startX, startY - 20, L"EDITAR AGENDAMENTO", 18);

        // Desenhar linhas visíveis
        int firstVisibleRow = max(0, (AgendamentosSelect_g_scrollY - 40) / cellHeight);
        int lastVisibleRow = min(9, firstVisibleRow + (AgendamentosSelect_g_clientHeight / cellHeight) + 2);

        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // CORREÇÃO: Usar background OPAQUE com cores consistentes
        SetBkMode(hdcMem, OPAQUE);

        for (int row = firstVisibleRow; row <= lastVisibleRow; row++) {
            if (row >= 10) break;

            HBRUSH hCurrentBrush = (row % 2 == 0) ? hBrushGray : hBrushWhite;
            COLORREF bgColor = (row % 2 == 0) ? RGB(240, 240, 240) : RGB(255, 255, 255);

            RECT rowRect = {
                startX,
                startY + (row + 1) * cellHeight,
                startX + width,
                startY + (row + 2) * cellHeight
            };

            // Preencher fundo da linha inteira
            FillRect(hdcMem, &rowRect, hCurrentBrush);

            int xPosLabel = startX + 10;
            int yPosLabel = startY + (row + 1) * cellHeight + 7;

            AgendamentosSelect_fonte(L"Header", RGB(0, 0, 0), hdcMem);

            // CORREÇÃO: Configurar cor de fundo para combinar com a linha
            SetBkColor(hdcMem, bgColor);

            // Desenhar labels
            const wchar_t* labels[] = {
                L"Nome do Tutor e do Pet:", L"Banho:", L"Tosa:", L"Observação:",
                L"Parasitas:", L"Lesões:", L"Observação:", L"Data:", L"Hora:", L"Preço R$:"
            };

            if (row < 10) {
                TextOut(hdcMem, xPosLabel, yPosLabel, labels[row], wcslen(labels[row]));
            }
        }

        // Limpar recursos
        DeleteObject(hBrushWhite);
        DeleteObject(hBrushGray);

        // Copiar buffer para tela
        BitBlt(hdc, 0, 0, AgendamentosSelect_g_clientWidth, AgendamentosSelect_g_clientHeight, hdcMem, 0, 0, SRCCOPY);

        // Limpar recursos do buffer
        SelectObject(hdcMem, hOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;

        // Verificar se o controle é um radio button
        if (GetWindowLongPtr(hCtrl, GWL_STYLE) & BS_AUTORADIOBUTTON) {
            SetBkMode(hdc, TRANSPARENT); // Definir fundo transparente
            return (INT_PTR)AgendamentosSelect_hBrushTransparent; // Retornar o pincel transparente
        }
        break;
    }

    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT); // Fundo transparente para botões push
        return (INT_PTR)AgendamentosSelect_hBrushTransparent;
    }

    case WM_DESTROY:
    {
        // Limpar array de controles
        AgendamentosSelect_g_editControlsEdit.clear();
        AgendamentosSelect_g_hButton = NULL;
        AgendamentosSelect_windowClose(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        AgendamentosSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcAgendamentosEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela Edit!", 21);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        AgendamentosSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitAgendamentosEdit(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcAgendamentosEdit;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"EditClass";
    return RegisterClassW(&wc);
}