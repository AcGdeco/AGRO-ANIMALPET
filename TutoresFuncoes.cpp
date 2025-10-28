// Pet.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "TutoresFuncoes.h"
#include "TutoresSelect.h"
#include "sqlite3.h"
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <locale.h>
#include "MenuUniversal.h"
#include <regex>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <cwctype>
#define min(a, b) ((a) < (b) ? (a) : (b)) 

int TutoresSelect_g_scrollX;      // Posição horizontal do scroll
int TutoresSelect_g_scrollY;      // Posição vertical do scroll
int TutoresSelect_g_contentWidth;   // Largura total do conteúdo
int TutoresSelect_g_contentHeight;   // Altura total do conteúdo
int TutoresSelect_g_clientWidth;       // Largura da área cliente
int TutoresSelect_g_clientHeight;      // Altura da área cliente
int TutoresSelect_rowsNumber;
int TutoresSelect_limitTableRow = 20;
int TutoresSelect_offsetTableRow = 1;
int TutoresSelect_numeroBtn;
int TutoresSelect_idNumeroUltimo = 1;
int TutoresSelect_rowsNumberSemCabecalho = 0;
LONG_PTR TutoresSelect_idBtnGlobal = 0;
std::wstring TutoresSelect_btnClicado;
int PetsSelect_numberColsTable = 9;

bool TutoresSelect_g_isRedrawing = false;

std::vector<HWND> TutoresSelect_g_editControls; // Array global para armazenar handles dos controles de edição
std::vector<HWND> TutoresSelect_g_editControlsFilters;
std::vector<HWND> TutoresSelect_g_editControlsOrder;
std::vector<HWND> TutoresSelect_g_editControlsLimit;
std::vector<HWND> TutoresSelect_g_editControlsOffsetLimit;
std::vector<std::vector<std::wstring>> TutoresSelect_g_tableData;
std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataFull;
std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataRowsNumber;
LONG_PTR TutoresSelect_idRecord;
std::string TutoresSelect_orderColumn = "ID";
std::string TutoresSelect_orderAscDesc = "DESC";
std::vector<int> TutoresSelect_naoDesenharInternRowsNumber;

std::vector<std::wstring> TutoresSelect_dados(10);
std::wstring TutoresSelect_dataAte;
std::wstring TutoresSelect_dataRegistroAte;

std::vector<HWND> TutoresSelect_g_buttons;

// Substitua o macro por constexpr conforme sugerido pelo VCR101
constexpr int TutoresSelect_MAX_LOADSTRING = 100;

int TutoresSelect_windowsNumber = 1;

// Variáveis Globais:
HWND TutoresSelect_g_hButton = NULL;
HBRUSH TutoresSelect_hBrushTransparent = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

void TutoresSelect_DestroyAllControls()
{
    // Itera sobre todos os HWNDs no contêiner
    for (HWND hWndControl : TutoresSelect_g_editControlsOrder)
    {
        // Verifica se o HWND é válido antes de tentar destruir
        if (IsWindow(hWndControl))
        {
            // A função DestroyWindow envia as mensagens WM_DESTROY e WM_NCDESTROY
            // para o controle. Isso remove o controle da tela e libera sua memória.
            if (DestroyWindow(hWndControl))
            {
                // Opcional: Para depuração
                // std::wcout << L"Controle destruído: " << hWndControl << std::endl;
            }
            else
            {
                // Opcional: Tratar falha na destruição (erro raro)
                // std::wcout << L"Erro ao destruir controle: " << hWndControl << " - Erro: " << GetLastError() << std::endl;
            }
        }
    }

    // 3. LIMPEZA DO CONTÊINER:
    // Após destruir todas as janelas (controles), é crucial limpar a lista de HWNDs
    // para evitar que códigos futuros tentem usar esses handles inválidos.
    TutoresSelect_g_editControlsOrder.clear();

    // Opcional: Se quiser liberar a memória interna do vetor
    // g_editControlsOrder.shrink_to_fit(); 
}

void TutoresSelect_MudarIconeDoBotao(HWND hButton, int novoIconeID) {
    HICON hNewIcon;
    HINSTANCE hInstance;

    // Obtenha a instância do aplicativo
    hInstance = (HINSTANCE)GetWindowLongPtr(hButton, GWLP_HINSTANCE);

    // 1. Carregue o NOVO ícone
    hNewIcon = (HICON)LoadImageW(
        hInstance,
        MAKEINTRESOURCE(novoIconeID), // ID do NOVO recurso (Ex: IDB_NOVA_IMAGEM)
        IMAGE_ICON,                  // Tipo de recurso: Ícone
        0,                           // Largura (0 = Tamanho padrão)
        0,                           // Altura (0 = Tamanho padrão)
        LR_DEFAULTSIZE | LR_SHARED
    );

    // 2. Envie o novo HICON para o botão
    if (hButton && hNewIcon) {
        // Envia o handle do NOVO ícone (hNewIcon) para o botão
        // O Windows gerenciará a substituição do ícone anterior
        HICON hOldIcon = (HICON)SendMessage(
            hButton,
            BM_SETIMAGE,
            (WPARAM)IMAGE_ICON, // Tipo de objeto que está sendo definido (Icon)
            (LPARAM)hNewIcon
        );

        DestroyIcon(hOldIcon);
        DestroyIcon(hNewIcon);
    }
}

void TutoresSelect_mudarPagina(int id) {

    // Variável temporária para a nova página selecionada
    int newPageId = TutoresSelect_idNumeroUltimo;

    // ----------------------------------------------------------------------
    // 1. Lógica do Botão 'Página Anterior' (id == 0)
    // ----------------------------------------------------------------------
    if (id == 0) {
        // Verifica se não estamos na primeira página (Página 1)
        if (TutoresSelect_idNumeroUltimo > 1) {
            newPageId = TutoresSelect_idNumeroUltimo - 1;
        }
    }
    // ----------------------------------------------------------------------
    // 2. Lógica do Botão 'Próxima Página' (id == numeroBtn + 1)
    // ----------------------------------------------------------------------
    else if (id == TutoresSelect_numeroBtn + 1) {
        // Verifica se não estamos na última página
        if (TutoresSelect_idNumeroUltimo < TutoresSelect_numeroBtn) {
            newPageId = TutoresSelect_idNumeroUltimo + 1;
        }
    }
    // ----------------------------------------------------------------------
    // 3. Lógica dos Botões Numéricos (id é o número da página)
    // ----------------------------------------------------------------------
    else {
        // O ID é o próprio número da página clicada.
        // Apenas verifica se o ID está dentro do intervalo válido [1, numeroBtn]
        if (id >= 1 && id <= TutoresSelect_numeroBtn) {
            newPageId = id;
        }
        else {
            // Se o ID for inválido (por segurança), não faz nada
            return;
        }
    }

    // ----------------------------------------------------------------------
    // 4. Atualização e Cálculo Final (Feito uma única vez)
    // ----------------------------------------------------------------------

    // Se a página mudou, atualiza a variável e recalcula o offset
    if (newPageId != TutoresSelect_idNumeroUltimo) {
        TutoresSelect_idNumeroUltimo = newPageId;

        // O offset é sempre o Tamanho da Página * (Página - 1)
        TutoresSelect_offsetTableRow = TutoresSelect_limitTableRow * (TutoresSelect_idNumeroUltimo - 1) + 1;

        // Agora você chamaria a função para recarregar os dados (selectDB) e redesenhar.
        // RecarregarDadosEDesenhar(); 
    }
}

