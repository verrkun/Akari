
#include "Hooks.h"
#include "Helpers.h"
#include "Menu/Base.h"
#include "Menu/Overlay.h"
#include "../Engine/Input.h"
#include "../Engine/Rendering.h"
#include "../Utilities/Memory/ImportsExports.h"
#include "../Utilities/System/Timers.h"
#include "../Utilities/System/SystemCalls.h"

using namespace Memory;

ImportExportDetour* pafFrameworkRender_Detour;
ImportExportDetour* cellPadGetDataExtra_0_Detour;

void pafFrameworkRender_Hook(void* framework, float frameTime)
{
	g_Helpers.OnUpdate();
	g_Render.OnUpdate();
	g_Overlay->OnUpdate();
	g_Menu->OnUpdate();

	pafFrameworkRender_Detour->CallOriginal<void>(framework, frameTime);
}

int cellPadGetDataExtra_0_Hook(unsigned int port, unsigned int deviceType, CellPadData* data)
{
	if (port != 0)
		return cellPadGetDataExtra_0_Detour->CallOriginal<int>(port, deviceType, data);

	int returnValue = cellPadGetDataExtra_0_Detour->CallOriginal<int>(port, deviceType, g_Input.GetInputData());
	std::memcpy(data, g_Input.GetInputData(), sizeof(CellPadData));

	if (g_Menu->IsOpened())
	{
		// Clear button flags
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL1] &= ~(CELL_PAD_CTRL_LEFT | CELL_PAD_CTRL_DOWN | CELL_PAD_CTRL_RIGHT | CELL_PAD_CTRL_UP);
		data->button[CELL_PAD_BTN_OFFSET_DIGITAL2] &= ~(CELL_PAD_CTRL_CROSS | CELL_PAD_CTRL_CIRCLE);
	
		if (g_Menu->IsMoving())
		{
			// Clear analog values
			data->button[CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X] = 128;
			data->button[CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y] = 128;
		}
	}

	return returnValue;
}

void RemoveCCAPIHooks()
{
	opd_s* pafFrameworkRender = FindExportByName("paf", 0x85D1D23B);
	opd_s* cellPadGetDataExtra_0 = FindExportByName("sys_io", 0x3733EA3C);

	// Checks if the exports haven't been detoured to avoid messing up the hooks
	// allowing compatibility with modules hooking the same functions
	if (Read<uint32_t>(pafFrameworkRender->sub + 0x4) == 0x7C0802A6)
	{
		Write<uint32_t>(pafFrameworkRender->sub + 0x14, 0xF80100D0);
	}

	if (Read<uint32_t>(cellPadGetDataExtra_0->sub + 0x4) == 0x7C0802A6)
	{
		Write<uint32_t>(cellPadGetDataExtra_0->sub + 0x0C, 0xF8010080);
	}
}

void InstallHooks()
{
	if (!Syscall::sys_mapi_is_hen())
		RemoveCCAPIHooks();

	pafFrameworkRender_Detour = new ImportExportDetour(ImportExportDetour::Export, "paf", 0x85D1D23B, (uint32_t)pafFrameworkRender_Hook);
	cellPadGetDataExtra_0_Detour = new ImportExportDetour(ImportExportDetour::Export, "sys_io", 0x3733EA3C, (uint32_t)cellPadGetDataExtra_0_Hook);
}

void RemoveHooks()
{
	delete pafFrameworkRender_Detour;
	delete cellPadGetDataExtra_0_Detour;
}