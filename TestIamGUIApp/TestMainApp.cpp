#include "pch.h"
#include "GameTimer.h"
#include "D2DRender.h"

////////////////////////////////////////////////////////////////////////////
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/////////////////////////////////////////////////////////////////////////
#include <shobjidl.h>            // IFileOpenDialog
#include <filesystem>            // C++17 std::filesystem

/////////////////////////////////////////////////////////////////////////
#include "TestMainApp.h"

using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
using namespace sample;

int playerX;
int playerY;

static bool up;
static bool left;
static bool down;
static bool right;

static XY mousePos;

struct KeyState
{
    int key;
    bool isDown;
};



/////////////////////////////////////////////////////////////////////////////
//
std::wstring ConvertToWString(const std::string& str)
{
    size_t len = 0;
    mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
    if (len == 0)
        return L"";

    std::wstring wstr(len, L'\0');
    mbstowcs_s(&len, &wstr[0], len, str.c_str(), _TRUNCATE);
    wstr.resize(len - 1); // Remove the null terminator added by mbstowcs_s  
    return wstr;
}

std::string WStringToString(const std::wstring& wstr)
{
    size_t len = 0;
    wcstombs_s(&len, nullptr, 0, wstr.c_str(), _TRUNCATE);
    if (len == 0)
        return "";
    std::string str(len, '\0');
    wcstombs_s(&len, &str[0], len, wstr.c_str(), _TRUNCATE);
    str.resize(len - 1); // Remove the null terminator added by wcstombs_s
    return str;
}


/////////////////////////////////////////////////////////////////////////////
// 
bool TestMainApp::Initialize()
{
    const wchar_t* className = L"D2DLesson2";
    const wchar_t* windowName = L"D2DLesson2";

    if (false == __super::Create(className, windowName, 1024, 800))
    {
        return false;
    }

    m_Renderer = std::make_shared<D2DRenderer>();
    m_Renderer->Initialize(m_hWnd);

    // [ImGUI] 컨텍스트 & 백엔드 초기화
    // 3-1) ImGui 컨텍스트 생성
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    
    ImGui_ImplWin32_Init(m_hWnd);

    ID3D11Device* pd3dDevice = m_Renderer->GetD3DDevice();

    // 2) 즉시 컨텍스트 얻기
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = m_Renderer->GetD3DContext();

    // [ImGUI] DirectX 11 백엔드 초기화
    ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

    // 타이머 초기화
    m_GameTimer.Reset();

    RAWINPUTDEVICE rid[2] = {};

    // 키보드
    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x06; // Keyboard
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = m_hWnd;

    // 마우스
    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x02; // Mouse
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = m_hWnd;

    if (RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
    {

    }

    //RAWINPUTDEVICE Rid[2];

    //Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    //Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    //Rid[0].dwFlags = RIDEV_NOLEGACY;    // adds mouse and also ignores legacy mouse messages
    //Rid[0].hwndTarget = 0;

    //Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    //Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    //Rid[1].dwFlags = RIDEV_NOLEGACY;    // adds keyboard and also ignores legacy keyboard messages
    //Rid[1].hwndTarget = 0;

    //if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    //{
    //    //registration failed. Call GetLastError for the cause of the error
    //}

    return true;
}

void TestMainApp::Run()
{
    MSG msg = { 0 };

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
               
            DispatchMessage(&msg);
        }
        else
        {
            UpdateTime();
            UpdateInput();
            UpdateLogic();
            Render();        
        }
    }
}
void TestMainApp::Finalize()
{
    // [ImGUI] DirectX 11 백엔드 정리
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_Renderer != nullptr)
    {
        m_Renderer->Uninitialize();
        m_Renderer.reset();
    }
}

bool TestMainApp::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN)
    {
        //switch (wParam)
        //{
        //case VK_LEFT:
        //    playerX--;
        //    break;
        //case VK_RIGHT:
        //    playerX++;
        //    break;
        //case VK_UP:
        //    playerY--;
        //    break;
        //case VK_DOWN:
        //    playerY++;
        //    break;
        //}
    }
    if (msg == WM_INPUT)
    {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

        BYTE* lpb = new BYTE[dwSize];

        //static KeyState;



        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
        {

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                RAWKEYBOARD& rk = raw->data.keyboard;

                USHORT key = rk.VKey;
                bool isDown = !(rk.Flags & RI_KEY_BREAK);  // 0: down, 1: up

                switch (key)
                {
                case 37:
                    left = !(rk.Flags & RI_KEY_BREAK);;
                    break;
                case 38:
                    up = !(rk.Flags & RI_KEY_BREAK);;
                    break;
                case 39:
                    right = !(rk.Flags & RI_KEY_BREAK);;
                    break;
                case 40:
                    down = !(rk.Flags & RI_KEY_BREAK);;
                    break;
                }


                

                if (isDown)
                {
                    std::wcout << L"Key down: " << key << std::endl;
                }
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                RAWMOUSE& rm = raw->data.mouse;

                // 상대 좌표 이동
                LONG dx = rm.lLastX;
                LONG dy = rm.lLastY;

                //std::wcout << L"Mouse move: dx=" << dx << L", dy=" << dy << std::endl;

                // 버튼 눌림 여부
                if (rm.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
                {
                    std::wcout << L"Right button down" << std::endl;

                    m_placeSprites.push_back(std::pair<XY, std::vector<SpriteAnimator>>(mousePos, m_curSprites));
                }
            }

        }



        delete[] lpb;
    }
    if(msg == WM_MOUSEMOVE)
    {
        int x = ((int)(short)LOWORD(lParam));  // LOWORD(lParam)
        int y = ((int)(short)HIWORD(lParam));  // HIWORD(lParam)

        mousePos.x = x;
        mousePos.y = y;
    }
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true; // ImGui가 메시지를 처리했으면 true 반환
    }

    return false;
}


