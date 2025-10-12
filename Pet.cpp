// Pet.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "Pet.h"
#include "sqlite3.h"
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <locale.h>
#include "Select.h"
#include "MenuUniversal.h"
#include <Add.h>
#include <Consultar.h>

// Substitua o macro por constexpr conforme sugerido pelo VCR101
constexpr int MAX_LOADSTRING = 100;

int windowsNumber = 1;

// Instância da aplicação (global para uso em outros arquivos)
HINSTANCE hInstMenu;

// Variáveis Globais:
const wchar_t title[] = L"AGRO ANIMALPET";
HINSTANCE hInst;                                // instância atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal
const wchar_t szSelectClass[] = L"HomeClass"; // Nova classe de janela

// Declarações de encaminhamento de funções incluídas nesse módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    setlocale(LC_ALL, "");

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Coloque o código aqui.

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PET, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realize a inicialização do aplicativo:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PET));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNÇÃO: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PET);
    wcex.lpszClassName  = L"HomeClass";
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PET));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL windowClose(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int windowsNumberString = windowsNumber;

    //Converte o número para string e exibe
    //wchar_t buffer[32];
    //swprintf_s(buffer, L"Número de janelas: %d", windowsNumberString);
    //MessageBoxW(hWnd, buffer, L"Info", MB_OK | MB_ICONINFORMATION);

    if (windowsNumber == 1) {
        PostQuitMessage(0);
    }

    windowsNumber -= 1;
    
    return 0;
}

BOOL Shortcuts(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    #ifndef VK_N
    #define VK_N 0x4E // Código virtual para a tecla 'N'
    #endif

    #ifndef VK_O
    #define VK_O 0x4F // Código virtual para a tecla 'O'
    #endif

    #ifndef VK_E
    #define VK_E 0x45 // Código virtual para a tecla 'E'
    #endif

    #ifndef VK_D
    #define VK_D 0x44 // Código virtual para a tecla 'D'
    #endif

    #ifndef VK_A
    #define VK_A 0x41 // Código virtual para a tecla 'A'
    #endif

    #ifndef VK_I
    #define VK_I 0x49 // Código virtual para a tecla 'I'
    #endif

    if (GetKeyState(VK_CONTROL) & 0x8000) // Verifica se Ctrl está pressionado
    {
        switch (wParam)
        {
        case VK_I: SendMessage(hWnd, WM_COMMAND, IDM_HOME_INICIO, 0); break;
        case VK_N: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_NOVO, 0); break;
        case VK_O: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_E: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_D: SendMessage(hWnd, WM_COMMAND, IDM_ARQUIVO_CONSULTAR, 0); break;
        case VK_A: SendMessage(hWnd, WM_COMMAND, IDM_AJUDA_SOBRE, 0); break;
        }
    }

    return 0;
}

BOOL ProcessarMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Primeiro processa os comandos do menu
    LRESULT resultMenuHome = ProcessarMenuHome(hWnd, message, wParam, lParam);
    if (resultMenuHome != 0)
    {
        return resultMenuHome;
    }

    LRESULT resultMenu = ProcessarMenuArquivo(hWnd, message, wParam, lParam);
    if (resultMenu != 0)
    {
        return resultMenu;
    }

    LRESULT resultMenuAjuda = ProcessarMenuAjuda(hWnd, message, wParam, lParam);
    if (resultMenuAjuda != 0)
    {
        return resultMenuAjuda;
    }
}

