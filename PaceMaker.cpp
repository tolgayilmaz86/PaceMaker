// Include Windows headers FIRST with minimal definitions
#include <raylib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

// Global font variable
Font gFont;

// Player data structure
struct Player
{
	int position;
	int number;
	std::string name;
	std::string currentTime;    // Current lap time
	std::string bestTime;       // Best lap time
	std::string gap;            // Gap to leader
	int teamColorIndex;         // 0=Red Bull, 1=Ferrari, 2=Mercedes, etc.
	int batteryPercent;         // Battery percentage
	bool inPit;                 // Is in pit
};

// Relative timing player structure
struct RelativePlayer
{
	int position;
	int number;
	std::string name;
	std::string teamCode;       // e.g., "HY", "BR3"
	float gap;                  // Gap in seconds (positive = ahead, negative = behind)
	int teamColorIndex;
};

// Tire data structure
struct TireData
{
	float temperatures[4];      // FL, FR, RL, RR temperatures
	float pressures[4];         // FL, FR, RL, RR pressures
	float wear[4];             // FL, FR, RL, RR wear percentage
};

// Vehicle data structure
struct VehicleData
{
	int speed;                  // Speed in MPH or KPH
	int gear;                   // Current gear
	float rpm;                  // RPM (0-1 normalized)
	float engineTemp;           // Engine temperature
	float oilTemp;              // Oil temperature
	float fuelPercent;          // Fuel percentage
	float ersPercent;           // ERS percentage
	bool drsEnabled;            // DRS active
	std::string lapTime;        // Current lap time
	std::string lastLap;        // Last lap time
};

// Draggable box structure
struct DraggableBox
{
	int x, y;
	int width, height;
	bool isDragging;
	bool isResizing;
	int dragOffsetX, dragOffsetY;
	int minWidth, minHeight;

	DraggableBox(int x, int y, int w, int h, int minW = 100, int minH = 50)
		: x(x), y(y), width(w), height(h), isDragging(false), isResizing(false),
		dragOffsetX(0), dragOffsetY(0), minWidth(minW), minHeight(minH) {
	}

	bool IsMouseOver(int mouseX, int mouseY) const
	{
		return mouseX >= x && mouseX <= x + width &&
			mouseY >= y && mouseY <= y + height;
	}

	bool IsMouseOverResizeHandle(int mouseX, int mouseY) const
	{
		// Resize handle in bottom-right corner (20x20 area)
		int handleSize = 20;
		return mouseX >= (x + width - handleSize) && mouseX <= (x + width) &&
			mouseY >= (y + height - handleSize) && mouseY <= (y + height);
	}
};

// Input telemetry data structure
struct InputTelemetry
{
	float steering;     // -1.0 (left) to 1.0 (right)
	float throttle;     // 0.0 to 1.0
	float brake;        // 0.0 to 1.0
};

// Input history for timeline graph
struct InputHistory
{
	static const int MAX_HISTORY = 200;
	std::vector<InputTelemetry> history;

	void AddSample(const InputTelemetry& sample)
	{
		history.push_back(sample);
		if (history.size() > MAX_HISTORY)
		{
			history.erase(history.begin());
		}
	}

	void Clear()
	{
		history.clear();
	}
};

// Team colors
Color teamColors[] = {
	{30, 65, 174, 255},     // Red Bull - Blue
	{220, 0, 0, 255},       // Ferrari - Red
	{0, 210, 190, 255},     // Mercedes - Cyan
	{255, 135, 0, 255},     // McLaren - Orange
	{0, 144, 54, 255},      // Aston Martin - Green
	{37, 87, 164, 255},     // Alpine - Blue
	{175, 32, 45, 255},     // Alfa Romeo - Red
	{43, 69, 98, 255},      // AlphaTauri - Navy
	{108, 211, 191, 255},   // Williams - Cyan
	{182, 186, 189, 255}    // Haas - Gray
};

void DrawCustomTextWithShadow(const char* text, int posX, int posY, int fontSize, Color color, int shadowOffset = 2)
{
	float spacing = 1;

	// Draw shadow
	Color shadowColor = { 0, 0, 0, 150 };
	DrawTextEx(gFont, text,
		{ (float)(posX + shadowOffset), (float)(posY + shadowOffset) },
		(float)fontSize, spacing, shadowColor);

	// Draw main text
	DrawTextEx(gFont, text, { (float)posX, (float)posY }, (float)fontSize, spacing, color);
}

