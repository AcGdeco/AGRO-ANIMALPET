#include <vector>
#include "sqlite3.h"
#include <string>
#include <windows.h>
#include "Global.h"
#include <iostream>

int windowsNumber = 1;
std::vector<std::vector<std::wstring>> TutoresSelect_Global_g_tableData;
std::vector<std::vector<std::wstring>> TutoresPetsSelect_Global_g_tableData;
LONG_PTR TutoresSelect_idRecord;
HWND PetsSelect_g_hButton_consultar;
HWND PetsSelect_g_hButton_consultar_consultar;
HWND AgendamentosSelect_g_hButton_consultar_tutor;
HWND AgendamentosSelect_g_hButton_consultar_pet;

struct ComboBoxItemData {
    long long idPet;
    long long idTutor;
};

std::wstring TutoresSelect_Global_utf8_to_wstring(const char* str) {
    if (!str) return L"NULL";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstr(size_needed - 1, 0); // -1 para não incluir o caractere nulo
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], size_needed);
    return wstr;
}

int TutoresSelect_Global_sqlite_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? TutoresSelect_Global_utf8_to_wstring(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? TutoresSelect_Global_utf8_to_wstring(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

void TutoresSelect_Global_selectDB() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    TutoresSelect_Global_g_tableData.clear();

    // Consultar o banco
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        // QUERY SIMPLIFICADA - APENAS SELECT * FROM Tutores
        std::string sqlSelect = "SELECT * FROM Tutores ORDER BY LOWER(Nome_do_Tutor) ASC";

        rc = sqlite3_exec(db, sqlSelect.c_str(), TutoresSelect_Global_sqlite_callback, &TutoresSelect_Global_g_tableData, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                TutoresSelect_Global_g_tableData.push_back({ L"Erro", wErrMsg });
            }
            else {
                TutoresSelect_Global_g_tableData.push_back({ L"Erro", L"Desconhecido" });
            }
            if (errMsg) sqlite3_free(errMsg);
        }
        sqlite3_close(db);
    }
    else {
        TutoresSelect_Global_g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

void TutoresSelect_Global_preencherComboBox(HWND hComboBox) {
    std::wstring nomeTutor;
    std::wstring cpfTutor;
    std::wstring valorInput;
    std::wstring idTutor;

    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
    for (int i = 0; i < TutoresSelect_Global_g_tableData.size(); ++i) {
        if (i != 0) {
            idTutor = L" - " + TutoresSelect_Global_g_tableData[i][0];
            nomeTutor = TutoresSelect_Global_g_tableData[i][1];
            cpfTutor = !TutoresSelect_Global_g_tableData[i][6].empty() ? L" - " + TutoresSelect_Global_g_tableData[i][6] : L"";
            valorInput = nomeTutor;

            LRESULT itemIndex = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)valorInput.c_str());
            LRESULT idTutorNum = std::stoll(TutoresSelect_Global_g_tableData[i][0]);
            SendMessage(
                hComboBox,
                CB_SETITEMDATA,
                (WPARAM)itemIndex,     // wParam: O ÍNDICE numérico recém-adicionado
                (LPARAM)idTutorNum     // lParam: O VALOR NUMÉRICO (ID) a ser armazenado
            );
        }
    }
}

int TutoresPetsSelect_Global_sqlite_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::vector<std::wstring>>* table = static_cast<std::vector<std::vector<std::wstring>>*>(data);
    // Primeira chamada: adicionar cabeçalhos (nomes das colunas)
    if (table->empty()) {
        std::vector<std::wstring> headers;
        for (int i = 0; i < argc; i++) {
            headers.push_back(azColName[i] ? TutoresSelect_Global_utf8_to_wstring(azColName[i]) : L"NULL");
        }
        table->push_back(headers);
    }

    // Adicionar linha de dados
    std::vector<std::wstring> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? TutoresSelect_Global_utf8_to_wstring(argv[i]) : L"NULL");
    }
    table->push_back(row);

    return 0;
}

void TutoresPetsSelect_Global_selectDB() {
    // 1. LIMPAR DADOS ANTIGOS ANTES DE CADA CONSULTA
    TutoresPetsSelect_Global_g_tableData.clear();

    // Consultar o banco
    sqlite3* db;
    char* errMsg = 0;
    int rc = sqlite3_open("pet.db", &db);
    if (rc == SQLITE_OK) {
        // QUERY SIMPLIFICADA - APENAS SELECT * FROM Tutores
        std::string sqlSelect = "SELECT *, P.ID AS ID_Pet, T.ID AS ID_Tutor FROM Pets AS P INNER JOIN Tutores AS T ON ID_Tutor_FK = ID_Tutor ORDER BY LOWER(Nome_do_Tutor) ASC";

        rc = sqlite3_exec(db, sqlSelect.c_str(), TutoresPetsSelect_Global_sqlite_callback, &TutoresPetsSelect_Global_g_tableData, &errMsg);
        if (rc != SQLITE_OK) {
            if (errMsg) {
                // Converte char* para wchar_t* corretamente
                size_t len = strlen(errMsg) + 1;
                std::wstring wErrMsg(len, L'\0');
                mbstowcs_s(nullptr, &wErrMsg[0], len, errMsg, _TRUNCATE);
                // Remove o caractere nulo extra do final
                wErrMsg.resize(wcslen(wErrMsg.c_str()));
                TutoresPetsSelect_Global_g_tableData.push_back({ L"Erro", wErrMsg });
            }
            else {
                TutoresPetsSelect_Global_g_tableData.push_back({ L"Erro", L"Desconhecido" });
            }
            if (errMsg) sqlite3_free(errMsg);
        }
        sqlite3_close(db);
    }
    else {
        TutoresPetsSelect_Global_g_tableData.push_back({ L"Erro", L"Não foi possível abrir o banco" });
    }
}

