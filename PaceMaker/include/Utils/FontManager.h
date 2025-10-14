#pragma once
#include <unordered_map>
#include <string>
#include <string_view>

struct Font;
namespace pacemaker
{
  /** @brief Represents the type of a font. */
  enum class FontType
  {
    Bold,
    Regular,
  };

  /** @brief Manages font resources for the application. */
  class FontManager
  {
  public:
    /** 
     * @brief Gets the singleton instance of the FontManager.
     * @return Reference to the FontManager instance.
     */
    static FontManager& Instance();

    /**
     * @brief Registers a font with the FontManager.
     * @param type The type of the font.
     * @param font Pointer to the Font object.
     */
    void RegisterFont(FontType type, Font* font);

    /**
     * @brief Retrieves a font by its type.
     * @param type The type of the font to retrieve.
     * @return Pointer to the Font object, or nullptr if not found.
     */
    Font* GetFont(FontType type) const;

    /** @brief Gets the bold font. */
    Font* GetBoldFont() const { return GetFont(FontType::Bold); }

    /** @brief Gets the regular font. */
    Font* GetRegularFont() const { return GetFont(FontType::Regular); }

  private:
    /** @brief Default constructor for FontManager. */
    FontManager() = default;

    /** @brief Default destructor for FontManager. */
    ~FontManager() = default;

    /** @brief Deleted copy constructor for FontManager. */
    FontManager(const FontManager&) = delete;

    /** @brief Deleted copy assignment operator for FontManager. */
    FontManager& operator=(const FontManager&) = delete;

  private:
    std::unordered_map<FontType, Font*> m_fonts; // Map of font types to Font pointers
  };

} // namespace pacemaker