void DrawBoxBorder(const DraggableBox& box, int mouseX, int mouseY)
{
	Color borderColor;

	if (box.isDragging)
		borderColor = YELLOW;
	else if (box.isResizing)
		borderColor = Color{ 255, 100, 0, 255 }; // Orange
	else if (box.IsMouseOver(mouseX, mouseY))
		borderColor = Color{ 255, 255, 255, 100 };
	else
		return; // Don't draw border if not interacting

	// Draw border
	DrawRectangleLines(box.x, box.y, box.width, box.height, borderColor);

	// Draw resize handle indicator
	if (box.IsMouseOverResizeHandle(mouseX, mouseY) || box.isResizing)
	{
		int handleSize = 15;
		DrawTriangle(
			{ (float)(box.x + box.width), (float)(box.y + box.height) },
			{ (float)(box.x + box.width - handleSize), (float)(box.y + box.height) },
			{ (float)(box.x + box.width), (float)(box.y + box.height - handleSize) },
			box.isResizing ? Color{ 255, 100, 0, 200 } : Color{ 255, 255, 255, 150 }
		);
	}
}

void DrawPlayerRow(const Player& player, int x, int y, int rowHeight, bool isHighlighted)
{
	// Row background
	Color bgColor = isHighlighted ? Color{ 70, 70, 70, 200 } : Color{ 40, 40, 40, 180 };
	DrawRectangle(x, y, 500, rowHeight, bgColor);

	int currentX = x + 10;
	int textY = y + (rowHeight / 2) - 8;

	// Position number
	char posStr[8];
	snprintf(posStr, sizeof(posStr), "%d", player.position);
	DrawCustomTextWithShadow(posStr, currentX, textY, 20, WHITE, 1);
	currentX += 35;

	// Team color indicator (small square)
	DrawRectangle(currentX, y + 8, 6, rowHeight - 16, teamColors[player.teamColorIndex]);
	currentX += 15;

	// Driver number
	char numStr[8];
	snprintf(numStr, sizeof(numStr), "%d", player.number);
	DrawCustomTextWithShadow(numStr, currentX, textY, 18, Color{ 200, 200, 200, 255 }, 1);
	currentX += 35;

	// Driver name
	DrawCustomTextWithShadow(player.name.c_str(), currentX, textY, 18, WHITE, 1);
	currentX = x + 280;

	// Current/Best time or Gap
	const char* timeText = player.gap.c_str();
	Color timeColor = player.inPit ? Color{ 255, 165, 0, 255 } : WHITE;
	DrawCustomTextWithShadow(timeText, currentX, textY, 18, timeColor, 1);
	currentX = x + 380;

	// Pit indicator or S indicator
	if (player.inPit)
	{
		DrawText("PIT", currentX, textY, 16, Color{ 255, 165, 0, 255 });
	}
	else
	{
		DrawCircle(currentX + 10, y + rowHeight / 2, 10, Color{ 200, 200, 200, 255 });
		DrawText("S", currentX + 6, textY, 14, BLACK);
	}
	currentX += 35;

	// Battery percentage bar
	int barWidth = 50;
	int barHeight = 16;
	DrawRectangle(currentX, y + (rowHeight - barHeight) / 2, barWidth, barHeight, Color{ 60, 60, 60, 255 });

	int fillWidth = (int)(barWidth * (player.batteryPercent / 100.0f));
	Color batteryColor = player.batteryPercent > 50 ? Color{ 0, 255, 0, 255 } :
		player.batteryPercent > 20 ? Color{ 255, 165, 0, 255 } :
		Color{ 255, 0, 0, 255 };
	DrawRectangle(currentX, y + (rowHeight - barHeight) / 2, fillWidth, barHeight, batteryColor);

	// Battery percentage text
	char batteryStr[8];
	snprintf(batteryStr, sizeof(batteryStr), "%d%%", player.batteryPercent);
	DrawText(batteryStr, currentX + 5, y + (rowHeight - barHeight) / 2 + 2, 12, WHITE);
}

void DrawLeaderboard(const std::vector<Player>& players, int x, int y, int width, int height)
{
	// Header background
	int headerHeight = 40;
	DrawRectangle(x, y, width, headerHeight, Color{ 20, 20, 20, 220 });

	// Session info
	DrawCustomTextWithShadow("Practice", x + 10, y + 10, 20, WHITE, 2);
	int timeX = x + width - 100;
	DrawCustomTextWithShadow("1:09:45", timeX, y + 10, 20, WHITE, 2);

	// Calculate row height based on available space
	int availableHeight = height - headerHeight;
	int rowHeight = (players.size() > 0) ? (availableHeight / (int)players.size()) : 35;
	if (rowHeight < 25) rowHeight = 25; // Minimum row height
	if (rowHeight > 50) rowHeight = 50; // Maximum row height

	// Draw each player row
	int startY = y + headerHeight;

	for (size_t i = 0; i < players.size(); i++)
	{
		DrawPlayerRow(players[i], x, startY + (i * rowHeight), rowHeight, i == 0);
	}
}