void TestMainApp::UpdateTime()
{
    m_GameTimer.Tick();
}

void TestMainApp::UpdateInput()
{
    if (left)
        playerX--;
    if (right)
        playerX++;
    if (up)
        playerY--;
    if (down)
        playerY++;
}

void TestMainApp::UpdateLogic()
{
    LoadAssets();

    if (m_selectedAssetKey.empty()) return; // 선택된 애셋 키가 비어있으면 리턴

    const AnimationClips& clips = m_AssetManager.GetClips(m_selectedAssetKey);

    if (clips.empty()) return; // 클립이 없으면 리턴

    if (m_bChangedFile)
    {
        // 파일이 변경되었으면 애니메이션 정보 초기화
        m_curSprites.clear();
        m_bChangedFile = false;

        // 애니메이션 플레이어 생성 및 클립 설정
        for (const auto& [name, clip] : clips)
        {
            SpriteAnimator ap;

            ap.SetClip(&clip);
            ap.Reset();

            ap.name = name;

            m_curSprites.push_back(ap);

            for (auto& [xy, clip2] : m_placeSprites)
            {
                for (auto& ap2 : clip2)
                {
                    if (ap2.name == name)
                        ap2.SetClip(&clip);
                }
            }
        }
    }

    // 애니메이션 플레이어 업데이트
    for (auto& sprite : m_curSprites)
    {
        sprite.Update(m_GameTimer.DeltaTime());
    }

    for (auto& [xy, clip] : m_placeSprites)
    {
        for (auto& ap : clip)
        {
            ap.Update(m_GameTimer.DeltaTime());
        }
    }
}


void TestMainApp::Render()
{
    if (m_Renderer == nullptr) return;

   
    m_Renderer->RenderBegin();

    int count = m_curSprites.size();
    // 여러 애니메이션을 모두 보여주는 렌더링
    // 화면 중앙을 기준으로 애니메이션을 나열
    int animationIndex = 0;
    for (auto& ap : m_curSprites)
    {
        if (ap.IsValid() == false) continue; // 유효하지 않은 플레이어는 스킵

        const Frame& frame = ap.GetCurrentFrame();

        int xOffset = static_cast<float>(animationIndex * frame.Width()) + playerX;
        int yOffset = 300 + playerY;
       
        D2D1_RECT_F renderRect = D2D1::RectF(xOffset, yOffset, xOffset + frame.Width(), yOffset + frame.Height());

        if (ap.GetClip()->GetBitmap() == nullptr)
            std::wcout << L"망함" << std::endl;

        m_Renderer->DrawBitmap(ap.GetClip()->GetBitmap(), renderRect, frame.ToRectF());

        animationIndex++;
    }





    for (auto& [xy, clip] : m_placeSprites)
    {
        int animationIndex = 0;
        for (auto& ap : clip)
        {
            const Frame& frame = ap.GetCurrentFrame();

            int xOffset = xy.x + static_cast<float>(animationIndex * frame.Width());
            int yOffset = xy.y;

            D2D1_RECT_F renderRect = D2D1::RectF(xOffset, yOffset, xOffset + frame.Width(), yOffset + frame.Height());

            if (ap.GetClip()->GetBitmap() == nullptr)
                std::wcout << L"망함" << std::endl;

            if (renderRect.left - renderRect.right == 0)
                continue;

            m_Renderer->DrawBitmap(ap.GetClip()->GetBitmap(), renderRect, frame.ToRectF());
            animationIndex++;
        }
    }

    m_Renderer->RenderEnd(false);

    RenderImGUI();

    // Present, ImGUI 때문에 enddraw 와 분리
    m_Renderer->Present();
}


