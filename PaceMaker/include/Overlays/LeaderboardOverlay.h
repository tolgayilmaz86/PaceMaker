#pragma once

#include <Core/Widgets/BaseWidget.h>
#include <Core/IDataConsumer.h>
#include <Core/IRenderable.h>
#include <Core/IConfigurable.h>
#include <Core/IDraggable.h>
#include <Data/DataBroker.hpp>
#include <Data/DataStructs.h>

#include <vector>
#include <span>

// Forward declarations for Raylib
struct Color;
struct Font;

namespace pacemaker
{

/**
 * @brief UI widget that displays a leaderboard, consumes leaderboard data updates, and renders player rows.
 */
class LeaderboardOverlay : public BaseWidget, public IDataConsumer<LeaderboardData>
{

public:
  /**
   * @brief Constructs a LeaderboardOverlay configured with layout bounds, minimum size, font, team colors, and a data broker.
   * @param bounds The area within which the overlay is laid out and rendered.
   * @param minSize Minimum size constraints for the overlay.
   * @param font Pointer to the Font used for rendering text in the overlay.
   * @param teamColors A read-only span of Colors used to color teams in the leaderboard.
   * @param broker Reference to a DataBroker<LeaderboardData> that provides and updates the leaderboard data.
   */
  LeaderboardOverlay(
    Bounds bounds,
    MinSize minSize,
    Font* font,
    std::span<const Color> teamColors,
    DataBroker<LeaderboardData>& broker
  );

  /**
   * @brief Defaulted destructor for LeaderboardOverlay that overrides the base class destructor.
   */
  ~LeaderboardOverlay() override = default;

  /**
   * @brief IDataConsumer implementation called when new data is available.
   * @param data The updated leaderboard data.
   */
  void OnDataUpdated(const LeaderboardData& data) override { m_data = data; }

  /**
   * @brief IRenderable implementation renders the leaderboard overlay.
   */
  void Render() const override;

private:
  /**
   * @brief Renders a single player's row in the UI at the specified position. This const method does not modify the object's observable state.
   * @param player Reference to the PlayerData containing the information to display (name, score, avatar, etc.).
   * @param x The x-coordinate (typically pixels) of the row's left edge.
   * @param y The y-coordinate (typically pixels) of the row's top edge.
   * @param rowHeight The height (typically in pixels) of the row to draw.
   * @param isHighlighted If true, render the row in its highlighted/selected style; otherwise render normally.
   */
  void DrawPlayerRow(const PlayerData& player, int x, int y, int rowHeight, bool isHighlighted) const;

// Private members
private:
  LeaderboardData m_data{}; // Current leaderboard data
  DataBroker<LeaderboardData>::SubscriptionId m_subscriptionId{ 0 }; // Subscription ID for data updates
  Font* m_font{ nullptr }; // Font used for rendering text
  std::span<const Color> m_teamColors; // Read-only span of team colors
};
} // namespace pacemaker