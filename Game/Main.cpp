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
    // ���� ���� ����� �ν��Ͻ� �ڵ�. â Ŭ���� ���/â ���� �� �ʿ�.
	HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* windowClassName = L"Game Window";
    Vector2 resolution(800, 600);
	
    // ������ Ŭ���� ���� ä���
	WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);                      // ����ü ũ�� (RegisterClassExW�� �䱸)
    wc.style = 0;                                          // CS_HREDRAW/CS_VREDRAW ���� ��Ÿ�� �ɼ� ����
    wc.lpfnWndProc = WndProc;            // �޽��� �ݹ�(�ʼ�)
    wc.cbClsExtra = 0;                              // Ŭ���� ������ �߰� ����(�Ϲ������� 0)
    wc.cbWndExtra = 0;                           // ������ �ν��Ͻ��� �߰� ����(�Ϲ������� 0)
    wc.hInstance = hInst;                       // �츮 ���� �ν��Ͻ�
    wc.hIcon = nullptr;                             // ū ������(������ �⺻)
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);                    // �⺻ ȭ��ǥ Ŀ��
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);      // ��� �귯��(�� ���� �� �ý��� ��)
    wc.lpszMenuName = nullptr;                           // ���ҽ� �޴� ������ nullptr
    wc.lpszClassName = windowClassName;       // ������ ���� Ŭ������
    wc.hIconSm = nullptr;                                       // ���� ������(������ �⺻)


    // ������ Ŭ���� ���
    if (!RegisterClassExW(&wc)) {
        // ���� �� ���� GetLastError()�� ���� Ȯ��. ���⼱ ������ ����.
        return 1;
    }

     HWND hWnd = CreateWindowExW(
        0,                                                                       // Ȯ�� ��Ÿ��(�ʿ��ϸ� WS_EX_TOPMOST �� ���)
         windowClassName,                                    // '�����' ������ Ŭ���� �̸�
        L"Hello window!",                                    // â Ÿ��Ʋ
        WS_OVERLAPPEDWINDOW,                     // â ��Ÿ��(ǥ�� Ÿ��Ʋ��/�׵θ�/��ư)
        CW_USEDEFAULT, CW_USEDEFAULT,    // �ʱ� ��ġ (OS�� ������ ��ġ)
         (int)resolution.x,
         (int)resolution.y,               // �ʱ� ũ��(Ŭ���̾�Ʈ�� �ƴ� ������ �ܰ� ����)
        nullptr,                // �θ� �ڵ�(�ֻ��� â�̸� nullptr)
        nullptr,                // �޴� �ڵ�(������ nullptr)
        hInst,                  // �ν��Ͻ� �ڵ�
        nullptr                 // ���� �� ������ ����� ������(���� nullptr)
    );
    if (!hWnd) return 1;

    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);  // ���� ��ȿ ������ ������ ��ٷ� �׸���(������ ���� ����)

    MSG msg{};
    
   while (true)
    {
        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) // ��� ���� �޼����� ó���� �� �ִ�.
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