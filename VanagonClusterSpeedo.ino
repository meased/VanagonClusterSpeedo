#include <FreqCount.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <DailyStruggleButton.h>
#include "big_digit.h"

#define EEPROM_ODO_ADDR         0
#define EEPROM_TRIP_A_ADDR      4
#define EEPROM_TRIP_B_ADDR      8
#define EEPROM_TRIP_OIL_ADDR    12
#define EEPROM_PULSES_ADDR      16
#define EEPROM_TRIP_INDEX_ADDR  20

// Pins
#define RS                      6
#define EN                      7
#define D4                      13
#define D5                      12
#define D6                      11
#define D7                      10

#define PIN_RESET               3
#define PIN_VSS                 5  // The FreqCount pin is always pin 5. This
                                   // is just for documentation.

#define UPDATE_RATE             1000  // ms
#define OIL_CHANGE_INTERVAL     50000  // 1/10 miles

const unsigned long pulses_per_mile = 9839;
const unsigned long pulses_per_tenth_mile = pulses_per_mile / 10;
const float pulses_to_mph = (1000.0/UPDATE_RATE * 3600.0) / pulses_per_mile;
const float pulses_to_mile = 1.0 / pulses_per_mile;

double sum = 0;
unsigned long pulses = 0;
unsigned long pulses_acc = 0;
unsigned long odo = 0;
unsigned long odo_new = 0;

// Everything is stored in 1/10th miles.
unsigned long trip[3] = {0, 0, 0};
byte trip_index;
bool speed_armed = false;
float mph = 0;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
DailyStruggleButton reset_button;

enum Trip {
    TRIP_A,
    TRIP_B,
    TRIP_OIL
};

void on_reset_button(uint8_t btnStatus){
    switch (btnStatus){
    case onRelease:
        trip_index = (trip_index + 1) % (sizeof(trip) / sizeof(trip[0]));
        EEPROM.put(EEPROM_TRIP_INDEX_ADDR, trip_index);
        draw();
        break;
    case onLongPress:
        // Flash the trip meter for user feedback.
        lcd.setCursor(10, 3);
        lcd.print("          ");
        delay(250);

        switch (trip_index) {
        case TRIP_A:
            trip[TRIP_A] = 0;
            break;
        case TRIP_B:
            trip[TRIP_B] = 0;
            break;
        case TRIP_OIL:
            trip[TRIP_OIL] = 0;
            break;
        }

        EEPROM_store();
        draw();
        break;
    }
}

///
// sprintf doesn't support longs on Arduino so we roll our own.
//
// buf must have space for at least 7 bytes.
//
void build_odo_string(unsigned long val, char *buf) {
    // Stored in 1/10th miles.
    val /= 10;

    int i = 5;
    while (i >= 0) {
        buf[i--] = '0' + val % 10;
        val /= 10;
        if (!val)
            break;
    }

    while (i >= 0)
        buf[i--] = ' ';

    buf[6] = '\0';
}

///
// Same as build_odo_string, but also draws 0.1 digit.
//
void build_trip_string(unsigned long val, char *buf) {
    int i = 5;

    while (i >= 0) {
        buf[i--] = '0' + val % 10;
        val /= 10;

        if (i == 4)
            buf[i--] = '.';
        else if (!val)
            break;
    }

    while (i >= 0)
        buf[i--] = ' ';

    buf[6] = '\0';
}

void draw(void) {
    char buf[21];

    int mphi = (int) round(mph < 1.0 ? 0.0 : mph);
    if (mphi > 99)
        mphi = 99;

    // Hide high digit when under 10 MPH.
    if (mphi >= 10)
        big_digit(5, 0, 0, mphi / 10);
    else
        big_space(5, 0, 0);
    big_digit(5, 0, 1, mphi % 10);

    lcd.setCursor(15, 1);
    lcd.print("MPH");

    lcd.setCursor(0, 0);
    lcd.write((byte) (trip[TRIP_OIL] > OIL_CHANGE_INTERVAL) ? OIL : ' ');

    build_odo_string(odo + odo_new, buf);
    lcd.setCursor(0, 3);
    lcd.print(buf);
    lcd.print(" mi");

    lcd.setCursor(19, 3);
    switch (trip_index) {
    case TRIP_A:
        lcd.print("A");
        break;
    case TRIP_B:
        lcd.print("B");
        break;
    case TRIP_OIL:
        lcd.write((byte) OIL);
        break;
    }

    build_trip_string(trip[trip_index], buf);
    lcd.setCursor(12, 3);
    lcd.print(buf);
}


