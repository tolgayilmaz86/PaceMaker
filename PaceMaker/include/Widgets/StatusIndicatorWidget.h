#pragma once
#include <Core/Widgets/SimpleWidget.h>

// Forward declarations
struct Font;

namespace pacemaker
{

/**
 * @brief A simple status indicator widget that displays a status message.
 */
class StatusIndicatorWidget : public SimpleWidget
{
public:
	/**
	 * @brief Constructs a StatusIndicatorWidget with the specified bounds and font.
	 * @param bounds The position and size of the widget.
	 * @param font The font used for rendering text.
   */
	StatusIndicatorWidget(Bounds bounds, Font* font);

	/**
	 * @brief Default virtual destructor for StatusIndicatorWidget.
    */
	~StatusIndicatorWidget() override = default;

	/**
	 * @copydoc IRenderable::Render
    */
	void Render() const override;

private:
  Font* m_font{ nullptr };	// Font used for rendering text
};

} // namespace pacemaker