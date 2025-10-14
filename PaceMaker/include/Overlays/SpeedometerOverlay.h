#pragma once

#include <Core/Widgets/BaseWidget.h>
#include <Core/IDataConsumer.h>
#include <Core/IRenderable.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>

#include <string>

// Forward declarations
struct Color;
struct Font;

namespace pacemaker
{

class SpeedometerOverlay : public BaseWidget, public IDataConsumer<VehicleData>
{
public:
    SpeedometerOverlay(
        Bounds bounds,
        MinSize minSize,
        Font* font,
        DataBroker<VehicleData>& broker
    );

    ~SpeedometerOverlay() override = default;

    void OnDataUpdated(const VehicleData& data) override { m_data = data; }
    void Render() const override;

private:
    VehicleData m_data{};
    DataBroker<VehicleData>::SubscriptionId m_subscriptionId{ 0 };
    Font* m_font{ nullptr };
};

} // namespace pacemaker