#include "Select.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <format>
#include <string>
#include "sqlite3.h"
#include <vector>
#include <cmath>

// ADICIONE estas linhas para usar as variáveis externas:
extern int g_scrollY;
extern int g_clientHeight;
extern int g_contentHeight;

bool g_wasInactive = false; // Inicialização
HWND g_hWndMain = NULL;     // Inicialização

std::vector<int> naoDesenhar;

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring utf8_to_wstring(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

std::wstring className = L"JanelaSelectClasse";
// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterSelectClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcSelect;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = className.c_str();
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Select
LRESULT CALLBACK WndProcSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        g_wasInactive = false;
        g_hWndMain = hWnd;

        // Garantir que a janela tenha WS_VSCROLL
        LONG style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!(style & WS_VSCROLL)) {
            SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_VSCROLL);
        }

        // Abrir ou criar o banco de dados (código original mantido)
        sqlite3* db = nullptr;
        char* errMsg = nullptr;
        int rc;
        OpenDatabase(db);

        if (!OpenDatabase(db)) {
            MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            sqlite3_free(errMsg);
        }
        else {
            // Código de criação de tabela e inserção (mantido como está)
            const char* sqlDrop = "DROP TABLE IF EXISTS Tudo;";
            rc = sqlite3_exec(db, sqlDrop, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                MessageBox(hWnd, L"Erro ao dropar tabela!", L"Erro", MB_OK | MB_ICONERROR);
                sqlite3_free(errMsg);
            }
            const char* sqlCreate = "CREATE TABLE IF NOT EXISTS Tudo (ID INTEGER PRIMARY KEY AUTOINCREMENT, Nome_do_Pet TEXT, Raca TEXT, Nome_do_Tutor TEXT, CEP TEXT, Cor TEXT, Idade TEXT, Peso TEXT, Sexo TEXT, Castrado TEXT, Endereco TEXT, Ponto_de_referencia TEXT, Banho TEXT, Tosa TEXT, Obs_Tosa TEXT, Parasitas TEXT, Lesoes TEXT, Obs_Lesoes TEXT, Telefone TEXT, CPF TEXT, Appointment_Date TEXT, Appointment_Hour TEXT, Date TEXT, Hour TEXT);";
            rc = sqlite3_exec(db, sqlCreate, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                wchar_t fullMsg[512] = L"Erro ao criar tabela! Código: ";
                wchar_t codeStr[32];
                swprintf_s(codeStr, L"%d", rc);
                wcscat_s(fullMsg, codeStr);
                if (errMsg) {
                    size_t len = strlen(errMsg) + 1;
                    wchar_t wErrMsg[256];
                    mbstowcs_s(NULL, wErrMsg, len, errMsg, _TRUNCATE);
                    wcscat_s(fullMsg, L" - Detalhes: ");
                    wcscat_s(fullMsg, wErrMsg);
                }
                MessageBox(hWnd, fullMsg, L"Erro", MB_OK | MB_ICONERROR);
                sqlite3_free(errMsg);
            }
            else {
                // Inserções (código original mantido)
                std::wstring currentDate = GetCurrentDate();
                std::wstring currentHour = GetCurrentHour();
                for (int i = 1; i <= 100; i++) {
                   std::wstring sqlInsertW = L"INSERT INTO Tudo (Nome_do_Pet, Raca, Nome_do_Tutor, CEP, Cor, Idade, Peso, Sexo, Castrado, Endereco, Ponto_de_referencia, Banho, Tosa, Obs_Tosa, Parasitas, Lesoes, Obs_Lesoes, Telefone, CPF, Appointment_Date, Appointment_Hour, Date, Hour) VALUES ('Fido', 'Bulldog', 'Laís', '36309016', 'Preto', 5, 25, 'Masculino', 'Sim', 'Rua das flores - Guarda Mor - São João del Rei', 'Perto da pizzaria Agostinho', 'Padrão', 'Tesoura', 'ir qpiofj adfjs kçjf dkfjeif çsdaf jkasdjf iejf sdçf aksdfjis fdfj çaklsfjaksdfj kdsjfçaejf idsjfkasdf jaies', 'Carrapatos', 'Pele', 'asdf façldj fçkalsdj fdaskljf dsçkf jçklasdf jkaldsfj çkldsa fjaçklds jfakdls fjkalsdjf klasdjf çasdfj çasfj çadsklfjklf ', '32998360862', '09813426632', '04/07/2025', '15:00', '" + currentDate + L"', '" + currentHour + L"');";
                    int required = WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    if (required > 0) {
                        std::string sqlInsertUtf8(required, '\0');
                        WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, &sqlInsertUtf8[0], required, nullptr, nullptr);
                        char* errMsg = nullptr;
                        int rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);
                        if (rc != SQLITE_OK) sqlite3_free(errMsg);
                    }
                }
                //std::wstring sqlInsertW2 = L"INSERT INTO Pets (Nome_do_Pet, Raca, Nome_do_Tutor, CEP, Cor, Idade, Peso, Sexo, Castrado, Endereco, Ponto_de_referencia, Banho, Tosa, Obs_Tosa, Parasitas, Lesoes, Obs_Lesoes, Telefone, CPF, Date, Hour) VALUES ('Astralis', 'Viralata', 'Débora', '36309022', 'Preto', 6, 18, 'Feminino', 'Não', 'Rua Ricador Geraldo dos Santos - Alto das Mercês - nº12', 'Perto da igreja das Mercês', 'Padrão', 'Tesoura', 'aa ksfj asldfj açlksdj fkasjd fçklasdjf aksdlfjkalçsdfj kçalsdjf kçlasjd çfkasdj fklçaj sdçlfkjakslfj çlasdjf çasd jfçaskdjfsdf', 'Carrapatos', 'Pele', ' asdfj açlsjf akçslj fkçlasdj fkçlasdjf lçkasjdf kasdjfkiujriwejfç dfkmdnfçnvçaidsjfçkdsfjaçksdjfkaçsjdfkasdjfkçlasjdçfaksdjf', '32998365552', '09813426789', '" + currentDate + L"', '" + currentHour + L"');";
                //int required2 = WideCharToMultiByte(CP_UTF8, 0, sqlInsertW2.c_str(), -1, nullptr, 0, nullptr, nullptr);
                //if (required2 > 0) {
                    //std::string sqlInsertUtf8(required2, '\0');
                    //WideCharToMultiByte(CP_UTF8, 0, sqlInsertW2.c_str(), -1, &sqlInsertUtf8[0], required2, nullptr, nullptr);
                    //char* errMsg = nullptr;
                    //int rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);
                    //if (rc != SQLITE_OK) sqlite3_free(errMsg);
                //}
                //if (rc != SQLITE_OK && errMsg) {
                    //size_t len = strlen(errMsg) + 1;
                    //wchar_t wErrMsg[256];
                    //mbstowcs_s(NULL, wErrMsg, len, errMsg, _TRUNCATE);
                    //wchar_t fullMsg[512];
                    //swprintf_s(fullMsg, L"Erro ao inserir dados! Detalhes: %s", wErrMsg);
                    //MessageBoxW(hWnd, fullMsg, L"Erro", MB_OK | MB_ICONERROR);
                    //sqlite3_free(errMsg);
                //}
            }
            sqlite3_close(db);

            RecarregarDadosTabela(hWnd);
            
        }
        return 0;
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        HWND hButton = (HWND)lParam; // Handle do botão que disparou o evento
        LONG_PTR id = GetWindowLongPtr(hButton, GWLP_USERDATA); // Recuperar o id do registro

        if (wmId == CONSULTAR) // Botões "Consultar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Consultar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            idRecord = id;

            if (!CreateNewWindow(hWnd, hInst, L"JanelaReadClasse", L"DADOS DO AGENDAMENTO"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        else if (wmId == EDITAR) // Botões "Editar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Editar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            idRecord = id;

            if (!CreateNewWindow(hWnd, hInst, L"JanelaEditClasse", L"EDITAR AGENDAMENTO"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaEditClasse", NULL);
                if (hWndRead != NULL)
                {
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                    SetForegroundWindow(hWndRead);
                }
            }
        }
        else if (wmId == DELETAR) // Botões "Deletar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Deletar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            idRecord = id;
            std::wstring msg = L"Deletar registro ID " + std::to_wstring(idRecord) + L"?";
            if (MessageBoxW(hWnd, msg.c_str(), L"Confirmar", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                deleteRecordById("pet.db", idRecord, hWnd);
                RecarregarDadosTabela(hWnd);
            }
        }
        else if (wmId == FILTRAR)
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado!", L"Filtrar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            for (int i = 0; i <= 25; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i + 20);
                if (i == 9 || i == 12 || i == 13 || i == 15 || i == 16) { // Se o comando veio do nosso ComboBox
                    // 1. Obter o índice do item selecionado
                    int indiceSelecionado = (int)SendMessageW(
                        input, CB_GETCURSEL, 0, 0
                    );

                    // 2. Obter o texto do item selecionado
                    if (indiceSelecionado != CB_ERR) {
                        wchar_t buffer[256];

                        // Obter o texto do índice
                        SendMessageW(
                            input,
                            CB_GETLBTEXT,
                            (WPARAM)indiceSelecionado,
                            (LPARAM)buffer
                        );
                        dados[i] = std::wstring(buffer);

                        // O valor selecionado está em 'buffer' (ex: L"Opção B")
                        // Faça algo com o valor, como atualizar o filtro:
                        // std::wstring valorFiltro = buffer;
                        // aplicarFiltro(valorFiltro);
                    }
                }
                else if (input) {
                    wchar_t buffer[256];
                    GetWindowText(input, buffer, 256);
                    dados[i] = std::wstring(buffer);
                }
            }

            HWND input = GetDlgItem(hWnd, 20 + 2 * 20);
            wchar_t buffer[256];
            GetWindowText(input, buffer, 256);
            dataAte = std::wstring(buffer);

            input = GetDlgItem(hWnd, 22 + 2 * 20);
            GetWindowText(input, buffer, 256);
            dataRegistroAte = std::wstring(buffer);

            idNumeroUltimo = 1;
            offsetTableRow = 1;

            RecarregarDadosTabela(hWnd);
        }
        else if (wmId == ORDENAR) // Botões "Ordenar"
        {
            btnClicado = L"ORDENAR";
            idBtnGlobal = id;
            RecarregarDadosTabela(hWnd);
        }
        // Verifica se a mensagem veio do seu ComboBox LIMITAR
        else if (wmId == LIMITAR) {

            // HIWORD(wParam) é o código de notificação específico do controle
            int notificationCode = HIWORD(wParam);

            // 1. VERIFICA A MUDANÇA DE SELEÇÃO
            if (notificationCode == CBN_SELCHANGE) {

                // A SELEÇÃO MUDOU! É AQUI QUE VOCÊ CHAMA SUA FUNÇÃO.

                // Obtém o Handle do ComboBox (opcional, mas bom para clareza)
                HWND hComboBox = (HWND)lParam;

                if (!IsWindow(hComboBox)) {
                    return 0;
                }

                // CHAMAR FUNÇÃO DE AÇÃO
                handleLimitChange(hComboBox);

                RecarregarDadosTabela(hWnd);

                // Certifique-se de retornar 0 após tratar a mensagem
                return 0;
            }
        }
        else if (wmId == OFFSET) {

            mudarPagina(id);
            RecarregarDadosTabela(hWnd);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
       
        fonte(L"Font", RGB(0, 0, 0), hdc);
       
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

        // LIMPAR a área de desenho primeiro
        HBRUSH hBgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        FillRect(hdc, &rect, hBgBrush);
        DeleteObject(hBgBrush);

        // Desenhar fundos alternados para as linhas
        HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));
        HBRUSH hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hBrushGray = CreateSolidBrush(RGB(240, 240, 240));

        // Desenhar o texto nas células
        SetBkMode(hdc, TRANSPARENT);

        //Título
        windowsTitle(hdc, startX, startY - 330, L"AGENDAMENTOS", 12);

        // Desenhar filtros
        createHeaderFilters(hdc, hWnd);

        //Header Table
        createHeaderTable(hWnd, hdc);

        int linha = 1;
        int counter = 0;

        // CORREÇÃO: Cálculo seguro do limite
        int limit;
        if (rowsNumber == 0) {
            limit = 0;
        }
        else {
            limit = min(offsetTableRow + limitTableRow, rowsNumber);
        }

        // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários
        for (size_t row = offsetTableRow; row < limit && row < g_tableData.size(); row++) {
            if (row < g_tableData.size()) {
                HBRUSH hCurrentBrush = (linha % 2 == 0) ? hBrushGray : hBrushWhite;

                if (linha == 0) {
                    hCurrentBrush = hBrushHeader;
                    fonte(L"Header", RGB(255, 255, 255), hdc);
                }
                else {
                    fonte(L"Font", RGB(0, 0, 0), hdc);
                }

                // Desenhar o fundo da linha
                RECT rowRect = {
                    startX,
                    startY + static_cast<int>(linha) * cellHeight,
                    startX + width,
                    startY + (static_cast<int>(linha) + 1) * cellHeight
                };
                FillRect(hdc, &rowRect, hCurrentBrush);

                counter = 0;
                // Desenhar as células de dados
                for (size_t col = 0; col < g_tableData[row].size(); col++) {
                    std::wstring displayText = g_tableData[row][col];

                    if (col == 0 || col == 1 || col == 3 || col == 12 || col == 13 || col == 20 || col == 21) {
                        int xPos = startX + counter * cellWidth + 10;
                        int yPos = startY + linha * cellHeight + 7;

                        int qtyCaracters = displayText.length();
                        if (width <= 1600 && displayText.length() > 15) {
                            qtyCaracters = 15;

                        }
                        else if (width <= 2000 && displayText.length() > 25) {
                            qtyCaracters = 25;
                        }
                        TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(qtyCaracters));
                        counter++;
                    }
                }

                if (counter != 0) {
                    linha++;
                }
            }
        }

        // DESENHAR CABEÇALHOS DOS BOTÕES APENAS UMA VEZ - fora do loop principal
        if (!g_tableData.empty()) {
            fonte(L"Font", RGB(255, 255, 255), hdc);

            int headerY = startY + 7;

            // Consultar
            int xPos = startX + columnNumber * cellWidth + 10;
            TextOut(hdc, xPos, headerY, L"Consultar", 9);

            // Editar
            xPos = startX + (columnNumber + 1) * cellWidth + 2;
            TextOut(hdc, xPos, headerY, L"Editar", 6);

            // Deletar
            xPos = startX + (columnNumber + 2) * cellWidth + 2;
            TextOut(hdc, xPos, headerY, L"Deletar", 7);
        }

        // Limpar recursos
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
    case WM_SIZE:
    {
        RecarregarDadosTabela(hWnd);
    }
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE) {
            g_wasInactive = true; // Marca como inativa quando perde foco para outra janela
        }
        else if (LOWORD(wParam) != WA_INACTIVE && g_wasInactive) {
            //RecarregarDadosTabela(hWnd);
            //MessageBoxW(hWnd, L"A janela ganhou foco novamente!", L"Aviso", MB_OK | MB_ICONINFORMATION);
            g_wasInactive = false; // Resetar após exibir o popup
        }
        return 0;
    }
    break;

    case WM_KILLFOCUS:
    {
        g_wasInactive = true; // Marca como inativa quando perde foco para outro controle
        return 0;
    }
    break;

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
            g_scrollY = si.nPos;

            RecarregarDadosTabela(hWnd);
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
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
            g_scrollY = si.nPos;

            RecarregarDadosTabela(hWnd);
        }
        return 0;
    }
    case WM_ERASEBKGND:
    {
        // Retornar TRUE para evitar que o sistema apague o fundo
        // Isso reduz o flicker durante o redesenho
        return 1;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDraw = (LPDRAWITEMSTRUCT)lParam;
        HWND hBotao = pDraw->hwndItem;

        // Obter o idBotao do GWLP_USERDATA
        LONG_PTR idBotao = GetWindowLongPtr(hBotao, GWLP_USERDATA);

        if (pDraw->CtlID == OFFSET) {
            RECT rcButton = pDraw->rcItem;

            // 1. PRIMEIRO desenhar a borda
            UINT uState = DFCS_BUTTONPUSH;
            if (pDraw->itemState & ODS_SELECTED) {
                uState |= DFCS_PUSHED;
            }
            DrawFrameControl(pDraw->hDC, &rcButton, DFC_BUTTON, uState);

            // 2. REDUZIR a área interna para não sobrepor a borda
            InflateRect(&rcButton, -2, -2);  // Reduz para dentro

            if (idBotao == idNumeroUltimo) {
                // Botão especial (vermelho)
                HBRUSH hBrush = CreateSolidBrush(RGB(150, 150, 150));
                FillRect(pDraw->hDC, &rcButton, hBrush);
                DeleteObject(hBrush);

                SetTextColor(pDraw->hDC, RGB(255, 255, 255));
            }
            else {
                // Botão normal
                COLORREF bgColor = GetSysColor(COLOR_BTNFACE);
                HBRUSH hBrush = CreateSolidBrush(bgColor);
                FillRect(pDraw->hDC, &rcButton, hBrush);
                DeleteObject(hBrush);

                SetTextColor(pDraw->hDC, GetSysColor(COLOR_BTNTEXT));
            }

            // 3. AGORA desenhar o texto
            SetBkMode(pDraw->hDC, TRANSPARENT);

            WCHAR buffer[32];
            swprintf(buffer, 32, L"%ld", (long)idBotao);

            DrawText(pDraw->hDC, buffer, -1, &rcButton,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        return TRUE;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
LRESULT CALLBACK NewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc, 10, 10, L"Esta é a nova janela!", 21);
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
BOOL InitSelect(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = NewWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SelectClass";
    return RegisterClassW(&wc);
}