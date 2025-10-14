#include <Overlays/RelativeTimingOverlay.h>

#include <raylib.h>

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace pacemaker
{
//------------------------------------------------------------------------------
RelativeTimingOverlay::RelativeTimingOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    std::span<const Color> teamColors,
    DataBroker<RelativeTimingData>& broker
)
    : BaseWidget("RelativeTiming", bounds, minSize)
    , m_font(font)
    , m_teamColors(teamColors)
{
    m_subscriptionId = broker.Subscribe([this](const RelativeTimingData& data) {
        OnDataUpdated(data);
    });
}
//------------------------------------------------------------------------------
void RelativeTimingOverlay::DrawPlayerRow(const RelativePlayerData& player, int x, int y, int rowHeight, bool isPlayer, int width) const
{
    // Row background - highlight player's row
    Color bgColor = isPlayer ? Color{60, 60, 80, 200} : Color{40, 40, 40, 180};
    DrawRectangle(x, y, width, rowHeight, bgColor);

    int currentX = x + 10;
    int textY = y + (rowHeight / 2) - 10;

    // Position number with background
    Color posColor = player.position <= 3 ? Color{220, 0, 0, 255} :
                     player.position <= 10 ? Color{0, 180, 0, 255} :
                     Color{100, 100, 100, 255};
    DrawRectangle(currentX, y + 8, 26, rowHeight - 16, posColor);
    
    char posStr[8];
    snprintf(posStr, sizeof(posStr), "%d", player.position);
    DrawTextEx(*m_font, posStr, {(float)(currentX + (player.position < 10 ? 8 : 4)), (float)(y + 11)}, 18, 1, WHITE);
    currentX += 35;

    // Team code box
    DrawRectangle(currentX, y + 8, 36, rowHeight - 16, m_teamColors[player.teamColorIndex]);
    DrawTextEx(*m_font, player.teamCode.c_str(), {(float)(currentX + 4), (float)(y + 11)}, 14, 1, WHITE);
    currentX += 45;

    // Driver name
    DrawTextEx(*m_font, player.name.c_str(), {(float)currentX, (float)textY}, 18, 1, WHITE);

    // Gap - position relative to right edge
    int gapX = x + width - 120;
    char gapStr[16];
    if (std::abs(player.gap) < 0.01f)
        snprintf(gapStr, sizeof(gapStr), "0.0");
    else
        snprintf(gapStr, sizeof(gapStr), "%+.1f", player.gap);

    Color gapColor = player.gap > 0 ? Color{100, 200, 100, 255} : Color{200, 100, 100, 255};
    if (std::abs(player.gap) < 0.01f) gapColor = WHITE;

    DrawTextEx(*m_font, gapStr, {(float)gapX, (float)textY}, 20, 1, gapColor);
}
//------------------------------------------------------------------------------
void RelativeTimingOverlay::Render() const
{
    if (!m_isVisible) return;

    const auto& [x, y, width, height] = m_bounds;
    constexpr int headerHeight = 35;

    // Header background
    DrawRectangle(x, y, width, headerHeight, Color{20, 20, 20, 220});
    DrawTextEx(*m_font, "Relative", {(float)(x + 10), (float)(y + 8)}, 18, 1, WHITE);

    // Icons placeholder (top right)
    int iconX = x + width - 150;
    for (int i = 0; i < 7; i++)
    {
        DrawCircle(iconX + (i * 22), y + 17, 8, Color{80, 80, 80, 200});
    }

    // Calculate row height
    int availableHeight = height - headerHeight;
    int rowHeight = !m_data.players.empty() ? 
        std::clamp(availableHeight / static_cast<int>(m_data.players.size()), 28, 50) : 38;

    // Draw player rows
    int startY = y + headerHeight;
    for (size_t i = 0; i < m_data.players.size(); i++)
    {
        bool isPlayer = (m_data.players[i].position == m_data.playerPosition);
        DrawPlayerRow(m_data.players[i], x, startY + (i * rowHeight), rowHeight, isPlayer, width);
    }
}

} // namespace pacemaker