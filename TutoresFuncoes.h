#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"
#include "TutoresSelect.h"
#include "Global.h"

extern int TutoresSelect_windowsNumber;
extern int TutoresSelect_g_scrollX;
extern int TutoresSelect_g_scrollY;
extern int TutoresSelect_g_contentWidth;
extern int TutoresSelect_g_contentHeight;
extern int TutoresSelect_g_clientWidth;
extern int TutoresSelect_g_clientHeight;
extern LPCWSTR TutoresSelect_error;
extern std::wstring TutoresSelect_mensagem;
extern LPCWSTR TutoresSelect_msg;
extern std::vector<HWND> TutoresSelect_g_editControls;
extern HWND TutoresSelect_g_hButton;
extern HBRUSH TutoresSelect_hBrushTransparent;
extern int TutoresSelect_g_scrollY;
extern int TutoresSelect_g_scrollX;
extern int TutoresSelect_g_clientHeight;
extern int TutoresSelect_g_clientWidth;
extern int TutoresSelect_g_contentHeight;
extern int TutoresSelect_g_contentWidth;
extern std::vector<std::vector<std::wstring>> TutoresSelect_g_tableData;
extern std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataFull;
extern std::vector<std::vector<std::wstring>> TutoresSelect_g_tableDataRowsNumber;
extern std::vector<HWND> TutoresSelect_g_editControlsLimit;
extern std::vector<HWND> TutoresSelect_g_editControlsOffsetLimit;
extern enum TutoresSelect_ButtonAction { TutoresSelect_CONSULTAR, TutoresSelect_EDITAR, TutoresSelect_DELETAR, TutoresSelect_FILTRAR, TutoresSelect_ORDENAR, TutoresSelect_LIMITAR, TutoresSelect_OFFSET };
extern std::vector<HWND> TutoresSelect_g_buttons;
extern std::vector<std::wstring> TutoresSelect_dados;
extern int TutoresSelect_rowsNumber;
extern std::wstring TutoresSelect_dataAte;
extern std::wstring TutoresSelect_dataRegistroAte;
extern std::string TutoresSelect_orderColumn;
extern std::string TutoresSelect_orderAscDesc;
extern int TutoresSelect_limitTableRow;
extern int TutoresSelect_offsetTableRow;
extern int TutoresSelect_g_totalRowCount;
extern void TutoresSelect_DestroyAllOffsetButtons();
extern int TutoresSelect_numeroBtn;
extern int TutoresSelect_idNumeroUltimo;
extern std::vector<HWND> TutoresSelect_g_editControlsOrder;
extern bool TutoresSelect_g_isRedrawing;
extern int TutoresSelect_rowsNumberSemCabecalho;
extern LONG_PTR TutoresSelect_idBtnGlobal;
extern std::wstring TutoresSelect_btnClicado;
extern std::wstring TutoresSelect_cpf;

std::wstring TutoresSelect_GetCurrentDate();
std::wstring TutoresSelect_GetCurrentHour();

BOOL TutoresSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL TutoresSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL TutoresSelect_Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL TutoresSelect_windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL TutoresSelect_scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight);
BOOL TutoresSelect_fonte(LPCWSTR fonte, COLORREF color, HDC hdc);
BOOL TutoresSelect_windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size);
bool TutoresSelect_isNumber(const std::wstring& str);
bool TutoresSelect_isDecimalNumber(const std::wstring& str);
bool TutoresSelect_isValidTime(const std::wstring& time);
bool TutoresSelect_isValidDate(const std::wstring& date);
std::wstring TutoresSelect_treatDataAppointment(std::wstring dado, int number);
std::string TutoresSelect_WideToUTF8(const std::wstring& wstr);
std::wstring TutoresSelect_UTF8ToWide(const std::string& str);
void TutoresSelect_ConfigurarScrollBarsAgendamento(HWND hWnd);
void TutoresSelect_AtualizarPosicoesControlesAgendamento(HWND hWnd);
int TutoresSelect_sqlite_callback(void* data, int argc, char** argv, char** azColName);
void TutoresSelect_checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData);
bool TutoresSelect_deleteRecordById(const std::string& databasePath, int id, HWND hWnd);
void TutoresSelect_CriarBotoesTabela(HWND hWnd);
void TutoresSelect_ConfigurarScrollBars(HWND hWnd);
void TutoresSelect_AtualizarPosicoesBotoes(HWND hWnd);
void TutoresSelect_updateWindow(LPCWSTR className);
void TutoresSelect_createHeaderFilters(HDC hdc, HWND hWnd);
void TutoresSelect_criarInputsFilters(HWND hWnd);
void TutoresSelect_AtualizarPosicoesInputs(HWND hWnd);
void TutoresSelect_verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern);
void TutoresSelect_selectDB();
void TutoresSelect_createOrderBtn(HWND hWnd);
void TutoresSelect_AtualizarPosicoesOrder(HWND hWnd);
int TutoresSelect_pragma_callback(void* data, int argc, char** argv, char** azColName);
void TutoresSelect_selectHeaderDB();
void TutoresSelect_createHeaderTable(HWND hWnd, HDC hdc);
void TutoresSelect_createInputLimit(HWND hWnd);
void TutoresSelect_AtualizarPosicoesLimit(HWND hWnd);
void TutoresSelect_handleLimitChange(HWND hComboBox);
void TutoresSelect_createBtnPageLimit(HWND hWnd);
void TutoresSelect_AtualizarPosicoesOffset(HWND hWnd);
void TutoresSelect_mudarPagina(int id);
void TutoresSelect_MudarIconeDoBotao(HWND hButton, int novoIconeID);
std::wstring TutoresSelect_toLower(std::wstring str);
void TutoresSelect_DestroyAllControls();
void TutoresSelect_DestroyControlsFromVector(std::vector<HWND>& controls);
void TutoresSelect_SetFilterValues(const std::vector<std::wstring>& dados);
void TutoresSelect_ordenarDefinicoesValores(HWND hWnd);
void TutoresSelect_ordenarMudarIcone(HWND hWnd);
BOOL TutoresSelect_CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);

// Definição da estrutura
struct TutoresSelect_Data {
    int ano;
    int mes;
    int dia;
};

inline bool operator<=(const TutoresSelect_Data& a, const TutoresSelect_Data& b) {
    if (a.ano != b.ano) {
        return a.ano <= b.ano;
    }
    if (a.mes != b.mes) {
        return a.mes <= b.mes;
    }
    return a.dia <= b.dia;
}