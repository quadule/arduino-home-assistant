#include <AUnit.h>
#include <ArduinoHA.h>

#define prepareTest \
    initMqttTest(testDeviceId) \
    lastCommandCallbackCall.reset();

#define assertCommandCallbackCalled(expectedString, callerPtr) \
    assertTrue(lastCommandCallbackCall.called); \
    assertTrue(expectedString == lastCommandCallbackCall.string); \
    assertEqual(callerPtr, lastCommandCallbackCall.caller);

#define assertCommandCallbackNotCalled() \
    assertFalse(lastCommandCallbackCall.called);

using aunit::TestRunner;

struct CommandCallback {
    bool called = false;
    String string;
    HAText* caller = nullptr;

    void reset() {
        called = false;
        string = "";
        caller = nullptr;
    }
};

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueText";
static CommandCallback lastCommandCallbackCall;

const char ConfigTopic[] PROGMEM = {"homeassistant/text/testDevice/uniqueText/config"};
const char CommandTopic[] PROGMEM = {"testData/testDevice/uniqueText/cmd_t"};
const char StateTopic[] PROGMEM = {"testData/testDevice/uniqueText/stat_t"};

void onCommandReceived(const char* string, HAText* caller)
{
    lastCommandCallbackCall.called = true;
    lastCommandCallbackCall.string = string;
    lastCommandCallbackCall.caller = caller;
}

AHA_TEST(TextTest, invalid_unique_id) {
    prepareTest

    HAText text(nullptr);
    text.buildSerializerTest();
    HASerializer* serializer = text.getSerializer();

    assertTrue(serializer == nullptr);
}

AHA_TEST(TextTest, default_params) {
    prepareTest

    HAText text(testUniqueId);
    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
    assertEqual(2, mock->getFlushedMessagesNb()); // config + default state
}

AHA_TEST(TextTest, command_subscription) {
    prepareTest

    HAText text(testUniqueId);
    mqtt.loop();

    assertEqual(1, mock->getSubscriptionsNb());
    assertEqual(AHATOFSTR(CommandTopic), mock->getSubscriptions()[0]->topic);
}

AHA_TEST(TextTest, availability) {
    prepareTest

    HAText text(testUniqueId);
    text.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HAText
    assertMqttMessage(
        1,
        F("testData/testDevice/uniqueText/avty_t"),
        "online",
        true
    )
}

AHA_TEST(TextTest, publish_nothing_if_retained) {
    prepareTest

    HAText text(testUniqueId);
    text.setRetain(true);
    text.setCurrentState("example");
    mqtt.loop();

    assertEqual(1, mock->getFlushedMessagesNb()); // only config should be pushed
}

AHA_TEST(TextTest, publish_state_empty_string) {
    prepareTest

    HAText text(testUniqueId);
    mqtt.loop();

    assertEqual(2, mock->getFlushedMessagesNb());
    assertMqttMessage(1, AHATOFSTR(StateTopic), "", true)
}

AHA_TEST(TextTest, set_state) {
    prepareTest

    mock->connectDummy();
    HAText text(testUniqueId);
    const char* value = "new state";

    assertTrue(text.setState(value));
    assertSingleMqttMessage(AHATOFSTR(StateTopic), "new state", true)
}

AHA_TEST(TextTest, current_state_setter_getter) {
    prepareTest

    HAText text(testUniqueId);
    const char* value = "current state";

    text.setCurrentState(value);
    assertTrue(text.getCurrentState());
    assertEqual(value, text.getCurrentState());
}

AHA_TEST(TextTest, name_setter) {
    prepareTest

    HAText text(testUniqueId);
    text.setName("testName");

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"name\":\"testName\","
            "\"uniq_id\":\"uniqueText\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, entity_category_setter) {
    initMqttTest(testDeviceId)

    HAText text(testUniqueId);
    text.setEntityCategory(HAText::EntityCategoryConfig);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"ent_cat\":\"config\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, icon_setter) {
    prepareTest

    HAText text(testUniqueId);
    text.setIcon("testIcon");

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"ic\":\"testIcon\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, retain_setter) {
    prepareTest

    HAText text(testUniqueId);
    text.setRetain(true);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"ret\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, optimistic_setter) {
    prepareTest

    HAText text(testUniqueId);
    text.setOptimistic(true);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"opt\":true,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, mode_setter_text) {
    prepareTest

    HAText text(testUniqueId);
    text.setMode(HAText::ModeText);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, mode_setter_password) {
    prepareTest

    HAText text(testUniqueId);
    text.setMode(HAText::ModePassword);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"mode\":\"password\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, min_setter) {
    initMqttTest(testDeviceId)

    HAText text(testUniqueId);
    text.setMin(2);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"min\":2,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, max_setter) {
    initMqttTest(testDeviceId)

    HAText text(testUniqueId);
    text.setMax(64);

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"max\":64,"
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, pattern_setter) {
    prepareTest

    HAText text(testUniqueId);
    text.setPattern("testPattern");

    assertEntityConfig(
        mock,
        text,
        (
            "{"
            "\"uniq_id\":\"uniqueText\","
            "\"ptrn\":\"testPattern\","
            "\"dev\":{\"ids\":\"testDevice\"},"
            "\"stat_t\":\"testData/testDevice/uniqueText/stat_t\","
            "\"cmd_t\":\"testData/testDevice/uniqueText/cmd_t\""
            "}"
        )
    )
}

AHA_TEST(TextTest, command_text_empty) {
    prepareTest

    HAText text(testUniqueId);
    text.onCommand(onCommandReceived);
    mock->fakeMessage(AHATOFSTR(CommandTopic), "");


    assertCommandCallbackCalled(String(""), &text)
}

void setup()
{
    delay(1000);
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    TestRunner::run();
    delay(1);
}