void TutoresSelect_AtualizarPosicoesOffset(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);

    int startYFull = 80 - TutoresSelect_g_scrollY;
    int width = rect.right;
    int yPos = startYFull + 240;

    // --- PARÂMETROS DO BOTÃO ---
    int buttonWidth = 23;
    int buttonHeight = 23;
    int gap = 5; // Use um gap menor para espaçamento entre botões. O valor '25' em seu código parece ser a folga + largura do botão, vamos redefinir.

    // Se o seu 'gap' de 25 representa a distância do início de um botão para o início do próximo, use-o para o cálculo:
    int step = 25; // Distância entre o início de um botão e o início do próximo.

    int numButtons = (int)TutoresSelect_g_editControlsOffsetLimit.size();

    // 1. Calcular a Largura Total Ocupada pelo Grupo
    // A largura total do grupo (do início do 1º ao início do último + largura do último)
    int totalGroupWidth = 0;
    if (numButtons > 0) {
        // Largura Total = (Número de Passos x Distância do Passo) + Largura do último botão
        totalGroupWidth = (numButtons - 1) * step + buttonWidth;
    }

    // 2. Calcular a Posição X de Início (Canto esquerdo do primeiro botão)
    int xStart = (width / 2) - (totalGroupWidth / 2);

    // 3. Posicionar os Botões
    for (int i = 0; i < numButtons; i++) {

        // Posição X do botão atual: Posição de Início + (Índice * Distância)
        int xPos = xStart + i * step;

        // Reposiciona o botão
        // Nota: Removi a duplicação de SetWindowPos (HIDE/SHOW) para simplificar,
        // mas usei SWP_SHOWWINDOW para garantir que seja visível.
        SetWindowPos(
            TutoresSelect_g_editControlsOffsetLimit[i],
            NULL,
            xPos,
            yPos,
            buttonWidth,
            buttonHeight,
            SWP_NOZORDER | SWP_SHOWWINDOW | SWP_HIDEWINDOW
        );

        SetWindowPos(
            TutoresSelect_g_editControlsOffsetLimit[i],
            NULL,
            xPos,
            yPos,
            buttonWidth,
            buttonHeight,
            SWP_NOZORDER | SWP_SHOWWINDOW | SWP_SHOWWINDOW
        );
    }
}

void TutoresSelect_DestroyAllOffsetButtons() {
    // 1. Itera sobre todos os handles (HWND) no vetor
    for (HWND hBtn : TutoresSelect_g_editControlsOffsetLimit) {

        // Verifica se o handle é válido antes de tentar destruir
        if (hBtn != NULL && IsWindow(hBtn)) {

            // 2. Destroi o controle de janela
            DestroyWindow(hBtn);
        }
        // Nota: Se você não está usando C++11 ou superior (range-based for loop),
        // use um iterador tradicional:
        /*
        for (size_t i = 0; i < g_editControlsOffsetLimit.size(); ++i) {
            DestroyWindow(g_editControlsOffsetLimit[i]);
        }
        */
    }

    // 3. Limpa o vetor
    // Isso é essencial para garantir que o vetor esteja vazio e 
    // pronto para armazenar novos handles na próxima criação.
    TutoresSelect_g_editControlsOffsetLimit.clear();
}

void TutoresSelect_createBtnPageLimit(HWND hWnd) {
    TutoresSelect_DestroyAllOffsetButtons();

    if (TutoresSelect_idNumeroUltimo != 1 && TutoresSelect_offsetTableRow > TutoresSelect_rowsNumberSemCabecalho) {
        TutoresSelect_mudarPagina(TutoresSelect_idNumeroUltimo - 1);
    }

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header
    int startYFull = 80 - TutoresSelect_g_scrollY;

    // Variáveis de posicionamento (Usadas para a criação, o reposicionamento final ocorre em AtualizarPosicoesOffset)
    int xPos_start;
    int yPos;
    int selectWidth = 50;
    int width = rect.right;
    yPos = startYFull + 240;
    xPos_start = (width / 2 - selectWidth / 2) + 55; // Posição inicial

    // 1. Calcular o número total de páginas (numeroBtn)
    TutoresSelect_numeroBtn = static_cast<int>(std::ceil(
        static_cast<double>(TutoresSelect_rowsNumberSemCabecalho) / TutoresSelect_limitTableRow
    ));

    // Se não houver páginas para exibir (tabela vazia), encerra
    if (TutoresSelect_numeroBtn < 1) {
        TutoresSelect_AtualizarPosicoesOffset(hWnd);
        return;
    }

    // 2. LÓGICA DE PAGINAÇÃO DINÂMICA (Máximo 10 Botões)
    const int TutoresSelect_MAX_PAGES_DISPLAY = 10;

    // Garantir que a página atual (idNumeroUltimo) esteja no intervalo válido
    if (TutoresSelect_idNumeroUltimo < 1) TutoresSelect_idNumeroUltimo = 1;
    if (TutoresSelect_idNumeroUltimo > TutoresSelect_numeroBtn) TutoresSelect_idNumeroUltimo = TutoresSelect_numeroBtn;

    int start_page, end_page;

    // A. Se o total de páginas for menor ou igual ao limite (10)
    if (TutoresSelect_numeroBtn <= TutoresSelect_MAX_PAGES_DISPLAY) {
        start_page = 1;
        end_page = TutoresSelect_numeroBtn;
    }
    // B. Se o total de páginas for maior que 10
    else {
        // Tentativa de centralizar: 5 antes, 4 depois da página atual
        start_page = TutoresSelect_idNumeroUltimo - 5;
        end_page = TutoresSelect_idNumeroUltimo + 4;

        // B1. Tratamento da borda inicial (garantir que não comece antes de 1)
        if (start_page < 1) {
            start_page = 1;
            end_page = TutoresSelect_MAX_PAGES_DISPLAY;
        }

        // B2. Tratamento da borda final (garantir que não ultrapasse numeroBtn)
        else if (end_page > TutoresSelect_numeroBtn) {
            end_page = TutoresSelect_numeroBtn;
            // Ajusta o início para mostrar os últimos 10 botões
            start_page = TutoresSelect_numeroBtn - TutoresSelect_MAX_PAGES_DISPLAY + 1;
        }
    }

    // 3. CRIAÇÃO DOS BOTÕES

    // Índice para posicionamento sequencial (xPos_start + i * 55)
    int i = 0;

    // Botão 'Voltar' (<) - ID 0
    HWND hBotao = CreateWindowEx(
        0, L"BUTTON", L"<", WS_VISIBLE | WS_CHILD,
        xPos_start + i * 55, yPos, 50, 50,
        hWnd, (HMENU)TutoresSelect_OFFSET,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    SetWindowLongPtr(hBotao, GWLP_USERDATA, 0); // ID 0 para 'Voltar'
    TutoresSelect_g_editControlsOffsetLimit.push_back(hBotao);
    i++; // Próxima posição sequencial

    // Botões dos números das páginas (start_page até end_page)
    for (int page_num = start_page; page_num <= end_page; page_num++) {
        std::wstring wText = std::to_wstring(page_num);

        hBotao = CreateWindowEx(
            0, L"BUTTON", wText.c_str(), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            xPos_start + i * 55, yPos, 50, 50,
            hWnd, (HMENU)TutoresSelect_OFFSET,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );

        // USERDATA é o número da página real
        SetWindowLongPtr(hBotao, GWLP_USERDATA, page_num);
        TutoresSelect_g_editControlsOffsetLimit.push_back(hBotao);
        i++; // Próxima posição sequencial
    }

    // Botão 'Avançar' (>) - ID numeroBtn + 1
    int forward_id = TutoresSelect_numeroBtn + 1;

    hBotao = CreateWindowEx(
        0, L"BUTTON", L">", WS_VISIBLE | WS_CHILD,
        xPos_start + i * 55, yPos, 50, 50,
        hWnd, (HMENU)TutoresSelect_OFFSET,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    SetWindowLongPtr(hBotao, GWLP_USERDATA, forward_id); // ID numeroBtn + 1 para 'Avançar'
    TutoresSelect_g_editControlsOffsetLimit.push_back(hBotao);

    // Reposicionar e Centralizar o Bloco com o novo tamanho (máximo 12 botões)
    TutoresSelect_AtualizarPosicoesOffset(hWnd);
}

void TutoresSelect_handleLimitChange(HWND hComboBox) {
    // 1. Obter o Índice Selecionado
    int selected_index = (int)SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);

    if (selected_index != CB_ERR) {
        // 2. Obter o Valor do Texto (Ex: "30")
        int text_len = (int)SendMessageW(hComboBox, CB_GETLBTEXTLEN, (WPARAM)selected_index, 0);
        std::wstring selected_text(text_len, L'\0');
        SendMessageW(hComboBox, CB_GETLBTEXT, (WPARAM)selected_index, (LPARAM)selected_text.data());
        selected_text.resize(wcslen(selected_text.c_str())); // Limpa possíveis caracteres nulos extras

        // 3. Converte a string para número (integer)
        TutoresSelect_limitTableRow = _wtoi(selected_text.c_str());

        TutoresSelect_offsetTableRow = 1;
        TutoresSelect_idNumeroUltimo = 1;
    }
}

void TutoresSelect_AtualizarPosicoesLimit(HWND hWnd) {
    // 1. Validação de Segurança
    // Garante que o vetor não está vazio e o handle é válido antes de tentar usá-lo.
    if (TutoresSelect_g_editControlsLimit.empty() || !IsWindow(TutoresSelect_g_editControlsLimit[0])) {
        return;
    }

    // Obter o HWND do ComboBox
    HWND hComboBox = TutoresSelect_g_editControlsLimit[0];

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header (A lógica de cálculo está correta)
    int startYFull = 80 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int xPos;
    int yPos;

    int selectWidth = 50;
    int width = rect.right;

    // A posição Y é calculada em relação ao topo da área cliente, ajustada pelo scroll
    yPos = startYFull + 214;
    xPos = (width) / 2 - selectWidth / 2;

    // 2. Uso CORRETO de SetWindowPos
    // SWP_NOMOVE: Não é usado, pois queremos mover (x, y)
    // SWP_NOSIZE: Usado para manter o tamanho (50x50) que foi definido na criação.
    // SWP_NOZORDER: Usado para manter a ordem Z (o controle não vai para frente/trás).
    // SWP_NOACTIVATE: Usado para não ativar o controle.

    SetWindowPos(
        hComboBox,
        NULL,
        xPos,
        yPos,
        0, 0, // Largura e Altura (0, 0) são ignoradas pelo flag SWP_NOSIZE
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
    );

    // Removida a segunda chamada com SWP_SHOWWINDOW, pois o controle deve permanecer visível.
}

void TutoresSelect_createInputLimit(HWND hWnd) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header
    int startYFull = 80 - TutoresSelect_g_scrollY;  // Posição Y com scroll

    int xPos;
    int yPos;

    int selectWidth = 50;
    int width = rect.right;
    yPos = startYFull + 214;
    xPos = (width) / 2 - selectWidth / 2;

    // 1. Defina a largura desejada para a lista suspensa (ex: 300 pixels)
    int desiredDroppedWidth = 170;

    HWND hComboBox = CreateWindowEx(
        0,                                 // Estilos estendidos
        L"ComboBox",                       // Nome da classe do controle ComboBox
        L"",                               // Texto inicial (vazio)
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
        xPos, yPos, selectWidth, 150,
        hWnd,                        // Janela pai
        (HMENU)(TutoresSelect_LIMITAR),                        // ID único do controle (para o WM_COMMAND)
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL
    );

    // Adicionar a opção A
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"10");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"20");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"30");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"40");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"50");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"75");
    // Adicionar a opção B
    SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"100");

    //Selecionar uma opção
    if (TutoresSelect_limitTableRow == 10) {
        SendMessageW(hComboBox, CB_SETCURSEL, 0, 0);
    }
    else if (TutoresSelect_limitTableRow == 20) {
        SendMessageW(hComboBox, CB_SETCURSEL, 1, 0);
    }
    else if (TutoresSelect_limitTableRow == 30) {
        SendMessageW(hComboBox, CB_SETCURSEL, 2, 0);
    }
    else if (TutoresSelect_limitTableRow == 40) {
        SendMessageW(hComboBox, CB_SETCURSEL, 3, 0);
    }
    else if (TutoresSelect_limitTableRow == 50) {
        SendMessageW(hComboBox, CB_SETCURSEL, 4, 0);
    }
    else if (TutoresSelect_limitTableRow == 75) {
        SendMessageW(hComboBox, CB_SETCURSEL, 5, 0);
    }
    else if (TutoresSelect_limitTableRow == 100) {
        SendMessageW(hComboBox, CB_SETCURSEL, 6, 0);
    }

    TutoresSelect_g_editControlsLimit.push_back(hComboBox);
}

