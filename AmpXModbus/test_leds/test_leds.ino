
//Define the Status indicating LEDs pins, Commented for 
#define LED_1_POWER 12
#define LED_2_METER 14
#define LED_3_WIFI 27
#define LED_4_INTERNET 26


void setup() {
 // initialize LED status pins as outputs.
  pinMode(LED_1_POWER, OUTPUT);
  pinMode(LED_2_METER, OUTPUT);
  pinMode(LED_3_WIFI, OUTPUT);
  pinMode(LED_4_INTERNET, OUTPUT);

    //Indicate that the power is on with a LED
  digitalWrite(LED_1_POWER, HIGH);
  digitalWrite(LED_2_METER, HIGH);
  digitalWrite(LED_3_WIFI, HIGH);
  digitalWrite(LED_4_INTERNET, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

}
