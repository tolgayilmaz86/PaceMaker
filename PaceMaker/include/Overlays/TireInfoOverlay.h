#pragma once

#include <Core/Widgets/BaseWidget.h>
#include <Core/IDataConsumer.h>
#include <Core/IRenderable.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>

// Forward declarations
struct Color;
struct Font;

namespace pacemaker
{

struct TireInfoData
{
    float temperatures[4];  // FL, FR, RL, RR
    float pressures[4];     // FL, FR, RL, RR
    float wear[4];          // FL, FR, RL, RR (percentage)
};

class TireInfoOverlay : public BaseWidget, public IDataConsumer<TireInfoData>
{
public:
    TireInfoOverlay(
        Bounds bounds,
        MinSize minSize,
        Font* font,
        DataBroker<TireInfoData>& broker
    );

    ~TireInfoOverlay() override = default;

    void OnDataUpdated(const TireInfoData& data) override { m_data = data; }
    void Render() const override;

private:
    TireInfoData m_data{};
    DataBroker<TireInfoData>::SubscriptionId m_subscriptionId{ 0 };
    Font* m_font{ nullptr };
};

} // namespace pacemaker