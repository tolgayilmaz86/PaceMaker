#include <Overlays/SpeedometerOverlay.h>

#include <raylib.h>

#include <algorithm>
#include <cstdio>

namespace pacemaker
{
//------------------------------------------------------------------------------
SpeedometerOverlay::SpeedometerOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    DataBroker<VehicleData>& broker
)
    : BaseWidget("Speedometer", bounds, minSize)
    , m_font(font)
{
    m_subscriptionId = broker.Subscribe([this](const VehicleData& data) {
        OnDataUpdated(data);
    });
}
//------------------------------------------------------------------------------
void SpeedometerOverlay::Render() const
{
    if (!m_isVisible) return;

    const auto& [x, y, width, height] = m_bounds;

    // Scale based on available space
    float scale = std::min(width / 250.0f, height / 270.0f);

    int centerX = x + width / 2;
    int centerY = y + (int)(120 * scale);
    int radius = (int)(90 * scale);

    // Background circle
    DrawCircle(centerX, centerY, radius + 10, Color{30, 30, 40, 220});
    DrawCircle(centerX, centerY, radius + 8, Color{50, 50, 60, 255});

    // RPM arc
    float rpmAngle = m_data.rpm * 270.0f;
    float startAngle = 135.0f;

    Color rpmColor = m_data.rpm < 0.85f ? Color{0, 255, 0, 255} :
                     m_data.rpm < 0.95f ? Color{255, 165, 0, 255} :
                     Color{255, 0, 0, 255};

    DrawCircleSector({(float)centerX, (float)centerY}, radius - 5, startAngle, startAngle + rpmAngle, 32, rpmColor);

    // Speed ring outline
    DrawRing({(float)centerX, (float)centerY}, radius - 10, radius, 0, 360, 64, Color{200, 200, 200, 100});

    // Gear number
    char gearStr[8];
    snprintf(gearStr, sizeof(gearStr), "%d", m_data.gear);
    int gearSize = (int)(80 * scale);
    int gearWidth = MeasureText(gearStr, gearSize);
    DrawText(gearStr, centerX - gearWidth / 2, centerY - (int)(50 * scale), gearSize, WHITE);

    // Speed
    char speedStr[16];
    snprintf(speedStr, sizeof(speedStr), "%d", m_data.speed);
    int speedSize = (int)(40 * scale);
    DrawTextEx(*m_font, speedStr, {(float)(centerX - (int)(30 * scale)), (float)(centerY + (int)(10 * scale))}, speedSize, 1, WHITE);
    DrawTextEx(*m_font, "MPH", {(float)(centerX - (int)(25 * scale)), (float)(centerY + (int)(50 * scale))}, (int)(16 * scale), 1, Color{180, 180, 180, 255});

    // FFB indicator
    DrawTextEx(*m_font, "FFB", {(float)(centerX - (int)(20 * scale)), (float)(centerY + (int)(70 * scale))}, (int)(12 * scale), 1, Color{150, 150, 150, 255});

    // Icons on the left side
    int iconY = centerY - (int)(30 * scale);
    int iconRadius = (int)(6 * scale);
    for (int i = 0; i < 3; i++)
    {
        DrawCircle(centerX - (int)(70 * scale), iconY, iconRadius, Color{100, 100, 100, 200});
        iconY += (int)(20 * scale);
    }

    // Temperature displays
    int tempY = y + (int)(200 * scale);
    char tempStr[32];
    snprintf(tempStr, sizeof(tempStr), "%.1f°C", m_data.engineTemp);
    DrawTextEx(*m_font, tempStr, {(float)(x + 10), (float)tempY}, (int)(14 * scale), 1, WHITE);

    snprintf(tempStr, sizeof(tempStr), "%.1f°C", m_data.oilTemp);
    DrawTextEx(*m_font, tempStr, {(float)(x + 10), (float)(tempY + (int)(20 * scale))}, (int)(14 * scale), 1, WHITE);

    // Lap times
    int lapY = y + (int)(210 * scale);
    int lapWidth = (int)(140 * scale);
    int lapHeight = (int)(25 * scale);
    
    DrawRectangle(x + (int)(60 * scale), lapY, lapWidth, lapHeight, Color{255, 0, 0, 200});
    DrawTextEx(*m_font, m_data.lapTime.c_str(), {(float)(x + (int)(65 * scale)), (float)(lapY + 5)}, (int)(14 * scale), 1, WHITE);

    DrawRectangle(x + (int)(60 * scale), lapY + (int)(28 * scale), lapWidth, lapHeight, Color{100, 100, 200, 200});
    DrawTextEx(*m_font, "NRG", {(float)(x + (int)(65 * scale)), (float)(lapY + (int)(32 * scale))}, (int)(12 * scale), 1, WHITE);
    DrawTextEx(*m_font, m_data.lastLap.c_str(), {(float)(x + (int)(100 * scale)), (float)(lapY + (int)(32 * scale))}, (int)(12 * scale), 1, WHITE);

    // Fuel and ERS bars
    int barX = centerX + (int)(80 * scale);
    int barWidth = (int)(15 * scale);
    int barHeight = (int)(80 * scale);

    // Fuel bar
    DrawRectangle(barX, centerY - (int)(40 * scale), barWidth, barHeight, Color{60, 60, 60, 200});
    int fuelFill = (int)(barHeight * (m_data.fuelPercent / 100.0f));
    Color fuelColor = m_data.fuelPercent > 20 ? Color{255, 200, 0, 255} : Color{255, 0, 0, 255};
    DrawRectangle(barX, centerY - (int)(40 * scale) + (barHeight - fuelFill), barWidth, fuelFill, fuelColor);

    // Battery indicator
    DrawRectangle(barX, centerY + (int)(45 * scale), barWidth, (int)(15 * scale), Color{60, 60, 60, 200});
}

} // namespace pacemaker