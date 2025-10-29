#pragma once

extern int windowsNumber;
extern LONG_PTR TutoresSelect_idRecord;
extern HWND PetsSelect_g_hButton_consultar;
extern HWND PetsSelect_g_hButton_consultar_consultar;
extern void PetsSelect_RecarregarDadosTabela(HWND hWnd);

void TutoresSelect_Global_selectDB();
void TutoresSelect_Global_preencherComboBox(HWND hComboBox);