void TutoresSelect_AtualizarPosicoesOrder(HWND hWnd) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

    for (int col = 0; col < 7; col++) {
        int xPos = startX + col * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        SetWindowPos(TutoresSelect_g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);

        SetWindowPos(TutoresSelect_g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void TutoresSelect_ordenarMudarIcone(HWND hWnd) {
    // Encontra o HWND do botão (se você não o salvou globalmente)
    HWND hButtonOrdenar = GetDlgItem(hWnd, TutoresSelect_ORDENAR);

    while (hButtonOrdenar) {
        // Verificar se é um botão de ordenar
        if (GetDlgCtrlID(hButtonOrdenar) >= TutoresSelect_ORDENAR &&
            GetDlgCtrlID(hButtonOrdenar) <= TutoresSelect_ORDENAR + 7) {

            // Verificar se é o botão da coluna desejada
            LONG_PTR coluna = GetWindowLongPtr(hButtonOrdenar, GWLP_USERDATA);
            if (coluna == 0 && TutoresSelect_orderAscDesc == "ASC") {
                // Encontrou o botão certo - mudar ícone
                // Mudar para o novo ícone
                TutoresSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS_BAIXO);
            }
            else if (coluna == 0 && TutoresSelect_orderAscDesc == "DESC") {
                TutoresSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS_CIMA);
            }
            else {
                TutoresSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS);
            }
        }
        hButtonOrdenar = GetWindow(hButtonOrdenar, GW_HWNDNEXT);
    }

    for (size_t i = 0; i < TutoresSelect_g_editControlsOrder.size(); i++) {
        if (i == TutoresSelect_idBtnGlobal && TutoresSelect_orderAscDesc == "ASC") {
            HWND hButton = TutoresSelect_g_editControlsOrder[i];
            TutoresSelect_MudarIconeDoBotao(hButton, IDB_SETAS_CIMA);
        }
        else if (i == TutoresSelect_idBtnGlobal && TutoresSelect_orderAscDesc == "DESC") {
            HWND hButton = TutoresSelect_g_editControlsOrder[i];
            TutoresSelect_MudarIconeDoBotao(hButton, IDB_SETAS_BAIXO);
        }
        else {
            HWND hButton = TutoresSelect_g_editControlsOrder[i];
            TutoresSelect_MudarIconeDoBotao(hButton, IDB_SETAS);
        }
    }
}

void TutoresSelect_ordenarDefinicoesValores(HWND hWnd) {
    if (TutoresSelect_btnClicado == L"ORDENAR") {
        std::string TutoresSelect_oldOrderColumn = TutoresSelect_orderColumn;

        switch (TutoresSelect_idBtnGlobal)
        {
        case 0:
            TutoresSelect_orderColumn = "ID";
            break;
        case 1:
            TutoresSelect_orderColumn = "Nome_do_Tutor";
            break;
        case 2:
            TutoresSelect_orderColumn = "CEP";
            break;
        case 3:
            TutoresSelect_orderColumn = "Endereco";
            break;
        case 4:
            TutoresSelect_orderColumn = "Ponto_de_referencia";
            break;
        case 5:
            TutoresSelect_orderColumn = "Telefone";
            break;
        case 6:
            TutoresSelect_orderColumn = "CPF";
            break;
        default:
            break;
        }

        if (TutoresSelect_orderAscDesc == "DESC" && TutoresSelect_orderColumn == TutoresSelect_oldOrderColumn) {
            TutoresSelect_orderAscDesc = "ASC";
        }
        else if (TutoresSelect_orderAscDesc == "ASC" && TutoresSelect_orderColumn == TutoresSelect_oldOrderColumn) {
            TutoresSelect_orderAscDesc = "DESC";
        }
        else if (TutoresSelect_orderColumn != TutoresSelect_oldOrderColumn) {
            if (TutoresSelect_idBtnGlobal == 0) {
                TutoresSelect_orderAscDesc = "DESC";
            }
            else {
                TutoresSelect_orderAscDesc = "ASC";
            }
        }
        TutoresSelect_btnClicado = L"";
    }
}

