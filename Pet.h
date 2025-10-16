#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"

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
extern enum ButtonAction { CONSULTAR, EDITAR, DELETAR };
extern std::vector<HWND> g_buttons;

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
void checarInput(HWND hinput, int col, std::wstring word);
bool deleteRecordById(const std::string& databasePath, int id, HWND hWnd);
void RecarregarDadosTabela(HWND hWnd);
void CriarBotoesTabela(HWND hWnd);
void ConfigurarScrollBars(HWND hWnd);
void AtualizarPosicoesBotoes(HWND hWnd);
void updateWindow(LPCWSTR className);
void createInputsFilters(HDC hdc, HWND hWnd);