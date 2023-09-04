#include "HAText.h"
#ifndef EX_ARDUINOHA_TEXT

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HAText::HAText(const char* uniqueId, unsigned int maxLength) :
    HABaseDeviceType(AHATOFSTR(HAComponentText), uniqueId),
    _class(nullptr),
    _category(EntityCategoryDefault),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _mode(ModeText),
    _minValue(),
    _maxValue(),
    _pattern(),
    _currentState(),
    _commandCallback(nullptr)
{
    _currentState.reserve(maxLength);
}

bool HAText::setState(const char* state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HAText::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 15); // 15 - max properties nb

    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(
        AHATOFSTR(HAEntityCategoryProperty),
        getCategoryProperty(),
        HASerializer::ProgmemPropertyValue
    );

    if (_mode != ModeText) {
        _serializer->set(
            AHATOFSTR(HAModeProperty),
            getModeProperty(),
            HASerializer::ProgmemPropertyValue
        );
    }

    if (_minValue.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMinProperty),
            &_minValue,
            HASerializer::NumberPropertyType
        );
    }

    if (_maxValue.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMaxProperty),
            &_maxValue,
            HASerializer::NumberPropertyType
        );
    }

    if (_pattern.length() > 0) {
        _serializer->set(
            AHATOFSTR(HAPatternProperty),
            _pattern.c_str(),
            HASerializer::ConstCharPropertyValue
        );
    }

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAText::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState.c_str());
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HAText::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        handleCommand(payload);
    }
}

bool HAText::publishState(const char* state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        state,
        true
    );
}

void HAText::handleCommand(const uint8_t* cmd)
{
    if (!_commandCallback) {
        return;
    }

    _commandCallback((const char*)cmd, this);
}

const __FlashStringHelper* HAText::getCategoryProperty() const
{
    switch (_category) {
    case EntityCategory::EntityCategoryConfig:
        return AHATOFSTR(HAEntityCategoryConfig);

    case EntityCategory::EntityCategoryDiagnostic:
        return AHATOFSTR(HAEntityCategoryDiagnostic);

    default:
        return nullptr;
    }
}

const __FlashStringHelper* HAText::getModeProperty() const
{
    switch (_mode) {
    case ModePassword:
        return AHATOFSTR(HAModePassword);

    default:
        return nullptr;
    }
}

#endif
