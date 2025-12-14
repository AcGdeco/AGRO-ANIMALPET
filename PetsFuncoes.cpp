// Pet.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "PetsFuncoes.h"
#include "PetsAdd.h"
#include "PetsSelect.h"
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

int PetsSelect_g_scrollX;      // Posição horizontal do scroll
int PetsSelect_g_scrollY;      // Posição vertical do scroll
int PetsSelect_g_contentWidth;   // Largura total do conteúdo
int PetsSelect_g_contentHeight;   // Altura total do conteúdo
int PetsSelect_g_clientWidth;       // Largura da área cliente
int PetsSelect_g_clientHeight;      // Altura da área cliente
int PetsSelect_rowsNumber;
int PetsSelect_limitTableRow = 20;
int PetsSelect_offsetTableRow = 1;
int PetsSelect_numeroBtn;
int PetsSelect_idNumeroUltimo = 1;
int PetsSelect_rowsNumberSemCabecalho = 0;
LONG_PTR PetsSelect_idBtnGlobal = 0;
std::wstring PetsSelect_btnClicado;
int PetsSelect_numberColsTable = 11;

bool PetsSelect_g_isRedrawing = false;

std::vector<HWND> PetsSelect_g_editControls; // Array global para armazenar handles dos controles de edição
std::vector<HWND> PetsSelect_g_editControlsFilters;
std::vector<HWND> PetsSelect_g_editControlsOrder;
std::vector<HWND> PetsSelect_g_editControlsLimit;
std::vector<HWND> PetsSelect_g_editControlsOffsetLimit;
std::vector<std::vector<std::wstring>> PetsSelect_g_tableData;
std::vector<std::vector<std::wstring>> PetsSelect_g_tableDataFull;
std::vector<std::vector<std::wstring>> PetsSelect_g_tableDataRowsNumber;
LONG_PTR PetsSelect_idRecord;
std::string PetsSelect_orderColumn = "ID";
std::string PetsSelect_orderAscDesc = "DESC";
std::vector<int> PetsSelect_naoDesenharInternRowsNumber;

std::vector<std::wstring> PetsSelect_dados(22);
std::wstring PetsSelect_dataAte;
std::wstring PetsSelect_dataRegistroAte;

std::vector<HWND> PetsSelect_g_buttons;

// Substitua o macro por constexpr conforme sugerido pelo VCR101
constexpr int PetsSelect_MAX_LOADSTRING = 100;

int PetsSelect_windowsNumber = 1;

// Variáveis Globais:
HWND PetsSelect_g_hButton = NULL;
HBRUSH PetsSelect_hBrushTransparent = (HBRUSH)GetStockObject(HOLLOW_BRUSH);

void PetsSelect_DestroyAllControls()
{
    // Itera sobre todos os HWNDs no contêiner
    for (HWND hWndControl : PetsSelect_g_editControlsOrder)
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
    PetsSelect_g_editControlsOrder.clear();

    // Opcional: Se quiser liberar a memória interna do vetor
    // g_editControlsOrder.shrink_to_fit(); 
}

void PetsSelect_MudarIconeDoBotao(HWND hButton, int novoIconeID) {
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

void PetsSelect_mudarPagina(int id) {

    // Variável temporária para a nova página selecionada
    int newPageId = PetsSelect_idNumeroUltimo;

    // ----------------------------------------------------------------------
    // 1. Lógica do Botão 'Página Anterior' (id == 0)
    // ----------------------------------------------------------------------
    if (id == 0) {
        // Verifica se não estamos na primeira página (Página 1)
        if (PetsSelect_idNumeroUltimo > 1) {
            newPageId = PetsSelect_idNumeroUltimo - 1;
        }
    }
    // ----------------------------------------------------------------------
    // 2. Lógica do Botão 'Próxima Página' (id == numeroBtn + 1)
    // ----------------------------------------------------------------------
    else if (id == PetsSelect_numeroBtn + 1) {
        // Verifica se não estamos na última página
        if (PetsSelect_idNumeroUltimo < PetsSelect_numeroBtn) {
            newPageId = PetsSelect_idNumeroUltimo + 1;
        }
    }
    // ----------------------------------------------------------------------
    // 3. Lógica dos Botões Numéricos (id é o número da página)
    // ----------------------------------------------------------------------
    else {
        // O ID é o próprio número da página clicada.
        // Apenas verifica se o ID está dentro do intervalo válido [1, numeroBtn]
        if (id >= 1 && id <= PetsSelect_numeroBtn) {
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
    if (newPageId != PetsSelect_idNumeroUltimo) {
        PetsSelect_idNumeroUltimo = newPageId;

        // O offset é sempre o Tamanho da Página * (Página - 1)
        PetsSelect_offsetTableRow = PetsSelect_limitTableRow * (PetsSelect_idNumeroUltimo - 1) + 1;

        // Agora você chamaria a função para recarregar os dados (selectDB) e redesenhar.
        // RecarregarDadosEDesenhar(); 
    }
}

void PetsSelect_AtualizarPosicoesOffset(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);

    int startYFull = 80 - PetsSelect_g_scrollY;
    int width = rect.right;
    int yPos = startYFull + 240;

    // --- PARÂMETROS DO BOTÃO ---
    int buttonWidth = 23;
    int buttonHeight = 23;
    int gap = 5; // Use um gap menor para espaçamento entre botões. O valor '25' em seu código parece ser a folga + largura do botão, vamos redefinir.

    // Se o seu 'gap' de 25 representa a distância do início de um botão para o início do próximo, use-o para o cálculo:
    int step = 25; // Distância entre o início de um botão e o início do próximo.

    int numButtons = (int)PetsSelect_g_editControlsOffsetLimit.size();

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
            PetsSelect_g_editControlsOffsetLimit[i],
            NULL,
            xPos,
            yPos,
            buttonWidth,
            buttonHeight,
            SWP_NOZORDER | SWP_SHOWWINDOW | SWP_HIDEWINDOW
        );

        SetWindowPos(
            PetsSelect_g_editControlsOffsetLimit[i],
            NULL,
            xPos,
            yPos,
            buttonWidth,
            buttonHeight,
            SWP_NOZORDER | SWP_SHOWWINDOW | SWP_SHOWWINDOW
        );
    }
}

