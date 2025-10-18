#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"
#include "Select.h"

extern int windowsNumber;

extern int g_scrollX;
extern int g_scrollY;
extern int g_contentWidth;
extern int g_contentHeight;
extern int g_clientWidth;
extern int g_clientHeight;
extern LPCWSTR error;
extern std::wstring mensagem;
extern LPCWSTR msg;
extern std::vector<HWND> g_editControls;
extern HWND g_hButton;
extern HBRUSH hBrushTransparent;
extern int g_scrollY;
extern int g_scrollX;
extern int g_clientHeight;
extern int g_clientWidth;
extern int g_contentHeight;
extern int g_contentWidth;
extern std::vector<std::vector<std::wstring>> g_tableData;
extern std::vector<std::vector<std::wstring>> g_tableDataFull;
extern LONG_PTR idRecord;
extern enum ButtonAction { CONSULTAR, EDITAR, DELETAR, FILTRAR, ORDENAR };
extern std::vector<HWND> g_buttons;
extern std::vector<std::wstring> dados;
extern int rowsNumber;
extern std::wstring dataAte;
extern std::wstring dataRegistroAte;
extern std::string orderColumn;
extern std::string orderAscDesc;

std::wstring GetCurrentDate();
std::wstring GetCurrentHour();

BOOL CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle);
BOOL ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL scroll(HWND hWnd, int scrollX, int scrollY, int contentWidth, int contentHeight, int clientWidth, int clientHeight);
BOOL fonte(LPCWSTR fonte, COLORREF color, HDC hdc);
BOOL windowsTitle(HDC hdc, int startX, int startY, LPCWSTR tit, int size);
bool isNumber(const std::wstring& str);
bool isDecimalNumber(const std::wstring& str);
bool isValidTime(const std::wstring& time);
bool isValidDate(const std::wstring& date);
std::wstring treatDataAppointment(std::wstring dado, int number);
std::string WideToUTF8(const std::wstring& wstr);
std::wstring UTF8ToWide(const std::string& str);
void ConfigurarScrollBarsAgendamento(HWND hWnd);
void AtualizarPosicoesControlesAgendamento(HWND hWnd);
int sqlite_callback(void* data, int argc, char** argv, char** azColName);
void checarInput(HWND hinput, int col, std::wstring word, std::wstring tableData);
bool deleteRecordById(const std::string& databasePath, int id, HWND hWnd);
void RecarregarDadosTabela(HWND hWnd);
void CriarBotoesTabela(HWND hWnd);
void ConfigurarScrollBars(HWND hWnd);
void AtualizarPosicoesBotoes(HWND hWnd);
void updateWindow(LPCWSTR className);
void createHeaderFilters(HDC hdc, HWND hWnd);
void criarInputsFilters(HWND hWnd);
void AtualizarPosicoesInputs(HWND hWnd);
void verificarFiltro(const std::vector<std::wstring>& dados, std::vector<int>& naoDesenharIntern);
void selectDB();
void createOrderBtn(HWND hWnd);
void AtualizarPosicoesOrder(HWND hWnd);

// Definição da estrutura
struct Data {
    int ano;
    int mes;
    int dia;
};

inline bool operator<=(const Data& a, const Data& b) {
    if (a.ano != b.ano) {
        return a.ano <= b.ano;
    }
    if (a.mes != b.mes) {
        return a.mes <= b.mes;
    }
    return a.dia <= b.dia;
}