#include <raylib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>

#if defined(_WIN32)
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

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

void DrawLeaderboard(const std::vector<Player>& players, int x, int y)
{
    // Header background
    DrawRectangle(x, y, 500, 40, Color{ 20, 20, 20, 220 });

    // Session info
    DrawCustomTextWithShadow("Practice", x + 10, y + 10, 20, WHITE, 2);
    DrawCustomTextWithShadow("1:09:45", x + 380, y + 10, 20, WHITE, 2);

    // Draw each player row
    int rowHeight = 35;
    int startY = y + 45;

    for (size_t i = 0; i < players.size(); i++)
    {
        DrawPlayerRow(players[i], x, startY + (i * rowHeight), rowHeight, i == 0);
    }
}

int main()
{
    // Get monitor dimensions
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_MSAA_4X_HINT);
    InitWindow(monitorWidth, monitorHeight, "PaceMaker - Racing Overlay");

    // Load font
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

    // Animation time for demo
    float time = 0.0f;

    while (!WindowShouldClose())
    {
        time += GetFrameTime();

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

        BeginDrawing();
        ClearBackground(BLANK);

        // Draw leaderboard in top-left corner
        DrawLeaderboard(players, 20, 20);

        EndDrawing();
    }

    UnloadFont(gFont);
    CloseWindow();

    return 0;
}