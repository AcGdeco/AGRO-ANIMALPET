#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"
#include "PetsSelect.h"
#include "Global.h"

extern int PetsSelect_windowsNumber;
extern int PetsSelect_g_scrollX;
extern int PetsSelect_g_scrollY;
extern int PetsSelect_g_contentWidth;
extern int PetsSelect_g_contentHeight;
extern int PetsSelect_g_clientWidth;
extern int PetsSelect_g_clientHeight;
extern LPCWSTR PetsSelect_error;
extern std::wstring PetsSelect_mensagem;
extern LPCWSTR PetsSelect_msg;
extern std::vector<HWND> PetsSelect_g_editControls;
extern std::vector<HWND> PetsSelect_g_editControls_edit;
extern HWND PetsSelect_g_hButton;
extern HBRUSH PetsSelect_hBrushTransparent;
extern int PetsSelect_g_scrollY;
extern int PetsSelect_g_scrollX;
extern int PetsSelect_g_clientHeight;
extern int PetsSelect_g_clientWidth;
extern int PetsSelect_g_contentHeight;
extern int PetsSelect_g_contentWidth;
extern std::vector<std::vector<std::wstring>> PetsSelect_g_tableData;
extern std::vector<std::vector<std::wstring>> PetsSelect_g_tableDataFull;
extern std::vector<std::vector<std::wstring>> PetsSelect_g_tableDataRowsNumber;
extern std::vector<HWND> PetsSelect_g_editControlsLimit;
extern std::vector<HWND> PetsSelect_g_editControlsOffsetLimit;
extern LONG_PTR PetsSelect_idRecord;
extern enum PetsSelect_ButtonAction { PetsSelect_CONSULTAR, PetsSelect_EDITAR, PetsSelect_DELETAR, PetsSelect_FILTRAR, PetsSelect_ORDENAR, PetsSelect_LIMITAR, PetsSelect_OFFSET };
extern std::vector<HWND> PetsSelect_g_buttons;
extern std::vector<std::wstring> PetsSelect_dados;
extern int PetsSelect_rowsNumber;
extern std::wstring PetsSelect_dataAte;
extern std::wstring PetsSelect_dataRegistroAte;
extern std::string PetsSelect_orderColumn;
extern std::string PetsSelect_orderAscDesc;
extern int PetsSelect_limitTableRow;
extern int PetsSelect_offsetTableRow;
extern int PetsSelect_g_totalRowCount;
extern void PetsSelect_DestroyAllOffsetButtons();
extern int PetsSelect_numeroBtn;
extern int PetsSelect_idNumeroUltimo;
extern std::vector<HWND> PetsSelect_g_editControlsOrder;
extern bool PetsSelect_g_isRedrawing;
extern int PetsSelect_rowsNumberSemCabecalho;
extern LONG_PTR PetsSelect_idBtnGlobal;
extern std::wstring PetsSelect_btnClicado;

std::wstring PetsSelect_GetCurrentDate();
std::wstring PetsSelect_GetCurrentHour();

BOOL PetsSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL PetsSelect_ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL PetsSelect_Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL PetsSelect_windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL PetsSelect_scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight);
BOOL PetsSelect_fonte(LPCWSTR fonte, COLORREF color, HDC hdc);
BOOL PetsSelect_windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size);
bool PetsSelect_isNumber(const std::wstring& str);
bool PetsSelect_isDecimalNumber(const std::wstring& str);
bool PetsSelect_isValidTime(const std::wstring& time);
bool PetsSelect_isValidDate(const std::wstring& date);
std::wstring PetsSelect_treatDataAppointment(std::wstring dado, int number);
std::string PetsSelect_WideToUTF8(const std::wstring& wstr);
std::wstring PetsSelect_UTF8ToWide(const std::string& str);
void PetsSelect_ConfigurarScrollBarsAgendamento(HWND hWnd);
void PetsSelect_AtualizarPosicoesControlesAgendamento(HWND hWnd);
int PetsSelect_sqlite_callback(void* data, int argc, char** argv, char** azColName);
void PetsSelect_checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData);
bool PetsSelect_deleteRecordById(const std::string& databasePath, int id, HWND hWnd);
void PetsSelect_CriarBotoesTabela(HWND hWnd);
void PetsSelect_ConfigurarScrollBars(HWND hWnd);
void PetsSelect_AtualizarPosicoesBotoes(HWND hWnd);
void PetsSelect_updateWindow(LPCWSTR className);
void PetsSelect_createHeaderFilters(HDC hdc, HWND hWnd);
void PetsSelect_criarInputsFilters(HWND hWnd);
void PetsSelect_AtualizarPosicoesInputs(HWND hWnd);
void PetsSelect_verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern);
void PetsSelect_selectDB();
void PetsSelect_createOrderBtn(HWND hWnd);
void PetsSelect_AtualizarPosicoesOrder(HWND hWnd);
int PetsSelect_pragma_callback(void* data, int argc, char** argv, char** azColName);
void PetsSelect_selectHeaderDB();
void PetsSelect_createHeaderTable(HWND hWnd, HDC hdc);
void PetsSelect_createInputLimit(HWND hWnd);
void PetsSelect_AtualizarPosicoesLimit(HWND hWnd);
void PetsSelect_handleLimitChange(HWND hComboBox);
void PetsSelect_createBtnPageLimit(HWND hWnd);
void PetsSelect_AtualizarPosicoesOffset(HWND hWnd);
void PetsSelect_mudarPagina(int id);
void PetsSelect_MudarIconeDoBotao(HWND hButton, int novoIconeID);
std::wstring PetsSelect_toLower(std::wstring str);
void PetsSelect_DestroyAllControls();
void PetsSelect_DestroyControlsFromVector(std::vector<HWND>& controls);
void PetsSelect_SetFilterValues(const std::vector<std::wstring>& dados);
void PetsSelect_ordenarDefinicoesValores(HWND hWnd);
void PetsSelect_ordenarMudarIcone(HWND hWnd);
BOOL PetsSelect_CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);
void PetsSelect_AtualizarPosicoesControlesAgendamentoEdit(HWND hWnd);

// Definição da estrutura
struct PetsSelect_Data {
    int ano;
    int mes;
    int dia;
};

inline bool operator<=(const PetsSelect_Data& a, const PetsSelect_Data& b) {
    if (a.ano != b.ano) {
        return a.ano <= b.ano;
    }
    if (a.mes != b.mes) {
        return a.mes <= b.mes;
    }
    return a.dia <= b.dia;
}