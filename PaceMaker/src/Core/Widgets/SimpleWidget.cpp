#include <Core/Widgets/SimpleWidget.h>

namespace pacemaker
{

//------------------------------------------------------------------------------
SimpleWidget::SimpleWidget(std::string_view name, Bounds bounds)
  : m_bounds(bounds), m_name(name)
{
}

} // namespace pacemaker