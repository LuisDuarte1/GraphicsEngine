/* This is required to not use the window.h header in header files and make their macros global 
to the engine  (which is bad, and may cause conflicts with the std and my own functions).
*/

extern "C"{
    #ifndef WIN32_BASE_TYPES_H
    #define WIN32_BASE_TYPES_H
    #include <stdio.h>
    #include <stdint.h>
    #ifndef _WINDEF_
    #define _WINDEF_
typedef uint64_t            WPARAM;
typedef int64_t            LPARAM;
typedef int64_t            LRESULT;
#define CONST const
#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HCURSOR);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HMODULE);
typedef char *NPSTR, *LPSTR, *PSTR;
typedef CONST char *LPCSTR, *PCSTR;
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
typedef LRESULT (CALLBACK* WNDPROC)(HWND, uint32_t, WPARAM, LPARAM);
#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINUSERAPI DECLSPEC_IMPORT
#define WINBASEAPI DECLSPEC_IMPORT
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_OWNDC            0x0020
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#define IDC_ARROW           MAKEINTRESOURCE(32512)
#define NULL 0

typedef struct tagWNDCLASSEXA {
    uint32_t        cbSize;
    /* Win 3.x */
    uint32_t        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
    /* Win 4.0 */
    HICON       hIconSm;
} WNDCLASSEXA, *PWNDCLASSEXA, *NPWNDCLASSEXA, *LPWNDCLASSEXA;
typedef WNDCLASSEXA WNDCLASS;
    #endif
    #endif
}