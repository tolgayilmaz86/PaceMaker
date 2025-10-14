#pragma once

#include <concepts>

namespace pacemaker
{

/**
 * @brief Concept that checks if a type is valid telemetry data.
 */
template<typename T>
concept TelemetryData = requires(T data) {
	requires std::is_copy_constructible_v<T>;
	requires std::is_move_constructible_v<T>;
};

/**
 * @brief Interface for classes that consume telemetry data updates.
 * 
 * @tparam T The type of telemetry data, must satisfy the TelemetryData concept.
 */
template<TelemetryData T>
class IDataConsumer
{
public:
	/**
	 * @brief Virtual destructor for IDataConsumer.
   */
	virtual ~IDataConsumer() = default;

	/**
	 * @brief Callback invoked when new telemetry data is available.
	 * 
	 * @param data The updated telemetry data.
   */
	virtual void OnDataUpdated(const T& data) = 0;
};
} // namespace pacemaker