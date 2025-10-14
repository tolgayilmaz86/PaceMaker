#pragma once

#include <Core/Widgets/BaseWidget.h>

#include <vector>
#include <memory>
#include <algorithm>

namespace pacemaker
{

/**
 * @brief Manages a collection of widgets, handling their lifecycle, rendering, and interaction.
 */
class WidgetManager {

public:
  /**
   * @brief Default constructor for WidgetManager.
   */
  WidgetManager() = default;
  
  /**
   * @brief Adds the provided widget and takes ownership of it.
   * @param widget A std::unique_ptr<BaseWidget> that is moved into the function. 
   *               Ownership of the widget is transferred to AddWidget.
   */
  void AddWidget(std::unique_ptr<BaseWidget> widget);

  /**
   * @brief Removes the widget with the specified name from the manager.
   * @param name The name of the widget to remove.
   */
  void RemoveWidget(std::string_view name);

  /**
   * @brief Updates all managed widgets.
   * @param deltaTime The time elapsed since the last update, typically in seconds.
   */
  void Update(float deltaTime) {
    for (auto& widget : m_widgets) {
      widget->Update(deltaTime);
    }
  }

  /**
   * @brief Renders all managed widgets.
   */
  void Render() const;

  /**
   * @brief Renders borders around widgets if edit mode is enabled.
   * @param mouseX The current x-coordinate of the mouse cursor.
   * @param mouseY The current y-coordinate of the mouse cursor.
   */
  void RenderBorders(int mouseX, int mouseY) const;

  /**
   * @brief Handles mouse button press events.
   * @param x The x-coordinate of the mouse cursor.
   * @param y The y-coordinate of the mouse cursor.
   */
  void HandleMousePressed(int x, int y);

  /**
   * @brief Handles mouse button release events.
   * @param x The x-coordinate of the mouse cursor.
   * @param y The y-coordinate of the mouse cursor.
   */
  void HandleMouseReleased(int x, int y);

  /**
   * @brief Handles mouse drag events.
   * @param x The current x-coordinate of the mouse cursor.
   * @param y The current y-coordinate of the mouse cursor.
   */
  void HandleMouseDragged(int x, int y);

  /**
   * @brief Enables or disables edit mode.
   * @param enabled true to enable edit mode; false to disable it.
   */
  void SetEditMode(bool enabled) noexcept { m_editMode = enabled; }
  
  /**
   * @brief Checks if edit mode is currently enabled.
   * @return true if edit mode is enabled; otherwise false.
   */
  [[nodiscard]] bool IsEditMode() const noexcept { return m_editMode; }

  /**
   * @brief Saves the configuration of all managed widgets.
   */
  void SaveAllConfigs() const;

  /**
   * @brief Loads the configuration of all managed widgets.
   */
  void LoadAllConfigs();

private:
  std::vector<std::unique_ptr<BaseWidget>> m_widgets; // Owned widgets
  bool m_editMode{ false }; // Edit mode flag

};
} // namespace pacemaker