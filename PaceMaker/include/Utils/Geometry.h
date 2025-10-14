#pragma once

namespace pacemaker
{

/**
 * @brief Represents the position and size of a widget.
 */
struct Bounds
{
	int x, y;
	int width, height;

	/**
	 * @brief Checks if a point (px, py) is within the bounds.
	 * @param px The x-coordinate of the point to check.
	 * @param py The y-coordinate of the point to check.
	 * @return true if the point is within the bounds; false otherwise.
   */
	[[nodiscard]] constexpr bool Contains(int px, int py) const noexcept {
		return px >= x && px <= x + width && py >= y && py <= y + height;
	}

	/**
	 * @brief Checks if a resize handle (typically at the bottom-right corner) contains the point (px, py).
	 * @param px The x-coordinate of the point to check.
	 * @param py The y-coordinate of the point to check.
	 * @param handleSize The size of the resize handle (default is 20).
	 * @return true if the point is within the resize handle; false otherwise.
	 */
	[[nodiscard]] constexpr bool ContainsResizeHandle(int px, int py, int handleSize = 20) const noexcept {
		return px >= (x + width - handleSize) && px <= (x + width) &&
			py >= (y + height - handleSize) && py <= (y + height);
	}
};

/**
 * @brief Represents the minimum size constraints for a widget.
 */
struct MinSize
{
	int width;
	int height;
};

} // namespace pacemaker