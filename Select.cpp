#include "Select.h"
#include "MenuUniversal.h"
#include <windows.h>
#include <sal.h>
#include "Pet.h"
#include <format>
#include <string>
#include "sqlite3.h"
#include <vector>

std::vector<std::vector<std::wstring>> g_tableData;
std::vector<HWND> g_buttons;

// Definições de ações (usadas para identificar o tipo de botão)
enum ButtonAction { CONSULTAR, EDITAR, DELETAR };

LONG_PTR idRecord;

// Declaração do procedimento da janela
LRESULT CALLBACK WndProcSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Função para obter a data atual como string
std::wstring GetCurrentDate()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t dateStr[80];
    swprintf_s(dateStr, L"%02d/%02d/%04d", st.wDay, st.wMonth, st.wYear );

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

// Função auxiliar para converter de UTF-8 (char*) para std::wstring (UTF-16)
std::wstring utf8_to_wstring(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
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
    wcex.lpszClassName = L"JanelaSelectClasse";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex) != 0;
}

// Procedimento da janela Select
LRESULT CALLBACK WndProcSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ProcessarMenu(hWnd, message, wParam, lParam);

    // Depois processa as mensagens específicas da janela
    switch (message)
    {
    case WM_CREATE:
    {
        // Abrir ou criar o banco de dados
        sqlite3* db;
        char* errMsg = 0;
        int rc = sqlite3_open("pet.db", &db);  // Cria o arquivo pet.db na pasta do exe

        if (rc) {
            MessageBox(hWnd, L"Erro ao abrir/criar o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
            sqlite3_free(errMsg);
        }
        else {
            //MessageBox(hWnd, L"Banco de dados 'pet.db' aberto/criado com sucesso!", L"Sucesso", MB_OK);

            sqlite3* db;
            char* errMsg = 0;
            int rc = sqlite3_open("pet.db", &db);
            if (rc == SQLITE_OK) {
                const char* sqlDrop = "DROP TABLE IF EXISTS Pets;";
                rc = sqlite3_exec(db, sqlDrop, 0, 0, &errMsg);
                if (rc != SQLITE_OK) {
                    MessageBox(hWnd, L"Erro ao dropar tabela!", L"Erro", MB_OK | MB_ICONERROR);
                    sqlite3_free(errMsg);
                }
                else {
                    //MessageBox(hWnd, L"Tabela 'Pets' removida com sucesso!", L"Sucesso", MB_OK);
                }
            }
            else {
                MessageBox(hWnd, L"Erro ao abrir o banco de dados!", L"Erro", MB_OK | MB_ICONERROR);
                sqlite3_free(errMsg);
            }

            // Criar uma tabela
            const char* sqlCreate = "CREATE TABLE IF NOT EXISTS Pets (ID INTEGER PRIMARY KEY AUTOINCREMENT, Nome_do_Pet TEXT, Raca TEXT, Nome_do_Tutor TEXT, CEP TEXT, Cor TEXT, Idade INTEGER, Peso REAL, Sexo TEXT, Castrado TEXT, Endereco TEXT, Ponto_de_referencia TEXT, Banho TEXT, Tosa TEXT, Obs_Tosa TEXT, Parasitas TEXT, Lesoes TEXT, Obs_Lesoes TEXT, Telefone TEXT, CPF TEXT, Date TEXT, Hour TEXT);";
            rc = sqlite3_exec(db, sqlCreate, 0, 0, &errMsg);
            if (rc != SQLITE_OK) {
                wchar_t fullMsg[512] = L"Erro ao criar tabela! Código: ";
                wchar_t codeStr[32];
                swprintf_s(codeStr, L"%d", rc);
                wcscat_s(fullMsg, codeStr);

                // Verificar se errMsg é válido antes de converter
                if (errMsg != NULL) {
                    size_t len = strlen(errMsg) + 1;
                    wchar_t wErrMsg[256];
                    mbstowcs_s(NULL, wErrMsg, len, errMsg, _TRUNCATE);
                    wcscat_s(fullMsg, L" - Detalhes: ");
                    wcscat_s(fullMsg, wErrMsg);
                }
                else {
                    wcscat_s(fullMsg, L" - Sem detalhes adicionais.");
                }

                MessageBox(hWnd, fullMsg, L"Erro", MB_OK | MB_ICONERROR);
                if (errMsg != NULL) {  // Liberar apenas se não for NULL
                    sqlite3_free(errMsg);
                }
            }
            else {
                //MessageBox(hWnd, L"Tabela 'Pets' criada com sucesso!", L"Sucesso", MB_OK);

                // uso
                std::wstring currentDate = GetCurrentDate();
                std::wstring currentHour = GetCurrentHour();

                std::wstring sqlInsertW =
                    L"INSERT INTO Pets (Nome_do_Pet, Raca, Nome_do_Tutor, CEP, Cor, Idade, Peso, Sexo, Castrado, Endereco, Ponto_de_referencia, Banho, Tosa, Obs_Tosa, Parasitas, Lesoes, Obs_Lesoes, Telefone, CPF, Date, Hour) "
                    L"VALUES ('Fido', 'Bulldog', 'Laís', '36309016', 'Preto', 5, 25, 'Masculino', 'Sim', 'Rua das flores - Guarda Mor - São João del Rei', 'Perto da pizzaria Agostinho', 'Padrão', 'Tesoura', 'ir qpiofj adfjs kçjf dkfjeif çsdaf jkasdjf iejf sdçf aksdfjis fdfj çaklsfjaksdfj kdsjfçaejf idsjfkasdf jaies', 'Carrapatos', 'Pele', 'asdf façldj fçkalsdj fdaskljf dsçkf jçklasdf jkaldsfj çkldsa fjaçklds jfakdls fjkalsdjf klasdjf çasdfj çasfj çadsklfjklf ', '32998360862', '09813426632', '"
                    + currentDate + L"', '" + currentHour + L"');";

                // converter para UTF-8
                int required = WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, nullptr, 0, nullptr, nullptr);
                if (required > 0) {
                    std::string sqlInsertUtf8(required, '\0'); // inclui \0
                    WideCharToMultiByte(CP_UTF8, 0, sqlInsertW.c_str(), -1, &sqlInsertUtf8[0], required, nullptr, nullptr);

                    // Executa
                    char* errMsg = nullptr;
                    int rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);
                    if (rc != SQLITE_OK) {
                        // tratar erro (errMsg)
                        sqlite3_free(errMsg);
                    }
                }

                // uso
                std::wstring sqlInsertW2 =
                    L"INSERT INTO Pets (Nome_do_Pet, Raca, Nome_do_Tutor, CEP, Cor, Idade, Peso, Sexo, Castrado, Endereco, Ponto_de_referencia, Banho, Tosa, Obs_Tosa, Parasitas, Lesoes, Obs_Lesoes, Telefone, CPF, Date, Hour) "
                    L"VALUES ('Astralis', 'Viralata', 'Débora', '36309022', 'Preto', 6, 18, 'Feminino', 'Não', 'Rua Ricador Geraldo dos Santos - Alto das Mercês - nº12', 'Perto da igreja das Mercês', 'Padrão', 'Tesoura', 'aa ksfj asldfj açlksdj fkasjd fçklasdjf aksdlfjkalçsdfj kçalsdjf kçlasjd çfkasdj fklçaj sdçlfkjakslfj çlasdjf çasd jfçaskdjfsdf', 'Carrapatos', 'Pele', ' asdfj açlsjf akçslj fkçlasdj fkçlasdjf lçkasjdf kasdjfkiujriwejfç dfkmdnfçnvçaidsjfçkdsfjaçksdjfkaçsjdfkasdjfkçlasjdçfaksdjf', '32998365552', '09813426789', '"
                    + currentDate + L"', '" + currentHour + L"');";

                // converter para UTF-8
                int required2 = WideCharToMultiByte(CP_UTF8, 0, sqlInsertW2.c_str(), -1, nullptr, 0, nullptr, nullptr);
                if (required2 > 0) {
                    std::string sqlInsertUtf8(required2, '\0'); // inclui \0
                    WideCharToMultiByte(CP_UTF8, 0, sqlInsertW2.c_str(), -1, &sqlInsertUtf8[0], required2, nullptr, nullptr);

                    // Executa
                    char* errMsg = nullptr;
                    int rc = sqlite3_exec(db, sqlInsertUtf8.c_str(), nullptr, nullptr, &errMsg);
                    if (rc != SQLITE_OK) {
                        // tratar erro (errMsg)
                        sqlite3_free(errMsg);
                    }
                }
                
                if (rc != SQLITE_OK) {
                    size_t len = strlen(errMsg) + 1;
                    wchar_t wErrMsg[256];
                    mbstowcs_s(NULL, wErrMsg, len, errMsg, _TRUNCATE);
                    wchar_t fullMsg[512];
                    swprintf_s(fullMsg, L"Erro ao inserir dados! Detalhes: %s", wErrMsg);
                    MessageBoxW(hWnd, fullMsg, L"Erro", MB_OK | MB_ICONERROR);
                    sqlite3_free(errMsg); // Liberar após uso
                }
                else {
                    //MessageBox(hWnd, L"Dados inseridos com sucesso!", L"Sucesso", MB_OK);
                }
            }
            sqlite3_close(db);  // Fechar o banco de dados
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        HWND hButton = (HWND)lParam; // Handle do botão que disparou o evento
        LONG_PTR id = GetWindowLongPtr(hButton, GWLP_USERDATA); // Recuperar o id do registro

        if (wmId == CONSULTAR) // Botões "Consultar"
        {
            wchar_t msg[50];
            swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Consultar", (int)id, (int)id);
            MessageBoxW(hWnd, msg, L"Info", MB_OK);

            idRecord = id;

            if (!CreateNewWindow(hWnd, hInst, L"JanelaReadClasse", L"CONSULTA REGISTRO"))
            {
                // O erro já é tratado dentro da função
                break;
            }
        }
        else if (wmId == EDITAR) // Botões "Editar"
        {
            wchar_t msg[50];
            swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Editar", (int)id, (int)id);
            MessageBoxW(hWnd, msg, L"Info", MB_OK);
        }
        else if (wmId == DELETAR) // Botões "Deletar"
        {
            wchar_t msg[50];
            swprintf_s(msg, L"Botão %s%d clicado! Id: %d", L"Deletar", (int)id, (int)id);
            MessageBoxW(hWnd, msg, L"Info", MB_OK);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Consultar o banco apenas se a tabela estiver vazia
        if (g_tableData.empty()) {
            sqlite3* db;
            char* errMsg = 0;
            int rc = sqlite3_open("pet.db", &db);
            if (rc == SQLITE_OK) {
                const char* sqlSelect = "SELECT ID, Nome_do_Pet, Raca, Nome_do_Tutor, Telefone, CPF, Date, Hour FROM Pets;";
                rc = sqlite3_exec(db, sqlSelect, sqlite_callback, &g_tableData, &errMsg);
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
            }
            else {
                g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
            }
        }

        // Obter dimensões da janela
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = (rect.right - rect.left) - 44;
        int height = rect.bottom - rect.top;

        // Configurar a tabela
        int cellHeight = 32;  // Altura de cada célula
        int numColumns = g_tableData.empty() ? 0 : g_tableData[0].size() + 3;  // Número de colunas baseado nos cabeçalhos
        int cellWidth = width / (numColumns > 0 ? numColumns : 1);  // Evitar divisão por zero
        int startY = (height - (g_tableData.size() * cellHeight)) / 2;  // Centralizar verticalmente
        int startX = 22;

        // Desenhar a grade
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));  // Caneta preta
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        //for (int i = 0; i <= static_cast<int>(g_tableData.size()); i++) {  // Linhas horizontais
            //MoveToEx(hdc, startX, startY + i * cellHeight, NULL);
            //LineTo(hdc, startX + width, startY + i * cellHeight);
        //}
        //for (int i = 0; i < numColumns + 1; i++) {  // Linhas verticais, corrigido para numColumns
            //MoveToEx(hdc, startX + i * cellWidth, startY, NULL);
            //LineTo(hdc, startX + i * cellWidth, startY + g_tableData.size() * cellHeight);
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
        for (size_t row = 0; row < g_tableData.size(); row++) {
            HBRUSH hCurrentBrush = (row % 2 == 0) ? hBrushGray : hBrushWhite;

            // Selecionar cor do texto
            COLORREF textColor = RGB(0, 0, 0);
            SetTextColor(hdc, textColor);
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont); // Selecionar a nova fonte
            
            if (row == 0) {
                hCurrentBrush = hBrushHeader;
                COLORREF textColor = RGB(255, 255, 255);
                SetTextColor(hdc, textColor);
                HFONT hOldFont = (HFONT)SelectObject(hdc, hFontHeader); // Selecionar a nova fonte
            }
            
            SelectObject(hdc, hCurrentBrush);

            // Desenhar o fundo da linha
            RECT rowRect = { startX, startY + static_cast<int>(row) * cellHeight,
                            startX + width, startY + (static_cast<int>(row) + 1) * cellHeight };
            FillRect(hdc, &rowRect, hCurrentBrush);

            for (size_t col = 0; col < g_tableData[row].size(); col++) {
                colNumber++;
                int xPos = startX + col * cellWidth + 2;  // Pequeno deslocamento para margem
                int yPos = startY + row * cellHeight + 7;  // Ajuste vertical
                if (g_tableData[row][col] == L"Nome_do_Pet") {
                    TextOut(hdc, xPos, yPos, L"Nome do Pet", static_cast<int>(g_tableData[row][col].length()));
                }
                else if (g_tableData[row][col] == L"Nome_do_Tutor") {
                    TextOut(hdc, xPos, yPos, L"Nome do Tutor", static_cast<int>(g_tableData[row][col].length()));
                }
                else if (g_tableData[row][col] == L"Raca") {
                    TextOut(hdc, xPos, yPos, L"Raça", static_cast<int>(g_tableData[row][col].length()));
                }
                else if (g_tableData[row][col] == L"Date") {
                    TextOut(hdc, xPos, yPos, L"Data", static_cast<int>(g_tableData[row][col].length()));
                }
                else if (g_tableData[row][col] == L"Hour") {
                    TextOut(hdc, xPos, yPos, L"Hora", static_cast<int>(g_tableData[row][col].length()));
                }
                else{
                    TextOut(hdc, xPos, yPos, g_tableData[row][col].c_str(), static_cast<int>(g_tableData[row][col].length()));
                }
            }

            int yPos;
            int xPos;

            if (row == 0) {
                yPos = startY + row * cellHeight + 7;

                xPos = startX + 8 * cellWidth + 2;
                TextOut(hdc, xPos, yPos, L"Consultar", static_cast<int>(9));

                xPos = startX + 9 * cellWidth + 2;
                TextOut(hdc, xPos, yPos, L"Editar", static_cast<int>(6));

                xPos = startX + 10 * cellWidth + 2;
                TextOut(hdc, xPos, yPos, L"Deletar", static_cast<int>(7));
            }
            else {

                // Supondo que g_tableData[row][0] contém o ID do registro (ajuste o índice se for diferente)
                LONG_PTR recordId = _wtoi(g_tableData[row][0].c_str());

                xPos = startX + 8 * cellWidth + 2;
                yPos = startY + row * cellHeight + 2;

                int buttonWidth = 70;
                int buttonHeight = 30;
                HWND hButton = CreateWindowW(
                    L"BUTTON",           // Classe do controle
                    L"Consultar",        // Texto do botão
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    xPos, yPos,          // Posição (x, y)
                    buttonWidth, buttonHeight,              // Largura e altura
                    hWnd,                // Handle da janela pai
                    (HMENU)(CONSULTAR), // ID simples, usado como base
                    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                    NULL
                );

                if (hButton)
                {
                    SetWindowLongPtr(hButton, GWLP_USERDATA, recordId); // Armazenar o ID do registro
                    g_buttons.push_back(hButton); // Armazenar o handle
                }

                // Botão "Editar"
                xPos = startX + 9 * cellWidth + 2;
                hButton = CreateWindowW(
                    L"BUTTON",
                    L"Editar",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    xPos, yPos,
                    buttonWidth, buttonHeight,
                    hWnd,
                    (HMENU)(EDITAR), // ID simples
                    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                    NULL
                );
                if (hButton)
                {
                    SetWindowLongPtr(hButton, GWLP_USERDATA, recordId); // Armazenar o ID do registro
                    g_buttons.push_back(hButton);
                }

                // Botão "Deletar"
                xPos = startX + 10 * cellWidth + 2;
                hButton = CreateWindowW(
                    L"BUTTON",
                    L"Deletar",
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    xPos, yPos,
                    buttonWidth, buttonHeight,
                    hWnd,
                    (HMENU)(DELETAR), // ID simples
                    (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                    NULL
                );
                if (hButton)
                {
                    SetWindowLongPtr(hButton, GWLP_USERDATA, recordId); // Armazenar o ID do registro
                    g_buttons.push_back(hButton);
                }
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