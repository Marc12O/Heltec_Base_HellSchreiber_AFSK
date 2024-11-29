// Turns the 'PRG' button into the power button, long press is off
#define HELTEC_POWER_BUTTON  // must be before "#include <heltec_unofficial.h>"

// creates 'radio', 'display' and 'button' instances
#include <heltec_unofficial.h>

// create AFSK client instance using the FSK module
// this requires connection to the module direct
// input pin, here connected to Arduino pin 5
// SX127x/RFM9x:  DIO2
// RF69:          DIO2
// SX1231:        DIO2
// CC1101:        GDO2
// Si443x/RFM2x:  GPIO
// SX126x/LLCC68: DIO2
AFSKClient audio(&radio, 13);

// create Hellschreiber client instance using the AFSK instance
HellClient hell(&audio);

#define FREQUENCY 869.525

void setup() {
  heltec_setup();

  both.print("SX12xx Init ... ");
  int state = radio.beginFSK(FREQUENCY);  // 869.525
  if (state == RADIOLIB_ERR_NONE) {
    both.println(F("FSK ok!"));
  } else {
    both.print("failed, code ");
    both.println(state);
    while (true) { delay(10); }
  }

  // initialize HellSchreiber client
  both.print("Hell Init ... ");
  // tone frequency:              900 Hz
  // speed:                       25 words per minute = 122.5 baud
  state = hell.begin(900, 122.5);
  if (state == RADIOLIB_ERR_NONE) {
    both.println(F("Hell ok!"));
  } else {
    both.print(F("Failed, code "));
    both.println(state);
    while (true) { delay(10); }
  }

  // Battery
  float vbat = heltec_vbat();
  both.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));
}

int tijd = 0;

void loop() {
  heltec_loop();

  both.println("Sending Hell AFSK data ... ");

  int i, len;

  char hellstring[] = "PA9MG TEST A QUICK BROWN FOX JUMPS OVER THE LAZY DOG K";

  len = strlen(hellstring);
  for (i = 0; i < len; i++) {
    hell.write(hellstring[i]);
    heltec_led(50);
    delay(50);
    heltec_led(0);
  }
  both.println("HellSchreiber AFSK done!");

  tijd = tijd + 60000;
  both.print("Seconds pause: ");
  both.println(tijd / 1000);

  // carrier did not stop!
  int state = radio.beginFSK(FREQUENCY);
  if (state == RADIOLIB_ERR_NONE) {
    both.println("Carrier reset");
  } else {
    both.printf("Fail, code: %i\n", state);
  }

  // wait for few seconds before transmitting again
  delay(tijd);
}