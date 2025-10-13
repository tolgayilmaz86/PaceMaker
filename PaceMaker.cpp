#include <raylib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>

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

// Draggable box structure
struct DraggableBox
{
    int x, y;
    int width, height;
    bool isDragging;
    int dragOffsetX, dragOffsetY;

    bool IsMouseOver(int mouseX, int mouseY) const
    {
        return mouseX >= x && mouseX <= x + width &&
            mouseY >= y && mouseY <= y + height;
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

void DrawRelativeTimingRow(const RelativePlayer& player, int x, int y, int rowHeight, bool isPlayer)
{
    // Row background - highlight player's row
    Color bgColor = isPlayer ? Color{ 60, 60, 80, 200 } : Color{ 40, 40, 40, 180 };
    DrawRectangle(x, y, 420, rowHeight, bgColor);

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
    currentX = x + 290;

    // Gap to player
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

    DrawCustomTextWithShadow(gapStr, currentX, textY, 20, gapColor, 1);
}

void DrawRelativeTimingBox(const std::vector<RelativePlayer>& players, int playerPosition, int x, int y)
{
    int rowHeight = 38;
    int headerHeight = 35;
    int boxWidth = 420;

    // Header background
    DrawRectangle(x, y, boxWidth, headerHeight, Color{ 20, 20, 20, 220 });
    DrawCustomTextWithShadow("Relative", x + 10, y + 8, 18, WHITE, 2);

    // Icons placeholder (top right)
    int iconX = x + boxWidth - 150;
    for (int i = 0; i < 7; i++)
    {
        DrawCircle(iconX + (i * 22), y + 17, 8, Color{ 80, 80, 80, 200 });
    }

    // Draw player rows
    int startY = y + headerHeight;

    for (size_t i = 0; i < players.size(); i++)
    {
        bool isPlayer = (players[i].position == playerPosition);
        DrawRelativeTimingRow(players[i], x, startY + (i * rowHeight), rowHeight, isPlayer);
    }
}

void UpdateDraggableBox(DraggableBox& box, int mouseX, int mouseY, bool mousePressed, bool mouseReleased)
{
    if (mousePressed && box.IsMouseOver(mouseX, mouseY) && !box.isDragging)
    {
        box.isDragging = true;
        box.dragOffsetX = mouseX - box.x;
        box.dragOffsetY = mouseY - box.y;
    }

    if (mouseReleased)
    {
        box.isDragging = false;
    }

    if (box.isDragging)
    {
        box.x = mouseX - box.dragOffsetX;
        box.y = mouseY - box.dragOffsetY;
    }
}

int main()
{

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_MSAA_4X_HINT);
    InitWindow(800, 600, "PaceMaker - Racing Overlay");

    // Get monitor dimensions
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);

    SetWindowSize(monitorWidth, monitorHeight);
    SetWindowPosition(0, 0);
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

    // Relative timing data
    std::vector<RelativePlayer> relativePlayers = {
        {7, 7, "S Vandoorne", "HY", -18.8f, 0},
        {13, 13, "F Heriau", "BR3", -7.0f, 2},
        {1, 1, "O Rasmussen", "HY", -1.7f, 0},  // Player position
        {12, 12, "J Munro", "HY", 0.0f, 9},
        {7, 7, "C Schiavoni", "BR3", 4.6f, 2},
        {14, 14, "J Caygill", "BR3", 14.2f, 2},
        {8, 8, "A A Harthy", "BR3", 14.8f, 2}
    };

    // Draggable boxes
    DraggableBox leaderboardBox = { 20, 20, 500, 325, false, 0, 0 };
    DraggableBox relativeBox = { 20, monitorHeight - 310, 420, 300, false, 0, 0 };

    // Animation time for demo
    float time = 0.0f;
    int playerPosition = 1; // Current player position

    while (!WindowShouldClose())
    {
        time += GetFrameTime();

        // Get mouse input
        Vector2 mousePos = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);

        // Update draggable boxes
        UpdateDraggableBox(relativeBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);
        UpdateDraggableBox(leaderboardBox, (int)mousePos.x, (int)mousePos.y, mousePressed, mouseReleased);

        // Update box dimensions
        leaderboardBox.width = 500;
        leaderboardBox.height = 325;
        relativeBox.width = 420;
        relativeBox.height = 300;

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

        // Draw leaderboard
        DrawLeaderboard(players, leaderboardBox.x, leaderboardBox.y);

        // Draw relative timing box
        DrawRelativeTimingBox(relativePlayers, playerPosition, relativeBox.x, relativeBox.y);

        // Draw drag indicators when hovering
        if (leaderboardBox.IsMouseOver((int)mousePos.x, (int)mousePos.y) || leaderboardBox.isDragging)
        {
            DrawRectangleLines(leaderboardBox.x, leaderboardBox.y, leaderboardBox.width, leaderboardBox.height,
                leaderboardBox.isDragging ? YELLOW : Color{ 255, 255, 255, 100 });
        }

        if (relativeBox.IsMouseOver((int)mousePos.x, (int)mousePos.y) || relativeBox.isDragging)
        {
            DrawRectangleLines(relativeBox.x, relativeBox.y, relativeBox.width, relativeBox.height,
                relativeBox.isDragging ? YELLOW : Color{ 255, 255, 255, 100 });
        }

        EndDrawing();
    }

    UnloadFont(gFont);
    CloseWindow();

    return 0;
}