void EEPROM_load() {
    EEPROM.get(EEPROM_ODO_ADDR, odo);
    EEPROM.get(EEPROM_TRIP_A_ADDR, trip[TRIP_A]);
    EEPROM.get(EEPROM_TRIP_B_ADDR, trip[TRIP_B]);
    EEPROM.get(EEPROM_TRIP_OIL_ADDR, trip[TRIP_OIL]);
    EEPROM.get(EEPROM_PULSES_ADDR, pulses_acc);
    EEPROM.get(EEPROM_TRIP_INDEX_ADDR, trip_index);

    // Don't load huge values (probably the first run on a new chip).
    if (pulses_acc > pulses_per_tenth_mile) {
        pulses_acc = 0;
        odo = 0;
        trip[TRIP_A] = 0;
        trip[TRIP_B] = 0;
        trip[TRIP_OIL] = 0;
        trip_index = TRIP_A;
    }
}


void EEPROM_store() {
    odo += odo_new;
    odo_new = 0;

    // !!!DO NOT POWER ARDUINO FROM BOTH USB AND VEHICLE!!!
    // Leave the vehicle power off.

    // To program mileage:
    //  1. Uncomment the following lines.
    //  2. Edit to the desired milage.
    //  3. Upload to unit.
    //  4. Hold reset button.
    //  5. Comment out the following lines.
    //  6. Upload to unit.

    //odo = 10 * 284536;
    //trip[TRIP_A] = 10 * 5.2;
    //trip[TRIP_B] = 10 * 0;
    //trip[TRIP_OIL] = 10 * 0;
    //pulses_acc = 0;
    //trip_index = TRIP_A;

    EEPROM.put(EEPROM_ODO_ADDR, odo);
    EEPROM.put(EEPROM_TRIP_A_ADDR, trip[TRIP_A]);
    EEPROM.put(EEPROM_TRIP_B_ADDR, trip[TRIP_B]);
    EEPROM.put(EEPROM_TRIP_OIL_ADDR, trip[TRIP_OIL]);
    EEPROM.put(EEPROM_TRIP_INDEX_ADDR, trip_index);

    // odo and trip units are 1/10 mile. We also store the pulse accumulator
    // to EEPROM so we don't lose partial 1/10ths on every save.
    EEPROM.put(EEPROM_PULSES_ADDR, pulses_acc);
}


void setup() {
    //Serial.begin(57600);
    delay(200);

    pinMode(PIN_VSS, INPUT);

    EEPROM_load();
    reset_button.set(PIN_RESET, on_reset_button, EXT_PULL_UP);
    reset_button.enableLongPress(1000);

    lcd.begin(20, 4);
    lcd.clear();
    big_digit_init();

    FreqCount.begin(UPDATE_RATE);
}

void loop() {
    reset_button.poll();

    if (FreqCount.available()) {
        pulses = FreqCount.read();
        mph = pulses * pulses_to_mph;

        // To save EEPROM writes, only save when we are going faster than
        // 15 MPH and then slow down to under 5 MPH.
        if (mph > 15 and !speed_armed)
            speed_armed = true;
        else if (mph < 5 and speed_armed) {
            speed_armed = false;
            EEPROM_store();
        }

        pulses_acc += pulses;
        if (pulses_acc > pulses_per_tenth_mile) {
            pulses_acc -= pulses_per_tenth_mile;
            ++odo_new;
            ++trip[TRIP_A];
            ++trip[TRIP_B];
            ++trip[TRIP_OIL];
        }

        draw();
    }
}
