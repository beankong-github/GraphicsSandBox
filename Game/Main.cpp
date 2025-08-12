#include "pch.h"  
#pragma comment(lib, "user32.lib")  

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int main()
{
    // 현재 실행 모듈의 인스턴스 핸들. 창 클래스 등록/창 생성 시 필요.
	HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* windowClassName = L"Game Window";
    Vector2 resolution(800, 600);
	
    // 윈도우 클래스 정보 채우기
	WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);                      // 구조체 크기 (RegisterClassExW가 요구)
    wc.style = 0;                                          // CS_HREDRAW/CS_VREDRAW 같은 스타일 옵션 가능
    wc.lpfnWndProc = WndProc;            // 메시지 콜백(필수)
    wc.cbClsExtra = 0;                              // 클래스 데이터 추가 공간(일반적으로 0)
    wc.cbWndExtra = 0;                           // 윈도우 인스턴스별 추가 공간(일반적으로 0)
    wc.hInstance = hInst;                       // 우리 앱의 인스턴스
    wc.hIcon = nullptr;                             // 큰 아이콘(없으면 기본)
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);                    // 기본 화살표 커서
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);      // 배경 브러시(흰 바탕 등 시스템 색)
    wc.lpszMenuName = nullptr;                           // 리소스 메뉴 없으면 nullptr
    wc.lpszClassName = windowClassName;       // 위에서 정한 클래스명
    wc.hIconSm = nullptr;                                       // 작은 아이콘(없으면 기본)


    // 윈도우 클래스 등록
    if (!RegisterClassExW(&wc)) {
        // 실패 시 보통 GetLastError()로 원인 확인. 여기선 간단히 종료.
        return 1;
    }

     HWND hWnd = CreateWindowExW(
        0,                                                                       // 확장 스타일(필요하면 WS_EX_TOPMOST 등 사용)
         windowClassName,                                    // '등록한' 윈도우 클래스 이름
        L"Hello window!",                                    // 창 타이틀
        WS_OVERLAPPEDWINDOW,                     // 창 스타일(표준 타이틀바/테두리/버튼)
        CW_USEDEFAULT, CW_USEDEFAULT,    // 초기 위치 (OS가 적절히 배치)
         (int)resolution.x,
         (int)resolution.y,               // 초기 크기(클라이언트가 아닌 윈도우 외곽 기준)
        nullptr,                // 부모 핸들(최상위 창이면 nullptr)
        nullptr,                // 메뉴 핸들(없으면 nullptr)
        hInst,                  // 인스턴스 핸들
        nullptr                 // 생성 시 전달할 사용자 데이터(보통 nullptr)
    );
    if (!hWnd) return 1;

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);  // 현재 무효 영역이 있으면 곧바로 그리기(없으면 영향 없음)

    MSG msg{};
    
   while (true)
    {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) // 대기 없이 메세지를 처리할 수 있다.
        {
            if (msg.message == WM_QUIT)
                break; 

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
            // TODO ::  Run Code

        }
    }
   WndProc(hWnd, msg.message, msg.wParam, msg.lParam);
   
   return 0;
}