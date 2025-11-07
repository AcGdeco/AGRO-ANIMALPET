#include "AgendamentosConsultar.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "AgendamentosFuncoes.h"
#include <string>
#include "AgendamentosSelect.h"
#include "sqlite3.h"
#include <vector>

// Variáveis externas para scroll
extern int AgendamentosSelect_g_scrollY;
extern int AgendamentosSelect_g_clientHeight;
extern int AgendamentosSelect_g_contentHeight;
extern int AgendamentosSelect_g_scrollX;
extern int AgendamentosSelect_g_clientWidth;
extern int AgendamentosSelect_g_contentWidth;

std::wstring idTutorAgendamento;
std::wstring idPetAgendamento;

std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableDataConsulta;


// Função para atualizar posição dos controles com scroll
void AgendamentosSelect_AtualizarPosicoesControlesAgendamentoConsultar(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
    int startY = 40 - AgendamentosSelect_g_scrollY;
    int startX = 22 - AgendamentosSelect_g_scrollX;
    int xPos = 0;
    int yPos = 0;
    int colNumber;
    int countRow = 0;

    xPos = startX;
    yPos = startY + 15 * cellHeight + 1;

    // Atualizar posição do botão
    if (AgendamentosSelect_g_hButton_consultar_tutor) {
        SetWindowPos(AgendamentosSelect_g_hButton_consultar_tutor, NULL, xPos, yPos, 80, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }

    yPos = startY + 18 * cellHeight + 1;
    if (AgendamentosSelect_g_hButton_consultar_pet) {
        SetWindowPos(AgendamentosSelect_g_hButton_consultar_pet, NULL, xPos, yPos, 80, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}


// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring AgendamentosSelect_utf8_to_wstring_consulta(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

int AgendamentosSelect_sqlite_callback_consulta(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? AgendamentosSelect_utf8_to_wstring_consulta(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? AgendamentosSelect_utf8_to_wstring_consulta(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcAgendamentosRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para configurar scroll bars
void AgendamentosSelect_ConfigurarScrollBarsConsulta(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    AgendamentosSelect_g_clientHeight = rect.bottom - rect.top;
    AgendamentosSelect_g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo baseado na tabela
    int cellHeight = 32;
    AgendamentosSelect_g_contentHeight = 24 * cellHeight + 100; // 22 colunas + margem
    AgendamentosSelect_g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = AgendamentosSelect_g_contentHeight;
    si.nPage = AgendamentosSelect_g_clientHeight;
    si.nPos = AgendamentosSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = AgendamentosSelect_g_contentWidth;
    si.nPage = AgendamentosSelect_g_clientWidth;
    si.nPos = AgendamentosSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterAgendamentosReadClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcAgendamentosRead;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaAgendamentosReadClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Read
LRESULT CALLBACK WndProcAgendamentosRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        if (wmId == 0) { //CONSULTAR
            idTutorAgendamento = AgendamentosSelect_g_tableDataConsulta[1][0];
            TutoresSelect_idRecord = std::stoll(idTutorAgendamento);

            if (!AgendamentosSelect_CreateNewWindow(hWnd, hInst, L"JanelaTutoresReadClasse", L"CONSULTAR TUTOR"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaTutoresReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    AgendamentosSelect_invalidateDrawing(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        else if (wmId == 1) { //CONSULTAR
            idPetAgendamento = AgendamentosSelect_g_tableDataConsulta[1][9];
            PetsSelect_idRecord = std::stoll(idPetAgendamento);

            if (!AgendamentosSelect_CreateNewWindow(hWnd, hInst, L"JanelaPetsReadClasse", L"CONSULTAR PET"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaPetsReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    AgendamentosSelect_invalidateDrawing(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        break;
    }
    case WM_CREATE: {
        // Resetar scroll para garantir que comece do topo
        AgendamentosSelect_g_scrollY = 0;
        AgendamentosSelect_g_scrollX = 0;
        AgendamentosSelect_ConfigurarScrollBarsConsulta(hWnd);

        // Obter dimensões da janela
        // Texto de exemplo
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela com scroll
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = AgendamentosSelect_g_tableDataConsulta.empty() ? 0 : AgendamentosSelect_g_tableDataConsulta[0].size() + 3;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - AgendamentosSelect_g_scrollY;  // Posição Y com scroll
        int startX = 22 - AgendamentosSelect_g_scrollX;  // Posição X com scroll

        int xPos = startX;
        int yPos = startY + 15 * cellHeight + 1;

        // Criar botão consultar
        AgendamentosSelect_g_hButton_consultar_tutor = CreateWindowW(
            L"BUTTON", L"Consultar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
            xPos, yPos, 80, 30,
            hWnd, (HMENU)(0),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );

        xPos = startX;
        yPos = startY + 18 * cellHeight + 1;

        // Criar botão consultar
        AgendamentosSelect_g_hButton_consultar_pet = CreateWindowW(
            L"BUTTON", L"Consultar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
            xPos, yPos, 80, 30,
            hWnd, (HMENU)(1),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );

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
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoConsultar(hWnd);
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
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoConsultar(hWnd);
            AgendamentosSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        break;
    }

    case WM_SIZE: {
        int newWidth = LOWORD(lParam);
        int newHeight = HIWORD(lParam);

        // Verificar se é uma mudança significativa de tamanho (maximizar/restaurar)
        static int oldWidth = 0;
        static int oldHeight = 0;

        if ((newWidth > oldWidth * 1.5) || (newHeight > oldHeight * 1.5) ||
            (newWidth < oldWidth * 0.7) || (newHeight < oldHeight * 0.7)) {
            // Mudança significativa - resetar scroll para o topo
            AgendamentosSelect_g_scrollY = 0;
            AgendamentosSelect_g_scrollX = 0;
        }

        oldWidth = newWidth;
        oldHeight = newHeight;

        AgendamentosSelect_g_clientWidth = newWidth;
        AgendamentosSelect_g_clientHeight = newHeight;
        SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
        AgendamentosSelect_ConfigurarScrollBarsConsulta(hWnd);
        AgendamentosSelect_AtualizarPosicoesControlesAgendamentoConsultar(hWnd);
        SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
        AgendamentosSelect_invalidateDrawing(hWnd);
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
            AgendamentosSelect_AtualizarPosicoesControlesAgendamentoConsultar(hWnd);
            AgendamentosSelect_invalidateDrawing(hWnd);
            UpdateWindow(hWnd);
        }
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        AgendamentosSelect_fonte(L"Font", RGB(0, 0, 0), hdc);

        // Texto de exemplo
        RECT rect;
        GetClientRect(hWnd, &rect);

        // Fundo branco
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
        AgendamentosSelect_g_tableDataConsulta.clear();

        // Abrir ou criar o banco de dados (código original mantido)
        char* errMsg = 0;
        sqlite3* db = nullptr;
        std::string dbPath = GetAppDataPath() + "pet.db";

        // Cria a pasta se não existir
        CreateDirectoryA(dbPath.substr(0, dbPath.find_last_of('\\')).c_str(), NULL);

        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc == SQLITE_OK) {

            // 1. Defina o buffer de destino. Escolha um tamanho adequado.
            char buffer[512];

            // 2. Use snprintf para formatar a string
            snprintf(
                buffer,
                sizeof(buffer),
                "SELECT * "
                "FROM Tutores T INNER JOIN Pets P ON T.ID = P.ID_Tutor_FK "
                "INNER JOIN Agendamentos A ON P.ID = A.ID_Pet_FK "
                "WHERE A.ID = %d;",
                AgendamentosSelect_idRecord // O valor que será inserido no '%d'
            );

            // 3. O 'buffer' agora contém a string SQL formatada.
            const char* sqlSelectConsulta = buffer;

            rc = sqlite3_exec(db, sqlSelectConsulta, AgendamentosSelect_sqlite_callback_consulta, &AgendamentosSelect_g_tableDataConsulta, &errMsg);
            if (rc != SQLITE_OK) {
                if (errMsg) {
                    // Converte char* para wchar_t* corretamente
                    size_t len = strlen(errMsg) + 1;
                    std::wstring wErrMsg(len, L'\0');
                    mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                    // Remove o caractere nulo extra do final
                    wErrMsg.resize(wcslen(wErrMsg.c_str()));
                    AgendamentosSelect_g_tableDataConsulta.push_back({ L"Erro", wErrMsg });
                }
                else {
                    AgendamentosSelect_g_tableDataConsulta.push_back({ L"Erro", L"Desconhecido" });
                }
                if (errMsg) sqlite3_free(errMsg);
            }
            sqlite3_close(db);
        }
        else {
            AgendamentosSelect_g_tableDataConsulta.push_back({ L"Erro", L"Não foi possível abrir o banco" });
        }

        if (AgendamentosSelect_g_tableDataConsulta.empty()) {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }

        idTutorAgendamento = AgendamentosSelect_g_tableDataConsulta[1][0];
        idPetAgendamento = AgendamentosSelect_g_tableDataConsulta[1][9];

        // Obter dimensões da janela
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela com scroll
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = AgendamentosSelect_g_tableDataConsulta.empty() ? 0 : AgendamentosSelect_g_tableDataConsulta[0].size() + 3;
        int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
        int startY = 40 - AgendamentosSelect_g_scrollY;  // Posição Y com scroll
        int startX = 22 - AgendamentosSelect_g_scrollX;  // Posição X com scroll

        //Título
        AgendamentosSelect_windowsTitle(hdc, startX, startY - 20, L"CONSULTAR AGENDAMENTO", 21);

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

        for (size_t col = 20; col < 32; col++) {
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

            for (size_t row = 0; row < AgendamentosSelect_g_tableDataConsulta.size(); row++) {
                int xPos;
                int yPos;

                if (row == 0) {
                    xPos = startX + 10;
                    yPos = startY + colNumber * cellHeight + 7;
                    AgendamentosSelect_fonte(L"Header", RGB(0, 0, 0), hdc);
                }
                else {
                    xPos = startX + cellWidth + 60;
                    yPos = startY + colNumber * cellHeight + 7;
                    AgendamentosSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
                }

                if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"ID") {
                    TextOut(hdc, xPos, yPos, L"ID:", 3);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Nome_do_Pet") {
                    TextOut(hdc, xPos, yPos, L"Nome do Pet:", 12);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Raca") {
                    TextOut(hdc, xPos, yPos, L"Raça:", 5);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Nome_do_Tutor") {
                    TextOut(hdc, xPos, yPos, L"Nome do Tutor:", 14);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"CEP") {
                    TextOut(hdc, xPos, yPos, L"CEP:", 4);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Cor") {
                    TextOut(hdc, xPos, yPos, L"Cor:", 4);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Idade") {
                    TextOut(hdc, xPos, yPos, L"Idade:", 6);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Peso") {
                    TextOut(hdc, xPos, yPos, L"Peso:", 5);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Sexo") {
                    TextOut(hdc, xPos, yPos, L"Sexo:", 5);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Castrado") {
                    TextOut(hdc, xPos, yPos, L"Castrado:", 9);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Endereco") {
                    TextOut(hdc, xPos, yPos, L"Endereço:", 9);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Ponto_de_referencia") {
                    TextOut(hdc, xPos, yPos, L"Ponto de Referência:", 20);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Banho") {
                    TextOut(hdc, xPos, yPos, L"Banho:", 6);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Tosa") {
                    TextOut(hdc, xPos, yPos, L"Tosa:", 5);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Obs_Tosa") {
                    TextOut(hdc, xPos, yPos, L"Observação:", 11);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Parasitas") {
                    TextOut(hdc, xPos, yPos, L"Parasitas:", 10);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Lesoes") {
                    TextOut(hdc, xPos, yPos, L"Lesões:", 7);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Obs_Lesoes") {
                    TextOut(hdc, xPos, yPos, L"Observação:", 11);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Telefone") {
                    TextOut(hdc, xPos, yPos, L"Telefone:", 9);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"CPF") {
                    TextOut(hdc, xPos, yPos, L"CPF:", 4);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Appointment_Date") {
                    TextOut(hdc, xPos, yPos, L"Data do Agendamento:", 20);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Appointment_Hour") {
                    TextOut(hdc, xPos, yPos, L"Hora do Agendamento:", 20);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Date") {
                    TextOut(hdc, xPos, yPos, L"Data do Registro:", 17);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Hour") {
                    TextOut(hdc, xPos, yPos, L"Hora do Registro:", 17);
                }
                else if (AgendamentosSelect_g_tableDataConsulta[row][col] == L"Price") {
                    TextOut(hdc, xPos, yPos, L"Preço R$:", 9);
                }
                else {
                    if (col == 31) {
                        std::wstring precoStr = AgendamentosSelect_g_tableDataConsulta[row][col];
                        double valor = std::stod(precoStr);
                        
                        TextOut(hdc, xPos, yPos, FormatarPrecoSemPontoMilhar(valor).c_str(),
                            static_cast<int>(FormatarPrecoSemPontoMilhar(valor).length()));
                    }
                    else {
                        TextOut(hdc, xPos, yPos, AgendamentosSelect_g_tableDataConsulta[row][col].c_str(),
                            static_cast<int>(AgendamentosSelect_g_tableDataConsulta[row][col].length()));
                    }
                }
                rowNumber++;
            }
        }

        SelectObject(hdc, hBrushGray);

        // Desenhar o fundo da linha (com scroll)
        RECT rowRect = {
            startX,
            startY + static_cast<int>(colNumber + 2) * cellHeight,
            startX + width,
            startY + (static_cast<int>(colNumber) + 3) * cellHeight
        };
        FillRect(hdc, &rowRect, hBrushGray);

        int yPos = startY + (colNumber + 2) * cellHeight + 7;
        int xPos = startX + 10;
        TextOut(hdc, xPos, yPos, L"Nome do Tutor:", 14);

        yPos = startY + (colNumber + 2) * cellHeight + 7;
        xPos = startX + cellWidth + 60;
        TextOut(hdc, xPos, yPos, AgendamentosSelect_g_tableDataConsulta[1][1].c_str(), static_cast<int>(AgendamentosSelect_g_tableDataConsulta[1][1].length()));

        // Desenhar o fundo da linha (com scroll)
        rowRect = {
            startX,
            startY + static_cast<int>(colNumber + 5) * cellHeight,
            startX + width,
            startY + (static_cast<int>(colNumber) + 6) * cellHeight
        };
        FillRect(hdc, &rowRect, hBrushGray);

        yPos = startY + (colNumber + 5) * cellHeight + 7;
        xPos = startX + 10;
        TextOut(hdc, xPos, yPos, L"Nome do Pet:", 12);

        yPos = startY + (colNumber + 5) * cellHeight + 7;
        xPos = startX + cellWidth + 60;
        TextOut(hdc, xPos, yPos, AgendamentosSelect_g_tableDataConsulta[1][10].c_str(), static_cast<int>(AgendamentosSelect_g_tableDataConsulta[1][10].length()));

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
        AgendamentosSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
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
    case WM_DESTROY:
    {
        AgendamentosSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProcAgendamentosRead(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        AgendamentosSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitAgendamentosRead(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProcAgendamentosRead;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ReadClass";
    return RegisterClassW(&wc);
}