void PetsSelect_DestroyAllOffsetButtons() {
    // 1. Itera sobre todos os handles (HWND) no vetor
    for (HWND hBtn : PetsSelect_g_editControlsOffsetLimit) {

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
    PetsSelect_g_editControlsOffsetLimit.clear();
}

void PetsSelect_createBtnPageLimit(HWND hWnd) {
    PetsSelect_DestroyAllOffsetButtons();

    if (PetsSelect_idNumeroUltimo != 1 && PetsSelect_offsetTableRow > PetsSelect_rowsNumberSemCabecalho) {
        PetsSelect_mudarPagina(PetsSelect_idNumeroUltimo - 1);
    }

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header
    int startYFull = 80 - PetsSelect_g_scrollY;

    // Variáveis de posicionamento (Usadas para a criação, o reposicionamento final ocorre em AtualizarPosicoesOffset)
    int xPos_start;
    int yPos;
    int selectWidth = 50;
    int width = rect.right;
    yPos = startYFull + 240;
    xPos_start = (width / 2 - selectWidth / 2) + 55; // Posição inicial

    // 1. Calcular o número total de páginas (numeroBtn)
    PetsSelect_numeroBtn = static_cast<int>(std::ceil(
        static_cast<double>(PetsSelect_rowsNumberSemCabecalho) / PetsSelect_limitTableRow
    ));

    // Se não houver páginas para exibir (tabela vazia), encerra
    if (PetsSelect_numeroBtn < 1) {
        PetsSelect_AtualizarPosicoesOffset(hWnd);
        return;
    }

    // 2. LÓGICA DE PAGINAÇÃO DINÂMICA (Máximo 10 Botões)
    const int PetsSelect_MAX_PAGES_DISPLAY = 10;

    // Garantir que a página atual (idNumeroUltimo) esteja no intervalo válido
    if (PetsSelect_idNumeroUltimo < 1) PetsSelect_idNumeroUltimo = 1;
    if (PetsSelect_idNumeroUltimo > PetsSelect_numeroBtn) PetsSelect_idNumeroUltimo = PetsSelect_numeroBtn;

    int start_page, end_page;

    // A. Se o total de páginas for menor ou igual ao limite (10)
    if (PetsSelect_numeroBtn <= PetsSelect_MAX_PAGES_DISPLAY) {
        start_page = 1;
        end_page = PetsSelect_numeroBtn;
    }
    // B. Se o total de páginas for maior que 10
    else {
        // Tentativa de centralizar: 5 antes, 4 depois da página atual
        start_page = PetsSelect_idNumeroUltimo - 5;
        end_page = PetsSelect_idNumeroUltimo + 4;

        // B1. Tratamento da borda inicial (garantir que não comece antes de 1)
        if (start_page < 1) {
            start_page = 1;
            end_page = PetsSelect_MAX_PAGES_DISPLAY;
        }

        // B2. Tratamento da borda final (garantir que não ultrapasse numeroBtn)
        else if (end_page > PetsSelect_numeroBtn) {
            end_page = PetsSelect_numeroBtn;
            // Ajusta o início para mostrar os últimos 10 botões
            start_page = PetsSelect_numeroBtn - PetsSelect_MAX_PAGES_DISPLAY + 1;
        }
    }

    // 3. CRIAÇÃO DOS BOTÕES

    // Índice para posicionamento sequencial (xPos_start + i * 55)
    int i = 0;

    // Botão 'Voltar' (<) - ID 0
    HWND hBotao = CreateWindowEx(
        0, L"BUTTON", L"<", WS_VISIBLE | WS_CHILD,
        xPos_start + i * 55, yPos, 50, 50,
        hWnd, (HMENU)PetsSelect_OFFSET,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    SetWindowLongPtr(hBotao, GWLP_USERDATA, 0); // ID 0 para 'Voltar'
    PetsSelect_g_editControlsOffsetLimit.push_back(hBotao);
    i++; // Próxima posição sequencial

    // Botões dos números das páginas (start_page até end_page)
    for (int page_num = start_page; page_num <= end_page; page_num++) {
        std::wstring wText = std::to_wstring(page_num);

        hBotao = CreateWindowEx(
            0, L"BUTTON", wText.c_str(), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            xPos_start + i * 55, yPos, 50, 50,
            hWnd, (HMENU)PetsSelect_OFFSET,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );

        // USERDATA é o número da página real
        SetWindowLongPtr(hBotao, GWLP_USERDATA, page_num);
        PetsSelect_g_editControlsOffsetLimit.push_back(hBotao);
        i++; // Próxima posição sequencial
    }

    // Botão 'Avançar' (>) - ID numeroBtn + 1
    int forward_id = PetsSelect_numeroBtn + 1;

    hBotao = CreateWindowEx(
        0, L"BUTTON", L">", WS_VISIBLE | WS_CHILD,
        xPos_start + i * 55, yPos, 50, 50,
        hWnd, (HMENU)PetsSelect_OFFSET,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    SetWindowLongPtr(hBotao, GWLP_USERDATA, forward_id); // ID numeroBtn + 1 para 'Avançar'
    PetsSelect_g_editControlsOffsetLimit.push_back(hBotao);

    // Reposicionar e Centralizar o Bloco com o novo tamanho (máximo 12 botões)
    PetsSelect_AtualizarPosicoesOffset(hWnd);
}

void PetsSelect_handleLimitChange(HWND hComboBox) {
    // 1. Obter o Índice Selecionado
    int selected_index = (int)SendMessageW(hComboBox, CB_GETCURSEL, 0, 0);

    if (selected_index != CB_ERR) {
        // 2. Obter o Valor do Texto (Ex: "30")
        int text_len = (int)SendMessageW(hComboBox, CB_GETLBTEXTLEN, (WPARAM)selected_index, 0);
        std::wstring selected_text(text_len, L'\0');
        SendMessageW(hComboBox, CB_GETLBTEXT, (WPARAM)selected_index, (LPARAM)selected_text.data());
        selected_text.resize(wcslen(selected_text.c_str())); // Limpa possíveis caracteres nulos extras

        // 3. Converte a string para número (integer)
        PetsSelect_limitTableRow = _wtoi(selected_text.c_str());

        PetsSelect_offsetTableRow = 1;
        PetsSelect_idNumeroUltimo = 1;
    }
}

void PetsSelect_AtualizarPosicoesLimit(HWND hWnd) {
    // 1. Validação de Segurança
    // Garante que o vetor não está vazio e o handle é válido antes de tentar usá-lo.
    if (PetsSelect_g_editControlsLimit.empty() || !IsWindow(PetsSelect_g_editControlsLimit[0])) {
        return;
    }

    // Obter o HWND do ComboBox
    HWND hComboBox = PetsSelect_g_editControlsLimit[0];

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header (A lógica de cálculo está correta)
    int startYFull = 80 - PetsSelect_g_scrollY;  // Posição Y com scroll
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

void PetsSelect_createInputLimit(HWND hWnd) {
    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Configurar a linha do header
    int startYFull = 80 - PetsSelect_g_scrollY;  // Posição Y com scroll

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
        (HMENU)(PetsSelect_LIMITAR),                        // ID único do controle (para o WM_COMMAND)
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
    if (PetsSelect_limitTableRow == 10) {
        SendMessageW(hComboBox, CB_SETCURSEL, 0, 0);
    }
    else if (PetsSelect_limitTableRow == 20) {
        SendMessageW(hComboBox, CB_SETCURSEL, 1, 0);
    }
    else if (PetsSelect_limitTableRow == 30) {
        SendMessageW(hComboBox, CB_SETCURSEL, 2, 0);
    }
    else if (PetsSelect_limitTableRow == 40) {
        SendMessageW(hComboBox, CB_SETCURSEL, 3, 0);
    }
    else if (PetsSelect_limitTableRow == 50) {
        SendMessageW(hComboBox, CB_SETCURSEL, 4, 0);
    }
    else if (PetsSelect_limitTableRow == 75) {
        SendMessageW(hComboBox, CB_SETCURSEL, 5, 0);
    }
    else if (PetsSelect_limitTableRow == 100) {
        SendMessageW(hComboBox, CB_SETCURSEL, 6, 0);
    }

    PetsSelect_g_editControlsLimit.push_back(hComboBox);
}

void PetsSelect_AtualizarPosicoesOrder(HWND hWnd) {
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
    int startY = 350 - PetsSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - PetsSelect_g_scrollX;  // Posição X com scroll

    for (int col = 0; col < 7; col++) {
        int xPos = startX + col * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        SetWindowPos(PetsSelect_g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);

        SetWindowPos(PetsSelect_g_editControlsOrder[col], NULL, xPos - 10, yPos, 0, 0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    }
}

void PetsSelect_ordenarMudarIcone(HWND hWnd) {
    // Encontra o HWND do botão (se você não o salvou globalmente)
    HWND hButtonOrdenar = GetDlgItem(hWnd, PetsSelect_ORDENAR);

    while (hButtonOrdenar) {
        // Verificar se é um botão de ordenar
        if (GetDlgCtrlID(hButtonOrdenar) >= PetsSelect_ORDENAR &&
            GetDlgCtrlID(hButtonOrdenar) <= PetsSelect_ORDENAR + 7) {

            // Verificar se é o botão da coluna desejada
            LONG_PTR coluna = GetWindowLongPtr(hButtonOrdenar, GWLP_USERDATA);
            if (coluna == 0 && PetsSelect_orderAscDesc == "ASC") {
                // Encontrou o botão certo - mudar ícone
                // Mudar para o novo ícone
                PetsSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS_BAIXO);
            }
            else if (coluna == 0 && PetsSelect_orderAscDesc == "DESC") {
                PetsSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS_CIMA);
            }
            else {
                PetsSelect_MudarIconeDoBotao(hButtonOrdenar, IDB_SETAS);
            }
        }
        hButtonOrdenar = GetWindow(hButtonOrdenar, GW_HWNDNEXT);
    }

    for (size_t i = 0; i < PetsSelect_g_editControlsOrder.size(); i++) {
        if (i == PetsSelect_idBtnGlobal && PetsSelect_orderAscDesc == "ASC") {
            HWND hButton = PetsSelect_g_editControlsOrder[i];
            PetsSelect_MudarIconeDoBotao(hButton, IDB_SETAS_CIMA);
        }
        else if (i == PetsSelect_idBtnGlobal && PetsSelect_orderAscDesc == "DESC") {
            HWND hButton = PetsSelect_g_editControlsOrder[i];
            PetsSelect_MudarIconeDoBotao(hButton, IDB_SETAS_BAIXO);
        }
        else {
            HWND hButton = PetsSelect_g_editControlsOrder[i];
            PetsSelect_MudarIconeDoBotao(hButton, IDB_SETAS);
        }
    }
}

void PetsSelect_ordenarDefinicoesValores(HWND hWnd) {
    if (PetsSelect_btnClicado == L"ORDENAR") {
        std::string PetsSelect_oldOrderColumn = PetsSelect_orderColumn;

        switch (PetsSelect_idBtnGlobal)
        {
        case 0:
            PetsSelect_orderColumn = "P.ID";
            break;
        case 1:
            PetsSelect_orderColumn = "Nome_do_Tutor";
            break;
        case 2:
            PetsSelect_orderColumn = "Nome_do_Pet";
            break;
        case 3:
            PetsSelect_orderColumn = "Raca";
            break;
        case 4:
            PetsSelect_orderColumn = "Cor";
            break;
        case 5:
            PetsSelect_orderColumn = "Idade";
            break;
        case 6:
            PetsSelect_orderColumn = "Peso";
            break;
        default:
            break;
        }

        if (PetsSelect_orderAscDesc == "DESC" && PetsSelect_orderColumn == PetsSelect_oldOrderColumn) {
            PetsSelect_orderAscDesc = "ASC";
        }
        else if (PetsSelect_orderAscDesc == "ASC" && PetsSelect_orderColumn == PetsSelect_oldOrderColumn) {
            PetsSelect_orderAscDesc = "DESC";
        }
        else if (PetsSelect_orderColumn != PetsSelect_oldOrderColumn) {
            if (PetsSelect_idBtnGlobal == 0) {
                PetsSelect_orderAscDesc = "DESC";
            }
            else {
                PetsSelect_orderAscDesc = "ASC";
            }
        }
        PetsSelect_btnClicado = L"";
    }
}

void PetsSelect_createOrderBtn(HWND hWnd) {
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
    int startY = 350 - PetsSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - PetsSelect_g_scrollX;  // Posição X com scroll

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
            (HMENU)(PetsSelect_ORDENAR), // Seu ID de controle (555 é um valor válido)
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

        PetsSelect_g_editControlsOrder.push_back(hButton);
    }
}

// Função auxiliar para converter a string "dd/mm/aa" para a struct Data
// Retorna um std::optional<Data> (C++17) ou uma struct com valores -1 em caso de erro.
// Aqui, usaremos um retorno com tupla e flag de sucesso para ser compatível com C++ mais antigo.
std::tuple<PetsSelect_Data, bool> PetsSelect_parseData(const std::wstring& dataStr) {
    PetsSelect_Data data = { 0, 0, 0 };
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
bool PetsSelect_estaEntreDatas(const std::wstring& dataIntervalo1,
    const std::wstring& dataIntervalo2,
    const std::wstring& dataParaTestar) {

    PetsSelect_Data dataA;
    bool sucessoA;
    std::tie(dataA, sucessoA) = PetsSelect_parseData(dataIntervalo1);

    PetsSelect_Data dataB;
    bool sucessoB;
    std::tie(dataB, sucessoB) = PetsSelect_parseData(dataIntervalo2);

    PetsSelect_Data dataTeste;
    bool sucessoTeste;
    std::tie(dataTeste, sucessoTeste) = PetsSelect_parseData(dataParaTestar);

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
        const PetsSelect_Data& dataInicio = dataA;
        const PetsSelect_Data& dataFim = dataB;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return (dataInicio <= dataTeste) && (dataTeste <= dataFim);
    }
    else if (!dataIntervalo1.empty() || dataIntervalo2.empty()) {
        const PetsSelect_Data& dataInicio = dataA;
        const PetsSelect_Data& dataFim = dataB;

        // 2. Verifica se a dataTeste está depois/igual ao início E antes/igual ao fim.
        // dataInicio <= dataTeste   => A data de teste é posterior ou igual ao início
        // dataTeste <= dataFim      => A data de teste é anterior ou igual ao fim
        return dataInicio <= dataTeste;
    }
    else if (dataIntervalo1.empty() || !dataIntervalo2.empty()) {
        const PetsSelect_Data& dataInicio = dataA;
        const PetsSelect_Data& dataFim = dataB;

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

std::wstring PetsSelect_arrumarNomesColunas(std::wstring displayText) {
    if (displayText == L"Nome_do_Pet") displayText = L"Nome Pet";
    else if (displayText == L"ID") displayText = L"ID Pet";
    else if (displayText == L"Nome_do_Tutor") displayText = L"Nome Tutor";
    else if (displayText == L"Raca") displayText = L"Raça";
    else if (displayText == L"Appointment_Date") displayText = L"Data de-até";
    else if (displayText == L"Appointment_Hour") displayText = L"Hora";
    else if (displayText == L"Date") displayText = L"Data R de-até";
    else if (displayText == L"Hour") displayText = L"Hora Reg.";
    else if (displayText == L"Ponto_de_referencia") displayText = L"Ponto de Ref.";
    else if (displayText == L"Obs_Tosa") displayText = L"Observação";
    else if (displayText == L"Lesoes") displayText = L"Lesões";
    else if (displayText == L"Obs_Lesoes") displayText = L"Observação";
    else if (displayText == L"Obs_Lesoes") displayText = L"Observação";
    else if (displayText == L"Endereco") displayText = L"Endereço";
    else if (displayText == L"ID_Tutor_FK") displayText = L"ID Tutor";

    return displayText;
}

void PetsSelect_AtualizarPosicoesInputs(HWND hWnd) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumnsFull = 10;
    int cellWidthFull = width / numColumnsFull;
    int cellHeight = 32;
    int startXFull = 22 - PetsSelect_g_scrollX;
    int startYFull = 80 - PetsSelect_g_scrollY;
    int row = 1;
    int xPos;
    int yPos;
    int widthDate = cellWidthFull / 2;

    for (int col = 0; col < 11; col++) {
        xPos = startXFull + col * cellWidthFull + 10;
        yPos = startYFull + row * cellHeight + 7;

        if (col == 8) {
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 9) {
            xPos = startXFull + (col - 1) * cellWidthFull + 10;

            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 10) {
            xPos = startXFull + (col - 1) * cellWidthFull + 10;

            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 6) {
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 200,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 200,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else {
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    int colNumber = 10;
    int colFinalNumber = colNumber + 12;

    for (int col = 11; col < colFinalNumber; col++) {
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - 1 - colNumber) * cellWidthFull + 10;

        if (col == 19) {
            xPos = startXFull + (col - 1 - 10) * cellWidthFull + 10;

            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 20) {
            xPos = startXFull + (col - 2 - 10) * cellWidthFull + 10;

            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos + widthDate, yPos, widthDate, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else if (col == 21) {
            xPos = startXFull + (col - 2 - 10) * cellWidthFull + 10;

            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        else {
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
            SetWindowPos(PetsSelect_g_editControlsFilters[col], NULL, xPos, yPos, cellWidthFull - 15, 25,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
    }

    startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;

    SetWindowPos(PetsSelect_g_editControlsFilters[colFinalNumber], NULL, xPos, yPos, 70, 30,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW);
    SetWindowPos(PetsSelect_g_editControlsFilters[colFinalNumber], NULL, xPos, yPos, 70, 30,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void PetsSelect_SetFilterValues(const std::vector<std::wstring>& dados) {

    // CORRIGIDO: O vetor 'dados' tem 26 posições.
    size_t dataSize = PetsSelect_numberColsTable;

    // NOTA: Se você ainda tiver o erro 'esperado um identificador', use (std::min)
    // Se o erro foi resolvido com NOMINMAX, use std::min
    size_t limit = (std::min)(PetsSelect_g_editControlsFilters.size(), dados.size());

    // Usamos dados.size() no min para garantir que não leremos além do vetor 'dados'.

    for (size_t i = 0; i < limit; ++i) {

        HWND hControl = PetsSelect_g_editControlsFilters[i];
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

void PetsSelect_criarInputsFilters(HWND hWnd) {

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
    int startYFull = 80 - PetsSelect_g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll

    int xPos;
    int yPos;
    int row = 1;

    for (int col = 0; col < 11; col++) {
        int controlID = col + 20;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + col * cellWidthFull + 10;

        if (col == 7) {
            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, inputWidth, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Sim");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Não");

            PetsSelect_g_editControlsFilters.push_back(hComboBox);
        }
        else if (col == 8) {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            PetsSelect_g_editControlsFilters.push_back(hEdit);
        }
        else if (col == 9) {
            HWND hEdit2 = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos + inputWidth / 2, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            PetsSelect_g_editControlsFilters.push_back(hEdit2);
        }
        else if (col == 6) {
            HWND hComboBox = CreateWindowW(
                L"COMBOBOX",                         // Classe do controle: MUDAR de "BUTTON" para "COMBOBOX"
                NULL,                                // Texto: NULL para ComboBox
                WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos IMPORTANTES
                xPos, yPos,                          // Posição X, Y
                inputWidth, 200,                            // Largura, Altura (A altura precisa ser maior para exibir a lista)
                hWnd,                                // Janela pai
                (HMENU)(controlID),                  // ID único
                NULL,                                // Instância
                NULL
            );

            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Macho");
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Fêmea");

            PetsSelect_g_editControlsFilters.push_back(hComboBox);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            PetsSelect_g_editControlsFilters.push_back(hEdit);
        }
    }

    // Configurar a linha do input
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    int colNumber = 11;
    int colFinalNumber = 22;

    for (int col = colNumber; col < colFinalNumber; col++) {
        int controlID = col + 20;
        yPos = startYFull + row * cellHeight + 7;
        xPos = startXFull + (col - 10) * cellWidthFull + 10;

        if (col == 19) {
            xPos = startXFull + (col - 1 - 10) * cellWidthFull + 10;

            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            PetsSelect_g_editControlsFilters.push_back(hEdit);
        }
        else if (col == 20) {
            xPos = startXFull + (col - 1 - 10) * cellWidthFull + 10;

            HWND hEdit2 = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos + inputWidth / 2, yPos, inputWidth / 2, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            PetsSelect_g_editControlsFilters.push_back(hEdit2);
        }
        else if (col == 21) {
            xPos = startXFull + (col - 1 - 10) * cellWidthFull + 10;

            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );

            PetsSelect_g_editControlsFilters.push_back(hEdit);
        }
        else if (col == 18) {
            HWND hComboBox = CreateWindowEx(
                0,                                 // Estilos estendidos
                L"ComboBox",                       // Nome da classe do controle ComboBox
                L"",                               // Texto inicial (vazio)
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, // Estilos: Filho, Visível, e lista suspensa que não pode ser editada (SELECT)
                xPos, yPos, inputWidth, 150,
                hWnd,                        // Janela pai
                (HMENU)(controlID),                        // ID único do controle (para o WM_COMMAND)
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                NULL
            );

            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
            // Adicionar a opção B
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Quinzenal");
            // Adicionar a opção A
            SendMessageW(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"Mensal");

            PetsSelect_g_editControlsFilters.push_back(hComboBox);
        }
        else {
            HWND hEdit = CreateWindowEx(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                xPos, yPos, inputWidth, 25, hWnd, (HMENU)(controlID), NULL, NULL
            );
            PetsSelect_g_editControlsFilters.push_back(hEdit);
        }
    }

    startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    xPos = startXFull;
    yPos = startYFull;

    //Criar botão para filtrar
    HWND hButton = CreateWindowW(
        L"BUTTON", L"Filtrar",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
        xPos, yPos, 70, 30,
        hWnd, (HMENU)(PetsSelect_FILTRAR),
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
    );
    if (hButton) {
        //SetWindowLongPtr(hButton, GWLP_USERDATA, controlID + 1);
        PetsSelect_g_editControlsFilters.push_back(hButton);
    }
}

void PetsSelect_criarHeaderLineFilter(HDC hdc, HWND hWnd, int startYFull, int startXFull, int colNumber, int colFinalNumber) {
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
            PetsSelect_fonte(L"Header", RGB(0, 0, 0), hdc);
        }
        else {
            PetsSelect_fonte(L"Font", RGB(0, 0, 0), hdc);
        }

        // Desenhar o fundo da linha
        RECT rowRect = {
            startXFull,
            startYFull + static_cast<int>(row) * cellHeight,
            startXFull + width,
            startYFull + (static_cast<int>(row) + 1) * cellHeight
        };
        FillRect(hdc, &rowRect, hCurrentBrush);

        std::wstring displayText;
        // Desenhar as células de dados
        for (size_t col = colNumber; col < colFinalNumber; col++) {
            int xPos;
            int yPos;
            yPos = startYFull + row * cellHeight + 7;
            xPos = startXFull + (col - colNumber) * cellWidthFull + 10;

            if (col > 10 && col < 20) {
                displayText = PetsSelect_g_tableDataFull[row][col + 1];
            }
            else {
                displayText = PetsSelect_g_tableDataFull[row][col];
            }

            // Traduzir cabeçalhos se necessário
            if (row == 0) {
                displayText = PetsSelect_arrumarNomesColunas(displayText);
            }

            TextOut(hdc, xPos, yPos, displayText.c_str(), static_cast<int>(displayText.length()));
        }
    }
}

// Callback para processar o resultado do PRAGMA table_info()
int PetsSelect_pragma_callback(void* data, int argc, char** argv, char** azColName) {
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

void PetsSelect_createHeaderTable(HWND hWnd, HDC hdc) {
    PetsSelect_selectHeaderDB();

    // Obter dimensões da janela
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int height = rect.bottom - rect.top;

    // Configurar a tabela
    int columnNumber = 7;
    int cellHeight = 32;
    int numColumns = PetsSelect_g_tableDataFull.empty() ? 0 : 7;
    int cellWidth = width / (numColumns > 0 ? numColumns + 3 : 1); // +3 para os botões
    int startY = 350 - PetsSelect_g_scrollY;  // Posição Y com scroll
    int startX = 22 - PetsSelect_g_scrollX;  // Posição X com scroll

    HBRUSH hCurrentBrush;

    HBRUSH hBrushHeader = CreateSolidBrush(RGB(150, 150, 150));

    hCurrentBrush = hBrushHeader;
    PetsSelect_fonte(L"Header", RGB(255, 255, 255), hdc);

    // Desenhar o fundo da linha
    RECT rowRect = {
        startX,
        startY + static_cast<int>(0) * cellHeight,
        startX + width,
        startY + (static_cast<int>(0) + 1) * cellHeight
    };
    FillRect(hdc, &rowRect, hCurrentBrush);

    int counter = 0;
    std::wstring displayText;
    for (size_t col = 0; col < PetsSelect_g_tableDataFull[0].size(); col++) {
        if (col == 1) {
            displayText = PetsSelect_g_tableDataFull[0][12];
        }
        else if (col == 0){
            displayText = PetsSelect_g_tableDataFull[0][col];
        }
        else {
            displayText = PetsSelect_g_tableDataFull[0][col - 1];
        }

        int xPos = startX + counter * cellWidth + 10;
        int yPos = startY + 0 * cellHeight + 7;

        if (col == 0 || col == 1 || col == 2 || col == 3 || col == 4 || col == 5 || col == 6) {
            // Traduzir cabeçalhos se necessário
            if (displayText == L"Nome_do_Pet") displayText = L"Nome Pet";
            else if (displayText == L"Nome_do_Tutor") displayText = L"Nome Tutor";
            else if (displayText == L"Raca") displayText = L"Raça";
            else if (displayText == L"Appointment_Date") displayText = L"Data";
            else if (displayText == L"Appointment_Hour") displayText = L"Hora";
            else if (displayText == L"Date") displayText = L"Data Registro";
            else if (displayText == L"Hour") displayText = L"Hora Registro";

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
}

void PetsSelect_selectHeaderDB() {
    PetsSelect_g_tableDataFull.clear(); // Limpa o vetor no início

    // Abrir ou criar o banco de dados (código original mantido)
    sqlite3* db = nullptr;
    char* errMsg = nullptr;
    int rc;
    OpenDatabase(db);

    if (!OpenDatabase(db)) {
        return;
    }

    // --- 1. PEGAR INFORMAÇÕES DA TABELA PETS ---
    const char* sqlPragmaPets = "PRAGMA table_info(Pets);";

    rc = sqlite3_exec(
        db,
        sqlPragmaPets,
        PetsSelect_pragma_callback,
        &PetsSelect_g_tableDataFull, // O callback adiciona aqui
        &errMsg
    );

    if (rc != SQLITE_OK) {
        if (errMsg) {
            fprintf(stderr, "SQL error (PRAGMA Pets): %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }

    // --- 2. PEGAR INFORMAÇÕES DA TABELA TUTORES ---
    const char* sqlPragmaTutores = "PRAGMA table_info(Tutores);";

    // Reutilizamos a mesma função callback e o mesmo vetor
    rc = sqlite3_exec(
        db,
        sqlPragmaTutores,
        PetsSelect_pragma_callback,
        &PetsSelect_g_tableDataFull, // O callback continua a adicionar
        &errMsg
    );

    if (rc != SQLITE_OK) {
        if (errMsg) {
            fprintf(stderr, "SQL error (PRAGMA Tutores): %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }

    sqlite3_close(db); // Fechar a conexão com o banco de dados
}

void PetsSelect_createHeaderFilters(HDC hdc, HWND hWnd) {
    PetsSelect_selectHeaderDB();

    // Configurar a linha do header
    int cellHeight = 32;
    int startYFull = 80 - PetsSelect_g_scrollY;  // Posição Y com scroll
    int startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    int colNumber = 0;
    int colFinalNumber = colNumber + 10;

    PetsSelect_criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);

    // Configurar a linha do header
    startYFull = startYFull + 2 * cellHeight + 7;  // Posição Y com scroll
    startXFull = 22 - PetsSelect_g_scrollX;  // Posição X com scroll
    colNumber = 10;
    colFinalNumber = PetsSelect_g_tableDataFull[0].size() - 1;

    PetsSelect_criarHeaderLineFilter(hdc, hWnd, startYFull, startXFull, colNumber, colFinalNumber);

}

void PetsSelect_updateWindow(LPCWSTR className) {
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        PetsSelect_selectDB();
        PetsSelect_RecarregarDadosTabela(hSelectWnd);
    }
}

// Função para atualizar posições dos botões com scroll
void PetsSelect_AtualizarPosicoesBotoes(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = PetsSelect_g_tableData.empty() ? 0 : 7 + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 350;
    int cellHeight = 32;

    // Desabilitar redesenho durante a atualização
    SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

    for (size_t i = 0; i < PetsSelect_g_buttons.size(); i++) {
        size_t row = (i / 3) + 1;
        int buttonType = i % 3;

        int yPos = startY + row * cellHeight + 2 - PetsSelect_g_scrollY;
        int xPos = startX + (7 + buttonType) * cellWidth + 2;

        // Verificar se o botão está visível na área da janela
        BOOL isVisible = (yPos >= -cellHeight && yPos <= rect.bottom);

        if (isVisible) {
            SetWindowPos(PetsSelect_g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        }
        else {
            // Esconder botões que estão fora da área visível
            SetWindowPos(PetsSelect_g_buttons[i], NULL, xPos, yPos, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
        }
    }

    // Reabilitar redesenho e forçar atualização
    SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
}

bool PetsSelect_deleteRecordById(const std::string& databasePath, int id, HWND hWnd) {
    // Abrir ou criar o banco de dados (código original mantido)
    sqlite3* db = nullptr;
    char* errMsg = nullptr;
    int rc;
    OpenDatabase(db);

    if (!OpenDatabase(db)) {
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

void PetsSelect_selectDB() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    PetsSelect_g_tableData.clear();

    // Abrir ou criar o banco de dados (código original mantido)
    sqlite3* db = nullptr;
    char* errMsg = nullptr;
    int rc;
    OpenDatabase(db);

    if (OpenDatabase(db)) {
        std::string sqlSelect;
        std::string sqlSelectCount;
        if (PetsSelect_orderColumn == "Appointment_Hour") {

            // Expressão para converter 'H:MM' ou 'HH:MM' para o formato ordenável 'HH:MM'
            std::string hourSorting =
                // 1. Pega a hora (antes do ':'), converte para INT, e preenche com zero (ex: '4' -> '04')
                "PRINTF('%02d', CAST(SUBSTR(Appointment_Hour, 1, INSTR(Appointment_Hour, ':') - 1) AS INTEGER))"
                // 2. Concatena com os minutos (incluindo o ':')
                " || SUBSTR(Appointment_Hour, INSTR(Appointment_Hour, ':'))";

            sqlSelect = "SELECT *, P.ID AS ID_Pet, T.Nome_do_Tutor, T.ID AS ID_Tutor FROM Pets AS P INNER JOIN Tutores AS T ON ID_Tutor_FK = ID_Tutor ORDER BY " + hourSorting + " " + PetsSelect_orderAscDesc;
        }
        else if (PetsSelect_orderColumn == "Appointment_Date") {

            // Define a string de ordenação complexa para a data DD/MM/YYYY
            std::string dataSorting =
                "SUBSTR(Appointment_Date, 7, 4) || SUBSTR(Appointment_Date, 4, 2) || SUBSTR(Appointment_Date, 1, 2)";
            sqlSelect = "SELECT *, P.ID AS ID_Pet, T.Nome_do_Tutor, T.ID AS ID_Tutor FROM Pets AS P INNER JOIN Tutores AS T ON ID_Tutor_FK = ID_Tutor ORDER BY " + dataSorting + " " + PetsSelect_orderAscDesc;

        }
        else {
            //const char* sqlSelect = "SELECT ID, Nome_do_Pet, Nome_do_Tutor, Banho, Tosa, Appointment_Date, Appointment_Hour FROM Pets;";
            sqlSelect = "SELECT *, P.ID AS ID_Pet, T.Nome_do_Tutor, T.ID AS ID_Tutor FROM Pets AS P INNER JOIN Tutores AS T ON ID_Tutor_FK = ID_Tutor ORDER BY " + PetsSelect_orderColumn + " COLLATE NOCASE " + PetsSelect_orderAscDesc;
        }

        //std::string limitClause = " LIMIT " + std::to_string(limitTableRow) + " OFFSET " + std::to_string(offsetTableRow);
        //sqlSelect = sqlSelect + limitClause + ";";

        rc = sqlite3_exec(db, sqlSelect.c_str(), PetsSelect_sqlite_callback, &PetsSelect_g_tableData, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                PetsSelect_g_tableData.push_back({ L"Erro", wErrMsg });
            }
            else {
                PetsSelect_g_tableData.push_back({ L"Erro", L"Desconhecido" });
            }
            if (errMsg) sqlite3_free(errMsg);
        }
        sqlite3_close(db);
        PetsSelect_naoDesenhar.resize(PetsSelect_g_tableData.size());
    }
    else {
        PetsSelect_g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

std::wstring PetsSelect_toLower(std::wstring str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](wchar_t c) {
            return std::towlower(c);
        });
    return str;
}

void PetsSelect_verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern) {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    PetsSelect_g_tableDataRowsNumber.clear();

    // CORREÇÃO: Verificar se g_tableData não está vazio antes de redimensionar
    if (!PetsSelect_g_tableData.empty()) {
        PetsSelect_g_tableDataRowsNumber.resize(PetsSelect_g_tableData.size());
        for (size_t row = 0; row < PetsSelect_g_tableData.size(); ++row) {
            PetsSelect_g_tableDataRowsNumber[row].resize(PetsSelect_g_tableData[row].size());
        }
    }

    std::wstring filtro;
    std::wstring dadoTable;

    // CORREÇÃO: Redimensionar naoDesenharIntern para o tamanho correto
    if (naoDesenharIntern.size() != PetsSelect_g_tableData.size()) {
        naoDesenharIntern.resize(PetsSelect_g_tableData.size(), 0);
    }

    int column;
    int numeroColIteracoes = 21;
    for (size_t row = 0; row < PetsSelect_g_tableData.size(); row++) {

        // CORREÇÃO: Verificar se o índice é válido
        if (row >= naoDesenharIntern.size()) {
            naoDesenharIntern.resize(row + 1, 0);
        }

        naoDesenharIntern[row] = 0;

        // CORREÇÃO: Pular linha 0 (cabeçalho) se necessário
        if (row == 0) continue; // Mantém o cabeçalho

        for (size_t col = 0; col < 22; col++) {
            if (col == 21) {
                column = col - 1;
            }
            else if (col >= 9 && col < 12) {
                column = col - 1;
            }
            else if (col >= 12) {
                column = col;
            }
            else {
                column = col;
            }
            

            dadoTable = PetsSelect_g_tableData[row][column];

            std::wstring displayText = dadoTable;

            if (!dados[col].empty() && (col == 0 || col == 4 || col == 5 || col == 7 || col == 11)) {
                filtro = dados[col];
                if (filtro != dadoTable) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
            else if (!dados[col].empty() && (col == 8 || col == 9)) {

                // CORREÇÃO: Verificar se a coluna existe na linha atual
                if (col == 9) {
                    dadoTable = PetsSelect_g_tableData[row][col - 1];

                }

                bool estaEntre = PetsSelect_estaEntreDatas(dados[8], dados[9], dadoTable);
                if (!estaEntre) {
                    if (col == 9 && dados[8].empty()) {
                        naoDesenharIntern[row] = 1;
                        break;
                    }
                    else {
                        naoDesenharIntern[row] = 1;
                        break;
                    }
                }
            }
            else if (!dados[col].empty() && (col == 19 || col == 20)) {

                // CORREÇÃO: Verificar se a coluna existe na linha atual
                if (col == 20) {
                    dadoTable = PetsSelect_g_tableData[row][col - 1];

                }

                bool estaEntre = PetsSelect_estaEntreDatas(dados[19], dados[20], dadoTable);
                if (!estaEntre) {
                    if (col == 20 && dados[19].empty()) {
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
                if (PetsSelect_toLower(dadoTable).find(PetsSelect_toLower(dados[col])) == std::wstring::npos) {
                    naoDesenharIntern[row] = 1;
                    break;
                }
            }
        }
    }

    // 1. Limpeza e Inicialização de Contadores
    PetsSelect_rowsNumber = 0;
    int PetsSelect_rowCount = 0;

    // CRUCIAL: Limpar o vetor de destino para garantir que ele comece vazio
    PetsSelect_g_tableDataRowsNumber.clear();

    // 2. Pré-alocação (Opcional, mas melhora o desempenho)
    if (!PetsSelect_g_tableData.empty()) {
        PetsSelect_g_tableDataRowsNumber.reserve(PetsSelect_g_tableData.size());
    }

    // 3. Iteração, Filtro e Construção do Novo Vetor
    for (size_t row = 0; row < PetsSelect_g_tableData.size(); row++) { // Começar da linha 1 (pular cabeçalho)

        // A. Lógica do Filtro: Se a linha for válida (flag != 1)
        if (row < naoDesenharIntern.size() && naoDesenharIntern[row] != 1) {

            // B. Cria e preenche uma nova linha com todas as colunas da linha original
            std::vector<std::wstring> newRow;

            // Copia todas as colunas (você não está filtrando colunas, apenas linhas)
            for (size_t col = 0; col < PetsSelect_g_tableData[row].size(); col++) {
                newRow.push_back(PetsSelect_g_tableData[row][col]);
            }

            // C. Adiciona a linha válida (newRow) ao vetor de destino
            PetsSelect_g_tableDataRowsNumber.push_back(newRow);

            // D. Atualiza o contador de linhas válidas
            PetsSelect_rowCount++;
            PetsSelect_rowsNumber++;
        }
        // Linhas com naoDesenharIntern[row] == 1 são automaticamente "removidas"
    }

    // CORREÇÃO CRÍTICA: Garantir que rowsNumber seja pelo menos 1 (cabeçalho)
    if (PetsSelect_rowsNumber == 0 && !PetsSelect_g_tableDataRowsNumber.empty()) {
        PetsSelect_rowsNumber = static_cast<int>(PetsSelect_g_tableDataRowsNumber.size());
    }
    if (PetsSelect_rowsNumber == 0 && !PetsSelect_g_tableData.empty()) {
        PetsSelect_rowsNumber = 1; // Pelo menos o cabeçalho
    }

    // 4. ATRIBUIÇÃO E REDIMENSIONAMENTO
    PetsSelect_g_tableData.clear();

    // CORREÇÃO: Só mover se houver dados
    if (!PetsSelect_g_tableDataRowsNumber.empty()) {
        PetsSelect_g_tableData = std::move(PetsSelect_g_tableDataRowsNumber);
    }
    else {
        // Se não há dados filtrados, manter pelo menos o cabeçalho se existir
        if (!PetsSelect_g_tableData.empty()) {
            // Manter apenas o cabeçalho
            std::vector<std::vector<std::wstring>> temp;
            temp.push_back(PetsSelect_g_tableData[0]);
            PetsSelect_g_tableData = std::move(temp);
            PetsSelect_rowsNumber = 1;
        }
    }

    // CORREÇÃO: Garantir que naoDesenhar tenha o tamanho correto
    if (PetsSelect_naoDesenhar.size() != PetsSelect_g_tableData.size()) {
        PetsSelect_naoDesenhar.resize(PetsSelect_g_tableData.size(), 0);
    }

    // CORREÇÃO: Copiar apenas os valores relevantes
    for (size_t i = 1; i < naoDesenharIntern.size() && i < PetsSelect_naoDesenhar.size(); i++) {
        PetsSelect_naoDesenhar[i] = naoDesenharIntern[i];
    }

    PetsSelect_rowsNumberSemCabecalho = PetsSelect_rowsNumber - 1;
}

void PetsSelect_CriarBotoesTabela(HWND hWnd)
{
    // Limpar botões existentes
    for (HWND hButton : PetsSelect_g_buttons) {
        DestroyWindow(hButton);
    }
    PetsSelect_g_buttons.clear();

    if (PetsSelect_g_tableData.size() <= 1) return;

    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;
    int numColumns = PetsSelect_g_tableData.empty() ? 0 : PetsSelect_g_tableData[0].size() + 3;
    int cellWidth = width / (numColumns > 0 ? numColumns : 1);
    int startX = 22;
    int startY = 80;
    int cellHeight = 32;

    int limit;
    limit = PetsSelect_offsetTableRow + PetsSelect_limitTableRow;

    if (limit <= PetsSelect_rowsNumberSemCabecalho) {
        limit = PetsSelect_offsetTableRow + PetsSelect_limitTableRow;
    }
    else {
        limit = PetsSelect_rowsNumberSemCabecalho + 1;
    }

    int inicio;
    if (PetsSelect_offsetTableRow == 0) {
        inicio = 1;
    }
    else {
        inicio = PetsSelect_offsetTableRow;
    }

    for (size_t row = inicio; row < limit; row++) {
        LONG_PTR recordId = _wtoi(PetsSelect_g_tableData[row][0].c_str());
        int yPos = startY + row * cellHeight + 2;

        // Botão Consultar
        int xPos = startX + 7 * cellWidth + 2;
        HWND hButton = CreateWindowW(
            L"BUTTON", L"Consultar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(PetsSelect_CONSULTAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            PetsSelect_g_buttons.push_back(hButton);
        }

        // Botão Editar
        xPos = startX + 8 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Editar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(PetsSelect_EDITAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            PetsSelect_g_buttons.push_back(hButton);
        }

        // Botão Deletar
        xPos = startX + 9 * cellWidth + 2;
        hButton = CreateWindowW(
            L"BUTTON", L"Deletar",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            xPos, yPos, 70, 30,
            hWnd, (HMENU)(PetsSelect_DELETAR),
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL
        );
        if (hButton) {
            SetWindowLongPtr(hButton, GWLP_USERDATA, recordId);
            PetsSelect_g_buttons.push_back(hButton);
        }
    }
}

// Função para configurar scroll bars
void PetsSelect_ConfigurarScrollBars(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    PetsSelect_g_clientHeight = rect.bottom - rect.top;

    int cellHeight = 32;
    int filtersHeight = 6 * cellHeight + 6 * 7;

    //offsetTableRow = limitTableRow
    int limit;
    int numeroDeLinhas = PetsSelect_limitTableRow;

    limit = PetsSelect_offsetTableRow + PetsSelect_limitTableRow;

    PetsSelect_g_contentHeight = static_cast<int>(numeroDeLinhas) * cellHeight + 160 + filtersHeight;

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = PetsSelect_g_contentHeight;
    si.nPage = PetsSelect_g_clientHeight;
    si.nPos = PetsSelect_g_scrollY;

    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

void PetsSelect_invalidateDrawing(HWND hWnd) {
    BlockInput(TRUE);
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    BlockInput(FALSE);
}

void PetsSelect_DestroyControlsFromVector(std::vector<HWND>& controls) {
    for (HWND hControl : controls) {
        if (hControl != NULL && IsWindow(hControl)) {
            DestroyWindow(hControl);
        }
    }
    controls.clear();
}

// Função para recarregar dados do banco
void PetsSelect_RecarregarDadosTabela(HWND hWnd) {
    // Assim, o Windows não redesenha a janela a cada SetWindowPos, apenas uma vez no final — mais rápido e visualmente limpo.
    SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

    // Destruir na ordem inversa da criação (mais seguro)
    PetsSelect_DestroyControlsFromVector(PetsSelect_g_buttons);
    PetsSelect_DestroyControlsFromVector(PetsSelect_g_editControlsOffsetLimit);
    PetsSelect_DestroyControlsFromVector(PetsSelect_g_editControlsLimit);
    PetsSelect_DestroyControlsFromVector(PetsSelect_g_editControlsOrder);
    PetsSelect_DestroyControlsFromVector(PetsSelect_g_editControlsFilters);

    // Limpeza extra para garantir
    PetsSelect_g_buttons.clear();
    PetsSelect_g_editControlsOffsetLimit.clear();
    PetsSelect_g_editControlsLimit.clear();
    PetsSelect_g_editControlsOrder.clear();
    PetsSelect_g_editControlsFilters.clear();

    // Definir valores para ordenação e imagem
    PetsSelect_ordenarDefinicoesValores(hWnd);

    // Limpar dados antigos
    PetsSelect_selectDB();

    // Criar inputs de limite
    PetsSelect_createInputLimit(hWnd);

    PetsSelect_naoDesenhar.resize(PetsSelect_g_tableData.size());
    //Verificar filtros
    PetsSelect_verificarFiltro(PetsSelect_dados, PetsSelect_naoDesenhar);

        // Criar botões de paginação
    PetsSelect_createBtnPageLimit(hWnd);

    // Criar botões após carregar os dados
    PetsSelect_CriarBotoesTabela(hWnd);

    // Criar inputs de filtros
    PetsSelect_criarInputsFilters(hWnd);

    // Definir valores dos filtros
    PetsSelect_SetFilterValues(PetsSelect_dados);

    // Criar inputs de order
    PetsSelect_createOrderBtn(hWnd);

    //Atualizar posição dos botões
    PetsSelect_AtualizarPosicoesBotoes(hWnd);

    //Atualizar inputs
    PetsSelect_AtualizarPosicoesInputs(hWnd);

    // Atualizar offset botões
    PetsSelect_AtualizarPosicoesOffset(hWnd);

    // Atualizar limite de linhas
    PetsSelect_AtualizarPosicoesLimit(hWnd);

    // Atualizar botões order
    PetsSelect_AtualizarPosicoesOrder(hWnd);

    // Mudar ícone do botão de ordenamento
    PetsSelect_ordenarMudarIcone(hWnd);

    // Reconfigurar scroll bars NÃO PODE TIRAR ISSO DAQUI, NESSA ORDEM SE NÃO O INPUT QUE ESCOLHE O Nº DE LINHAS DA TABELA VAI BUGAR
    PetsSelect_ConfigurarScrollBars(hWnd);

    // Assim, o Windows não redesenha a janela a cada SetWindowPos, apenas uma vez no final — mais rápido e visualmente limpo.
    SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);

    // Forçar redesenho da janela
    PetsSelect_invalidateDrawing(hWnd);
}

void PetsSelect_checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData) {

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
    else {
        SendMessage(hinput, BM_SETCHECK, BST_UNCHECKED, 0);
    }
}

int PetsSelect_sqlite_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? PetsSelect_utf8_to_wstring(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? PetsSelect_utf8_to_wstring(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

// Função para atualizar posição dos controles com scroll
void PetsSelect_AtualizarPosicoesControlesAgendamentoEdit(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
    int startY = 40 - PetsSelect_g_scrollY;
    int startX = 22 - PetsSelect_g_scrollX;
    int xPos = 0;
    int yPos = 0;
    int colNumber;
    int countRow = 0;

    // Atualizar posição dos campos de entrada
    for (size_t i = 0; i < PetsSelect_g_editControls_edit.size(); i++) {
        colNumber = countRow + 1;

        if (i == 2) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 610, 200,
                SWP_NOZORDER | SWP_NOACTIVATE);

            // Atualizar posição do botão
            if (PetsSelect_g_hButton_consultar) {
                SetWindowPos(PetsSelect_g_hButton_consultar, NULL, xPos + 620, yPos, 80, 25,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }

            countRow++;
        }
        else if (i == 6) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;

            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 700, 200,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 12) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 13) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 14) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 15) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 16) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 17) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 18) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 600, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 8) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 25, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 20) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 21) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 22) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 23) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 24) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 25) {
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls_edit[i], NULL, xPos, yPos, 700, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
    }

    // Atualizar posição do botão
    if (PetsSelect_g_hButton) {
        int buttonY = startY + 9 * cellHeight + 3;
        SetWindowPos(PetsSelect_g_hButton, NULL, startX, buttonY, 150, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Função para atualizar posição dos controles com scroll
void PetsSelect_AtualizarPosicoesControlesAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int width = (rect.right - rect.left) - 44;

    int cellHeight = 32;
    int numColumns = 21;
    int cellWidth = (width + 2000) / (numColumns > 0 ? numColumns : 1);
    int startY = 40 - PetsSelect_g_scrollY;
    int startX = 22 - PetsSelect_g_scrollX;
    int xPos = 0;
    int yPos = 0;
    int colNumber;
    int countRow = 0;

    // Atualizar posição dos campos de entrada
    for (size_t i = 0; i < PetsSelect_g_editControls.size(); i++) {
        colNumber = countRow + 1;

        if (i == 2) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 610, 200,
                SWP_NOZORDER | SWP_NOACTIVATE);

            // Atualizar posição do botão
            if (PetsSelect_g_hButton_consultar) {
                SetWindowPos(PetsSelect_g_hButton_consultar, NULL, xPos + 620, yPos, 80, 25,
                    SWP_NOZORDER | SWP_NOACTIVATE);
            }

            countRow++;
        }
        else if (i == 6) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;

            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 700, 200,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 12) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 13) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 14) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 15) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 16) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 17) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 18) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 600, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 8) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 25, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 20) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 21) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 22) {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 110, 25, // Ajustado para checkbox
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
        else if (i == 23) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 150, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 24) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 300, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else if (i == 25) {
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos + 450, yPos, 110, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
        else {
            xPos = startX + cellWidth + 10;
            yPos = startY + colNumber * cellHeight + 3;
            SetWindowPos(PetsSelect_g_editControls[i], NULL, xPos, yPos, 700, 25,
                SWP_NOZORDER | SWP_NOACTIVATE);
            countRow++;
        }
    }

    // Atualizar posição do botão
    if (PetsSelect_g_hButton) {
        int buttonY = startY + 9 * cellHeight + 3;
        SetWindowPos(PetsSelect_g_hButton, NULL, startX, buttonY, 150, 30,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Função para configurar scroll bars
void PetsSelect_ConfigurarScrollBarsAgendamento(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    PetsSelect_g_clientHeight = rect.bottom - rect.top;
    PetsSelect_g_clientWidth = rect.right - rect.left;

    // Calcular altura total do conteúdo (19 linhas + título + botão)
    int cellHeight = 32;
    PetsSelect_g_contentHeight = 22 * cellHeight + 100; // 19 campos + título + botão + margem
    PetsSelect_g_contentWidth = 2000; // Largura fixa para conteúdo largo

    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = PetsSelect_g_contentHeight;
    si.nPage = PetsSelect_g_clientHeight;
    si.nPos = PetsSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = PetsSelect_g_contentWidth;
    si.nPage = PetsSelect_g_clientWidth;
    si.nPos = PetsSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}

// Função para obter a data atual como string
std::wstring PetsSelect_GetCurrentDate()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t dateStr[80];
    swprintf_s(dateStr, L"%02d/%02d/%04d", st.wDay, st.wMonth, st.wYear);

    return std::wstring(dateStr);
}

// Função para obter a hora atual como string
std::wstring PetsSelect_GetCurrentHour()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t timeStr[80];
    swprintf_s(timeStr, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

    return std::wstring(timeStr);
}

bool PetsSelect_isNumber(const std::wstring& str) {
    if (str.empty()) return false;

    for (wchar_t c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Versão que permite números decimais
bool PetsSelect_isDecimalNumber(const std::wstring& str) {
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

bool PetsSelect_isValidTime(const std::wstring& time) {
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

bool PetsSelect_isValidDate(const std::wstring& date) {
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

LPCWSTR PetsSelect_error = L"0";
std::wstring PetsSelect_mensagem = L"";
LPCWSTR PetsSelect_msg = L"";

std::wstring PetsSelect_treatDataAppointment(std::wstring dado, int number) {
    std::wstring dado_escaped = dado;
    size_t pos = 0;

    if (number == 2 && dado.empty()) {
        PetsSelect_error = L"1";
        PetsSelect_mensagem = L"Insira: 'Nome do Pet'.\n" + PetsSelect_mensagem;
    }
    else if (number == 3 && dado.empty()) {
        PetsSelect_error = L"1";
        PetsSelect_mensagem = L"Insira: 'Raça'.\n" + PetsSelect_mensagem;
    }
    else if (number == 4 && dado.empty()) {
        PetsSelect_error = L"1";
        PetsSelect_mensagem = L"Insira: 'Nome do Tutor'.\n" + PetsSelect_mensagem;
    }
    else if (number == 5 && dado.empty()) {
        PetsSelect_error = L"1";
        PetsSelect_mensagem = L"Insira: 'Cor'.\n" + PetsSelect_mensagem;
    }
    else if (number == 7 && !dado.empty()) {
        if (!PetsSelect_isDecimalNumber(dado)) {
            PetsSelect_error = L"1";
            PetsSelect_mensagem = L"Insira: Apenas números decimais ou inteiros em 'Peso'.\n" + PetsSelect_mensagem;
        }
    }
    else if (number == 8 && dado.empty()) {
        PetsSelect_error = L"1";
        PetsSelect_mensagem = L"Insira: 'Sexo'.\n" + PetsSelect_mensagem;
    }
    else if (dado.empty()) {
        dado_escaped = L"";
    }

    while ((pos = dado_escaped.find(L"'", pos)) != std::wstring::npos) {
        dado_escaped.replace(pos, 1, L"''");
        pos += 2;
    }

    PetsSelect_msg = PetsSelect_mensagem.c_str();

    return dado_escaped;
}

// Converter std::wstring para std::string UTF-8
std::string PetsSelect_WideToUTF8(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Converter std::string UTF-8 para std::wstring
std::wstring PetsSelect_UTF8ToWide(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

BOOL PetsSelect_fonte(LPCWSTR fonte, COLORREF color, HDC hdc) {
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

BOOL PetsSelect_windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size) {
    PetsSelect_fonte(L"Title", RGB(0, 0, 0), hdc);
    TextOut(hdc, startX, startY, tit, size);

    return 0;
}

BOOL PetsSelect_scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight)
{
    // Variáveis de scroll
    PetsSelect_g_scrollX = scrollX;      // Posição horizontal do scroll
    PetsSelect_g_scrollY = scrollY;      // Posição vertical do scroll
    PetsSelect_g_contentWidth = contentWidth;   // Largura total do conteúdo
    PetsSelect_g_contentHeight = contentHeight;   // Altura total do conteúdo
    PetsSelect_g_clientWidth = clientWidth;       // Largura da área cliente
    PetsSelect_g_clientHeight = clientHeight;      // Altura da área cliente

    // Obter dimensões da área cliente
    RECT rect;
    GetClientRect(hWnd, &rect);
    PetsSelect_g_clientWidth = rect.right - rect.left;
    PetsSelect_g_clientHeight = rect.bottom - rect.top;

    // Configurar scroll bars
    SCROLLINFO si = {};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;

    // Scroll vertical
    si.nMin = 0;
    si.nMax = PetsSelect_g_contentHeight;
    si.nPage = PetsSelect_g_clientHeight;
    si.nPos = PetsSelect_g_scrollY;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    // Scroll horizontal
    si.nMin = 0;
    si.nMax = PetsSelect_g_contentWidth;
    si.nPage = PetsSelect_g_clientWidth;
    si.nPos = PetsSelect_g_scrollX;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

    return 0;
}

BOOL PetsSelect_windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL PetsSelect_Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

BOOL PetsSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
BOOL PetsSelect_CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle)
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
        wc.lpfnWndProc = WndProcPetsSelect;

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
LRESULT CALLBACK PetsSelect_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Processar o menu APENAS para mensagens específicas
    if (message == WM_COMMAND || message == WM_INITMENU || message == WM_MENUSELECT) {
        if (PetsSelect_ProcessarMenu(hWnd, message, wParam, lParam)) {
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
        if (PetsSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        if (PetsSelect_g_isRedrawing)
        {
            return 0; // Ignora o clique durante o redesenho
        }
        // Lógica existente para clique, se aplicável
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_KEYDOWN:
    {
        PetsSelect_Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    case WM_DESTROY:
        PetsSelect_windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK PetsSelect_About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
