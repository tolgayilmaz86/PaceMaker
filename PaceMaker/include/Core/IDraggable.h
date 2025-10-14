#pragma once

namespace pacemaker
{

class IDraggable
{
public:
  /**
   * @brief Virtual destructor for IDraggable.
   */
  virtual ~IDraggable() = default;

  /**
   * @brief Handles a mouse button press event (not released yet) at the specified coordinates.
   * @param x The x-coordinate of the mouse cursor, typically in pixels.
   * @param y The y-coordinate of the mouse cursor, typically in pixels.
   */
  virtual void OnMousePressed(int x, int y) = 0;

  /**
   * @brief Called when a mouse button is released at the specified coordinates. This is a pure virtual function that must be implemented by derived classes.
   * @param x The x-coordinate of the mouse cursor (typically in pixels).
   * @param y The y-coordinate of the mouse cursor (typically in pixels).
   */
  virtual void OnMouseReleased(int x, int y) = 0;

  /**
   * @brief Called when the mouse is dragged; a pure virtual callback that must be implemented by derived classes.
   * @param x The x-coordinate of the mouse pointer during the drag (typically in pixels).
   * @param y The y-coordinate of the mouse pointer during the drag (typically in pixels).
   */
  virtual void OnMouseDragged(int x, int y) = 0;

  /**
   * @brief Determines whether the object is currently being dragged.
   * @return true if the object is currently being dragged; otherwise false.
   */
  [[nodiscard]] virtual bool IsDragging() const noexcept = 0;

  /**
   * @brief Determines whether the object is currently being resized.
   * @return true if the object is currently being resized; otherwise false.
   */
  [[nodiscard]] virtual bool IsResizing() const noexcept = 0;
};
} // namespace pacemaker