void DrawRelativeTimingRow(const RelativePlayer& player, int x, int y, int rowHeight, bool isPlayer, int width)
{
	// Row background - highlight player's row
	Color bgColor = isPlayer ? Color{ 60, 60, 80, 200 } : Color{ 40, 40, 40, 180 };
	DrawRectangle(x, y, width, rowHeight, bgColor);

	int currentX = x + 10;
	int textY = y + (rowHeight / 2) - 10;

	// Position number with background
	Color posColor = player.position <= 3 ? Color{ 220, 0, 0, 255 } :
		player.position <= 10 ? Color{ 0, 180, 0, 255 } :
		Color{ 100, 100, 100, 255 };
	DrawRectangle(currentX, y + 8, 26, rowHeight - 16, posColor);
	char posStr[8];
	snprintf(posStr, sizeof(posStr), "%d", player.position);
	DrawText(posStr, currentX + (player.position < 10 ? 8 : 4), y + 11, 18, WHITE);
	currentX += 35;

	// Team code box
	DrawRectangle(currentX, y + 8, 36, rowHeight - 16, teamColors[player.teamColorIndex]);
	DrawText(player.teamCode.c_str(), currentX + 4, y + 11, 14, WHITE);
	currentX += 45;

	// Driver name
	DrawCustomTextWithShadow(player.name.c_str(), currentX, textY, 18, WHITE, 1);

	// Gap - position it relative to the right edge
	int gapX = x + width - 120;
	char gapStr[16];
	if (std::abs(player.gap) < 0.01f)
	{
		snprintf(gapStr, sizeof(gapStr), "0.0");
	}
	else
	{
		snprintf(gapStr, sizeof(gapStr), "%+.1f", player.gap);
	}

	Color gapColor = player.gap > 0 ? Color{ 100, 200, 100, 255 } : Color{ 200, 100, 100, 255 };
	if (std::abs(player.gap) < 0.01f) gapColor = WHITE;

	DrawCustomTextWithShadow(gapStr, gapX, textY, 20, gapColor, 1);
}

void DrawRelativeTimingBox(const std::vector<RelativePlayer>& players, int playerPosition, int x, int y, int width, int height)
{
	int headerHeight = 35;

	// Header background
	DrawRectangle(x, y, width, headerHeight, Color{ 20, 20, 20, 220 });
	DrawCustomTextWithShadow("Relative", x + 10, y + 8, 18, WHITE, 2);

	// Icons placeholder (top right) - scale with width
	int iconX = x + width - 150;
	for (int i = 0; i < 7; i++)
	{
		DrawCircle(iconX + (i * 22), y + 17, 8, Color{ 80, 80, 80, 200 });
	}

	// Calculate row height based on available space
	int availableHeight = height - headerHeight;
	int rowHeight = (players.size() > 0) ? (availableHeight / (int)players.size()) : 38;
	if (rowHeight < 28) rowHeight = 28; // Minimum row height
	if (rowHeight > 50) rowHeight = 50; // Maximum row height

	// Draw player rows
	int startY = y + headerHeight;

	for (size_t i = 0; i < players.size(); i++)
	{
		bool isPlayer = (players[i].position == playerPosition);
		DrawRelativeTimingRow(players[i], x, startY + (i * rowHeight), rowHeight, isPlayer, width);  // Pass width!
	}
}

