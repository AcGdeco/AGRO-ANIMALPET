#pragma once
#include <string>

extern int windowsNumber;
extern LONG_PTR TutoresSelect_idRecord;
extern LONG_PTR PetsSelect_idRecord;
extern HWND PetsSelect_g_hButton_consultar;
extern HWND PetsSelect_g_hButton_consultar_consultar;
extern HWND AgendamentosSelect_g_hButton_consultar_tutor;
extern HWND AgendamentosSelect_g_hButton_consultar_pet;

std::wstring GetActiveClassWindowName();

void PetsSelect_RecarregarDadosTabela(HWND hWnd);
void PetsSelect_invalidateDrawing(HWND hWnd);
void AgendamentosSelect_RecarregarDadosTabela(HWND hWnd);
void AgendamentosSelect_invalidateDrawing(HWND hWnd);
void TutoresSelect_RecarregarDadosTabela(HWND hWnd);
void TutoresSelect_invalidateDrawing(HWND hWnd);
std::string GetAppDataPath();

void TutoresSelect_Global_selectDB();
void TutoresSelect_Global_preencherComboBox(HWND hComboBox);

void TutoresPetsSelect_Global_selectDB();
void TutoresPetsSelect_Global_preencherComboBox(HWND hComboBox);
void AtualizarJanelas();

int ChecarOpcaoComboBoxPorID(HWND hComboBox, const std::wstring& idPetAchecar);