#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"
#include "AgendamentosSelect.h"
#include "Global.h"

extern int AgendamentosSelect_windowsNumber;
extern int AgendamentosSelect_g_scrollX;
extern int AgendamentosSelect_g_scrollY;
extern int AgendamentosSelect_g_contentWidth;
extern int AgendamentosSelect_g_contentHeight;
extern int AgendamentosSelect_g_clientWidth;
extern int AgendamentosSelect_g_clientHeight;
extern LPCWSTR AgendamentosSelect_error;
extern std::wstring AgendamentosSelect_mensagem;
extern LPCWSTR AgendamentosSelect_msg;
extern std::vector<HWND> AgendamentosSelect_g_editControls;
extern std::vector<HWND> AgendamentosSelect_g_editControlsEdit;
extern HWND AgendamentosSelect_g_hButton;
extern HBRUSH AgendamentosSelect_hBrushTransparent;
extern int AgendamentosSelect_g_scrollY;
extern int AgendamentosSelect_g_scrollX;
extern int AgendamentosSelect_g_clientHeight;
extern int AgendamentosSelect_g_clientWidth;
extern int AgendamentosSelect_g_contentHeight;
extern int AgendamentosSelect_g_contentWidth;
extern std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableData;
extern std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableDataFull;
extern std::vector<std::vector<std::wstring>> AgendamentosSelect_g_tableDataRowsNumber;
extern std::vector<HWND> AgendamentosSelect_g_editControlsLimit;
extern std::vector<HWND> AgendamentosSelect_g_editControlsOffsetLimit;
extern LONG_PTR AgendamentosSelect_idRecord;
extern enum AgendamentosSelect_ButtonAction { AgendamentosSelect_CONSULTAR, AgendamentosSelect_EDITAR, AgendamentosSelect_DELETAR, AgendamentosSelect_FILTRAR, AgendamentosSelect_ORDENAR, AgendamentosSelect_LIMITAR, AgendamentosSelect_OFFSET };
extern std::vector<HWND> AgendamentosSelect_g_buttons;
extern std::vector<std::wstring> AgendamentosSelect_dados;
extern int AgendamentosSelect_rowsNumber;
extern std::wstring AgendamentosSelect_dataAte;
extern std::wstring AgendamentosSelect_dataRegistroAte;
extern std::string AgendamentosSelect_orderColumn;
extern std::string AgendamentosSelect_orderAscDesc;
extern int AgendamentosSelect_limitTableRow;
extern int AgendamentosSelect_offsetTableRow;
extern int AgendamentosSelect_g_totalRowCount;
extern void AgendamentosSelect_DestroyAllOffsetButtons();
extern int AgendamentosSelect_numeroBtn;
extern int AgendamentosSelect_idNumeroUltimo;
extern std::vector<HWND> AgendamentosSelect_g_editControlsOrder;
extern bool AgendamentosSelect_g_isRedrawing;
extern int AgendamentosSelect_rowsNumberSemCabecalho;
extern LONG_PTR AgendamentosSelect_idBtnGlobal;
extern std::wstring AgendamentosSelect_btnClicado;
extern HWND AgendamentosSelect_g_hButton_consultar;
extern HWND AgendamentosSelect_g_hButton_consultar_Pet;

std::wstring AgendamentosSelect_GetCurrentDate();
std::wstring AgendamentosSelect_GetCurrentHour();

BOOL AgendamentosSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL AgendamentosSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL AgendamentosSelect_Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL AgendamentosSelect_windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL AgendamentosSelect_scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight);
BOOL AgendamentosSelect_fonte(LPCWSTR fonte, COLORREF color, HDC hdc);
BOOL AgendamentosSelect_windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size);
bool AgendamentosSelect_isNumber(const std::wstring& str);
bool AgendamentosSelect_isDecimalNumber(const std::wstring& str);
bool AgendamentosSelect_isValidTime(const std::wstring& time);
bool AgendamentosSelect_isValidDate(const std::wstring& date);
std::wstring AgendamentosSelect_treatDataAppointment(std::wstring dado, int number);
std::string AgendamentosSelect_WideToUTF8(const std::wstring& wstr);
std::wstring AgendamentosSelect_UTF8ToWide(const std::string& str);
void AgendamentosSelect_ConfigurarScrollBarsAgendamento(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesControlesAgendamento(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesControlesAgendamentoEdit(HWND hWnd);
int AgendamentosSelect_sqlite_callback(void* data, int argc, char** argv, char** azColName);
void AgendamentosSelect_checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData);
bool AgendamentosSelect_deleteRecordById(const std::string& databasePath, int id, HWND hWnd);
void AgendamentosSelect_CriarBotoesTabela(HWND hWnd);
void AgendamentosSelect_ConfigurarScrollBars(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesBotoes(HWND hWnd);
void AgendamentosSelect_updateWindow(LPCWSTR className);
void AgendamentosSelect_createHeaderFilters(HDC hdc, HWND hWnd);
void AgendamentosSelect_criarInputsFilters(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesInputs(HWND hWnd);
void AgendamentosSelect_verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern);
void AgendamentosSelect_selectDB();
void AgendamentosSelect_createOrderBtn(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesOrder(HWND hWnd);
int AgendamentosSelect_pragma_callback(void* data, int argc, char** argv, char** azColName);
void AgendamentosSelect_selectHeaderDB();
void AgendamentosSelect_createHeaderTable(HWND hWnd, HDC hdc);
void AgendamentosSelect_createInputLimit(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesLimit(HWND hWnd);
void AgendamentosSelect_handleLimitChange(HWND hComboBox);
void AgendamentosSelect_createBtnPageLimit(HWND hWnd);
void AgendamentosSelect_AtualizarPosicoesOffset(HWND hWnd);
void AgendamentosSelect_mudarPagina(int id);
void AgendamentosSelect_MudarIconeDoBotao(HWND hButton, int novoIconeID);
std::wstring AgendamentosSelect_toLower(std::wstring str);
void AgendamentosSelect_DestroyAllControls();
void AgendamentosSelect_DestroyControlsFromVector(std::vector<HWND>& controls);
void AgendamentosSelect_SetFilterValues(const std::vector<std::wstring>& dados);
void AgendamentosSelect_ordenarDefinicoesValores(HWND hWnd);
void AgendamentosSelect_ordenarMudarIcone(HWND hWnd);
BOOL AgendamentosSelect_CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);
int AgendamentosSelect_columnValue(int col);

// Definição da estrutura
struct AgendamentosSelect_Data {
    int ano;
    int mes;
    int dia;
};

inline bool operator<=(const AgendamentosSelect_Data& a, const AgendamentosSelect_Data& b) {
    if (a.ano != b.ano) {
        return a.ano <= b.ano;
    }
    if (a.mes != b.mes) {
        return a.mes <= b.mes;
    }
    return a.dia <= b.dia;
}