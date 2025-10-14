#include <Overlays/LeaderboardOverlay.h>

#include <Utils/FontManager.h>

#include <raylib.h>

#include <algorithm>
#include <cstdio>

namespace pacemaker
{
//------------------------------------------------------------------------------
LeaderboardOverlay::LeaderboardOverlay(
    Bounds bounds, 
    MinSize minSize,
    Font* font,
    std::span<const Color> teamColors,
    DataBroker<LeaderboardData>& broker)
    : BaseWidget("Leaderboard", bounds, minSize), m_font(font), m_teamColors(teamColors)
{
    m_font = FontManager::Instance().GetRegularFont();
    // Subscribe to data updates
    m_subscriptionId = broker.Subscribe([this](const LeaderboardData& data) {
        OnDataUpdated(data);
    });
}
//------------------------------------------------------------------------------
void LeaderboardOverlay::DrawPlayerRow(const PlayerData& player, int x, int y, int rowHeight, bool isHighlighted) const {
    // Row background
    Color bgColor = isHighlighted ? Color{70, 70, 70, 200} : Color{40, 40, 40, 180};
    DrawRectangle(x, y, 500, rowHeight, bgColor);

    int currentX = x + 10;
    int textY = y + (rowHeight / 2) - 8;

    // Position number
    char posStr[8];
    snprintf(posStr, sizeof(posStr), "%d", player.position);
    DrawTextEx(*m_font, posStr, {(float)currentX, (float)textY}, 20, 1, WHITE);
    currentX += 35;

    // Team color indicator (small square)
    DrawRectangle(currentX, y + 8, 6, rowHeight - 16, m_teamColors[player.teamColorIndex]);
    currentX += 15;

    // Driver number
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%d", player.number);
    DrawTextEx(*m_font, numStr, {(float)currentX, (float)textY}, 18, 1, Color{200, 200, 200, 255});
    currentX += 35;

    // Driver name
    DrawTextEx(*m_font, player.name.c_str(), {(float)currentX, (float)textY}, 18, 1, WHITE);
    currentX = x + 280;

    // Current/Best time or Gap
    const char* timeText = player.gap.c_str();
    Color timeColor = player.inPit ? Color{255, 165, 0, 255} : WHITE;
    DrawTextEx(*m_font, timeText, {(float)currentX, (float)textY}, 18, 1, timeColor);
    currentX = x + 380;

    auto boldFont = FontManager::Instance().GetBoldFont();
    // Pit indicator or S indicator
    if (player.inPit) {
      DrawTextEx(*boldFont, "PIT", { (float)currentX, (float)textY }, 16, 1, Color{ 255, 165, 0, 255 });
    } else {
        DrawCircle(currentX + 10, y + rowHeight / 2, 10, Color{200, 200, 200, 255});
        DrawTextEx(*m_font, "S", { (float)currentX + 6, (float)textY }, 14, 1, BLACK);
    }
    currentX += 35;

    // Battery percentage bar
    int barWidth = 50;
    int barHeight = 16;
    DrawRectangle(currentX, y + (rowHeight - barHeight) / 2, barWidth, barHeight, Color{60, 60, 60, 255});

    int fillWidth = (int)(barWidth * (player.batteryPercent / 100.0f));
    Color batteryColor = player.batteryPercent > 50 ? Color{0, 255, 0, 255} :
                         player.batteryPercent > 20 ? Color{255, 165, 0, 255} :
                         Color{255, 0, 0, 255};
    DrawRectangle(currentX, y + (rowHeight - barHeight) / 2, fillWidth, barHeight, batteryColor);

    // Battery percentage text
    char batteryStr[8];
    snprintf(batteryStr, sizeof(batteryStr), "%d%%", player.batteryPercent);
    DrawTextEx(*boldFont, batteryStr, { (float)(currentX + 5), (float)(y + (rowHeight - barHeight) / 2 + 2) }, 12, 1, WHITE);
}
//------------------------------------------------------------------------------
void LeaderboardOverlay::Render() const {
    if (!m_isVisible) return;

    const auto& [x, y, width, height] = m_bounds;
    
    // Header
    constexpr int headerHeight = 40;
    DrawRectangle(x, y, width, headerHeight, Color{20, 20, 20, 220});
    
    DrawTextEx(*m_font, m_data.sessionType.c_str(),
               {(float)(x + 10), (float)(y + 10)}, 20, 1, WHITE);
    
    int timeX = x + width - 100;
    DrawTextEx(*m_font, m_data.sessionTime.c_str(),
               {(float)timeX, (float)(y + 10)}, 20, 1, WHITE);

    // Calculate row height
    int availableHeight = height - headerHeight;
    int rowHeight = !m_data.players.empty() ? 
        std::clamp(availableHeight / static_cast<int>(m_data.players.size()), 25, 50) : 35;

    // Draw player rows
    int startY = y + headerHeight;
    int index = 0;
    
    for (const auto& player : m_data.players) {
        DrawPlayerRow(player, x, startY + (index * rowHeight), rowHeight, index == 0);  
        ++index;
    }
}
} // namespace pacemaker