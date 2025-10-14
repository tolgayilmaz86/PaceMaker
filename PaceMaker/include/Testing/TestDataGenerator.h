#pragma once

#include <Data/DataStructs.h>
#include <Overlays/TireInfoOverlay.h>

namespace pacemaker
{

class TestDataGenerator
{
public:
  TestDataGenerator();
  ~TestDataGenerator() = default;

  // Update methods - call these with deltaTime in your game loop
  void UpdateLeaderboardData(float time);
  void UpdateRelativeTimingData(float time);
  void UpdateTireData(float time);
  void UpdateVehicleData(float time);
  void UpdateInputTelemetryData(float time);

  // Getters for data
  const LeaderboardData& GetLeaderboardData() const { return m_leaderboardData; }
  const RelativeTimingData& GetRelativeTimingData() const { return m_relativeTimingData; }
  const TireInfoData& GetTireData() const { return m_tireData; }
  const VehicleData& GetVehicleData() const { return m_vehicleData; }
  const InputTelemetryData& GetInputTelemetryData() const { return m_inputTelemetryData; }

private:
  void InitializeLeaderboardData();
  void InitializeRelativeTimingData();
  void InitializeTireData();
  void InitializeVehicleData();
  void InitializeInputTelemetryData();

private:
  LeaderboardData m_leaderboardData;
  RelativeTimingData m_relativeTimingData;
  TireInfoData m_tireData;
  VehicleData m_vehicleData;
  InputTelemetryData m_inputTelemetryData;
};

} // namespace pacemaker