void TutoresSelect_createOrderBtn(HWND hWnd) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

    for (int i = 0; i < 7; i++) {
        int xPos = startX + i * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        HICON hIcon;
        if (i == 0) {
            // 1. Mude o tipo de HBITMAP para HICON
            hIcon = (HICON)LoadImageW(
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), // Handle da instância
                MAKEINTRESOURCE(IDB_SETAS_CIMA), // ID do recurso (definido no resource.h)
                IMAGE_ICON, // <-- CORREÇÃO: Carregue como Ícone (ICON)
                10, 15,
                LR_SHARED
            );
        }
        else {
            // 1. Mude o tipo de HBITMAP para HICON
            hIcon = (HICON)LoadImageW(
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), // Handle da instância
                MAKEINTRESOURCE(IDB_SETAS), // ID do recurso (definido no resource.h)
                IMAGE_ICON, // <-- CORREÇÃO: Carregue como Ícone (ICON)
                10, 15,
                LR_SHARED
            );
        }

        // 2. Mude o estilo do botão de BS_BITMAP para BS_ICON
        HWND hButton = CreateWindowW(
            L"BUTTON",
            L"", // Remova o texto, pois a imagem será exibida
            WS_VISIBLE | WS_CHILD | BS_ICON, // <-- CORREÇÃO: Use BS_ICON
            xPos - 10, yPos, 10, 15, // Ajuste X, Y, Largura e Altura conforme necessário
            hWnd,
            (HMENU)(TutoresSelect_ORDENAR), // Seu ID de controle (555 é um valor válido)
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

            DestroyIcon(hIcon);
        }

        TutoresSelect_g_editControlsOrder.push_back(hButton);
    }
}