void TutoresPetsSelect_Global_preencherComboBox(HWND hComboBox) {
    std::wstring nomeTutor;
    std::wstring cpfTutor;
    std::wstring valorInput;
    std::wstring idTutor;

    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"");
    for (int i = 0; i < TutoresPetsSelect_Global_g_tableData.size(); ++i) {
        if (i != 0) {
            idTutor = L" - " + TutoresPetsSelect_Global_g_tableData[i][0];
            nomeTutor = TutoresPetsSelect_Global_g_tableData[i][12] + L" - " + TutoresPetsSelect_Global_g_tableData[i][1];
            cpfTutor = !TutoresPetsSelect_Global_g_tableData[i][6].empty() ? L" - " + TutoresPetsSelect_Global_g_tableData[i][6] : L"";
            valorInput = nomeTutor;

            LRESULT itemIndex = SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)valorInput.c_str());
            LRESULT idTutorNum = std::stoll(TutoresPetsSelect_Global_g_tableData[i][11]);
            LRESULT idPetNum = std::stoll(TutoresPetsSelect_Global_g_tableData[i][0]);

            ComboBoxItemData* data = new ComboBoxItemData();
            data->idPet = idPetNum;
            data->idTutor = idTutorNum;

            SendMessage(
                hComboBox,
                CB_SETITEMDATA,
                (WPARAM)itemIndex,     // wParam: O ÍNDICE numérico recém-adicionado
                (LPARAM)data     // lParam: O VALOR NUMÉRICO (ID) a ser armazenado
            );
        }
        else {
            ComboBoxItemData* data = new ComboBoxItemData();
            data->idPet = 0;
            data->idTutor = 0;

            SendMessage(
                hComboBox,
                CB_SETITEMDATA,
                (WPARAM)0,     // wParam: O ÍNDICE numérico recém-adicionado
                (LPARAM)data     // lParam: O VALOR NUMÉRICO (ID) a ser armazenado
            );
        }
    }
}

int ChecarOpcaoComboBoxPorID(HWND hComboBox, const std::wstring& idPetAchecar) {
    // 1. Converter o ID de string para o tipo numérico esperado (LRESULT/long long)
    LRESULT targetID_long;
    try {
        // Usa std::stoll para converter wstring para long long (que é LRESULT em 64bit)
        targetID_long = std::stoll(idPetAchecar);
    }
    catch (const std::exception& e) {
        // Tratar erro de conversão (ex: string vazia ou não numérica)
        // Poderia logar o erro ou apenas retornar erro.
        // std::cerr << "Erro de conversão de ID: " << e.what() << std::endl;
        return CB_ERR;
    }

    // 2. Obter o número total de itens no ComboBox
    int count = (int)SendMessage(hComboBox, CB_GETCOUNT, 0, 0);

    // 3. Iterar por todos os itens
    for (int i = 0; i < count; ++i) {
        // 4. Obter o valor de CB_ITEMDATA (que é o ponteiro para ComboBoxItemData)
        LRESULT itemDataLParam = SendMessage(hComboBox, CB_GETITEMDATA, (WPARAM)i, 0);

        // O valor CB_ERR (-1) é retornado se o item não tiver dados ou se for o item 0 que você configurou.
        if (itemDataLParam != CB_ERR && itemDataLParam != 0) {
            // Converter o LPARAM de volta para um ponteiro para a estrutura
            ComboBoxItemData* data = reinterpret_cast<ComboBoxItemData*>(itemDataLParam);

            // 5. Comparar o 'idPet' armazenado com o 'targetID'
            if (data->idPet == targetID_long) {
                // Encontrado! Selecionar o item e retornar o índice
                SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)i, 0);
                return i;
            }
        }
        // Se itemDataLParam for 0 (o primeiro item que você inicializou com idPet=0), 
        // a comparação será feita no item 0, mas garantimos que não tentamos
        // desreferenciar um ponteiro nulo se a API retornar 0 por algum motivo.
        // Já que você usa o ID 0 para o item vazio, este bloco de 'else if' lida com isso.
        else if (itemDataLParam != CB_ERR && i == 0) {
            ComboBoxItemData* data = reinterpret_cast<ComboBoxItemData*>(itemDataLParam);
            if (data->idPet == targetID_long) {
                SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)i, 0);
                return i;
            }
        }
    }

    // 6. Não encontrado
    return CB_ERR;
}

