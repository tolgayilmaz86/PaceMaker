#include <Widgets/StatusIndicatorWidget.h>

#include <raylib.h>

namespace pacemaker
{

//------------------------------------------------------------------------------
StatusIndicatorWidget::StatusIndicatorWidget(Bounds bounds, Font* font)
	: SimpleWidget("StatusIndicator", bounds)
	, m_font(font)
{
}

//------------------------------------------------------------------------------
void StatusIndicatorWidget::Render() const
{
	if (!m_isVisible)
		return;

	// Draw background
	DrawRectangle(
		m_bounds.x, 
		m_bounds.y, 
		m_bounds.width, 
		m_bounds.height, 
		Color{ 0, 0, 0, 180 }
	);

	// Draw text
	DrawTextEx(
		*m_font, 
		"MOVE MODE - Press Ctrl+F6 to exit",
		{ (float)(m_bounds.x + 15), (float)(m_bounds.y + 5) }, 
		28, 
		1, 
		YELLOW
	);
}

} // namespace pacemaker