void DrawSpeedometer(const VehicleData& vehicle, int x, int y, int width, int height)
{
	// Scale based on available space
	float scale = std::min(width / 250.0f, height / 270.0f);

	int centerX = x + width / 2;
	int centerY = y + (int)(120 * scale);
	int radius = (int)(90 * scale);

	// Background circle
	DrawCircle(centerX, centerY, radius + 10, Color{ 30, 30, 40, 220 });
	DrawCircle(centerX, centerY, radius + 8, Color{ 50, 50, 60, 255 });

	// RPM arc (green, changes to red at high RPM)
	float rpmAngle = vehicle.rpm * 270.0f;
	float startAngle = 135.0f;

	Color rpmColor = vehicle.rpm < 0.85f ? Color{ 0, 255, 0, 255 } :
		vehicle.rpm < 0.95f ? Color{ 255, 165, 0, 255 } :
		Color{ 255, 0, 0, 255 };

	DrawCircleSector({ (float)centerX, (float)centerY }, radius - 5, startAngle, startAngle + rpmAngle, 32, rpmColor);

	// Speed ring outline
	DrawRing({ (float)centerX, (float)centerY }, radius - 10, radius, 0, 360, 64, Color{ 200, 200, 200, 100 });

	// Gear number
	char gearStr[8];
	snprintf(gearStr, sizeof(gearStr), "%d", vehicle.gear);
	int gearSize = (int)(80 * scale);
	int gearWidth = MeasureText(gearStr, gearSize);
	DrawText(gearStr, centerX - gearWidth / 2, centerY - (int)(50 * scale), gearSize, WHITE);

	// Speed
	char speedStr[16];
	snprintf(speedStr, sizeof(speedStr), "%d", vehicle.speed);
	int speedSize = (int)(40 * scale);
	DrawCustomTextWithShadow(speedStr, centerX - (int)(30 * scale), centerY + (int)(10 * scale), speedSize, WHITE, 2);
	DrawCustomTextWithShadow("MPH", centerX - (int)(25 * scale), centerY + (int)(50 * scale), (int)(16 * scale), Color{ 180, 180, 180, 255 }, 1);

	// FFB indicator
	DrawText("FFB", centerX - (int)(20 * scale), centerY + (int)(70 * scale), (int)(12 * scale), Color{ 150, 150, 150, 255 });

	// Icons on the left side
	int iconY = centerY - (int)(30 * scale);
	int iconRadius = (int)(6 * scale);
	DrawCircle(centerX - (int)(70 * scale), iconY, iconRadius, Color{ 100, 100, 100, 200 });
	iconY += (int)(20 * scale);
	DrawCircle(centerX - (int)(70 * scale), iconY, iconRadius, Color{ 100, 100, 100, 200 });
	iconY += (int)(20 * scale);
	DrawCircle(centerX - (int)(70 * scale), iconY, iconRadius, Color{ 100, 100, 100, 200 });

	// Temperature displays (bottom left)
	int tempY = y + (int)(200 * scale);
	char tempStr[32];
	snprintf(tempStr, sizeof(tempStr), "%.1f°C", vehicle.engineTemp);
	DrawText(tempStr, x + 10, tempY, (int)(14 * scale), WHITE);

	snprintf(tempStr, sizeof(tempStr), "%.1f°C", vehicle.oilTemp);
	DrawText(tempStr, x + 10, tempY + (int)(20 * scale), (int)(14 * scale), WHITE);

	// Lap times (bottom)
	int lapY = y + (int)(210 * scale);
	int lapWidth = (int)(140 * scale);
	int lapHeight = (int)(25 * scale);
	DrawRectangle(x + (int)(60 * scale), lapY, lapWidth, lapHeight, Color{ 255, 0, 0, 200 });
	char lapStr[32];
	snprintf(lapStr, sizeof(lapStr), "%s", vehicle.lapTime.c_str());
	DrawText(lapStr, x + (int)(65 * scale), lapY + 5, (int)(14 * scale), WHITE);

	DrawRectangle(x + (int)(60 * scale), lapY + (int)(28 * scale), lapWidth, lapHeight, Color{ 100, 100, 200, 200 });
	DrawText("NRG", x + (int)(65 * scale), lapY + (int)(32 * scale), (int)(12 * scale), WHITE);
	snprintf(lapStr, sizeof(lapStr), "%s", vehicle.lastLap.c_str());
	DrawText(lapStr, x + (int)(100 * scale), lapY + (int)(32 * scale), (int)(12 * scale), WHITE);

	// Fuel and ERS bars (right side)
	int barX = centerX + (int)(80 * scale);
	int barWidth = (int)(15 * scale);
	int barHeight = (int)(80 * scale);

	// Fuel bar
	DrawRectangle(barX, centerY - (int)(40 * scale), barWidth, barHeight, Color{ 60, 60, 60, 200 });
	int fuelFill = (int)(barHeight * (vehicle.fuelPercent / 100.0f));
	DrawRectangle(barX, centerY - (int)(40 * scale) + (barHeight - fuelFill), barWidth, fuelFill,
		vehicle.fuelPercent > 20 ? Color{ 255, 200, 0, 255 } : Color{ 255, 0, 0, 255 });

	// Battery indicator below
	DrawRectangle(barX, centerY + (int)(45 * scale), barWidth, (int)(15 * scale), Color{ 60, 60, 60, 200 });
}

