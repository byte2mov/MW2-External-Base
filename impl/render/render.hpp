#pragma once
#include <vector>
#include "../../dependencies/framework/imgui_internal.h"
#include "../../dependencies/framework/imgui.h"
#include "../../dependencies/framework/hashes.hpp"
enum RENDER_INFORMATION : int {
	RENDER_HIJACK_FAILED = 0,
	RENDER_IMGUI_FAILED = 1,
	RENDER_DRAW_FAILED = 2,
	RENDER_SETUP_SUCCESSFUL = 3,
    RENDER_COD_FAILED = 4,
};
using namespace ImGui;

HWND window_handle;

ID3D11Device* d3d_device;
ID3D11DeviceContext* d3d_device_ctx;
IDXGISwapChain* d3d_swap_chain;
ID3D11RenderTargetView* d3d_render_target;
D3DPRESENT_PARAMETERS d3d_present_params;
#define vec4( r, g, b, a ) ImColor( r / 255.f, g / 255.f, b / 255.f, a )

enum e_fonts : int {

    REGULAR = 0,
    MEDIUM,
    BOLD,
    LOGO
};

using namespace std;

IDirect3DTexture9* background = nullptr;
HWND hwnd;
namespace render {
    class c_render {

        HWND fortnite_window = { };

    public:

        auto Setup() -> RENDER_INFORMATION {

            fortnite_window = FindWindowA(nullptr, skCrypt("Call of Duty® HQ").decrypt());
            if (!fortnite_window) {
                return RENDER_COD_FAILED;
            }

            if (!render::c_render::Hijack()) return RENDER_HIJACK_FAILED;

            if (!render::c_render::ImGui()) return RENDER_IMGUI_FAILED;

            return RENDER_SETUP_SUCCESSFUL;

        }

        auto get_screen_status() -> bool
        {
            if (this->fortnite_window == GetForegroundWindow()) {
                return true;
            }

            if (this->fortnite_window == GetActiveWindow()) {
                return true;
            }

            if (GetActiveWindow() == GetForegroundWindow()) {
                return true;
            }

            return false;
        }

        auto Render() -> bool {

            static RECT rect_og;
            MSG msg = { NULL };
            ZeroMemory(&msg, sizeof(MSG));

            while (msg.message != WM_QUIT)
            {

                UpdateWindow(window_handle);
                ShowWindow(window_handle, SW_SHOW);

                if ((PeekMessageA)(&msg, window_handle, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                ImGuiIO& io = ImGui::GetIO();
                io.ImeWindowHandle = window_handle;
                io.DeltaTime = 1.0f / 60.0f;


                POINT p_cursor;
                GetCursorPos(&p_cursor);
                io.MousePos.x = p_cursor.x;
                io.MousePos.y = p_cursor.y;

                if (GetAsyncKeyState(VK_LBUTTON)) {
                    io.MouseDown[0] = true;
                    io.MouseClicked[0] = true;
                    io.MouseClickedPos[0].x = io.MousePos.x;
                    io.MouseClickedPos[0].x = io.MousePos.y;
                }
                else
                    io.MouseDown[0] = false;

                render::c_render::Draw();

            }
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            (DestroyWindow)(window_handle);

            return true;

        }

        auto ImGui() -> bool {

            DXGI_SWAP_CHAIN_DESC swap_chain_description;
            ZeroMemory(&swap_chain_description, sizeof(swap_chain_description));
            swap_chain_description.BufferCount = 2;
            swap_chain_description.BufferDesc.Width = 0;
            swap_chain_description.BufferDesc.Height = 0;
            swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
            swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
            swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_description.OutputWindow = window_handle;
            swap_chain_description.SampleDesc.Count = 1;
            swap_chain_description.SampleDesc.Quality = 0;
            swap_chain_description.Windowed = 1;
            swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

            D3D_FEATURE_LEVEL d3d_feature_lvl;

            const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

            D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &swap_chain_description, &d3d_swap_chain, &d3d_device, &d3d_feature_lvl, &d3d_device_ctx);

            ID3D11Texture2D* pBackBuffer;

            d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

            d3d_device->CreateRenderTargetView(pBackBuffer, NULL, &d3d_render_target);

            pBackBuffer->Release();

            IMGUI_CHECKVERSION();

            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO();

            // Our state
            ImVec4 clear_color = GetStyleColorVec4(ImGuiCol_WindowBg);
            io.IniFilename = NULL;

            (void)io;

            io.Fonts->AddFontFromMemoryTTF(burbank, sizeof(burbank), 16.0f);

            ImGui_ImplWin32_Init(window_handle);

            ImGui_ImplDX11_Init(d3d_device, d3d_device_ctx);

            d3d_device->Release();

            return true;

        }

        auto Hijack() -> bool
        {

            window_handle = FindWindowA(skCrypt("MedalOverlayClass").decrypt(), skCrypt("MedalOverlay").decrypt());
            if (!window_handle) return false;
            MARGINS Margin = { -1 };
            DwmExtendFrameIntoClientArea(window_handle, &Margin);
            SetWindowPos(window_handle, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
            ShowWindow(window_handle, SW_SHOW);
            UpdateWindow(window_handle);
            return true;
        }
        RECT rect;

        auto Menu() -> void
        {
            ImGui::StyleColorsDark();

            ImGui::SetWindowSize(ImVec2(400, 400));

            ImGui::Begin("Seemo MW2 External");
          
            ImGui::Checkbox("Aimbot", &globals->aimbot);
            ImGui::SliderFloat("Aimbot Smoothness",  &globals->smooth, 1, 20);
            ImGui::Checkbox("Box ESP", &globals->box);
            ImGui::Checkbox("Distance ESP", &globals->distance);

            ImGui::End();
        }
    

		auto Draw() -> void {

			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (GetAsyncKeyState(VK_INSERT) & 1)
				globals->render_menu = !globals->render_menu;

            if ( get_screen_status( ) ) {

                if ( globals->render_field_of_view ) {
                    ImGui::GetForegroundDrawList( )->AddCircle( ImVec2( globals->width / 2, globals->height / 2 ), globals->field_of_view, ImColor( 255, 255, 255 ), 64, 1 );
                }

                Game->ActorLoop( );

                if ( globals->render_menu ) {

                    Menu( );
                }

            }

			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
			d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			d3d_swap_chain->Present(0, 0);

		}
	};
} static render::c_render* Renderer = new render::c_render();

