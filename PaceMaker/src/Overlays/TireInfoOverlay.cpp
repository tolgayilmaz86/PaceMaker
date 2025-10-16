#include <Overlays/TireInfoOverlay.h>

#include <raylib.h>

#include <algorithm>
#include <cstdio>

namespace pacemaker
{
//------------------------------------------------------------------------------
TireInfoOverlay::TireInfoOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    DataBroker<TireInfoData>& broker
)
    : BaseWidget("TireInfo", bounds, minSize)
    , m_font(font)
{
    m_subscriptionId = broker.Subscribe([this](const TireInfoData& data) {
        OnDataUpdated(data);
    });
}
//------------------------------------------------------------------------------
void TireInfoOverlay::Render() const
{
    if (!m_isVisible) return;

    const auto& [x, y, width, height] = m_bounds;

    // Background
    DrawRectangle(x, y + 56, width, height, Color{30, 30, 40, 220});

    // Scale tire layout based on available space
    float scale = std::min(width / 90.0f, height / 70.0f);

    int tireWidth = (int)(30 * scale);
    int tireHeight = (int)(50 * scale);
    int carCenterX = x + width / 2;
    int topY = y + (int)(40 * scale);

    // Draw car outline
    int carWidth = (int)(50 * scale);
    int carHeight = (int)(80 * scale);
    DrawRectangleLines(carCenterX - carWidth / 2, topY + (int)(10 * scale), carWidth, carHeight, Color{100, 100, 100, 150});

    char tempStr[16];

    // Front Left Tire
    int flX = carCenterX - (int)(40 * scale);
    int flY = topY;
    Color flColor = Color{
        (unsigned char)(255 - m_data.wear[0] * 2.55f),
        (unsigned char)(m_data.wear[0] * 2.55f),
        0, 255
    };
    DrawRectangle(flX, flY, tireWidth, tireHeight, flColor);
    snprintf(tempStr, sizeof(tempStr), "%.0f", m_data.temperatures[0]);
    DrawTextEx(*m_font, tempStr, {(float)(flX + 5), (float)(flY + (int)(15 * scale))}, (int)(12 * scale), 1, WHITE);

    // Front Right Tire
    int frX = carCenterX + (int)(10 * scale);
    int frY = topY;
    Color frColor = Color{
        (unsigned char)(255 - m_data.wear[1] * 2.55f),
        (unsigned char)(m_data.wear[1] * 2.55f),
        0, 255
    };
    DrawRectangle(frX, frY, tireWidth, tireHeight, frColor);
    snprintf(tempStr, sizeof(tempStr), "%.0f", m_data.temperatures[1]);
    DrawTextEx(*m_font, tempStr, {(float)(frX + 5), (float)(frY + (int)(15 * scale))}, (int)(12 * scale), 1, WHITE);

    // Rear Left Tire
    int rlX = carCenterX - (int)(40 * scale);
    int rlY = topY + (int)(60 * scale);
    Color rlColor = Color{
        (unsigned char)(255 - m_data.wear[2] * 2.55f),
        (unsigned char)(m_data.wear[2] * 2.55f),
        0, 255
    };
    DrawRectangle(rlX, rlY, tireWidth, tireHeight, rlColor);
    snprintf(tempStr, sizeof(tempStr), "%.0f", m_data.temperatures[2]);
    DrawTextEx(*m_font, tempStr, {(float)(rlX + 5), (float)(rlY + (int)(15 * scale))}, (int)(12 * scale), 1, WHITE);

    // Rear Right Tire
    int rrX = carCenterX + (int)(10 * scale);
    int rrY = topY + (int)(60 * scale);
    Color rrColor = Color{
        (unsigned char)(255 - m_data.wear[3] * 2.55f),
        (unsigned char)(m_data.wear[3] * 2.55f),
        0, 255
    };
    DrawRectangle(rrX, rrY, tireWidth, tireHeight, rrColor);
    snprintf(tempStr, sizeof(tempStr), "%.0f", m_data.temperatures[3]);
    DrawTextEx(*m_font, tempStr, {(float)(rrX + 5), (float)(rrY + (int)(15 * scale))}, (int)(12 * scale), 1, WHITE);
}

} // namespace pacemaker