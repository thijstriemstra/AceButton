#line 2 "AceButtonLongPressTest.ino"
/*
   Copyright 2018 Brian T. Park

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ArduinoUnit.h>
#include <AdjustableButtonConfig.h>
#include <AceButton.h>
#include <testing/TestableButtonConfig.h>
#include <testing/EventTracker.h>
#include <testing/TestHelper.h>

using namespace ace_button;
using namespace ace_button::testing;

const uint8_t PIN = 13;
const uint8_t BUTTON_ID = 1;

TestableButtonConfig testableConfig;
AceButton button;
EventTracker eventTracker;
TestHelper helper(&testableConfig, &button, &eventTracker);

// The event handler takes the arguments sent with the event and stored them
// into the EventTracker circular buffer.
void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  eventTracker.addEvent(eventType, buttonState);
}

void setup() {
  Serial.begin(9600);
  testableConfig.setEventHandler(handleEvent);
  button.setButtonConfig(&testableConfig);
  while (!Serial); // for the Arduino Leonardo/Micro only
}

void loop() {
  Test::run();
}

// Test a long press without suppression should generate a released event at
// the end.
test(long_press_without_suppression) {
  const uint8_t DEFAULT_RELEASED_STATE = HIGH;
  const unsigned long BASE_TIME = 65500;
  uint8_t expected;

  // reset the button
  helper.init(PIN, DEFAULT_RELEASED_STATE, BUTTON_ID);
  testableConfig.setFeature(ButtonConfig::kFeatureLongPress);

  // initial button state
  helper.releaseButton(BASE_TIME + 0);
  assertEqual(0, eventTracker.getNumEvents());

  // initilization phase
  helper.releaseButton(BASE_TIME + 50);
  assertEqual(0, eventTracker.getNumEvents());

  // button pressed, but must wait to debounce
  helper.pressButton(BASE_TIME + 140);
  assertEqual(0, eventTracker.getNumEvents());

  // after 50 ms or more, we should get an event
  helper.pressButton(BASE_TIME + 190);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventPressed;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(LOW, eventTracker.getRecord(0).getButtonState());

  // keeping holding the button
  helper.pressButton(BASE_TIME + 1100);
  assertEqual(0, eventTracker.getNumEvents());

  // keeping holding the button longer than 1000 ms
  helper.pressButton(BASE_TIME + 1200);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventLongPressed;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(LOW, eventTracker.getRecord(0).getButtonState());

  // finally release the button
  helper.releaseButton(BASE_TIME + 1600);
  assertEqual(0, eventTracker.getNumEvents());

  // Must wait for debouncing for the kEventReleased.
  helper.releaseButton(BASE_TIME + 1660);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventReleased;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(HIGH, eventTracker.getRecord(0).getButtonState());
}

// Test a long press with suppression should produce no released event.
test(long_press_with_supression) {
  const uint8_t DEFAULT_RELEASED_STATE = HIGH;
  const unsigned long BASE_TIME = 65500;
  uint8_t expected;

  // reset the button
  helper.init(PIN, DEFAULT_RELEASED_STATE, BUTTON_ID);
  testableConfig.setFeature(ButtonConfig::kFeatureLongPress);
  testableConfig.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);

  // initial button state
  helper.releaseButton(BASE_TIME + 0);
  assertEqual(0, eventTracker.getNumEvents());

  // initilization phase
  helper.releaseButton(BASE_TIME + 50);
  assertEqual(0, eventTracker.getNumEvents());

  // button pressed, but must wait to debounce
  helper.pressButton(BASE_TIME + 140);
  assertEqual(0, eventTracker.getNumEvents());

  // after 50 ms or more, we should get an event
  helper.pressButton(BASE_TIME + 190);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventPressed;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(LOW, eventTracker.getRecord(0).getButtonState());

  // keeping holding the button
  helper.pressButton(BASE_TIME + 1100);
  assertEqual(0, eventTracker.getNumEvents());

  // keeping holding the button longer than 1000 ms
  helper.pressButton(BASE_TIME + 1200);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventLongPressed;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(LOW, eventTracker.getRecord(0).getButtonState());

  // finally release the button
  helper.releaseButton(BASE_TIME + 1600);
  assertEqual(0, eventTracker.getNumEvents());

  // Must wait for debouncing. We elected kFeatureSuppressAfterLongPress so
  // no kEventReleased is generated.
  helper.releaseButton(BASE_TIME + 1660);
  assertEqual(0, eventTracker.getNumEvents());
}

// Test that no LongPress generated with isFeature() flag off.
test(no_long_press_without_feature_flag) {
  const uint8_t DEFAULT_RELEASED_STATE = HIGH;
  const unsigned long BASE_TIME = 65500;
  uint8_t expected;

  // reset the button
  helper.init(PIN, DEFAULT_RELEASED_STATE, BUTTON_ID);
  testableConfig.clearFeature(ButtonConfig::kFeatureLongPress);

  // initial button state
  helper.releaseButton(BASE_TIME + 0);
  assertEqual(0, eventTracker.getNumEvents());

  // initilization phase
  helper.releaseButton(BASE_TIME + 50);
  assertEqual(0, eventTracker.getNumEvents());

  // button pressed, but must wait to debounce
  helper.pressButton(BASE_TIME + 140);
  assertEqual(0, eventTracker.getNumEvents());

  // after 50 ms or more, we should get an event
  helper.pressButton(BASE_TIME + 190);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventPressed;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(LOW, eventTracker.getRecord(0).getButtonState());

  // keeping holding the button
  helper.pressButton(BASE_TIME + 1100);
  assertEqual(0, eventTracker.getNumEvents());

  // keeping holding the button longer than 1000 ms
  helper.pressButton(BASE_TIME + 1200);
  assertEqual(0, eventTracker.getNumEvents());

  // finally release the button
  helper.releaseButton(BASE_TIME + 1600);
  assertEqual(0, eventTracker.getNumEvents());

  // Must wait for debouncing. Only a Released event should be generated.
  helper.releaseButton(BASE_TIME + 1660);
  assertEqual(1, eventTracker.getNumEvents());
  expected = AceButton::kEventReleased;
  assertEqual(expected, eventTracker.getRecord(0).getEventType());
  assertEqual(HIGH, eventTracker.getRecord(0).getButtonState());
}