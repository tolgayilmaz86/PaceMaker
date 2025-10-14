#pragma once
#include <Core/Widgets/IWidget.h>

#include <Core/IRenderable.h>
#include <Core/IConfigurable.h>
#include <Core/IDraggable.h>
#include <Utils/Geometry.h>

#include <string>

namespace pacemaker
{

/**
 * @brief Base implementation of a widget that provides common functionality for draggable, renderable, and configurable widgets.
 */
class BaseWidget : public IWidget, public IDraggable, public IRenderable, public IConfigurable
{

public:
  /**
   * @brief Initializes a BaseWidget with the specified name, bounds, and minimum size.
   * @param name The widget's name.
   * @param bounds The widget's position and size (Bounds).
   * @param minSize The widget's minimum size constraints (MinSize).
   */
  BaseWidget(std::string_view name, Bounds bounds, MinSize minSize);

  /**
   * @brief Default virtual destructor for BaseWidget.
   */
  virtual ~BaseWidget() = default;

  // IDraggable implementation

  /**
   * @copydoc IDraggable::OnMousePressed
   */
  void OnMousePressed(int x, int y) override;

  /**
   * @copydoc IDraggable::OnMouseReleased
   */
  void OnMouseReleased(int x, int y) override;

  /**
   * @copydoc IDraggable::OnMouseDragged
   */
  void OnMouseDragged(int x, int y) override;

  /**
   * @copydoc IDraggable::IsDragging
   */
  [[nodiscard]] bool IsDragging() const noexcept override { return m_isDragging; }

  /**
   * @copydoc IDraggable::IsResizing
   */
  [[nodiscard]] bool IsResizing() const noexcept override { return m_isResizing; }

  /**
   * @copydoc IWidget::GetName
   */
  [[nodiscard]] std::string_view GetName() const noexcept override { return m_name; }

  /**
   * @copydoc IWidget::GetBounds
   */
  [[nodiscard]] const Bounds& GetBounds() const noexcept override { return m_bounds; }

  /**
   * @copydoc IWidget::GetMinSize
   */
  [[nodiscard]] const MinSize& GetMinSize() const noexcept override { return m_minSize; }

  /**
   *  @copydoc IWidget::SetBounds
   */
  void SetBounds(const Bounds& bounds) override { m_bounds = bounds; }

  /**
   * @copydoc IWidget::IsVisible
   */
  [[nodiscard]] bool IsVisible() const noexcept override { return m_isVisible; }

  /**
   * @copydoc IWidget::SetVisible
   */
  void SetVisible(bool visible) noexcept override { m_isVisible = visible; }

  /**
   * @copydoc IRenderable::RenderBorder
   */
  void RenderBorder(int mouseX, int mouseY) const override;

  /**
   * @copydoc IConfigurable::SaveConfig
   */
  void SaveConfig() const override {}

  /**
   * @copydoc IConfigurable::LoadConfig
   */
  void LoadConfig() override {}

  /**
   * @copydoc IWidget::Update
   */
  void Update(float deltaTime) override {}


protected:
  Bounds m_bounds{};          // Position and size of the widget
  MinSize m_minSize{};        // Minimum size constraints
  bool m_isDragging{ false }; // Indicates if the widget is currently being dragged
  bool m_isResizing{ false }; // Indicates if the widget is currently being resized
  bool m_isVisible{ true };   // Indicates if the widget is visible
  int m_dragOffsetX{ 0 };     // X Offset from mouse position to widget origin when dragging
  int m_dragOffsetY{ 0 };     // Y Offset from mouse position to widget origin when dragging
  std::string m_name{};       // Name of the widget
};
}