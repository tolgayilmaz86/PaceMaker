#include <raylib.h>

// Include new overlay classes
#include <Overlays/LeaderboardOverlay.h>
#include <Overlays/RelativeTimingOverlay.h>
#include <Overlays/TireInfoOverlay.h>
#include <Overlays/SpeedometerOverlay.h>
#include <Overlays/InputTelemetryOverlay.h>
#include <Widgets/StatusIndicatorWidget.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>
#include <Utils/FontManager.h>
#include <Testing/TestDataGenerator.h>

#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <memory>
#include <span>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

// Global font variable
Font gFont;
Font gRegularFont;

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

// Windows API declarations for click-through functionality
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

static void SetWindowClickThrough(bool clickThrough)
{
  HWND hwnd = GetActiveWindow();
  if (hwnd)
  {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (clickThrough)
    {
      exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    }
    else
    {
      exStyle &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
  }
}

static std::pair<int, int> InitializeSystem()
{
  SetConfigFlags(
    FLAG_WINDOW_TRANSPARENT |
    FLAG_WINDOW_HIGHDPI |
    FLAG_MSAA_4X_HINT |
    FLAG_WINDOW_TOPMOST |
    FLAG_WINDOW_MAXIMIZED);

  InitWindow(800, 600, "PaceMaker - Racing Overlay");

  // Load and set window icon
  Image icon = LoadImage("logo.png");
  SetWindowIcon(icon);
  UnloadImage(icon);

  // Get monitor dimensions
  int monitorWidth = GetMonitorWidth(0);
  int monitorHeight = GetMonitorHeight(0);

  SetWindowSize(monitorWidth, monitorHeight);
  SetWindowPosition(0, 0);

  // Load font
  gFont = LoadFontEx("Formula1-Bold.otf", 64, 0, 95);
  SetTextureFilter(gFont.texture, TEXTURE_FILTER_BILINEAR);

  gRegularFont = LoadFontEx("Formula1-Regular.otf", 64, 0, 95);
  SetTextureFilter(gRegularFont.texture, TEXTURE_FILTER_BILINEAR);

  // Create data brokers for each overlay type
  pacemaker::FontManager::Instance().RegisterFont(pacemaker::FontType::Bold, &gFont);
  pacemaker::FontManager::Instance().RegisterFont(pacemaker::FontType::Regular, &gRegularFont);

  return std::pair<int, int>(monitorWidth, monitorHeight);
}

int main()
{
  auto [monitorWidth, monitorHeight] = InitializeSystem();

  using namespace pacemaker;
  DataBroker<LeaderboardData> leaderboardBroker;
  DataBroker<RelativeTimingData> relativeTimingBroker;
  DataBroker<TireInfoData> tireInfoBroker;
  DataBroker<VehicleData> vehicleBroker;
  DataBroker<InputTelemetryData> inputTelemetryBroker;

  // Create team colors span
  std::span<const Color> teamColorsSpan(teamColors, 10);

  // Create overlay instances
  auto leaderboardOverlay = std::make_unique<LeaderboardOverlay>(
    Bounds{ 20, 20, 500, 325 },
    MinSize{ 400, 250 },
    &gFont,
    teamColorsSpan,
    leaderboardBroker
  );

  auto relativeTimingOverlay = std::make_unique<RelativeTimingOverlay>(
    Bounds{ 20, monitorHeight - 310, 420, 300 },
    MinSize{ 350, 250 },
    &gFont,
    teamColorsSpan,
    relativeTimingBroker
  );

  auto tireInfoOverlay = std::make_unique<TireInfoOverlay>(
    Bounds{ monitorWidth - 400, monitorHeight - 320, 150, 200 },
    MinSize{ 150, 120 },
    &gFont,
    tireInfoBroker
  );

  auto speedometerOverlay = std::make_unique<SpeedometerOverlay>(
    Bounds{ monitorWidth - 270, monitorHeight - 280, 250, 270 },
    MinSize{ 200, 220 },
    &gFont,
    vehicleBroker
  );

  auto inputTelemetryOverlay = std::make_unique<InputTelemetryOverlay>(
    Bounds{ monitorWidth / 2 - 550, monitorHeight / 2 - 100, 700, 150 },
    MinSize{ 650, 130 },
    &gFont,
    inputTelemetryBroker
  );

  // Create status indicator widget
  auto statusIndicator = std::make_unique<StatusIndicatorWidget>(
    Bounds{ monitorWidth / 2 - 315, monitorHeight / 3, 620, 40 },
    &gFont
  );
  statusIndicator->SetVisible(false);

  // Collect all overlays in a vector for easier management
  std::vector<BaseWidget*> overlays = {
    leaderboardOverlay.get(),
    relativeTimingOverlay.get(),
    tireInfoOverlay.get(),
    speedometerOverlay.get(),
    inputTelemetryOverlay.get()
  };

  // Timing variables
  float time = 0.0f;
  float sampleTimer = 0.0f;
  bool widgetMoveMode = false;
  SetWindowClickThrough(true);

  while (!WindowShouldClose())
  {
    float deltaTime = GetFrameTime();
    time += deltaTime;
    sampleTimer += deltaTime;

    // Toggle move mode with Ctrl+F6
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_F6))
    {
      widgetMoveMode = !widgetMoveMode;
      SetWindowClickThrough(!widgetMoveMode);
      statusIndicator->SetVisible(widgetMoveMode);
    }

    // Get mouse input
    Vector2 mousePos = GetMousePosition();
    int mouseX = (int)mousePos.x;
    int mouseY = (int)mousePos.y;

    // Update draggable overlays when in move mode
    if (widgetMoveMode)
    {
      for (auto* overlay : overlays)
      {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
          overlay->OnMousePressed(mouseX, mouseY);
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
          overlay->OnMouseReleased(mouseX, mouseY);
        }
        if (overlay->IsDragging() || overlay->IsResizing())
        {
          overlay->OnMouseDragged(mouseX, mouseY);
        }
      }
    }

    // Initialize test data generator
    TestDataGenerator testDataGenerator;

    { // Update test data
      testDataGenerator.UpdateInputTelemetryData(time);
      testDataGenerator.UpdateLeaderboardData(time);
      testDataGenerator.UpdateVehicleData(time);
      testDataGenerator.UpdateTireData(time);
      testDataGenerator.UpdateRelativeTimingData(time);

      // Publish input telemetry data at 60Hz (throttled)
      if (sampleTimer >= 1.0f / 60.0f)
      {
        inputTelemetryBroker.Publish(testDataGenerator.GetInputTelemetryData());
        sampleTimer = 0.0f;
      }

      // Publish updated data to brokers
      leaderboardBroker.Publish(testDataGenerator.GetLeaderboardData());
      relativeTimingBroker.Publish(testDataGenerator.GetRelativeTimingData());
      tireInfoBroker.Publish(testDataGenerator.GetTireData());
      vehicleBroker.Publish(testDataGenerator.GetVehicleData());
    }

    // Render
    BeginDrawing();
    ClearBackground(BLANK);

    // Render all overlays
    for (const auto* overlay : overlays)
    {
      overlay->Render();
    }

    // Render status indicator
    statusIndicator->Render();

    // Draw borders when in move mode
    if (widgetMoveMode)
    {
      for (const auto* overlay : overlays)
      {
        overlay->RenderBorder(mouseX, mouseY);
      }
    }

    EndDrawing();
  }

  UnloadFont(gFont);
  UnloadFont(gRegularFont);
  CloseWindow();

  return 0;
}