// Função auxiliar para converter a string "dd/mm/aa" para a struct Data
// Retorna um std::optional<Data> (C++17) ou uma struct com valores -1 em caso de erro.
// Aqui, usaremos um retorno com tupla e flag de sucesso para ser compatível com C++ mais antigo.
std::tuple<TutoresSelect_Data, bool> TutoresSelect_parseData(const std::wstring& dataStr) {
    TutoresSelect_Data data = { 0, 0, 0 };
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
bool TutoresSelect_estaEntreDatas(const std::wstring& dataIntervalo1,
    const std::wstring& dataIntervalo2,
    const std::wstring& dataParaTestar) {

    TutoresSelect_Data dataA;
    bool sucessoA;
    std::tie(dataA, sucessoA) = TutoresSelect_parseData(dataIntervalo1);

    TutoresSelect_Data dataB;
    bool sucessoB;
    std::tie(dataB, sucessoB) = TutoresSelect_parseData(dataIntervalo2);

    TutoresSelect_Data dataTeste;
    bool sucessoTeste;
    std::tie(dataTeste, sucessoTeste) = TutoresSelect_parseData(dataParaTestar);

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
        const TutoresSelect_Data& dataInicio = dataA;
        const TutoresSelect_Data& dataFim = dataB;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return (dataInicio <= dataTeste) && (dataTeste <= dataFim);
    }
    else if (!dataIntervalo1.empty() || dataIntervalo2.empty()) {
        const TutoresSelect_Data& dataInicio = dataA;
        const TutoresSelect_Data& dataFim = dataB;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return dataInicio <= dataTeste;
    }
    else if (dataIntervalo1.empty() || !dataIntervalo2.empty()) {
        const TutoresSelect_Data& dataInicio = dataA;
        const TutoresSelect_Data& dataFim = dataB;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return dataTeste <= dataFim;
    }
    else if (!dataIntervalo1.empty() || !dataIntervalo2.empty()) {
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

std::wstring TutoresSelect_arrumarNomesColunas(std::wstring displayText) {
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

void TutoresSelect_AtualizarPosicoesInputs(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumnsFull = 10;
    int cellWidthFull = width / numColumnsFull;
    int cellHeight = 32;
    int startXFull = 22 - TutoresSelect_g_scrollX;
    int startYFull = 80 - TutoresSelect_g_scrollY;
    int row = 1;
    int xPos;
    int yPos;

    for (int col = 0; col < 10; col++) {
        xPos = startXFull + col * cellWidthFull + 10;
        yPos = startYFull + row * cellHeight + 7;
        int colNumber = 7;
        int widthDate = cellWidthFull / 2;

        if (col == 7) {
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 8) {
            xPos = startXFull + (col - 1) * cellWidthFull + 10;

            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 9) {
            xPos = startXFull + (col - 1) * cellWidthFull + 10;

            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else {
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(TutoresSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
    }

    startXFull = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;

    SetWindowPos(TutoresSelect_g_editControlsFilters[10], NULL, xPos, yPos, 70, 30,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
    SetWindowPos(TutoresSelect_g_editControlsFilters[10], NULL, xPos, yPos, 70, 30,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void TutoresSelect_SetFilterValues(const std::vector<std::wstring>& dados) {

    // CORRIGIDO: O vetor 'dados' tem 26 posições.
    size_t dataSize = PetsSelect_numberColsTable;

    // NOTA: Se você ainda tiver o erro 'esperado um identificador', use (std::min)
    // Se o erro foi resolvido com NOMINMAX, use std::min
    size_t limit = (std::min)(TutoresSelect_g_editControlsFilters.size(), dados.size());

    // Usamos dados.size() no min para garantir que não leremos além do vetor 'dados'.

    for (size_t i = 0; i < limit; ++i) {

        HWND hControl = TutoresSelect_g_editControlsFilters[i];
        const std::wstring& value = dados[i];

        // Se o handle for válido
        if (hControl && IsWindow(hControl)) {

            // 1. Verificar o tipo de controle
            WCHAR className[20];
            GetClassNameW(hControl, className, 20);

            if (_wcsicmp(className, L"EDIT") == 0) {
                // 2. É um Edit Control (Input de Texto): Usar SetWindowTextW
                SetWindowTextW(hControl, value.c_str());

            }
            else if (_wcsicmp(className, L"COMBOBOX") == 0) {
                // 3. É um ComboBox (Lista Suspensa): Usar CB_SELECTSTRING
                // Isso irá procurar e selecionar o item que corresponde exatamente ao valor.
                SendMessageW(
                    hControl,
                    CB_SELECTSTRING,
                    (WPARAM)-1, // Iniciar a busca do início
                    (LPARAM)value.c_str()
                );
            }
        }
    }
}

void TutoresSelect_criarInputsFilters(HWND hWnd) {

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int cellHeight = 32;
    int numColumnsFull = 10;
    int cellWidthFull = width / (numColumnsFull > 0 ? numColumnsFull : 1); // +3 para os botões
    int inputWidth = cellWidthFull - 10;

    // Configurar a linha do header
    int startYFull = 80 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

    int xPos;
    int yPos;
    int row = 1;

    for (int col = 0; col < 10; col++) {
        int controlID = col + 20;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + col * cellWidthFull + 10;

        if (col == 7) {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            TutoresSelect_g_editControlsFilters.push_back(hEdit);
        }
        else if (col == 8) {
            HWND hEdit2 = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos + inputWidth / 2, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            TutoresSelect_g_editControlsFilters.push_back(hEdit2);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            
            TutoresSelect_g_editControlsFilters.push_back(hEdit);
        }
    }

    startXFull = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;

    //Criar botão para filtrar
    HWND hButton = CreateWindowW(
        L"BUTTON", L"Filtrar",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        xPos, yPos, 70, 30,
        hWnd, (HMENU)(TutoresSelect_FILTRAR),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    if (hButton) {
        //SetWindowLongPtr(hButton, GWLP_USERDATA, controlID + 1);
        TutoresSelect_g_editControlsFilters.push_back(hButton);
    }
}

void TutoresSelect_criarHeaderLineFilter(HDC hdc, HWND hWnd, int startYFull, int startXFull, int colNumber, int colFinalNumber) {
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
            TutoresSelect_fonte(L"Header", RGB(0, 0, 0), hdc);
        }
        else {
            TutoresSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
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

            std::wstring displayText = TutoresSelect_g_tableDataFull[row][col];

            // Traduzir cabeçalhos se necessário
            if (row == 0) {
                displayText = TutoresSelect_arrumarNomesColunas(displayText);
            }

            TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(displayText.length()));
        }
    }
}

// Callback para processar o resultado do PRAGMA table_info()
int TutoresSelect_pragma_callback(void* data, int argc, char** argv, char** azColName) {
    // 'data' é o ponteiro para o vetor de destino, neste caso, g_tableDataFull.
    auto* tableData = static_cast<std::vector<std::vector<std::wstring>>*>(data);

    // O PRAGMA table_info() retorna 6 colunas por linha (cada linha é uma coluna real da tabela 'Pets').
    // Colunas do PRAGMA: cid, name, type, notnull, dflt_value, pk

    // Se o vetor ainda estiver vazio (o que é esperado ao iniciar),
    // criamos a linha 0 (onde guardaremos os nomes das colunas da tabela 'Pets').
    if (tableData->empty()) {
        tableData->emplace_back(); // Adiciona uma nova linha vazia: tableData[0]
    }

    // A coluna "name" do PRAGMA (o nome da coluna real da tabela 'Pets') está no índice 1 (coluna azColName[1] ou argv[1]).
    if (argc >= 2 && argv[1]) {
        // Converte o nome da coluna (char*) para wstring
        size_t len = strlen(argv[1]) + 1;
        std::wstring wName(len, L'\0');
        mbstowcs_s(nullptr, &wName[0], len, argv[1], _TRUNCATE);
        wName.resize(wcslen(wName.c_str()));

        // Adiciona o nome da coluna à primeira linha (índice 0)
        (*tableData)[0].push_back(wName);
    }

    // Retorna 0 para continuar processando a próxima linha do PRAGMA
    return 0;
}

void TutoresSelect_createHeaderTable(HWND hWnd, HDC hdc) {
    TutoresSelect_selectHeaderDB();

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = TutoresSelect_g_tableDataFull.empty() ? 0 : 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll

    HBRUSH hCurrentBrush;

    HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));

    hCurrentBrush = hBrushHeader;
    TutoresSelect_fonte(L"Header", RGB(255, 255, 255), hdc);

    // Desenhar o fundo da linha
    RECT rowRect = {
        startX,
        startY + static_cast<int>(0) * cellHeight,
        startX + width,
        startY + (static_cast<int>(0) + 1) * cellHeight
    };
    FillRect(hdc, &rowRect, hCurrentBrush);

    int counter = 0;
    for (size_t col = 0; col < TutoresSelect_g_tableDataFull[0].size() - 2; col++) {
        std::wstring displayText = TutoresSelect_g_tableDataFull[0][col];
        int xPos = startX + counter * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        // Traduzir cabeçalhos se necessário
        if (displayText == L"Nome_do_Pet") displayText = L"Nome do Pet";
        else if (displayText == L"Nome_do_Tutor") displayText = L"Nome do Tutor";
        else if (displayText == L"Raca") displayText = L"Raça";
        else if (displayText == L"Appointment_Date") displayText = L"Data";
        else if (displayText == L"Appointment_Hour") displayText = L"Hora";
        else if (displayText == L"Date") displayText = L"Data Registro";
        else if (displayText == L"Hour") displayText = L"Hora Registro";
        else if (displayText == L"Endereco") displayText = L"Endereço";
        else if (displayText == L"Ponto_de_referencia") displayText = L"Referência";

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

void TutoresSelect_selectHeaderDB() {
    TutoresSelect_g_tableDataFull.clear();

    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);

    errMsg = 0;
    const char* sqlPragma = "PRAGMA table_info(Tutores);";

    // 2. Executar o PRAGMA usando o callback que criamos
    rc = sqlite3_exec(
        db,
        sqlPragma,
        TutoresSelect_pragma_callback, // Nosso callback personalizado
        &TutoresSelect_g_tableDataFull, // Passamos o vetor global para o callback
        &errMsg
    );

    if (rc != SQLITE_OK) {
        // Tratar erro (opcional)
        if (errMsg) {
            fprintf(stderr, "SQL error (PRAGMA): %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }
}

void TutoresSelect_createHeaderFilters(HDC hdc, HWND hWnd) {
    TutoresSelect_selectHeaderDB();

    // Configurar a linha do header
    int cellHeight = 32;
    int startYFull = 80 - TutoresSelect_g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - TutoresSelect_g_scrollX;  // Posição X com scroll
    int colNumber = 0;
    int colFinalNumber = colNumber + 9;

    TutoresSelect_criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);
    
}

void TutoresSelect_updateWindow(LPCWSTR className) {
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        TutoresSelect_selectDB();
        TutoresSelect_RecarregarDadosTabela(hSelectWnd);
    }
}

// Função para atualizar posições dos botões com scroll
void TutoresSelect_AtualizarPosicoesBotoes(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = TutoresSelect_g_tableData.empty() ? 0 : 7 + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 350;
    int cellHeight = 32;

    // Desabilitar redesenho durante a atualização
    SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

    for (size_t i = 0; i < TutoresSelect_g_buttons.size(); i++) {
        size_t row = (i / 3) + 1;
        int buttonType = i % 3;

        int yPos = startY + row * cellHeight + 2 - TutoresSelect_g_scrollY;
        int xPos = startX + (7 + buttonType) * cellWidth + 2;

        // Verificar se o botão está visível na área da janela
        BOOL isVisible = (yPos >= -cellHeight && yPos <= rect.bottom);

        if (isVisible) {
            SetWindowPos(TutoresSelect_g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        else {
            // Esconder botões que estão fora da área visível
            SetWindowPos(TutoresSelect_g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
        }
    }

    // Reabilitar redesenho e forçar atualização
    SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
}

bool TutoresSelect_deleteRecordById(const std::string& databasePath, int id, HWND hWnd) {
    sqlite3* db;
    char* errMsg = 0;

    // Abrir conexão com o banco
    int rc = sqlite3_open(databasePath.c_str(), &db);
    if (rc != SQLITE_OK) {
        MessageBoxW(hWnd, L"Erro ao abrir banco de dados", L"Erro", MB_ICONERROR);
        return false;
    }

    // Preparar a query SQL
    std::string sql = "DELETE FROM Tutores WHERE ID = " + std::to_string(id) + ";";

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

void TutoresSelect_selectDB() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    TutoresSelect_g_tableData.clear();

    // Consultar o banco apenas se a tabela estiver vazia
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        std::string sqlSelect;
        std::string sqlSelectCount;
        if (TutoresSelect_orderColumn == "Appointment_Hour") {

            // Expressão para converter 'H:MM' ou 'HH:MM' para o formato ordenável 'HH:MM'
            std::string hourSorting =
                // 1. Pega a hora (antes do ':'), converte para INT, e preenche com zero (ex: '4' -> '04')
                "PRINTF('%02d', CAST(SUBSTR(Appointment_Hour, 1, INSTR(Appointment_Hour, ':') - 1) AS INTEGER))"
                // 2. Concatena com os minutos (incluindo o ':')
                " || SUBSTR(Appointment_Hour, INSTR(Appointment_Hour, ':'))";

            sqlSelect = "SELECT * FROM Tutores ORDER BY " + hourSorting + " " + TutoresSelect_orderAscDesc;
        }
        else if (TutoresSelect_orderColumn == "Appointment_Date") {

            // Define a string de ordenação complexa para a data DD/MM/YYYY
            std::string dataSorting =
                "SUBSTR(Appointment_Date, 7, 4) || SUBSTR(Appointment_Date, 4, 2) || SUBSTR(Appointment_Date, 1, 2)";
            sqlSelect = "SELECT * FROM Tutores ORDER BY " + dataSorting + " " + TutoresSelect_orderAscDesc;

        }
        else {
            //const char* sqlSelect = "SELECT ID, Nome_do_Pet, Nome_do_Tutor, Banho, Tosa, Appointment_Date, Appointment_Hour FROM Pets;";
            sqlSelect = "SELECT * FROM Tutores ORDER BY " + TutoresSelect_orderColumn + " COLLATE NOCASE " + TutoresSelect_orderAscDesc;
        }

        //std::string limitClause = " LIMIT " + std::to_string(limitTableRow) + " OFFSET " + std::to_string(offsetTableRow);
        //sqlSelect = sqlSelect + limitClause + ";";

        rc = sqlite3_exec(db, sqlSelect.c_str(), TutoresSelect_sqlite_callback, &TutoresSelect_g_tableData, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                TutoresSelect_g_tableData.push_back({ L"Erro", wErrMsg });
            }
            else {
                TutoresSelect_g_tableData.push_back({ L"Erro", L"Desconhecido" });
            }
            if (errMsg) sqlite3_free(errMsg);
        }
        sqlite3_close(db);
        TutoresSelect_naoDesenhar.resize(TutoresSelect_g_tableData.size());
    }
    else {
        TutoresSelect_g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

std::wstring TutoresSelect_toLower(std::wstring str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](wchar_t c) {
            return std::towlower(c);
        });
    return str;
}

void TutoresSelect_verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern) {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    TutoresSelect_g_tableDataRowsNumber.clear();

    // CORREÇÃO: Verificar se g_tableData não está vazio antes de redimensionar
    if (!TutoresSelect_g_tableData.empty()) {
        TutoresSelect_g_tableDataRowsNumber.resize(TutoresSelect_g_tableData.size());
        for (size_t row = 0; row < TutoresSelect_g_tableData.size(); ++row) {
            TutoresSelect_g_tableDataRowsNumber[row].resize(TutoresSelect_g_tableData[row].size());
        }
    }

    std::wstring filtro;
    std::wstring dadoTable;

    // CORREÇÃO: Redimensionar naoDesenharIntern para o tamanho correto
    if (naoDesenharIntern.size() != TutoresSelect_g_tableData.size()) {
        naoDesenharIntern.resize(TutoresSelect_g_tableData.size(), 0);
    }

    int column;
    int numeroColIteracoes = PetsSelect_numberColsTable;
    for (size_t row = 0; row < TutoresSelect_g_tableData.size(); row++) {

        // CORREÇÃO: Verificar se o índice é válido
        if (row >= naoDesenharIntern.size()) {
            naoDesenharIntern.resize(row + 1, 0);
        }

        naoDesenharIntern[row] = 0;

        // CORREÇÃO: Pular linha 0 (cabeçalho) se necessário
        if (row == 0) continue; // Mantém o cabeçalho

        for (size_t col = 0; col < numeroColIteracoes; col++) {
            if (col == 21) {
                column = 20;
            }
            else if (col == 22) {
                column = 21;
            }
            else if (col == 23 || col == 24) {
                column = 22;
            }
            else if (col == 25) {
                column = 23;
            }
            else {
                column = col;
            }

            dadoTable = TutoresSelect_g_tableData[row][column];

            std::wstring displayText = dadoTable;

            if (!dados[col].empty() && (col == 0)) {
                filtro = dados[col];
                if (filtro != dadoTable) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
            else if (!dados[col].empty() && (col == 7 || col == 8)) {

                // CORREÇÃO: Verificar se a coluna existe na linha atual
                if (col == 8) {
                    dadoTable = TutoresSelect_g_tableData[row][col - 1];

                }

                bool estaEntre = TutoresSelect_estaEntreDatas(dados[7], dados[8], dadoTable);
                if (!estaEntre) {
                    if (col == 8 && dados[7].empty()) {
                        naoDesenharIntern[row] = 1;
                        break;
                    }
                    else {
                        naoDesenharIntern[row] = 1;
                        break;
                    }
                }
            }
            else if (!dados[col].empty()) {
                if (TutoresSelect_toLower(dadoTable).find(TutoresSelect_toLower(dados[col])) == std::wstring::npos) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
        }
    }

    // 1. Limpeza e Inicialização de Contadores
    TutoresSelect_rowsNumber = 0;
    int TutoresSelect_rowCount = 0;

    // CRUCIAL: Limpar o vetor de destino para garantir que ele comece vazio
    TutoresSelect_g_tableDataRowsNumber.clear();

    // 2. Pré-alocação (Opcional, mas melhora o desempenho)
    if (!TutoresSelect_g_tableData.empty()) {
        TutoresSelect_g_tableDataRowsNumber.reserve(TutoresSelect_g_tableData.size());
    }

    // 3. Iteração, Filtro e Construção do Novo Vetor
    for (size_t row = 0; row < TutoresSelect_g_tableData.size(); row++) { // Começar da linha 1 (pular cabeçalho)

        // A. Lógica do Filtro: Se a linha for válida (flag != 1)
        if (row < naoDesenharIntern.size() && naoDesenharIntern[row] != 1) {

            // B. Cria e preenche uma nova linha com todas as colunas da linha original
            std::vector<std::wstring> newRow;

            // Copia todas as colunas (você não está filtrando colunas, apenas linhas)
            for (size_t col = 0; col < TutoresSelect_g_tableData[row].size(); col++) {
                newRow.push_back(TutoresSelect_g_tableData[row][col]);
            }

            // C. Adiciona a linha válida (newRow) ao vetor de destino
            TutoresSelect_g_tableDataRowsNumber.push_back(newRow);

            // D. Atualiza o contador de linhas válidas
            TutoresSelect_rowCount++;
            TutoresSelect_rowsNumber++;
        }
        // Linhas com naoDesenharIntern[row] == 1 são automaticamente "removidas"
    }

    // CORREÇÃO CRÍTICA: Garantir que rowsNumber seja pelo menos 1 (cabeçalho)
    if (TutoresSelect_rowsNumber == 0 && !TutoresSelect_g_tableDataRowsNumber.empty()) {
        TutoresSelect_rowsNumber = static_cast<int>(TutoresSelect_g_tableDataRowsNumber.size());
    }
    if (TutoresSelect_rowsNumber == 0 && !TutoresSelect_g_tableData.empty()) {
        TutoresSelect_rowsNumber = 1; // Pelo menos o cabeçalho
    }

    // 4. ATRIBUIÇÃO E REDIMENSIONAMENTO
    TutoresSelect_g_tableData.clear();

    // CORREÇÃO: Só mover se houver dados
    if (!TutoresSelect_g_tableDataRowsNumber.empty()) {
        TutoresSelect_g_tableData = std::move(TutoresSelect_g_tableDataRowsNumber);
    }
    else {
        // Se não há dados filtrados, manter pelo menos o cabeçalho se existir
        if (!TutoresSelect_g_tableData.empty()) {
            // Manter apenas o cabeçalho
            std::vector<std::vector<std::wstring>> temp;
            temp.push_back(TutoresSelect_g_tableData[0]);
            TutoresSelect_g_tableData = std::move(temp);
            TutoresSelect_rowsNumber = 1;
        }
    }

    // CORREÇÃO: Garantir que naoDesenhar tenha o tamanho correto
    if (TutoresSelect_naoDesenhar.size() != TutoresSelect_g_tableData.size()) {
        TutoresSelect_naoDesenhar.resize(TutoresSelect_g_tableData.size(), 0);
    }

    // CORREÇÃO: Copiar apenas os valores relevantes
    for (size_t i = 1; i < naoDesenharIntern.size() && i < TutoresSelect_naoDesenhar.size(); i++) {
        TutoresSelect_naoDesenhar[i] = naoDesenharIntern[i];
    }

    TutoresSelect_rowsNumberSemCabecalho = TutoresSelect_rowsNumber - 1;
}

void TutoresSelect_CriarBotoesTabela(HWND hWnd)
{
    // Limpar botões existentes
    for (HWND hButton : TutoresSelect_g_buttons) {
        DestroyWindow(hButton);
    }
    TutoresSelect_g_buttons.clear();

    if (TutoresSelect_g_tableData.size() <= 1) return;

    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = TutoresSelect_g_tableData.empty() ? 0 : TutoresSelect_g_tableData[0].size() + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 80;
    int cellHeight = 32;

    int limit;
    limit = TutoresSelect_offsetTableRow + TutoresSelect_limitTableRow;

    if (limit < TutoresSelect_rowsNumberSemCabecalho) {
        limit = TutoresSelect_offsetTableRow + TutoresSelect_limitTableRow;
    }
    else {
        limit = TutoresSelect_rowsNumberSemCabecalho + 1;
    }

    int inicio;
    if (TutoresSelect_offsetTableRow == 0) {
        inicio = 1;
    }
    else {
        inicio = TutoresSelect_offsetTableRow;
    }

    for (size_t row = inicio; row < limit; row++) {
        LONG_PTR recordId = _wtoi(TutoresSelect_g_tableData[row][0].c_str());
        int yPos = startY + row * cellHeight + 2;

        // Botão Consultar
        int xPos = startX + 7 * cellWidth + 2;
        HWND hButton = CreateWindowW(
            L"BUTTON", L"Consultar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(TutoresSelect_CONSULTAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            TutoresSelect_g_buttons.push_back(hButton);
        }

        // Botão Editar
        xPos = startX + 8 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Editar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(TutoresSelect_EDITAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            TutoresSelect_g_buttons.push_back(hButton);
        }

        // Botão Deletar
        xPos = startX + 9 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Deletar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(TutoresSelect_DELETAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            TutoresSelect_g_buttons.push_back(hButton);
        }
    }
}

// Função para configurar scroll bars
void TutoresSelect_ConfigurarScrollBars(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    TutoresSelect_g_clientHeight = rect.bottom - rect.top;

    int cellHeight = 32;
    int filtersHeight = 6 * cellHeight + 6 * 7;

    //offsetTableRow = limitTableRow
    int limit;
    int numeroDeLinhas = TutoresSelect_limitTableRow;

    limit = TutoresSelect_offsetTableRow + TutoresSelect_limitTableRow;

    if (limit > TutoresSelect_rowsNumberSemCabecalho) {
        numeroDeLinhas = TutoresSelect_rowsNumber - TutoresSelect_offsetTableRow;
    }

    TutoresSelect_g_contentHeight = static_cast<int>(numeroDeLinhas) * cellHeight + 160 + filtersHeight;

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentHeight;
    si.nPage = TutoresSelect_g_clientHeight;
    si.nPos = TutoresSelect_g_scrollY;

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

void TutoresSelect_invalidateDrawing(HWND hWnd) {
    BlockInput(TRUE);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    BlockInput(FALSE);
}

void TutoresSelect_DestroyControlsFromVector(std::vector<HWND>& controls) {
    for (HWND hControl : controls) {
        if (hControl != NULL && IsWindow(hControl)) {
            DestroyWindow(hControl);
        }
    }
    controls.clear();
}

// Função para recarregar dados do banco
void TutoresSelect_RecarregarDadosTabela(HWND hWnd) {
    // Assim, o Windows não redesenha a janela a cada SetWindowPos, apenas uma vez no final — mais rápido e visualmente limpo.
    SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

    // Destruir na ordem inversa da criação (mais seguro)
    TutoresSelect_DestroyControlsFromVector(TutoresSelect_g_buttons);
    TutoresSelect_DestroyControlsFromVector(TutoresSelect_g_editControlsOffsetLimit);
    TutoresSelect_DestroyControlsFromVector(TutoresSelect_g_editControlsLimit);
    TutoresSelect_DestroyControlsFromVector(TutoresSelect_g_editControlsOrder);
    TutoresSelect_DestroyControlsFromVector(TutoresSelect_g_editControlsFilters);

    // Limpeza extra para garantir
    TutoresSelect_g_buttons.clear();
    TutoresSelect_g_editControlsOffsetLimit.clear();
    TutoresSelect_g_editControlsLimit.clear();
    TutoresSelect_g_editControlsOrder.clear();
    TutoresSelect_g_editControlsFilters.clear();

    // Definir valores para ordenação e imagem
    TutoresSelect_ordenarDefinicoesValores(hWnd);

    // Limpar dados antigos
    TutoresSelect_selectDB();

    // Criar inputs de limite
    TutoresSelect_createInputLimit(hWnd);

    TutoresSelect_naoDesenhar.resize(TutoresSelect_g_tableData.size());
    //Verificar filtros
    TutoresSelect_verificarFiltro(TutoresSelect_dados, TutoresSelect_naoDesenhar);

    // Criar botões após carregar os dados
    TutoresSelect_CriarBotoesTabela(hWnd);

    // Criar botões de paginação
    TutoresSelect_createBtnPageLimit(hWnd);

    // Criar inputs de filtros
    TutoresSelect_criarInputsFilters(hWnd);

    // Definir valores dos filtros
    TutoresSelect_SetFilterValues(TutoresSelect_dados);

    // Criar inputs de order
    TutoresSelect_createOrderBtn(hWnd);

    //Atualizar posição dos botões
    TutoresSelect_AtualizarPosicoesBotoes(hWnd);

    //Atualizar inputs
    TutoresSelect_AtualizarPosicoesInputs(hWnd);

    // Atualizar offset botões
    TutoresSelect_AtualizarPosicoesOffset(hWnd);

    // Atualizar limite de linhas
    TutoresSelect_AtualizarPosicoesLimit(hWnd);

    // Atualizar botões order
    TutoresSelect_AtualizarPosicoesOrder(hWnd);

    // Mudar ícone do botão de ordenamento
    TutoresSelect_ordenarMudarIcone(hWnd);

    // Reconfigurar scroll bars NÃO PODE TIRAR ISSO DAQUI, NESSA ORDEM SE NÃO O INPUT QUE ESCOLHE O Nº DE LINHAS DA TABELA VAI BUGAR
    TutoresSelect_ConfigurarScrollBars(hWnd);

    // Assim, o Windows não redesenha a janela a cada SetWindowPos, apenas uma vez no final — mais rápido e visualmente limpo.
    SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);

    // Forçar redesenho da janela
    TutoresSelect_invalidateDrawing(hWnd);
}

void TutoresSelect_checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData) {

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

int TutoresSelect_sqlite_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? TutoresSelect_utf8_to_wstring(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? TutoresSelect_utf8_to_wstring(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Função para atualizar posição dos controles com scroll
void TutoresSelect_AtualizarPosicoesControlesAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
    int startY = 40 - TutoresSelect_g_scrollY;
    int startX = 22 - TutoresSelect_g_scrollX;
    int xPos = 0;
    int yPos = 0;
    int colNumber;
    int countRow = 0;

    // Atualizar posição dos campos de entrada
    for (size_t i = 0; i < TutoresSelect_g_editControls.size(); i++) {
        colNumber = countRow + 1;

        xPos = startX + cellWidth + 10;
        yPos = startY + colNumber * cellHeight + 3;
        SetWindowPos(TutoresSelect_g_editControls[i], NULL, xPos, yPos, 700, 25,
            SWP_NOZORDER | SWP_NOACTIVATE);
        countRow++;
    }

    // Atualizar posição do botão
    if (TutoresSelect_g_hButton) {
        int buttonY = startY + 7 * cellHeight + 3;
        SetWindowPos(TutoresSelect_g_hButton, NULL, startX, buttonY, 150, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Função para configurar scroll bars
void TutoresSelect_ConfigurarScrollBarsAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    TutoresSelect_g_clientHeight = rect.bottom - rect.top;
    TutoresSelect_g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo (19 linhas + título + botão)
    int cellHeight = 32;
    TutoresSelect_g_contentHeight = 22 * cellHeight + 100; // 19 campos + título + botão + margem
    TutoresSelect_g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentHeight;
    si.nPage = TutoresSelect_g_clientHeight;
    si.nPos = TutoresSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentWidth;
    si.nPage = TutoresSelect_g_clientWidth;
    si.nPos = TutoresSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para obter a data atual como string
std::wstring TutoresSelect_GetCurrentDate()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t dateStr[80];
    swprintf_s(dateStr, L"%02d/%02d/%04d", st.wDay, st.wMonth, st.wYear);

    return std::wstring(dateStr);
}

// Função para obter a hora atual como string
std::wstring TutoresSelect_GetCurrentHour()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t timeStr[80];
    swprintf_s(timeStr, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

    return std::wstring(timeStr);
}

bool TutoresSelect_isNumber(const std::wstring& str) {
    if (str.empty()) return false;

    for (wchar_t c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Versão que permite números decimais
bool TutoresSelect_isDecimalNumber(const std::wstring& str) {
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

bool TutoresSelect_isValidTime(const std::wstring& time) {
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

bool TutoresSelect_isValidDate(const std::wstring& date) {
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

LPCWSTR TutoresSelect_error = L"0";
std::wstring TutoresSelect_mensagem = L"";
LPCWSTR TutoresSelect_msg = L"";

std::wstring TutoresSelect_treatDataAppointment(std::wstring dado, int number) {
    std::wstring dado_escaped = dado;
    size_t pos = 0;

    if (number == 2 && dado.empty()) {
        TutoresSelect_error = L"1";
        TutoresSelect_mensagem = L"Insira: 'Nome do Tutor'.\n" + TutoresSelect_mensagem;
    }
    else if (number == 3 && !dado.empty()) {
        if (!TutoresSelect_isNumber(dado)) {
            TutoresSelect_error = L"1";
            TutoresSelect_mensagem = L"Insira: Apenas números em 'CEP'.\n" + TutoresSelect_mensagem;
        }
    }
    else if (number == 6 && !dado.empty()) {
        if (!TutoresSelect_isNumber(dado)) {
            TutoresSelect_error = L"1";
            TutoresSelect_mensagem = L"Insira: Apenas números em 'Telefone'.\n" + TutoresSelect_mensagem;
        }
    }
    else if (number == 7 && !dado.empty()) {
        if (!TutoresSelect_isNumber(dado)) {
            TutoresSelect_error = L"1";
            TutoresSelect_mensagem = L"Insira: Apenas números em 'CPF'.\n" + TutoresSelect_mensagem;
        }
    }
    else if (dado.empty()) {
        dado_escaped = L"";
    }

    while ((pos = dado_escaped.find(L"'", pos)) != std::wstring::npos) {
        dado_escaped.replace(pos, 1, L"''");
        pos += 2;
    }

    TutoresSelect_msg = TutoresSelect_mensagem.c_str();

    return dado_escaped;
}

// Converter std::wstring para std::string UTF-8
std::string TutoresSelect_WideToUTF8(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Converter std::string UTF-8 para std::wstring
std::wstring TutoresSelect_UTF8ToWide(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

BOOL TutoresSelect_fonte(LPCWSTR fonte, COLORREF color, HDC hdc) {
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

BOOL TutoresSelect_windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size) {
    TutoresSelect_fonte(L"Title", RGB(0, 0, 0), hdc);
    TextOut(hdc, startX, startY, tit, size);

    return 0;
}

BOOL TutoresSelect_scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight)
{
    // Variáveis de scroll
    TutoresSelect_g_scrollX = scrollX;      // Posição horizontal do scroll
    TutoresSelect_g_scrollY = scrollY;      // Posição vertical do scroll
    TutoresSelect_g_contentWidth = contentWidth;   // Largura total do conteúdo
    TutoresSelect_g_contentHeight = contentHeight;   // Altura total do conteúdo
    TutoresSelect_g_clientWidth = clientWidth;       // Largura da área cliente
    TutoresSelect_g_clientHeight = clientHeight;      // Altura da área cliente

    // Obter dimensões da área cliente
    RECT rect;
    GetClientRect(hWnd, &rect);
    TutoresSelect_g_clientWidth = rect.right - rect.left;
    TutoresSelect_g_clientHeight = rect.bottom - rect.top;

    // Configurar scroll bars
    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentHeight;
    si.nPage = TutoresSelect_g_clientHeight;
    si.nPos = TutoresSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = TutoresSelect_g_contentWidth;
    si.nPage = TutoresSelect_g_clientWidth;
    si.nPos = TutoresSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

    return 0;
}

BOOL TutoresSelect_windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL TutoresSelect_Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

#ifndef VK_Q
#define VK_Q 0x51
#endif

#ifndef VK_W
#define VK_W 0x57
#endif

#ifndef VK_E
#define VK_E 0x45
#endif

#ifndef VK_R
#define VK_R 0x52
#endif

#ifndef VK_T
#define VK_T 0x54
#endif

#ifndef VK_Y
#define VK_Y 0x59
#endif

#ifndef VK_U
#define VK_U 0x55
#endif

#ifndef VK_I
#define VK_I 0x49
#endif

#ifndef VK_A
#define VK_A 0x41
#endif

#ifndef VK_S
#define VK_S 0x53
#endif

#ifndef VK_D
#define VK_D 0x44
#endif

#ifndef VK_F
#define VK_F 0x46
#endif

#ifndef VK_Z
#define VK_Z 0x5A
#endif

#ifndef VK_X
#define VK_X 0x58
#endif

#ifndef VK_C
#define VK_C 0x43
#endif

#ifndef VK_V
#define VK_V 0x56
#endif

#ifndef VK_P
#define VK_P 0x50 // Código virtual para a tecla 'P'
#endif

#ifndef VK_G
#define VK_G 0x47 // Código virtual para a tecla 'G'
#endif

    if (GetKeyState(VK_CONTROL) & 0x8000) // Verifica se Ctrl está pressionado
    {
        switch (wParam)
        {
        case VK_P: SendMessage(hWnd, WM_COMMAND, IDM_HOME_INICIO, 0); break;
        case VK_G: SendMessage(hWnd, WM_COMMAND, IDM_AJUDA_SOBRE, 0); break;

        case VK_Q: SendMessage(hWnd, WM_COMMAND, IDM_TUTORES_NOVO, 0); break;
        case VK_W: SendMessage(hWnd, WM_COMMAND, IDM_TUTORES_CONSULTAR, 0); break;
        case VK_E: SendMessage(hWnd, WM_COMMAND, IDM_TUTORES_CONSULTAR, 0); break;
        case VK_R: SendMessage(hWnd, WM_COMMAND, IDM_TUTORES_CONSULTAR, 0); break;

        case VK_T: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_NOVO, 0); break;
        case VK_Y: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_U: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_I: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;

            // --- NOVAS TECLAS (ASDF) ---
        case VK_A: SendMessage(hWnd, WM_COMMAND, IDM_PETS_NOVO, 0); break;
        case VK_S: SendMessage(hWnd, WM_COMMAND, IDM_PETS_CONSULTAR, 0); break;
        case VK_D: SendMessage(hWnd, WM_COMMAND, IDM_PETS_CONSULTAR, 0); break;
        case VK_F: SendMessage(hWnd, WM_COMMAND, IDM_PETS_CONSULTAR, 0); break;

            // --- NOVAS TECLAS (ZXCV) ---
        case VK_Z: SendMessage(hWnd, WM_COMMAND, IDM_AGENDAMENTOS_NOVO, 0); break;
        case VK_X: SendMessage(hWnd, WM_COMMAND, IDM_AGENDAMENTOS_CONSULTAR, 0); break;
        case VK_C: SendMessage(hWnd, WM_COMMAND, IDM_AGENDAMENTOS_CONSULTAR, 0); break;
        case VK_V: SendMessage(hWnd, WM_COMMAND, IDM_AGENDAMENTOS_CONSULTAR, 0); break;
        }
    }

    return 0;
}

BOOL TutoresSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
BOOL TutoresSelect_CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle)
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
        wc.lpfnWndProc = WndProcTutoresSelect;

        // Criar a janela Nova
        HWND hNewWnd = CreateWindowW(
            className,
            windowTittle,
            WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL | WS_EX_CONTROLPARENT,
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
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK TutoresSelect_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (TutoresSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
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
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        if (TutoresSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        if (TutoresSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        TutoresSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    case WM_DESTROY:
        TutoresSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK TutoresSelect_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
