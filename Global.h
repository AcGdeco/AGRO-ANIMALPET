#pragma once
#include <string>
#include "sqlite3.h"
#include <filesystem>
#include <stdexcept>
#include <windows.h>
#include <shlobj.h>

extern int windowsNumber;
extern LONG_PTR TutoresSelect_idRecord;
extern LONG_PTR PetsSelect_idRecord;
extern HWND PetsSelect_g_hButton_consultar;
extern HWND PetsSelect_g_hButton_consultar_consultar;
extern HWND AgendamentosSelect_g_hButton_consultar_tutor;
extern HWND AgendamentosSelect_g_hButton_consultar_pet;
namespace fs = std::filesystem;

struct JanelaAtivaInfo {
    std::wstring className;
    std::wstring windowTitle;
};

extern std::vector<JanelaAtivaInfo> g_historicoJanelas;

std::wstring GetActiveClassWindowName();

void PetsSelect_RecarregarDadosTabela(HWND hWnd);
void PetsSelect_invalidateDrawing(HWND hWnd);
void AgendamentosSelect_RecarregarDadosTabela(HWND hWnd);
void AgendamentosSelect_invalidateDrawing(HWND hWnd);
void TutoresSelect_RecarregarDadosTabela(HWND hWnd);
void TutoresSelect_invalidateDrawing(HWND hWnd);
std::string GetAppDataPath();
bool OpenDatabase(sqlite3*& db);

void TutoresSelect_Global_selectDB();
void TutoresSelect_Global_preencherComboBox(HWND hComboBox);

void TutoresPetsSelect_Global_selectDB();
void TutoresPetsSelect_Global_preencherComboBox(HWND hComboBox);
void AtualizarJanelas();

int ChecarOpcaoComboBoxPorID(HWND hComboBox, const std::wstring& idPetAchecar);

fs::path GetAppDataLocalPath();