void DrawTireInfo(const TireData& tires, int x, int y, int width, int height)
{
	// Background
	DrawRectangle(x, y, width, height, Color{ 30, 30, 40, 220 });

	// Title
	DrawCustomTextWithShadow("TIRES", x + 10, y + 10, 16, WHITE, 1);

	// Scale tire layout based on available space
	float scale = std::min(width / 180.0f, height / 140.0f);

	int tireWidth = (int)(30 * scale);
	int tireHeight = (int)(50 * scale);
	int carCenterX = x + width / 2;
	int topY = y + (int)(40 * scale);

	// Draw car outline
	int carWidth = (int)(50 * scale);
	int carHeight = (int)(80 * scale);
	DrawRectangleLines(carCenterX - carWidth / 2, topY + (int)(10 * scale), carWidth, carHeight, Color{ 100, 100, 100, 150 });

	// Front Left Tire
	int flX = carCenterX - (int)(40 * scale);
	int flY = topY;
	Color flColor = Color{ (unsigned char)(255 - tires.wear[0] * 2.55f), (unsigned char)(tires.wear[0] * 2.55f), 0, 255 };
	DrawRectangle(flX, flY, tireWidth, tireHeight, flColor);
	char tempStr[16];
	snprintf(tempStr, sizeof(tempStr), "%.0f", tires.temperatures[0]);
	DrawText(tempStr, flX + 5, flY + (int)(15 * scale), (int)(12 * scale), WHITE);

	// Front Right Tire
	int frX = carCenterX + (int)(10 * scale);
	int frY = topY;
	Color frColor = Color{ (unsigned char)(255 - tires.wear[1] * 2.55f), (unsigned char)(tires.wear[1] * 2.55f), 0, 255 };
	DrawRectangle(frX, frY, tireWidth, tireHeight, frColor);
	snprintf(tempStr, sizeof(tempStr), "%.0f", tires.temperatures[1]);
	DrawText(tempStr, frX + 5, frY + (int)(15 * scale), (int)(12 * scale), WHITE);

	// Rear Left Tire
	int rlX = carCenterX - (int)(40 * scale);
	int rlY = topY + (int)(60 * scale);
	Color rlColor = Color{ (unsigned char)(255 - tires.wear[2] * 2.55f), (unsigned char)(tires.wear[2] * 2.55f), 0, 255 };
	DrawRectangle(rlX, rlY, tireWidth, tireHeight, rlColor);
	snprintf(tempStr, sizeof(tempStr), "%.0f", tires.temperatures[2]);
	DrawText(tempStr, rlX + 5, rlY + (int)(15 * scale), (int)(12 * scale), WHITE);

	// Rear Right Tire
	int rrX = carCenterX + (int)(10 * scale);
	int rrY = topY + (int)(60 * scale);
	Color rrColor = Color{ (unsigned char)(255 - tires.wear[3] * 2.55f), (unsigned char)(tires.wear[3] * 2.55f), 0, 255 };
	DrawRectangle(rrX, rrY, tireWidth, tireHeight, rrColor);
	snprintf(tempStr, sizeof(tempStr), "%.0f", tires.temperatures[3]);
	DrawText(tempStr, rrX + 5, rrY + (int)(15 * scale), (int)(12 * scale), WHITE);
}

