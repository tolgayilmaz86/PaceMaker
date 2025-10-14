#pragma once

#include <Core/Widgets/BaseWidget.h>
#include <Core/IDataConsumer.h>
#include <Core/IRenderable.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>

#include <vector>
#include <span>

// Forward declarations
struct Color;
struct Font;

namespace pacemaker
{

class RelativeTimingOverlay : public BaseWidget, public IDataConsumer<RelativeTimingData>
{
public:
    RelativeTimingOverlay(
        Bounds bounds,
        MinSize minSize,
        Font* font,
        std::span<const Color> teamColors,
        DataBroker<RelativeTimingData>& broker
    );

    ~RelativeTimingOverlay() override = default;

    void OnDataUpdated(const RelativeTimingData& data) override { m_data = data; }
    void Render() const override;

private:
    void DrawPlayerRow(const RelativePlayerData& player, int x, int y, int rowHeight, bool isPlayer, int width) const;

private:
    RelativeTimingData m_data{};
    DataBroker<RelativeTimingData>::SubscriptionId m_subscriptionId{ 0 };
    Font* m_font{ nullptr };
    std::span<const Color> m_teamColors;
};

} // namespace pacemaker