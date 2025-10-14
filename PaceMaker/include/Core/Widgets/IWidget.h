#pragma once
#include <Utils/Geometry.h>

#include <string_view>

namespace pacemaker
{

// Base widget interface
class IWidget
{
public:
	/**
	 * @brief Virtual destructor for IWidget.
   */
	virtual ~IWidget() = default;

	/**
	 * @brief Gets the widget's name giving opportunity to distinguish among other widgets.
	 * @return The name of the widget.
	 */
	[[nodiscard]] virtual std::string_view GetName() const noexcept = 0;

	/**
	 * @brief Gets the widget's position and size (Bounds).
	 * @return The widget's bounds.
	 */
	[[nodiscard]] virtual const Bounds& GetBounds() const noexcept = 0;
	/**
	 * @brief Sets the widget's position and size (Bounds).
	 * @param bounds The new bounds to set.
   */
	virtual void SetBounds(const Bounds& bounds) = 0;

	/**
	 * @brief Gets the widget's minimum size constraints (MinSize).
	 * @return The widget's minimum size.
	 */
  [[nodiscard]] virtual const MinSize& GetMinSize() const noexcept = 0;

	/**
	 * @brief Pure virtual method that indicates whether the object is visible.
	 * @return true if the object is visible; false otherwise.
	 */
	[[nodiscard]] virtual bool IsVisible() const noexcept = 0;

	/**
	 * @brief Sets the visibility of the widget.
	 * @param visible true to make the widget visible; false to hide it.
   */
	virtual void SetVisible(bool visible) noexcept = 0;

	/**
	 * @brief Updates the widget's state based on the elapsed time since the last update.
	 * @param deltaTime The time elapsed since the last update, typically in seconds.
   */
	virtual void Update(float deltaTime) = 0;
};
} // namespace pacemaker