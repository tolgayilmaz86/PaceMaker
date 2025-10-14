#include <Utils/FontManager.h>

#include <raylib.h>

namespace pacemaker
{

//------------------------------------------------------------------------------
FontManager& FontManager::Instance() {
  static FontManager instance;
  return instance;
}

//------------------------------------------------------------------------------
void FontManager::RegisterFont(FontType type, Font* font) {
  m_fonts[type] = font;
}

//------------------------------------------------------------------------------
Font* FontManager::GetFont(FontType type) const {
  auto it = m_fonts.find(type);
  if (it != m_fonts.end()) {
    return it->second;
  }
  return nullptr;
}

} // namespace pacemaker