#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include <format>
#include <string>
#include "sqlite3.h"
#include <vector>
#include <cmath>
#include "PetsFuncoes.h"

// ADICIONE estas linhas para usar as variáveis externas:
extern int PetsSelect_g_scrollY;
extern int PetsSelect_g_clientHeight;
extern int PetsSelect_g_contentHeight;

bool PetsSelect_g_wasInactive = false; // Inicialização
HWND PetsSelect_g_hWndMain = NULL;     // Inicialização

std::vector<int> PetsSelect_naoDesenhar;

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcPetsSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring PetsSelect_utf8_to_wstring(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

// Função para registrar a classe da janela (pode ser chamada de outro lugar, como Pet.cpp)
BOOL RegisterPetsSelectClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcPetsSelect;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;  // Menu será definido dinamicamente
    wcex.lpszClassName = L"JanelaPetsSelectClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Select
LRESULT CALLBACK WndProcPetsSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (PetsSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
            return 0; // Mensagem já processada pelo menu
        }
    }

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        PetsSelect_g_wasInactive = false;
        PetsSelect_g_hWndMain = hWnd;

        // Garantir que a janela tenha WS_VSCROLL
        LONG style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!(style & WS_VSCROLL)) {
            SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_VSCROLL);
        }

        // Abrir ou criar o banco de dados (código original mantido)
        sqlite3* db;
        char* errMsg = 0;
        int rc = sqlite3_open("pet.db", &db);
        if (rc) {
            MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            sqlite3_free(errMsg);
        }
        else {
            // Código de criação de tabela e inserção (mantido como está)
            //const char* sqlDrop = "DROP TABLE IF EXISTS Pets;";
            //rc = sqlite3_exec(db, sqlDrop, 0, 0, &errMsg);
            //if (rc != SQLITE_OK) {
                //MessageBox(hWnd, L"Erro ao dropar tabela!", L"Erro", MB_OK | MB_ICONERROR);
                //sqlite3_free(errMsg);
            //}
            const char* sqlCreate = "CREATE TABLE IF NOT EXISTS Pets (ID INTEGER PRIMARY KEY AUTOINCREMENT, Nome_do_Pet TEXT, Raca TEXT, Cor TEXT, Idade TEXT, Peso TEXT, Sexo TEXT, Castrado TEXT, Date TEXT, Hour TEXT, ID_Tutor_FK INTEGER, FOREIGN KEY (ID_Tutor_FK) REFERENCES Tutores(ID) ON DELETE CASCADE ON UPDATE CASCADE);";
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
                //std::wstring currentDate = PetsSelect_GetCurrentDate();
                //std::wstring currentHour = PetsSelect_GetCurrentHour();
                //for (int i = 1; i <= 100; i++) {
                   //std::wstring sqlInsertW = L"INSERT INTO Pets (Nome_do_Pet, Raca, Cor, Idade, Peso, Sexo, Castrado, Date, Hour, ID_Tutor_FK) VALUES ('Fido', 'Bulldog', 'Preto', 5, 25, 'Masculino', 'Sim', '" + currentDate + L"', '" + currentHour + L"', 1);";
                    //int required = WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    //if (required > 0) {
                        //std::string sqlInsertUtf8(required, '\0');
                        //WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, &sqlInsertUtf8[0], required, nullptr, nullptr);
                        //char* errMsg = nullptr;
                        //int rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);
                        //if (rc != SQLITE_OK) sqlite3_free(errMsg);
                    //}
                //}
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

            PetsSelect_RecarregarDadosTabela(hWnd);

        }
        return 0;
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        HWND hButton = (HWND)lParam; // Handle do botão que disparou o evento
        LONG_PTR id = GetWindowLongPtr(hButton, GWLP_USERDATA); // Recuperar o id do registro

        if (wmId == PetsSelect_CONSULTAR) // Botões "Consultar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Consultar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            PetsSelect_idRecord = id;

            if (!PetsSelect_CreateNewWindow(hWnd, hInst, L"JanelaPetsReadClasse", L"CONSULTAR PET"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaPetsReadClasse", NULL);
                if (hWndRead != NULL)
                {
                    PetsSelect_invalidateDrawing(hWndRead);
                    UpdateWindow(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                }
            }
        }
        else if (wmId == PetsSelect_EDITAR) // Botões "Editar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Editar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            PetsSelect_idRecord = id;

            if (!PetsSelect_CreateNewWindow(hWnd, hInst, L"JanelaPetsEditClasse", L"EDITAR PET"))
            {
                // O erro já é tratado dentro da função
                break;
            }
            else {
                HWND hWndRead = FindWindowW(L"JanelaPetsEditClasse", NULL);
                if (hWndRead != NULL)
                {
                    PetsSelect_invalidateDrawing(hWndRead);
                    UpdateWindow(hWndRead);
                    ShowWindow(hWndRead, SW_MAXIMIZE);
                }
            }
        }
        else if (wmId == PetsSelect_DELETAR) // Botões "Deletar"
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Deletar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            // PetsSelect_idRecord = id;
            std::wstring msg = L"Deletar registro ID " + std::to_wstring(id) + L"?\nAgendamentos cadastrados com esse Pet serão deletados.";
            if (MessageBoxW(hWnd, msg.c_str(), L"Confirmar", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                PetsSelect_deleteRecordById("pet.db", id, hWnd);
                AtualizarJanelas();
            }
        }
        else if (wmId == PetsSelect_FILTRAR)
        {
            //wchar_t msg[50];
            //swprintf_s(msg, L"Botão %s%d clicado!", L"Filtrar", (int)id, (int)id);
            //MessageBoxW(hWnd, msg, L"Info", MB_OK);

            for (int i = 0; i <= 20; i++) {
                std::wstring controlIDStr = std::to_wstring(i);
                HWND input = GetDlgItem(hWnd, i + 20);
                if (i == 7) { // Se o comando veio do nosso ComboBox
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
                        PetsSelect_dados[i] = std::wstring(buffer);

                        // O valor selecionado está em 'buffer' (ex: L"Opção B")
                        // Faça algo com o valor, como atualizar o filtro:
                        // std::wstring valorFiltro = buffer;
                        // aplicarFiltro(valorFiltro);
                    }
                }
                else if (i == 6) {
                    if (input) {
                        LRESULT selectedIndex = 0;

                        // 1. Envia a mensagem CB_GETCURSEL (Get Current Selection) para obter o índice.
                        selectedIndex = SendMessage(
                            input,
                            CB_GETCURSEL,
                            0,
                            0
                        );

                        // Verifica se a seleção é válida (índice 0 ou maior)
                        if (selectedIndex == CB_ERR) // CB_ERR (-1) é retornado se não houver seleção ou erro.
                        {
                            // Se a seleção for inválida ou inexistente, armazena string vazia.
                            PetsSelect_dados[i] = L"";
                            continue;
                        }

                        // 2. Envia a mensagem CB_GETLBTEXTLEN (Get ListBox Text Length) para obter o tamanho do texto.
                        // O resultado é o tamanho da string em caracteres (sem incluir o terminador NULL).
                        LRESULT textLength = SendMessage(
                            input,
                            CB_GETLBTEXTLEN,
                            (WPARAM)selectedIndex, // wParam: o índice do item
                            0                      // lParam: Não usado
                        );

                        if (textLength == CB_ERR)
                        {
                            // Erro ao obter o tamanho do texto.
                            PetsSelect_dados[i] = L"";
                            continue;
                        }

                        // 3. Cria uma std::wstring com o tamanho exato + 1 (para o terminador NULL).
                        // O resize() garante que o buffer interno seja grande o suficiente.
                        std::wstring selectedText;
                        selectedText.resize(textLength + 1); // +1 para o terminador NULL

                        // 4. Envia a mensagem CB_GETLBTEXT (Get ListBox Text) para copiar o texto.
                        // O LPARAM é o ponteiro para o buffer (wchar_t*).
                        LRESULT result = SendMessage(
                            input,
                            CB_GETLBTEXT,
                            (WPARAM)selectedIndex,           // wParam: o índice do item
                            (LPARAM)selectedText.data()      // lParam: Ponteiro para o buffer de caracteres (wchar_t*)
                        );

                        if (result == CB_ERR)
                        {
                            // Erro ao copiar o texto.
                            PetsSelect_dados[i] = L"";
                            continue;
                        }

                        // 5. Finalização: O texto real copiado exclui o terminador NULL, 
                        // então é necessário ajustar o tamanho da wstring.
                        // O 'result' de CB_GETLBTEXT é o número de caracteres copiados (textLength).
                        selectedText.resize(result);

                        // 6. Armazena o texto no seu vetor de dados.
                        PetsSelect_dados[i] = selectedText;
                    }
                }
                else if (input) {
                    wchar_t buffer[256];
                    GetWindowText(input, buffer, 256);
                    PetsSelect_dados[i] = std::wstring(buffer);
                }
            }

            PetsSelect_idNumeroUltimo = 1;
            PetsSelect_offsetTableRow = 1;

            PetsSelect_RecarregarDadosTabela(hWnd);
        }
        else if (wmId == PetsSelect_ORDENAR) // Botões "Ordenar"
        {
            PetsSelect_btnClicado = L"ORDENAR";
            PetsSelect_idBtnGlobal = id;
            PetsSelect_RecarregarDadosTabela(hWnd);
        }
        // Verifica se a mensagem veio do seu ComboBox LIMITAR
        else if (wmId == PetsSelect_LIMITAR) {

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
                PetsSelect_handleLimitChange(hComboBox);

                PetsSelect_RecarregarDadosTabela(hWnd);

                // Certifique-se de retornar 0 após tratar a mensagem
                return 0;
            }
        }
        else if (wmId == PetsSelect_OFFSET) {

            PetsSelect_mudarPagina(id);
            PetsSelect_RecarregarDadosTabela(hWnd);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        PetsSelect_fonte(L"Font", RGB(0, 0, 0), hdc);

        // Obter dimensões da janela
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela
        int columnNumber = 7;
        int cellHeight = 32;
        int numColumns = PetsSelect_g_tableData.empty() ? 0 : 7;
        int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
        int startY = 350 - PetsSelect_g_scrollY;  // Posição Y com scroll
        int startX = 22 - PetsSelect_g_scrollX;  // Posição X com scroll

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
        PetsSelect_windowsTitle(hdc, startX, startY - 330, L"PETS", 4);

        // Desenhar filtros
        PetsSelect_createHeaderFilters(hdc, hWnd);

        //Header Table
        PetsSelect_createHeaderTable(hWnd, hdc);

        int linha = 1;
        int counter = 0;

        // CORREÇÃO: Cálculo seguro do limite
        int limit;
        if (PetsSelect_rowsNumber == 0) {
            limit = 0;
        }
        else {
            limit = min(PetsSelect_offsetTableRow + PetsSelect_limitTableRow, PetsSelect_rowsNumber);
        }

        std::wstring displayText;
        // DESENHAR APENAS UMA VEZ - REMOVER loops desnecessários
        for (size_t row = PetsSelect_offsetTableRow; row < limit && row < PetsSelect_g_tableData.size(); row++) {
            if (row < PetsSelect_g_tableData.size()) {
                HBRUSH hCurrentBrush = (linha % 2 == 0) ? hBrushGray : hBrushWhite;

                if (linha == 0) {
                    hCurrentBrush = hBrushHeader;
                    PetsSelect_fonte(L"Header", RGB(255, 255, 255), hdc);
                }
                else {
                    PetsSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
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
                for (size_t col = 0; col < PetsSelect_g_tableData[row].size(); col++) {
                    if (col == 1) {
                        displayText = PetsSelect_g_tableData[row][12];
                    }
                    else if (col == 0) {
                        displayText = PetsSelect_g_tableData[row][col];
                    }
                    else {
                        displayText = PetsSelect_g_tableData[row][col - 1];
                    }

                    if (col == 0 || col == 1 || col == 2 || col == 3 || col == 4 || col == 5 || col == 6) {
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
        if (!PetsSelect_g_tableData.empty()) {
            PetsSelect_fonte(L"Font", RGB(255, 255, 255), hdc);

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
        PetsSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }

    case WM_DESTROY:
    {
        PetsSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    }
    case WM_SIZE:
    {
        PetsSelect_RecarregarDadosTabela(hWnd);
    }
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE) {
            PetsSelect_g_wasInactive = true; // Marca como inativa quando perde foco para outra janela
        }
        else if (LOWORD(wParam) != WA_INACTIVE && PetsSelect_g_wasInactive) {
            //RecarregarDadosTabela(hWnd);
            //MessageBoxW(hWnd, L"A janela ganhou foco novamente!", L"Aviso", MB_OK | MB_ICONINFORMATION);
            PetsSelect_g_wasInactive = false; // Resetar após exibir o popup
        }
        return 0;
    }
    break;

    case WM_KILLFOCUS:
    {
        PetsSelect_g_wasInactive = true; // Marca como inativa quando perde foco para outro controle
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
            PetsSelect_g_scrollY = si.nPos;

            PetsSelect_RecarregarDadosTabela(hWnd);
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
            PetsSelect_g_scrollY = si.nPos;

            PetsSelect_RecarregarDadosTabela(hWnd);
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

        if (pDraw->CtlID == PetsSelect_OFFSET) {
            RECT rcButton = pDraw->rcItem;

            // 1. PRIMEIRO desenhar a borda
            UINT uState = DFCS_BUTTONPUSH;
            if (pDraw->itemState & ODS_SELECTED) {
                uState |= DFCS_PUSHED;
            }
            DrawFrameControl(pDraw->hDC, &rcButton, DFC_BUTTON, uState);

            // 2. REDUZIR a área interna para não sobrepor a borda
            InflateRect(&rcButton, -2, -2);  // Reduz para dentro

            if (idBotao == PetsSelect_idNumeroUltimo) {
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
LRESULT CALLBACK WndProcSelectPets(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        PetsSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Função obsoleta (removida do WinMain, mas mantida para compatibilidade se necessária)
BOOL InitPetSelect(HINSTANCE hInstance)
{
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProcPetsSelect;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SelectClass";
    return RegisterClassW(&wc);
}