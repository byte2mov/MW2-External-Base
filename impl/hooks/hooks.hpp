#include <Windows.h>
#include "../../impl/importer.hpp"
#include "../../impl/skcrypt.hpp"

enum InjectedInputMouseOptions
{
	move = 1,
	left_up = 4,
	left_down = 2,
	right_up = 8,
	right_down = 16
};

struct InjectedInputMouseInfo
{
	int move_direction_x;
	int move_direction_y;
	unsigned int mouse_data;
	InjectedInputMouseOptions mouse_options;
	unsigned int time_offset_in_miliseconds;
	void* extra_info;
};

bool(*_NtUserInjectMouseInput)(InjectedInputMouseInfo*, int) = nullptr;

enum HOOK_INFORMATION : int {
	HOOK_INJECTMOUSE_FAILED = 0,
	HOOKS_SUCCESSFUL = 1
};

namespace hook
{
	class c_hook {
		public:
		auto WINAPI Setup() -> BOOLEAN {
			auto win32u = LI_FN(LoadLibraryA).safe_cached()(skCrypt("win32u.dll").decrypt());
			void* NtUserInjectMouseInputAddress = (void*)LI_FN(GetProcAddress).safe_cached()(win32u, skCrypt("NtUserInjectMouseInput").decrypt());
			if (!NtUserInjectMouseInputAddress) return HOOK_INFORMATION::HOOK_INJECTMOUSE_FAILED;
			*(void**)&_NtUserInjectMouseInput = NtUserInjectMouseInputAddress;
			LI_FN(FreeLibrary).get()(win32u);
			return HOOK_INFORMATION::HOOKS_SUCCESSFUL;
		}
	};
} static hook::c_hook* Hook = new hook::c_hook();

namespace inject {
	class c_inject {
	public:
		auto WINAPI set_cursor_position(int X, int Y, unsigned int time = 0) -> BOOLEAN {
			InjectedInputMouseInfo temp = {};
			temp.mouse_data = 0;
			temp.mouse_options = InjectedInputMouseOptions::move;
			temp.move_direction_x = X;
			temp.move_direction_y = Y;
			temp.time_offset_in_miliseconds = time;
			return _NtUserInjectMouseInput(&temp, 1);
		}

		bool left_down(int x = 0, int y = 0)
		{
			InjectedInputMouseInfo temp = {};
			temp.mouse_data = 0;
			temp.mouse_options = InjectedInputMouseOptions::left_down;
			temp.move_direction_x = x;
			temp.move_direction_y = y;
			temp.time_offset_in_miliseconds = 0;
			return _NtUserInjectMouseInput(&temp, 1);
		}

		bool left_up(int x = 0, int y = 0)
		{
			InjectedInputMouseInfo temp = {};
			temp.mouse_data = 0;
			temp.mouse_options = InjectedInputMouseOptions::left_up;
			temp.move_direction_x = x;
			temp.move_direction_y = y;
			temp.time_offset_in_miliseconds = 0;
			return _NtUserInjectMouseInput(&temp, 1);
		}
	};
} static inject::c_inject* Inject = new inject::c_inject();