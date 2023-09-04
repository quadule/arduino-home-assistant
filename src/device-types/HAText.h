#ifndef AHA_HATEXT_H
#define AHA_HATEXT_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_TEXT

#define HATEXT_CALLBACK(name) void (*name)(const char* str, HAText* sender)

/**
 * HAText adds a input field in the Home Assistant panel
 * that controls the string value stored on your device.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/text.mqtt/
 */
class HAText : public HABaseDeviceType
{
public:
    /// Represents mode of the string.
    enum Mode {
        ModeText = 0,
        ModePassword
    };

    /**
     * Creates instance of the HAText entity with the given maximum length.
     *
     * @param uniqueId The unique ID of the string. It needs to be unique in a scope of your device.
     * @param maxLength The maximum capacity of the string. Home Assistant has a max of 255.
     */
    HAText(const char* uniqueId, unsigned int maxLength = 255);

    /**
     * Changes state of the string and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the string.
     * @param force Forces to update state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const char* state, const bool force = false);

    /**
     * Sets current state of the string without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the string.
     */
    inline void setCurrentState(const char* state)
        { _currentState = state; }

    /**
     * Returns last known state of the string.
     * If setState method wasn't called the initial value will be returned.
     */
    inline const char* getCurrentState() const
        { return _currentState.c_str(); }

    /**
     * Sets the entity category of the device if different from the default.
     *
     * @param entityCategory `EntityCategoryConfig` or `EntityCategoryDiagnostic`
     */
    inline void setEntityCategory(EntityCategory entityCategory)
        { _category = entityCategory; }

    /**
     * Sets icon of the string.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the string's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the string state.
     * In this mode the string state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Sets mode of the string.
     * It controls how the string should be displayed in the UI.
     * By default it's `HAText::ModeText`.
     *
     * @param mode Mode to set.
     */
    inline void setMode(const Mode mode)
        { _mode = mode; }

    /**
     * Sets the minimum value that can be set from the Home Assistant panel.
     *
     * @param min The minimal value. By default the value is not set.
     */
    inline void setMin(const unsigned int min)
        { _minValue = HANumeric(min, 0); }

    /**
     * Sets a regex pattern for client-side validation of the text.
     *
     * @param pattern The regex pattern.
     */
    inline void setPattern(const char* pattern)
        { _pattern = pattern; }

    /**
     * Sets the maximum value that can be set from the Home Assistant panel.
     *
     * @param min The maximum value. By default the value is not set.
     */
    inline void setMax(const unsigned int max)
        { _maxValue = HANumeric(max, 0); }

    /**
     * Registers callback that will be called each time the string is changed in the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same string.
     *
     * @param callback
     * @note In non-optimistic mode, the string must be reported back to HA using the HAText::setState method.
     */
    inline void onCommand(HATEXT_CALLBACK(callback))
        { _commandCallback = callback; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const char* state);

    /**
     * Parses the given command and executes the string's callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleCommand(const uint8_t* cmd);

    /**
     * Returns progmem string representing the entity category.
     */
    const __FlashStringHelper* getCategoryProperty() const;

    /**
     * Returns progmem string representing mode of the string
     */
    const __FlashStringHelper* getModeProperty() const;

    /// The device class. It can be nullptr.
    const char* _class;

    /// The entity category.
    EntityCategory _category;

    /// The icon of the string. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the string (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// Controls how the string should be displayed in the UI. By default it's `HAText::ModeAuto`.
    Mode _mode;

    /// The minimal value that can be set from the HA panel.
    HANumeric _minValue;

    /// The maximum value that can be set from the HA panel.
    HANumeric _maxValue;

    /// A regex pattern for client-side validation.
    String _pattern;

    /// The current state of the string. By default the value is not set.
    String _currentState;

    /// The callback that will be called when the command is received from the HA.
    HATEXT_CALLBACK(_commandCallback);
};

#endif
#endif
