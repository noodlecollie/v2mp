#include <cstddef>
#include <cstdio>
#include "raylib.h"
#include "raygui.h"
#include "LibV2MP/Defs.h"

static constexpr float MARGIN = 10.0f;
static constexpr float ROW_HEIGHT = 25.0f;
static constexpr float ROW_PADDING = 4.0f;
static constexpr float COL1_WIDTH = 50.0f;
static constexpr float COL2_WIDTH = 50.0f;
static constexpr float COL_PADDING = 10.0f;

static constexpr size_t NUM_REGISTERS = V2MP_REGID_MAX + 1;
static constexpr size_t REG_STR_BUFFER_SIZE = 8;

struct GuiRegister
{
	char strBuffer[REG_STR_BUFFER_SIZE];
	bool editable = false;

	GuiRegister()
	{
		Set(0);
	}

	void Set(V2MP_Word value)
	{
		snprintf(strBuffer, sizeof(strBuffer), "%04X", value);
		strBuffer[sizeof(strBuffer) - 1] = '\0';
	}
};

static GuiRegister g_Registers[NUM_REGISTERS];

static void DrawRegisterBoxRow(size_t index, const char* label, bool editable = false)
{
	static constexpr float COL1_LEFT = MARGIN;
	static constexpr float COL2_LEFT = MARGIN + COL1_WIDTH + COL_PADDING;

	GuiRegister& reg = g_Registers[0];
	float yOffset = MARGIN + (static_cast<float>(index) * (ROW_HEIGHT + ROW_PADDING));

	GuiLabel({COL1_LEFT, yOffset, COL1_WIDTH, ROW_HEIGHT}, label);

	if ( GuiTextBox({COL2_LEFT, yOffset, COL2_WIDTH, ROW_HEIGHT}, reg.strBuffer, 20, editable && reg.editable) )
	{
		reg.editable = !reg.editable;
	}
}

static void DrawRegisterBoxes()
{
	DrawRegisterBoxRow(0, "R0");
	DrawRegisterBoxRow(1, "R1");
	DrawRegisterBoxRow(2, "LR");
	DrawRegisterBoxRow(3, "PC");
}

int main(int, char**)
{
	SetTraceLogLevel(LOG_NONE);

	InitWindow(800, 600, "V2MP Explorer");
	SetExitKey(0);
	SetTargetFPS(30);

	bool exitWindow = false;

	while ( !exitWindow )
	{
		exitWindow = WindowShouldClose();

		BeginDrawing();

		ClearBackground({255, 255, 255, 255});
		DrawRegisterBoxes();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
