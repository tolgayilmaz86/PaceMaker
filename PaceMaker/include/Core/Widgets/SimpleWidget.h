#pragma once
#include <Core/Widgets/IWidget.h>
#include <Core/IRenderable.h>
#include <Utils/Geometry.h>

#include <string>

namespace pacemaker
{

/**
 * @brief SimpleWidget is a lightweight widget that only supports rendering.
 */
class SimpleWidget : public IWidget, public IRenderable
{

public:
	/**
	 * @brief Constructs a SimpleWidget with the specified name and bounds.
	 * @param name The widget's name as a string view.
	 * @param bounds A Bounds object that specifies the widget's position and size.
	 */
	SimpleWidget(std::string_view name, Bounds bounds);

	/**
	 * @brief Default virtual destructor for SimpleWidget.
   */
	virtual ~SimpleWidget() = default;

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
	 * @copydoc IWidget::SetBounds
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
	 * @copydoc IRenderable::Render
   */
	void Update(float deltaTime) override {}

	/**
	 * @copydoc IRenderable::RenderBorder
   */
	void RenderBorder([[maybe_unused]] int mouseX, [[maybe_unused]] int mouseY) const override {}

protected:
  Bounds m_bounds{};					// Position and size of the widget
	MinSize m_minSize{};        // Minimum size constraints
  bool m_isVisible{ true };		// Indicates if the widget is visible
  std::string m_name{};				// Name of the widget

};

} // namespace pacemaker