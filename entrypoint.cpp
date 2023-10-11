#ifndef ENTRYPOINT_CPP
#define ENTRYPOINT_CPP
#include "impl/include.hpp"



auto main( ) -> void
{

	// attach to our process 
	globals->width = GetSystemMetrics(SM_CXSCREEN); globals->height = GetSystemMetrics(SM_CYSCREEN);

	MessageBoxA(0, skCrypt("Press [OK] In Lobby."), skCrypt("Alert"), MB_OK);

	auto result = request->attach(L"cod.exe");
	if (!result) {
		std::printf("\n [log] -> failed to initialize driver.\n");
		std::cin.get();
	}
	std::printf("\n [log] -> driver initialized.\n");

	if (Hook->Setup() != HOOK_INFORMATION::HOOKS_SUCCESSFUL)
		std::printf(" [hook] -> failed to setup hook.\n ");

	if (Renderer->Setup() != RENDER_INFORMATION::RENDER_SETUP_SUCCESSFUL)
		std::printf(" [render] -> failed to setup render.\n ");

	if (Game->Setup() != GAME_INFORMATION::GAME_SETUP_SUCCESSFUL)
		std::printf(" [game] -> failed to setup game.\n ");

	Renderer->Render();
	std::cin.get();

}

#endif // !ENTRYPOINT_CPP

