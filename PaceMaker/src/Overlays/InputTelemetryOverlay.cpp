#include <Overlays/InputTelemetryOverlay.h>

#include <raylib.h>

#include <algorithm>
#include <cstdio>
#include <cmath>

namespace pacemaker
{

  constexpr float THROTTLE_THICKNESS = 2.0f;
  constexpr float BRAKE_THICKNESS = 2.0f;
  constexpr float STEERING_THICKNESS = 1.0f;
  constexpr int OPACITY = 200;
  //------------------------------------------------------------------------------
  InputTelemetryOverlay::InputTelemetryOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    DataBroker<InputTelemetryData>& broker)
    : BaseWidget("InputTelemetry", bounds, minSize), m_font(font)
  {
    m_subscriptionId = broker.Subscribe([this](const InputTelemetryData& data) {
      OnDataUpdated(data);
      });
  }
  //------------------------------------------------------------------------------
  void InputTelemetryOverlay::OnDataUpdated(const InputTelemetryData& data)
  {
    m_data = data;

    // Add to history
    m_history.push_back(data);

    // Keep history size limited
    if (m_history.size() > InputTelemetryData::MAX_HISTORY)
    {
      m_history.pop_front();
    }
  }
  //------------------------------------------------------------------------------
  void InputTelemetryOverlay::Render() const
  {
    if (!m_isVisible) return;

    const auto& [x, y, width, height] = m_bounds;

    // Calculate responsive dimensions
    int barWidth = std::clamp((int)(width * 0.01f), 12, 15);
    int barHeight = height - 80;
    int graphWidth = width - 200 - (barWidth * 3) - 40;
    int graphHeight = height - 50;

    // Graph area background
    int graphX = x + 80;
    int graphY = y + 30;
    DrawRectangle(graphX, graphY, graphWidth, graphHeight, Color{ 30, 30, 30, OPACITY });
    DrawRectangleLines(graphX, graphY, graphWidth, graphHeight, Color{ 60, 60, 60, 255 });

    // Draw grid lines
    for (int i = 1; i < 4; i++)
    {
      int gridY = graphY + (graphHeight / 4) * i;
      DrawLine(graphX, gridY, graphX + graphWidth, gridY, Color{ 50, 50, 50, OPACITY / 2 });
    }

    const auto historySize = m_history.size();
    // Draw input history timeline
    if (historySize > 1)
    {
      float xStep = (float)graphWidth / (float)InputTelemetryData::MAX_HISTORY;

      for (size_t i = 1; i < historySize; i++)
      {
        float x1 = graphX + (i - 1) * xStep;
        float x2 = graphX + i * xStep;

        // Throttle line (green)
        float throttle1 = m_history[i - 1].throttle;
        float throttle2 = m_history[i].throttle;
        float y1_throttle = graphY + graphHeight - (throttle1 * graphHeight);
        float y2_throttle = graphY + graphHeight - (throttle2 * graphHeight);
        DrawLineEx({ x1, y1_throttle }, { x2, y2_throttle }, THROTTLE_THICKNESS, GREEN);

        // Brake line (red)
        float brake1 = m_history[i - 1].brake;
        float brake2 = m_history[i].brake;
        float y1_brake = graphY + graphHeight - (brake1 * graphHeight);
        float y2_brake = graphY + graphHeight - (brake2 * graphHeight);
        DrawLineEx({ x1, y1_brake }, { x2, y2_brake }, BRAKE_THICKNESS, RED);

        // Draw steering line (blue)
        float steering1 = m_history[i - 1].steering;
        float steering2 = m_history[i].steering;
        float centerY = graphY + graphHeight / 2.0f;
        float y1_steering = centerY - (steering1 * graphHeight * 0.4f);
        float y2_steering = centerY - (steering2 * graphHeight * 0.4f);
        DrawLineEx({ x1, y1_steering }, { x2, y2_steering }, STEERING_THICKNESS, SKYBLUE);

        /********************************************
         * Gear indicators on the graph
         ********************************************/

        // Detect gear changes and draw shift indicators
        short gear1 = m_history[i - 1].gear;
        short gear2 = m_history[i].gear;

        if (gear1 != gear2 && gear1 > 0 && gear2 > 0) // Only show for forward gears
        {
          float triangleSize = 12.0f;
          float triangleX = x2;

          if (gear2 > gear1) // Upshift
          {
            // Draw upward pointing triangle at the top
            float triangleY = graphY; // Position near top of graph
            Vector2 p1 = { triangleX, triangleY };
            Vector2 p2 = { triangleX - triangleSize / 2.0f, triangleY + triangleSize };
            Vector2 p3 = { triangleX + triangleSize / 2.0f, triangleY + triangleSize };
            DrawTriangle(p1, p2, p3, GREEN);
          }
          else // Downshift
          {
            // Draw downward pointing triangle at the bottom
            float triangleY = graphY + graphHeight - triangleSize; // Position near bottom with margin
            Vector2 p1 = { triangleX, triangleY + triangleSize }; // Apex points down
            Vector2 p2 = { triangleX - triangleSize / 2.0f, triangleY }; // Top left
            Vector2 p3 = { triangleX + triangleSize / 2.0f, triangleY }; // Top right
            DrawTriangle(p3, p2, p1, Color{ 255, 191, 0, 255 }); // Amber, draws CCW order
          }
        }
      }
    }

    // Draw center line for steering reference
    int centerLineY = graphY + graphHeight / 2;
    DrawLine(graphX, centerLineY, graphX + graphWidth, centerLineY, Color{ 100, 100, 100, OPACITY / 3 });

    // Draw bars on the right
    int barsX = graphX + graphWidth - barWidth * 2; // Start bars to the right of the graph
    int barsY = graphY;

    /********************************************
     * Brake and Throttle bars
     ********************************************/
    barsX += barWidth;
    // Do not show numeric values for now
    //char brakePercent[8];
    //snprintf(brakePercent, sizeof(brakePercent), "%d", (int)(m_data.brake * 100));
    //DrawTextEx(*m_font, brakePercent, { (float)(barsX + barWidth / 2 - 5), (float)(barsY - 15) }, 14, 1, GRAY);
    //DrawRectangle(barsX, barsY, barWidth, graphHeight, DARKGRAY);
    //DrawRectangleLines(barsX, barsY, barWidth, graphHeight, DARKGRAY);

    int brakeFill = (int)(m_data.brake * graphHeight);
    DrawRectangle(barsX, barsY + graphHeight - brakeFill, barWidth, brakeFill, RED);

    // Throttle bar
    barsX += barWidth;
    // Do not show numeric values for now
    //char throttlePercent[8];
    //snprintf(throttlePercent, sizeof(throttlePercent), "%d", (int)(m_data.throttle * 100));
    //DrawTextEx(*m_font, throttlePercent, { (float)(barsX + barWidth / 2 - 10), (float)(barsY - 15) }, 14, 1, GRAY);
    //DrawRectangle(barsX, barsY, barWidth, graphHeight, DARKGRAY);
    //DrawRectangleLines(barsX, barsY, barWidth, graphHeight, DARKGRAY);

    int throttleFill = (int)(m_data.throttle * graphHeight);
    DrawRectangle(barsX, barsY + graphHeight - throttleFill, barWidth, throttleFill, GREEN);

    /********************************************
     * Gear and RPM display
     ********************************************/
     // Draw Gear and RPM display on the right side
    int gearRpmX = graphX + graphWidth + 12;
    int gearRpmWidth = 80;

    // First calculate rpm area height to put gear on top of that
    static constexpr int rpmFontSize = 18;

    // Format RPM text (assuming rpm is normalized 0-1, display as actual RPM)
    char rpmText[6];
    snprintf(rpmText, sizeof(rpmText), "%d", (int)(m_data.rpm * 10000)); // Assuming max 10000 RPM

    // Calculate text centering for RPM
    Vector2 rpmTextSize = MeasureTextEx(*m_font, rpmText, rpmFontSize, 1);

    // RPM display
    int rpmBoxHeight = rpmTextSize.y + 5;
    int rpmBoxY = graphY + graphHeight - rpmBoxHeight; // Align bottom
    DrawRectangle(gearRpmX, rpmBoxY, gearRpmWidth, rpmBoxHeight, Color{ 30, 30, 30, OPACITY });
    DrawRectangleLines(gearRpmX, rpmBoxY, gearRpmWidth, rpmBoxHeight, Color{ 70, 70, 70, OPACITY });

    // Gear display (amber color)
    int gearBoxY = graphY;
    int gearBoxHeight = graphHeight - rpmBoxHeight;
    DrawRectangle(gearRpmX, gearBoxY, gearRpmWidth, gearBoxHeight, Color{ 30, 30, 30, OPACITY });
    DrawRectangleLines(gearRpmX, gearBoxY, gearRpmWidth, gearBoxHeight, Color{ 70, 70, 70, OPACITY });

    // Format gear text (R for reverse, N for neutral)
    char gearText[2];
    if (m_data.gear == -1)
      snprintf(gearText, sizeof(gearText), "R");
    else if (m_data.gear == 0)
      snprintf(gearText, sizeof(gearText), "N");
    else
      snprintf(gearText, sizeof(gearText), "%d", m_data.gear);

    // Calculate text centering for gear
    static constexpr int gearFontSize = 96;
    Vector2 gearTextSize = MeasureTextEx(*m_font, gearText, gearFontSize, 2);
    float gearTextX = gearRpmX + (gearRpmWidth - gearTextSize.x) / 2.0f;
    float gearTextY = gearBoxY + (gearBoxHeight - gearTextSize.y) / 2.0f;

    // Draw gear text centered in gear box
    DrawTextEx(*m_font, gearText, { gearTextX, gearTextY }, gearFontSize, 2, Color{ 255, 191, 0, OPACITY }); // Amber color

    // Draw RPM text centered in rpm box
    float rpmTextX = gearRpmX + (gearRpmWidth - rpmTextSize.x) / 2.0f;
    float rpmTextY = rpmBoxY + (rpmBoxHeight - rpmTextSize.y) / 2.0f;
    DrawTextEx(*m_font, rpmText, { rpmTextX, rpmTextY }, rpmFontSize, 1, BEIGE);
  }

} // namespace pacemaker