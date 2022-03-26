
#include "Core/Configuration.h"
#include "Core/Helpers.h"
#include "Core/Hooks.h"
#include "Core/Version.h"
#include "Core/Menu/Base.h"
#include "Core/Menu/Submenus.h"
#include "Core/Menu/Overlay.h"
#include "Engine/Input.h"
#include "Engine/Rendering.h"
#include "Utilities/System/SystemCalls.h"
#include "Utilities/System/Timers.h"
#include <sys/prx.h>
#include <sys/ppu_thread.h>

// Macros defining our module informations as well as our entry/exit point
SYS_MODULE_INFO(Akari, 0, BUILD_VERSION_MAJOR, BUILD_VERSION_MINOR);
SYS_MODULE_START(module_start);
SYS_MODULE_STOP(module_stop);

sys_ppu_thread_t gMainPpuThreadId = SYS_PPU_THREAD_ID_INVALID;

extern "C"
{
	// This function is called automatically when sys_prx_start_module is called
	int module_start(size_t args, const void* argp)
	{
		sys_ppu_thread_create(&gMainPpuThreadId, [](uint64_t arg) -> void
		{
			g_Input = CInput();
			g_Render = CRender();
			g_Helpers = Helpers();

			g_Config = new Config();
			g_Menu = new Menu(MainSubmenu);
			g_Overlay = new Overlay();

			InstallHooks();

			sys_ppu_thread_exit(0);
		}, 
			0, 0x421, 2048, SYS_PPU_THREAD_CREATE_JOINABLE, "AkariModuleStart");

		// Exit thread using directly the syscall and not the user mode library or else we will crash
		Syscall::_sys_ppu_thread_exit(0);
		return 0;
	}

	// This function is called automatically when sys_prx_stop_module is called
	int module_stop(size_t args, const void* argp)
	{
		sys_ppu_thread_t stopPpuThreadId;
		int ret = sys_ppu_thread_create(&stopPpuThreadId, [](uint64_t arg) -> void
		{
			RemoveHooks();

			delete g_Overlay;
			delete g_Menu;
			delete g_Config;
			g_Render.DestroyPlanesAndTexts();

			// Prevent unload too fast (give time to other threads to finish)
			sys_ppu_thread_yield();
			Timers::Sleep(1000);

			if (gMainPpuThreadId != SYS_PPU_THREAD_ID_INVALID)
			{
				uint64_t exitCode;
				sys_ppu_thread_join(gMainPpuThreadId, &exitCode);
			}

			sys_ppu_thread_exit(0);
		},
			0, 2814, 1024, SYS_PPU_THREAD_CREATE_JOINABLE, "AkariModuleStop");

		if (ret == 0)
		{
			uint64_t exitCode;
			sys_ppu_thread_join(stopPpuThreadId, &exitCode);
		}

		Timers::Sleep(5);

		Syscall::finalize_module();
		// Exit thread using directly the syscall and not the user mode library or else we will crash
		Syscall::_sys_ppu_thread_exit(0);
		return 0;
	}
}