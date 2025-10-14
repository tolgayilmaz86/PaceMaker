#pragma once

namespace pacemaker
{

/**
 * @brief Interface for configurable objects that can save and load their configuration.
 */
class IConfigurable
{
public:
	/**
	 * @brief Virtual destructor for IConfigurable.
    */
	virtual ~IConfigurable() = default;

	/**
	 * @brief Saves the current configuration of the object.
    */
	virtual void SaveConfig() const = 0;

	/**
	 * @brief Loads the configuration of the object.
    */
	virtual void LoadConfig() = 0;
};

} // namespace pacemaker