void TestMainApp::RenderImGUI()
{
    ID3D11DeviceContext* pd3dDeviceContext = nullptr;
    pd3dDeviceContext = m_Renderer->GetD3DContext();
    ID3D11RenderTargetView* rtvs[] = { m_Renderer->GetD3DRenderTargetView() };

    if (pd3dDeviceContext == nullptr || rtvs[0] == nullptr)
    {
        return; // 렌더링 컨텍스트나 뷰가 없으면 리턴
    }
    m_Renderer->GetD3DContext()->OMSetRenderTargets(1, rtvs, nullptr);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 데모 UI 창 표시
    //static bool showDemo = true;
    //ImGui::ShowDemoWindow(&showDemo);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // 메뉴 아이템을 누르면 패널 열기 플래그 토글
            if (ImGui::MenuItem("Open Folder", "Ctrl+O"))
            {
                m_showFolderPanel = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // 모달리스 패널로 변경
    if (m_showFolderPanel)
    {
        ImGui::Begin("Folder Browser", &m_showFolderPanel, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::Button("Browse..."))
        {
            BrowseForFolder();
            m_pathInput = std::filesystem::path(m_folderPath).u8string();
        }

        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            if (false == m_folderPath.empty())
            {
                UpdateFileList();
                m_selectedFile.clear();
            }
        }

        ImGui::Text("Folder: %ls", m_folderPath.c_str());

        // 파일 목록 렌더링
        if (false == m_folderPath.empty())
        {
            if (ImGui::BeginListBox("Files", ImVec2(300.0f, 8 * ImGui::GetTextLineHeightWithSpacing())))
            {
                for (const auto& name : m_fileList)
                {
                    bool isSelected = (m_selectedFile == name);
                    if (ImGui::Selectable(WStringToString(name).c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        m_selectedFile = name;
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    {
                        m_selectedFile = name;
                    }
                }

                ImGui::EndListBox();
            }

            if (!m_selectedFile.empty())
            {
                ImGui::Text("Selected File: %ls", m_selectedFile.c_str());
            }
        }

        ImGui::End(); // Folder Browser 패널 끝
    }

    // ImGui 렌더링
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void TestMainApp::LoadAssets()
{
    std::filesystem::path fullPath =
        m_folderPath / std::filesystem::path(m_selectedFile);

    auto ext = fullPath.extension();
    static std::filesystem::path ext2;
    if (ext.empty()) return; // 확장자가 없으면 리턴

    std::filesystem::path keyPath = fullPath;
    keyPath.replace_extension(); // 확장자 제거하고 키로 쓸꺼에요

    std::wstring keyWide = keyPath.wstring();



    if (keyWide == m_selectedAssetKey && !ext2.empty() && ext2 == ext)
    {
        //m_bChangedFile = true;

        return;
    }
    m_selectedAssetKey = keyWide; // 선택된 파일 키 저장
    ext2 = ext;

    if (m_AssetManager.GetIsJson(keyWide))
    {
        //for (auto& sprite : m_curSprites)
        //{
        //    sprite.Reset();
        //}

        m_bChangedFile = true;
        return;
    }

    if (ext == L".png")
    {
        m_AssetManager.LoadTexture(m_Renderer.get(), keyWide, fullPath);
    }
    else if (ext == L".json")
    {
        m_AssetManager.LoadAseprite(m_Renderer.get(), keyWide, fullPath);
    }


    m_bChangedFile = true; // 파일이 변경되었음을 표시
}


void TestMainApp::OnResize(int width, int height)
{
    __super::OnResize(width, height);

    if (m_Renderer != nullptr) m_Renderer->Resize(width, height);
}

void TestMainApp::OnClose()
{
    std::cout << "OnClose" << std::endl;
}

void TestMainApp::BrowseForFolder()
{
    HRESULT hr;
    IFileOpenDialog* pDialog = nullptr;

    // COM 대화상자 생성
    hr = CoCreateInstance(
        CLSID_FileOpenDialog, nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDialog)
    );

    if (FAILED(hr) || !pDialog) return;

    // 폴더 선택 모드로 설정
    DWORD opts = 0;
    if (SUCCEEDED(pDialog->GetOptions(&opts)))
        pDialog->SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

    // 다이얼로그 표시
    hr = pDialog->Show(m_hWnd);

    if (SUCCEEDED(hr))
    {
        IShellItem* pItem = nullptr;
        hr = pDialog->GetResult(&pItem);

        if (SUCCEEDED(hr) && pItem)
        {
            PWSTR pszFolder = nullptr;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolder);
            if (SUCCEEDED(hr) && pszFolder)
            {
                m_folderPath = pszFolder;        // 선택된 폴더 경로 저장
                CoTaskMemFree(pszFolder);
            }
            pItem->Release();
        }   
    }

    pDialog->Release();
}

void TestMainApp::UpdateFileList()
{
    m_fileList.clear();
    for (const auto& entry : std::filesystem::directory_iterator(m_folderPath))
    {
        if (entry.is_regular_file())
        {
            // 파일만 추가, 디렉토리는 제외
            if (entry.path().extension() == L".png" ||
                entry.path().extension() == L".json")
                // 이미지 파일만 추가
                m_fileList.push_back(entry.path().filename().wstring());
        }
    }
}

