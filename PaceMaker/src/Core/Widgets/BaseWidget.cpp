#include <Core/Widgets/BaseWidget.h>

#include <raylib.h>

namespace pacemaker
{
//------------------------------------------------------------------------------
BaseWidget::BaseWidget(std::string_view name, Bounds bounds, MinSize minSize)
	: m_bounds(bounds), m_minSize(minSize), m_name(name) {
}
//------------------------------------------------------------------------------
void BaseWidget::OnMousePressed(int x, int y) {
	if (m_bounds.ContainsResizeHandle(x, y) && !m_isDragging && !m_isResizing)
	{
		m_isResizing = true;
	}
	else if (m_bounds.Contains(x, y) && !m_isDragging && !m_isResizing)
	{
		m_isDragging = true;
		m_dragOffsetX = x - m_bounds.x;
		m_dragOffsetY = y - m_bounds.y;
	}
}
//------------------------------------------------------------------------------
void BaseWidget::OnMouseReleased([[maybe_unused]] int x, [[maybe_unused]] int y) {
	m_isDragging = false;
	m_isResizing = false;
}
//------------------------------------------------------------------------------
void BaseWidget::OnMouseDragged(int x, int y) {
	if (m_isDragging)
	{
		m_bounds.x = x - m_dragOffsetX;
		m_bounds.y = y - m_dragOffsetY;
	}
	else if (m_isResizing)
	{
		int newWidth = x - m_bounds.x;
		int newHeight = y - m_bounds.y;

		if (newWidth >= m_minSize.width)
			m_bounds.width = newWidth;
		if (newHeight >= m_minSize.height)
			m_bounds.height = newHeight;
	}
}
//------------------------------------------------------------------------------
void BaseWidget::RenderBorder(int mouseX, int mouseY) const {
	Color borderColor;

	if (m_isDragging)
		borderColor = YELLOW;
	else if (m_isResizing)
		borderColor = Color{ 255, 100, 0, 255 };
	else if (m_bounds.Contains(mouseX, mouseY))
		borderColor = Color{ 255, 255, 255, 100 };
	else
		return;

	DrawRectangleLines(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height, borderColor);

	if (m_bounds.ContainsResizeHandle(mouseX, mouseY) || m_isResizing)
	{
		int handleSize = 15;
		DrawTriangle(
			{ (float)(m_bounds.x + m_bounds.width), (float)(m_bounds.y + m_bounds.height) },
			{ (float)(m_bounds.x + m_bounds.width - handleSize), (float)(m_bounds.y + m_bounds.height) },
			{ (float)(m_bounds.x + m_bounds.width), (float)(m_bounds.y + m_bounds.height - handleSize) },
			m_isResizing ? Color{ 255, 100, 0, 200 } : Color{ 255, 255, 255, 150 }
		);
	}
}
} // namespace pacemaker