#include <vector>
#include "sqlite3.h"
#include <string>
#include <windows.h>

int windowsNumber = 1;
std::vector<std::vector<std::wstring>> TutoresSelect_Global_g_tableData;
LONG_PTR TutoresSelect_idRecord;
HWND PetsSelect_g_hButton_consultar;
HWND PetsSelect_g_hButton_consultar_consultar;

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

            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)valorInput.c_str());
        }
    }
}