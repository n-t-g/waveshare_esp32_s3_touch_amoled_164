#include <Arduino.h>
#include <unity.h>

void setUp() {
    // run before each test
}

void tearDown() {
    // run after each test
}

void test_serial_is_ready() {
    // This test assumes LED and serial interface are available on the target board.
    // It checks that baud can be initialized without hard failure.
    Serial.begin(115200);
    delay(100);
    TEST_ASSERT_TRUE(Serial);
    Serial.println("unit-test: serial is enabled");
}

void test_dummy_counting() {
    TEST_ASSERT_EQUAL(1, 1);
}

void setup() {
    delay(200);
    UNITY_BEGIN();
    RUN_TEST(test_serial_is_ready);
    RUN_TEST(test_dummy_counting);
    UNITY_END();
}

void loop() {
    // nothing to do after tests
}
