#pragma once

#include <Core/Widgets/BaseWidget.h>
#include <Core/IDataConsumer.h>
#include <Core/IRenderable.h>
#include <Core/IConfigurable.h>
#include <Core/IDraggable.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>

// Forward declarations for Raylib
struct Font;

namespace pacemaker
{

/**
 * @brief UI widget that displays input telemetry data such as steering, throttle, and brake over time.
 */
class InputTelemetryOverlay : public BaseWidget, public IDataConsumer<InputTelemetryData>
{
public:
  /**
   * @brief Constructs a new InputTelemetryOverlay.
   * @param bounds The initial bounds of the overlay.
   * @param minSize The minimum size of the overlay.
   * @param font The font to use for rendering text.
   * @param broker The data broker to subscribe to for input telemetry data.
   */
  InputTelemetryOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    DataBroker<InputTelemetryData>& broker
  );

  /**
   * @brief Defaulted destructor for InputTelemetryOverlay that overrides the base class destructor.
   */
  ~InputTelemetryOverlay() override = default;

  /**
   * @copydoc IDataConsumer::OnDataUpdated
   */
  void OnDataUpdated(const InputTelemetryData& data) override;

  /**
   * @copydoc IRenderable::Render
   */
  void Render() const override;

private:
  InputTelemetryData m_data{}; // Latest telemetry data
  std::deque<InputTelemetryData> m_history; // History of telemetry data for graphing
  DataBroker<InputTelemetryData>::SubscriptionId m_subscriptionId{ 0 }; // Subscription ID for data updates
  Font* m_font{ nullptr }; // Font used for rendering text
};

} // namespace pacemaker