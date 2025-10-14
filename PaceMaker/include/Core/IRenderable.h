#pragma once

namespace pacemaker
{
/**
 * @brief Interface for renderable objects.
 */
class IRenderable
{
public:
  /**
   * @brief Virtual destructor for IRenderable.
   */
  virtual ~IRenderable() = default;

  /**
   * @brief Renders the object.
   */
  virtual void Render() const = 0;

  /**
   * @brief Renders a border around the object, typically used in edit mode.
   * @param mouseX The current x-coordinate of the mouse cursor.
   * @param mouseY The current y-coordinate of the mouse cursor.
   */
  virtual void RenderBorder(int mouseX, int mouseY) const = 0;
};
} // namespace pacemaker