void DrawTelemetryViewer(const InputHistory& inputHistory, const InputTelemetry& currentInput, int x, int y, int width, int height)
{
	// Main background
	DrawRectangle(x, y, width, height, Color{ 20, 20, 20, 220 });

	// Title
	DrawText("TELEMETRY", x + 10, y + 10, 20, Color{ 200, 200, 200, 255 });

	// Calculate responsive dimensions
	int barWidth = (int)(width * 0.03f); // 3% of width per bar
	if (barWidth < 30) barWidth = 30;
	if (barWidth > 50) barWidth = 50;

	int barHeight = height - 80; // Leave room for title and labels
	int graphWidth = width - 200 - (barWidth * 3) - 40; // Leave room for bars
	int graphHeight = height - 50;

	// Graph area background
	int graphX = x + 120;
	int graphY = y + 30;
	DrawRectangle(graphX, graphY, graphWidth, graphHeight, Color{ 30, 30, 30, 255 });
	DrawRectangleLines(graphX, graphY, graphWidth, graphHeight, Color{ 60, 60, 60, 255 });

	// Draw grid lines
	for (int i = 1; i < 4; i++)
	{
		int gridY = graphY + (graphHeight / 4) * i;
		DrawLine(graphX, gridY, graphX + graphWidth, gridY, Color{ 50, 50, 50, 150 });
	}

	// Draw input history timeline
	if (inputHistory.history.size() > 1)
	{
		float xStep = (float)graphWidth / (float)InputHistory::MAX_HISTORY;

		// Draw throttle (green) and brake (red) lines
		for (size_t i = 1; i < inputHistory.history.size(); i++)
		{
			float x1 = graphX + (i - 1) * xStep;
			float x2 = graphX + i * xStep;

			// Throttle line (green)
			float throttle1 = inputHistory.history[i - 1].throttle;
			float throttle2 = inputHistory.history[i].throttle;
			float y1_throttle = graphY + graphHeight - (throttle1 * graphHeight);
			float y2_throttle = graphY + graphHeight - (throttle2 * graphHeight);
			DrawLineEx({ x1, y1_throttle }, { x2, y2_throttle }, 3.0f, Color{ 0, 255, 0, 255 });

			// Brake line (red)
			float brake1 = inputHistory.history[i - 1].brake;
			float brake2 = inputHistory.history[i].brake;
			float y1_brake = graphY + graphHeight - (brake1 * graphHeight);
			float y2_brake = graphY + graphHeight - (brake2 * graphHeight);
			DrawLineEx({ x1, y1_brake }, { x2, y2_brake }, 3.0f, Color{ 255, 0, 0, 255 });
		}

		// Draw steering line (green outline style) - centered at middle
		for (size_t i = 1; i < inputHistory.history.size(); i++)
		{
			float x1 = graphX + (i - 1) * xStep;
			float x2 = graphX + i * xStep;

			float steering1 = inputHistory.history[i - 1].steering;
			float steering2 = inputHistory.history[i].steering;
			float centerY = graphY + graphHeight / 2.0f;
			float y1_steering = centerY - (steering1 * graphHeight * 0.4f);
			float y2_steering = centerY - (steering2 * graphHeight * 0.4f);
			DrawLineEx({ x1, y1_steering }, { x2, y2_steering }, 4.0f, Color{ 0, 255, 0, 255 });
		}
	}

	// Draw center line for steering reference
	int centerLineY = graphY + graphHeight / 2;
	DrawLine(graphX, centerLineY, graphX + graphWidth, centerLineY, Color{ 100, 100, 100, 100 });

	// Draw bars on the right
	int barsX = graphX + graphWidth + 40;
	int barsY = graphY + 15;

	// Steering bar (center-based, left-right)
	DrawText("0", barsX + barWidth / 2 - 5, barsY - 15, 14, Color{ 150, 150, 150, 255 });
	DrawRectangle(barsX, barsY, barWidth, barHeight, Color{ 40, 40, 40, 255 });
	DrawRectangleLines(barsX, barsY, barWidth, barHeight, Color{ 80, 80, 80, 255 });

	// Draw steering indicator
	int steerCenter = barsY + barHeight / 2;
	int steerOffset = (int)(currentInput.steering * (barHeight / 2));
	int steerBarY = steerCenter - steerOffset;
	int steerBarHeight = abs(steerOffset);
	if (steerOffset < 0) steerBarY = steerCenter;

	Color steerColor = currentInput.steering > 0 ? Color{ 0, 200, 255, 255 } : Color{ 255, 200, 0, 255 };
	if (abs(currentInput.steering) < 0.01f) steerColor = Color{ 100, 100, 100, 255 };
	DrawRectangle(barsX, steerBarY, barWidth, steerBarHeight, steerColor);

	// Draw center line on steering bar
	DrawLine(barsX, steerCenter, barsX + barWidth, steerCenter, Color{ 150, 150, 150, 255 });

	// Brake bar
	barsX += barWidth + 10;
	DrawText("0", barsX + barWidth / 2 - 5, barsY - 15, 14, Color{ 150, 150, 150, 255 });
	DrawRectangle(barsX, barsY, barWidth, barHeight, Color{ 40, 40, 40, 255 });
	DrawRectangleLines(barsX, barsY, barWidth, barHeight, Color{ 80, 80, 80, 255 });

	int brakeFill = (int)(currentInput.brake * barHeight);
	DrawRectangle(barsX, barsY + barHeight - brakeFill, barWidth, brakeFill, Color{ 255, 0, 0, 255 });

	// Throttle bar
	barsX += barWidth + 10;
	char throttlePercent[8];
	snprintf(throttlePercent, sizeof(throttlePercent), "%d", (int)(currentInput.throttle * 100));
	DrawText(throttlePercent, barsX + barWidth / 2 - 10, barsY - 15, 14, Color{ 150, 150, 150, 255 });
	DrawRectangle(barsX, barsY, barWidth, barHeight, Color{ 40, 40, 40, 255 });
	DrawRectangleLines(barsX, barsY, barWidth, barHeight, Color{ 80, 80, 80, 255 });

	int throttleFill = (int)(currentInput.throttle * barHeight);
	DrawRectangle(barsX, barsY + barHeight - throttleFill, barWidth, throttleFill, Color{ 0, 255, 0, 255 });

	// Labels below bars
	int labelY = barsY + barHeight + 5;
	DrawText("STEER", barsX - (barWidth * 2) - 20, labelY, 12, Color{ 150, 150, 150, 255 });
	DrawText("BRAKE", barsX - barWidth - 10, labelY, 12, Color{ 150, 150, 150, 255 });
	DrawText("THROTTLE", barsX - 10, labelY, 12, Color{ 150, 150, 150, 255 });
}

void UpdateDraggableBox(DraggableBox& box, int mouseX, int mouseY, bool mousePressed, bool mouseReleased)
{
	if (mousePressed)
	{
		// Check for resize handle first
		if (box.IsMouseOverResizeHandle(mouseX, mouseY) && !box.isDragging && !box.isResizing)
		{
			box.isResizing = true;
		}
		// Then check for drag
		else if (box.IsMouseOver(mouseX, mouseY) && !box.isDragging && !box.isResizing)
		{
			box.isDragging = true;
			box.dragOffsetX = mouseX - box.x;
			box.dragOffsetY = mouseY - box.y;
		}
	}

	if (mouseReleased)
	{
		box.isDragging = false;
		box.isResizing = false;
	}

	if (box.isDragging)
	{
		box.x = mouseX - box.dragOffsetX;
		box.y = mouseY - box.dragOffsetY;
	}

	if (box.isResizing)
	{
		int newWidth = mouseX - box.x;
		int newHeight = mouseY - box.y;

		// Apply minimum size constraints
		if (newWidth >= box.minWidth)
			box.width = newWidth;
		if (newHeight >= box.minHeight)
			box.height = newHeight;
	}
}