// Função para criar a janela nova com menu
BOOL CreateNewWindow(HWND hWndParent, HINSTANCE hInst, LPCWSTR className, LPCWSTR windowTittle)
{
    // Procurar por uma janela da classe JanelaClasse
    HWND hSelectWnd = FindWindowW(className, NULL); // NULL ignora o título, busca apenas pela classe

    if (hSelectWnd != NULL)
    {
        SetForegroundWindow(hSelectWnd);
    }
    else {
        // Criar a janela Nova
        HWND hNewWnd = CreateWindowW(
            className,
            windowTittle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            1400, 400, // Tamanho inicial (pode ser ignorado ao maximizar)
            NULL,
            NULL,
            hInst,
            NULL
        );

        if (hNewWnd)
        {
            // Criar e associar o menu à nova janela
            HMENU hMenu = CriarMenu();
            if (hMenu)
            {
                SetMenu(hNewWnd, hMenu);
                DrawMenuBar(hNewWnd); // Atualiza a barra de menu
            }
            else
            {
                MessageBoxW(hNewWnd, L"Erro ao criar o menu na nova janela!", L"Erro", MB_OK | MB_ICONERROR);
            }

            windowsNumber += 1;
            int windowsNumberString = windowsNumber;

            // Converte o número para string e exibe
            //wchar_t buffer[32];
            //swprintf_s(buffer, L"Número de janelas: %d", windowsNumberString);
            //MessageBoxW(hWndParent, buffer, L"Info", MB_OK | MB_ICONINFORMATION);

            // Perguntar se deseja fechar a janela
            int response = MessageBoxW(hWndParent,
                L"Deseja fechar a janela atual?",
                L"Confirmação",
                MB_YESNO | MB_ICONQUESTION);

            // Verificar a resposta
            if (response == IDYES)
            {
                DestroyWindow(hWndParent);
            }

            // Maximizar a janela
            ShowWindow(hNewWnd, SW_MAXIMIZE);
            UpdateWindow(hNewWnd);

            return TRUE;
        }
        else
        {
            // Obter o código de erro
            DWORD errorCode = GetLastError();
            wchar_t errorMsg[256];
            if (FormatMessageW(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                errorMsg,
                sizeof(errorMsg) / sizeof(wchar_t),
                NULL) == 0)
            {
                // Se FormatMessage falhar, use uma mensagem genérica com o código
                swprintf_s(errorMsg, L"Erro desconhecido. Código: %lu", errorCode);
            }

            // Construir a mensagem completa
            wchar_t fullMsg[512];
            swprintf_s(fullMsg, L"Falha ao criar janela %s! Detalhes: %s", className, errorMsg);
            MessageBoxW(hWndParent, fullMsg, L"Erro", MB_OK | MB_ICONERROR);

            return FALSE;
        }
    }
}

//
//   FUNÇÃO: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de instância e cria a janela principal
//
//   COMENTÁRIOS:
//
//        Nesta função, o identificador de instâncias é salvo em uma variável global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de instância em nossa variável global

   // Registrar a nova classe de janela
   if (!InitSelect(hInstance)) {
       return FALSE;
   }

   HWND hWnd = CreateWindowW(L"HomeClass", title, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
  
   // Registrar a classe da janela Select, se ainda não registrada
   if (!RegisterSelectClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Select!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   // Registrar a classe da janela Add, se ainda não registrada
   if (!RegisterAddClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Add!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   // Registrar a classe da janela Read, se ainda não registrada
   if (!RegisterReadClass(hInst))
   {
       MessageBoxW(hWnd, L"Falha ao registrar classe da janela Read!", L"Erro", MB_OK | MB_ICONERROR);
       return FALSE;
   }

   HMENU hMenu = CriarMenu();
   if (hMenu)
   {
       SetMenu(hWnd, hMenu);
       DrawMenuBar(hWnd); // Atualiza a barra de menu
   }
   else
   {
       MessageBox(hWnd, L"Erro ao criar o menu na nova janela!", L"Erro", MB_OK | MB_ICONERROR);
   }

   ShowWindow(hWnd, SW_MAXIMIZE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNÇÃO: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de saída e retornar
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ProcessarMenu(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analise as seleções do menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Obter as dimensões da janela
            RECT rect;
            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Criar uma fonte grande
            HFONT hFont = CreateFont(
                72,                        // Altura da fonte (aumente para fonte maior)
                0,                         // Largura (0 para proporção automática)
                0,                         // Escapamento
                0,                         // Orientação
                FW_BOLD,                   // Peso (negrito)
                FALSE,                     // Itálico
                FALSE,                     // Sublinhado
                FALSE,                     // Tachado
                DEFAULT_CHARSET,           // Conjunto de caracteres
                OUT_DEFAULT_PRECIS,        // Precisão de saída
                CLIP_DEFAULT_PRECIS,       // Precisão de recorte
                ANTIALIASED_QUALITY,       // Qualidade
                DEFAULT_PITCH | FF_DONTCARE, // Tipo de pitch e família
                L"Arial"                   // Nome da fonte (pode mudar para outra, ex.: "Times New Roman")
            );
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            // Configurar texto sem fundo
            SetBkMode(hdc, TRANSPARENT);

            // Calcular posição central para o texto
            const wchar_t* text = L"AGRO ANIMALPET";
            SIZE textSize;
            GetTextExtentPoint32W(hdc, text, wcslen(text), &textSize);
            int x = (width - textSize.cx) / 2;  // Centralizar horizontalmente
            int y = (height - textSize.cy) / 2; // Centralizar verticalmente

            // Desenhar o texto
            TextOutW(hdc, x, y, text, wcslen(text));

            // Restaurar a fonte antiga e liberar recursos
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_KEYDOWN:
    {
        Shortcuts(hWnd, message, wParam, lParam);
        break;
    }
    case WM_DESTROY:
        windowClose(hWnd, message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
