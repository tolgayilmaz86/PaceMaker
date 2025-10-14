#pragma once

#include <vector>
#include <string>
#include <deque>

namespace pacemaker
{
  // Player data structure
  struct PlayerData
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

  // Leaderboard data structure
  struct LeaderboardData
  {
    std::vector<PlayerData> players;
    std::string sessionType;
    std::string sessionTime;
  };

  // Relative timing player structure
  struct RelativePlayer
  {
    int position;
    int number;
    std::string name;
    std::string teamCode;       // e.g., "HY", "GT3"
    float gap;                  // Gap in seconds (positive = ahead, negative = behind)
    int teamColorIndex;
  };

  // Tire data structure
  struct TireData
  {
    float temperatures[4];      // FL, FR, RL, RR temperatures
    float pressures[4];         // FL, FR, RL, RR pressures
    float wear[4];             // FL, FR, RL, RR wear percentage
  };

  // Vehicle data structure
  struct VehicleData
  {
    int speed;                  // Speed in MPH or KPH
    int gear;                   // Current gear
    float rpm;                  // RPM (0-1 normalized)
    float engineTemp;           // Engine temperature
    float oilTemp;              // Oil temperature
    float fuelPercent;          // Fuel percentage
    float ersPercent;           // ERS percentage
    bool drsEnabled;            // DRS active
    std::string lapTime;        // Current lap time
    std::string lastLap;        // Last lap time
  };

  struct RelativePlayerData
  {
    int position;
    int number;
    std::string name;
    std::string teamCode;
    float gap;  // Gap in seconds (positive = ahead, negative = behind)
    int teamColorIndex;
  };

  struct RelativeTimingData
  {
    std::vector<RelativePlayerData> players;
    int playerPosition;
  };

  struct InputTelemetryData
  {
    float steering;     // -1.0 (left) to 1.0 (right)
    float throttle;     // 0.0 to 1.0
    float brake;        // 0.0 to 1.0
    short gear{1};         // -1=Reverse, 0=Neutral, 1+=Forward gears
    float rpm{1.85f};   // Engine RPM (0 to 10000)
    static constexpr int MAX_HISTORY = 200;
  };
} // namespace pacemaker