// Instead of including windows.h clashing with raylib
// we declared functions and types here
#define WS_EX_TRANSPARENT       0x00000020L
#define WS_EX_LAYERED           0x00080000
#define GWL_EXSTYLE				(-20)
#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE(HWND);
typedef __int64 LONG_PTR, * PLONG_PTR;
#define VOID void
#define WINAPI      __stdcall
#define WINUSERAPI DECLSPEC_IMPORT

extern "C"
{
	int __stdcall GetWindowLongPtrW(HWND hWnd, int nIndex);
	int __stdcall SetWindowLongPtrW(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
	HWND __stdcall GetActiveWindow(VOID);
}

void SetWindowClickThrough(bool clickThrough)
{
	HWND hwnd = GetActiveWindow();
	if (hwnd)
	{
		LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
		if (clickThrough)
		{
			// Enable click-through
			exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
		}
		else
		{
			// Disable click-through
			exStyle &= ~WS_EX_TRANSPARENT;
		}
		SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
	}
}

int main()
{
	SetConfigFlags(
		FLAG_WINDOW_TRANSPARENT |
		FLAG_WINDOW_HIGHDPI |
		FLAG_MSAA_4X_HINT |
		FLAG_WINDOW_TOPMOST |
		FLAG_WINDOW_MAXIMIZED);

	InitWindow(800, 600, "PaceMaker - Racing Overlay");

	// Get monitor dimensions
	int monitorWidth = GetMonitorWidth(0);
	int monitorHeight = GetMonitorHeight(0);

	SetWindowSize(monitorWidth, monitorHeight);
	// Load font
	SetWindowPosition(0, 0);

	gFont = LoadFontEx("F:/repos/PaceMaker/Formula1-Bold.otf", 64, 0, 95);
	SetTextureFilter(gFont.texture, TEXTURE_FILTER_BILINEAR);

	// Sample player data
	std::vector<Player> players = {
		{1, 38, "O Rasmussen", "2:06.358", "2:06.358", "2:06.358", 0, 15, false},
		{2, 51, "A P Guidi", "-", "-", "-", 1, 96, false},
		{3, 35, "P Chatin", "-", "-", "-", 3, 18, false},
		{4, 83, "R Kubica", "-", "-", "-", 1, 19, false},
		{5, 11, "J Munro", "-", "-", "-", 9, 99, false},
		{6, 36, "N Lapierre", "-", "-", "-", 3, 99, false},
		{7, 8, "S Buemi", "-", "-", "-", 2, 25, false},
		{8, 5, "M Campbell", "-", "-", "PIT", 0, 0, true}
	};

	// Relative timing data
	std::vector<RelativePlayer> relativePlayers = {
		{7, 7, "S Vandoorne", "HY", -18.8f, 0},
		{13, 13, "F Heriau", "BR3", -7.0f, 2},
		{1, 1, "O Rasmussen", "HY", -1.7f, 0},
		{12, 12, "J Munro", "HY", 0.0f, 9},
		{7, 7, "C Schiavoni", "BR3", 4.6f, 2},
		{14, 14, "J Caygill", "BR3", 14.2f, 2},
		{8, 8, "A A Harthy", "BR3", 14.8f, 2}
	};

	// Vehicle data
	VehicleData vehicle = {
		94,                     // speed
		3,                      // gear
		0.6f,                   // rpm
		62.1f,                  // engine temp
		59.4f,                  // oil temp
		75.0f,                  // fuel percent
		98.6f,                  // ers percent
		false,                  // drs
		"88.71 (26.4 laps)",   // lap time
		"98.6% (25.0 laps)"    // last lap
	};

	// Tire data
	TireData tires = {
		{85.0f, 87.0f, 83.0f, 84.0f},  // temperatures
		{2.2f, 2.2f, 2.1f, 2.1f},       // pressures
		{20.0f, 25.0f, 15.0f, 18.0f}    // wear
	};

	// Add input telemetry data
	InputTelemetry currentInput = { 0.0f, 0.0f, 0.0f };
	InputHistory inputHistory;

	// Draggable boxes with minimum sizes
	DraggableBox leaderboardBox(20, 20, 500, 325, 400, 250);
	DraggableBox relativeBox(20, monitorHeight - 310, 420, 300, 350, 250);
	DraggableBox speedometerBox(monitorWidth - 270, monitorHeight - 280, 250, 270, 200, 220);
	DraggableBox tireBox(monitorWidth - 470, monitorHeight - 280, 180, 140, 150, 120);
	DraggableBox telemetryBox(monitorWidth / 2 - 600, monitorHeight / 2 - 100, 1200, 200, 800, 150);

	float time = 0.0f;
	float sampleTimer = 0.0f;
	int playerPosition = 1;

	bool widgetMoveMode = false;
	SetWindowClickThrough(true);

	while (!WindowShouldClose())
	{
		time += GetFrameTime();
		sampleTimer += GetFrameTime();

		// Toggle move mode with Ctrl+F6
		if (IsKeyDown(KEY_LEFT_CONTROL))
		{
			if (IsKeyPressed(KEY_F6))
			{
				widgetMoveMode = !widgetMoveMode;
				SetWindowClickThrough(!widgetMoveMode);
			}
		}

		// Get mouse input
		Vector2 mousePos = GetMousePosition();
		bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
		bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

		// Update draggable boxes when NOT in click-through mode
		if (widgetMoveMode)
		{
			UpdateDraggableBox(relativeBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
			UpdateDraggableBox(leaderboardBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
			UpdateDraggableBox(speedometerBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
			UpdateDraggableBox(tireBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
			UpdateDraggableBox(telemetryBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
		}

		// Simulate input changes
		currentInput.steering = sin(time * 1.5f) * 0.8f;
		currentInput.throttle = (sin(time * 2.0f) + 1.0f) * 0.5f;
		currentInput.brake = (sin(time * 3.0f + 1.0f) + 1.0f) * 0.3f;

		// Clamp values
		currentInput.throttle = std::clamp(currentInput.throttle, 0.0f, 1.0f);
		currentInput.brake = std::clamp(currentInput.brake, 0.0f, 1.0f);

		// Sample input history at 60Hz
		if (sampleTimer >= 1.0f / 60.0f)
		{
			inputHistory.AddSample(currentInput);
			sampleTimer = 0.0f;
		}

		// Simulate battery changes
		for (auto& player : players)
		{
			if (!player.inPit)
			{
				player.batteryPercent = 50 + (int)(30 * sin(time * 0.5f + player.number));
				if (player.batteryPercent < 0) player.batteryPercent = 0;
				if (player.batteryPercent > 100) player.batteryPercent = 100;
			}
		}

		// Simulate vehicle data changes
		vehicle.speed = 80 + (int)(20 * sin(time * 2.0f));
		vehicle.rpm = 0.5f + 0.4f * sin(time * 3.0f);
		vehicle.gear = std::clamp(2 + (int)(2 * (sin(time * 1.5f) + 1)), 1, 6);

		// Simulate tire temperature changes
		for (int i = 0; i < 4; i++)
		{
			tires.temperatures[i] = 75.0f + 15.0f * sin(time * 0.8f + i);
		}

		BeginDrawing();
		ClearBackground(BLANK);

		// Draw all widgets with their box dimensions
		DrawLeaderboard(players, leaderboardBox.x, leaderboardBox.y, leaderboardBox.width, leaderboardBox.height);
		DrawRelativeTimingBox(relativePlayers, playerPosition, relativeBox.x, relativeBox.y, relativeBox.width, relativeBox.height);
		DrawSpeedometer(vehicle, speedometerBox.x, speedometerBox.y, speedometerBox.width, speedometerBox.height);
		DrawTireInfo(tires, tireBox.x, tireBox.y, tireBox.width, tireBox.height);
		DrawTelemetryViewer(inputHistory, currentInput, telemetryBox.x, telemetryBox.y, telemetryBox.width, telemetryBox.height);

		// Draw borders and resize handles (only when not click-through)
		if (widgetMoveMode)
		{
			DrawBoxBorder(leaderboardBox, (int)mousePos.x, (int)mousePos.y);
			DrawBoxBorder(relativeBox, (int)mousePos.x, (int)mousePos.y);
			DrawBoxBorder(speedometerBox, (int)mousePos.x, (int)mousePos.y);
			DrawBoxBorder(tireBox, (int)mousePos.x, (int)mousePos.y);
			DrawBoxBorder(telemetryBox, (int)mousePos.x, (int)mousePos.y);

			// Show status indicator
			int xPos = monitorWidth / 2 - 300;
			int yPos = monitorHeight / 3;
			DrawRectangle(xPos - 15, yPos, 550, 40, Color{ 0, 0, 0, 180 });
			DrawCustomTextWithShadow("MOVE MODE - Press Ctrl to exit", xPos, yPos + 5, 28, YELLOW);
		}

		EndDrawing();
	}

	UnloadFont(gFont);
	CloseWindow();

	return 0;
}