void AtualizarJanelas() {
    std::wstring className = GetActiveClassWindowName();

    // Agendamentos
    HWND hWndAgendamentosAdd = FindWindowW(L"JanelaAgendamentosAddClasse", NULL);
    if (hWndAgendamentosAdd != NULL && className != L"JanelaAgendamentosAddClasse")
    {
        AgendamentosSelect_invalidateDrawing(hWndAgendamentosAdd);
        UpdateWindow(hWndAgendamentosAdd);
    }

    HWND hWndAgendamentosEdit = FindWindowW(L"JanelaAgendamentosEditClasse", NULL);
    if (hWndAgendamentosEdit != NULL)
    {
        AgendamentosSelect_invalidateDrawing(hWndAgendamentosEdit);
        UpdateWindow(hWndAgendamentosEdit);
    }

    HWND hWndAgendamentosSelect = FindWindowW(L"JanelaAgendamentosSelectClasse", NULL);
    if (hWndAgendamentosSelect != NULL)
    {
        AgendamentosSelect_RecarregarDadosTabela(hWndAgendamentosSelect);
    }

    HWND hWndAgendamentosRead = FindWindowW(L"JanelaAgendamentosReadClasse", NULL);
    if (hWndAgendamentosRead != NULL)
    {
        AgendamentosSelect_invalidateDrawing(hWndAgendamentosRead);
        UpdateWindow(hWndAgendamentosRead);
    }

    // Pets
    HWND hWndPetsAdd = FindWindowW(L"JanelaPetsAddClasse", NULL);
    if (hWndPetsAdd != NULL && className != L"JanelaPetsAddClasse")
    {
        PetsSelect_invalidateDrawing(hWndPetsAdd);
        UpdateWindow(hWndPetsAdd);
    }

    HWND hWndPetsEdit = FindWindowW(L"JanelaPetsEditClasse", NULL);
    if (hWndPetsEdit != NULL)
    {
        PetsSelect_invalidateDrawing(hWndPetsEdit);
        UpdateWindow(hWndPetsEdit);
    }

    HWND hWndPetsSelect = FindWindowW(L"JanelaPetsSelectClasse", NULL);
    if (hWndPetsSelect != NULL)
    {
        PetsSelect_RecarregarDadosTabela(hWndPetsSelect);
    }

    HWND hWndPetsRead = FindWindowW(L"JanelaPetsReadClasse", NULL);
    if (hWndPetsRead != NULL)
    {
        PetsSelect_invalidateDrawing(hWndPetsRead);
        UpdateWindow(hWndPetsRead);
    }

    // Tutores
    HWND hWndTutoresAdd = FindWindowW(L"JanelaTutoresAddClasse", NULL);
    if (hWndTutoresAdd != NULL && className != L"JanelaTutoresAddClasse")
    {
        TutoresSelect_invalidateDrawing(hWndTutoresAdd);
        UpdateWindow(hWndTutoresAdd);
    }

    HWND hWndTutoresEdit = FindWindowW(L"JanelaTutoresEditClasse", NULL);
    if (hWndTutoresEdit != NULL)
    {
        TutoresSelect_invalidateDrawing(hWndTutoresEdit);
        UpdateWindow(hWndTutoresEdit);
    }

    HWND hWndTutoresSelect = FindWindowW(L"JanelaTutoresSelectClasse", NULL);
    if (hWndTutoresSelect != NULL)
    {
        TutoresSelect_RecarregarDadosTabela(hWndTutoresSelect);
    }

    HWND hWndTutoresRead = FindWindowW(L"JanelaTutoresReadClasse", NULL);
    if (hWndTutoresRead != NULL)
    {
        TutoresSelect_invalidateDrawing(hWndTutoresRead);
        UpdateWindow(hWndTutoresRead);
    }
}

std::wstring GetActiveClassWindowName() {
    // 1. Obter o Handle (HWND) da janela em primeiro plano (ativa)
    HWND hWnd = GetForegroundWindow();

    // 2. Verifica se o Handle é válido
    if (hWnd == NULL) {
        return L"";
    }

    // Define um tamanho máximo razoável para o nome da classe.
    // O nome da classe pode ter até 256 caracteres (incluindo o terminador nulo) em WinAPI.
    const int MAX_CLASS_NAME = 256;

    // Alocar um buffer para armazenar o nome da classe
    std::wstring className(MAX_CLASS_NAME, L'\0');

    // 3. Obter o nome da classe
    // GetClassNameW é a versão WIDE (Unicode) da função.
    int copied = GetClassNameW(
        hWnd,
        &className[0], // Ponteiro para o buffer de destino
        MAX_CLASS_NAME
    );

    if (copied > 0) {
        // Redimensionar a string para o tamanho real do nome copiado
        className.resize(copied);
        return className;
    }
    else {
        // Falha ao obter o nome da classe
        return L"";
    }
}