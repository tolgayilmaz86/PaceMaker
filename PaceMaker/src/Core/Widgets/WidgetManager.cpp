#include <Core/Widgets/WidgetManager.h>

#include <ranges>

namespace pacemaker
{
//------------------------------------------------------------------------------
void WidgetManager::AddWidget(std::unique_ptr<BaseWidget> widget) {
    m_widgets.push_back(std::move(widget));
}

void WidgetManager::RemoveWidget(std::string_view name) {
    std::erase_if(m_widgets, [name](const auto& widget) {
        return widget->GetName() == name;
        });
}
//------------------------------------------------------------------------------
void WidgetManager::Render() const {
    for (const auto& widget : m_widgets)
    {
        widget->Render();
    }
}
//------------------------------------------------------------------------------
void WidgetManager::RenderBorders(int mouseX, int mouseY) const {
    if (!m_editMode) return;

    for (const auto& widget : m_widgets)
    {
        widget->RenderBorder(mouseX, mouseY);
    }
}
//------------------------------------------------------------------------------
void WidgetManager::HandleMousePressed(int x, int y) {
    if (!m_editMode) return;

    for (auto& widget : m_widgets | std::views::reverse)
    {
        if (widget->GetBounds().Contains(x, y))
        {
            widget->OnMousePressed(x, y);
            break;
        }
    }
}
//------------------------------------------------------------------------------
void WidgetManager::HandleMouseReleased(int x, int y) {
    if (!m_editMode) return;

    for (auto& widget : m_widgets)
    {
        widget->OnMouseReleased(x, y);
    }
}
//------------------------------------------------------------------------------
void WidgetManager::HandleMouseDragged(int x, int y) {
    if (!m_editMode) return;

    for (auto& widget : m_widgets)
    {
        if (widget->IsDragging() || widget->IsResizing())
        {
            widget->OnMouseDragged(x, y);
        }
    }
}
//------------------------------------------------------------------------------
void WidgetManager::SaveAllConfigs() const {
    for (const auto& widget : m_widgets)
    {
        widget->SaveConfig();
    }
}
//------------------------------------------------------------------------------
void WidgetManager::LoadAllConfigs() {
    for (auto& widget : m_widgets)
    {
        widget->LoadConfig();
    }
}
} // namespace pacemaker