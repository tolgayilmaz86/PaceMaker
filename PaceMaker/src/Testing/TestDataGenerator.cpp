#include <Testing/TestDataGenerator.h>
#include <cmath>
#include <algorithm>

namespace pacemaker
{

//------------------------------------------------------------------------------
TestDataGenerator::TestDataGenerator()
{
  InitializeLeaderboardData();
  InitializeRelativeTimingData();
  InitializeTireData();
  InitializeVehicleData();
  InitializeInputTelemetryData();
}

//------------------------------------------------------------------------------
void TestDataGenerator::InitializeLeaderboardData()
{
  m_leaderboardData.sessionType = "Practice";
  m_leaderboardData.sessionTime = "1:09:45";
  m_leaderboardData.players = {
      {1, 38, "O Rasmussen", "2:06.358", "", "", 0, 15, false},
      {2, 51, "A P Guidi", "-", "", "", 1, 96, false},
      {3, 35, "P Chatin", "-", "", "", 3, 18, false},
      {4, 83, "R Kubica", "-", "", "", 1, 19, false},
      {5, 11, "J Munro", "-", "", "", 9, 99, false},
      {6, 36, "N Lapierre", "-", "", "", 3, 99, false},
      {7, 8,  "S Buemi", "-", "", "", 2, 25, false},
      {8, 5,  "M Campbell", "PIT", "", "", 0, 0, true}
  };
}

//------------------------------------------------------------------------------
void TestDataGenerator::InitializeRelativeTimingData()
{
  m_relativeTimingData.playerPosition = 12;
  m_relativeTimingData.players = {
      {7, 7, "S Vandoorne", "HY", -18.8f, 0},
      {13, 13, "F Heriau", "BR3", -7.0f, 2},
      {1, 1, "O Rasmussen", "HY", -1.7f, 0},
      {12, 12, "J Munro", "HY", 0.0f, 9},
      {7, 7, "C Schiavoni", "BR3", 4.6f, 2},
      {14, 14, "J Caygill", "BR3", 14.2f, 2},
      {8, 8, "A A Harthy", "BR3", 14.8f, 2}
  };
}

//------------------------------------------------------------------------------
void TestDataGenerator::InitializeTireData()
{
  m_tireData.temperatures[0] = 85.0f;
  m_tireData.temperatures[1] = 87.0f;
  m_tireData.temperatures[2] = 83.0f;
  m_tireData.temperatures[3] = 84.0f;
  m_tireData.pressures[0] = 2.2f;
  m_tireData.pressures[1] = 2.2f;
  m_tireData.pressures[2] = 2.1f;
  m_tireData.pressures[3] = 2.1f;
  m_tireData.wear[0] = 20.0f;
  m_tireData.wear[1] = 25.0f;
  m_tireData.wear[2] = 15.0f;
  m_tireData.wear[3] = 18.0f;
}

//------------------------------------------------------------------------------
void TestDataGenerator::InitializeVehicleData()
{
  m_vehicleData.speed = 94;
  m_vehicleData.gear = 3;
  m_vehicleData.rpm = 0.6f;
  m_vehicleData.engineTemp = 62.1f;
  m_vehicleData.oilTemp = 59.4f;
  m_vehicleData.fuelPercent = 75.0f;
  m_vehicleData.ersPercent = 98.6f;
  m_vehicleData.drsEnabled = false;
  m_vehicleData.lapTime = "88.71 (26.4 laps)";
  m_vehicleData.lastLap = "98.6% (25.0 laps)";
}

//------------------------------------------------------------------------------
void TestDataGenerator::InitializeInputTelemetryData()
{
  m_inputTelemetryData.steering = 0.0f;
  m_inputTelemetryData.throttle = 0.0f;
  m_inputTelemetryData.brake = 0.0f;
  m_inputTelemetryData.gear = 1;
  m_inputTelemetryData.rpm = 0.0f;
}

//------------------------------------------------------------------------------
void TestDataGenerator::UpdateLeaderboardData(float time)
{
  // Simulate battery changes for leaderboard
  for (auto& player : m_leaderboardData.players)
  {
    if (!player.inPit)
    {
      player.batteryPercent = 50 + (int)(30 * std::sin(time * 0.5f + player.number));
      player.batteryPercent = std::clamp(player.batteryPercent, 0, 100);
    }
  }
}

//------------------------------------------------------------------------------
void TestDataGenerator::UpdateRelativeTimingData(float time)
{
  // Could add dynamic gap changes here in the future
  // For now, data remains static after initialization
}

//------------------------------------------------------------------------------
void TestDataGenerator::UpdateTireData(float time)
{
  // Simulate tire temperature changes
  for (int i = 0; i < 4; i++)
  {
    m_tireData.temperatures[i] = 75.0f + 15.0f * std::sin(time * 0.8f + i);
  }
}

//------------------------------------------------------------------------------
void TestDataGenerator::UpdateVehicleData(float time)
{
  // Simulate vehicle data changes
  m_vehicleData.speed = 80 + (int)(20 * std::sin(time * 2.0f));
  m_vehicleData.rpm = 0.5f + 0.4f * std::sin(time * 3.0f);
  m_vehicleData.gear = std::clamp(2 + (int)(2 * (std::sin(time * 1.5f) + 1)), 1, 6);
}

//------------------------------------------------------------------------------
void TestDataGenerator::UpdateInputTelemetryData(float time)
{
  // Simulate input changes
  m_inputTelemetryData.steering = std::sin(time * 1.5f) * 0.8f;
  m_inputTelemetryData.throttle = std::clamp((std::sin(time * 2.0f) + 1.0f) * 0.5f, 0.0f, 1.0f);
  m_inputTelemetryData.brake = std::clamp((std::sin(time * 3.0f + 1.0f) + 1.0f) * 0.3f, 0.0f, 1.0f);
  m_inputTelemetryData.rpm = std::clamp((std::sin(time * 3.0f + 1.0f) + 1.0f) * 0.5f, 0.0f, 1.0f);
  m_inputTelemetryData.gear = 1 + (int)((time * 0.5f)) % 12 < 6 ? (int)((time * 0.5f)) % 6 : 11 - (int)((time * 0.5f)) % 12;
}

} // namespace pacemaker