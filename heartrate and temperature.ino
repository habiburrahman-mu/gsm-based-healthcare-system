#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
SoftwareSerial sim(9, 10);

String number = "+880XXXXXXXXXXX";

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
String readString ;
//switch
int sw_pin1 = 6;
int sw_pin2 = 7;
int sw1_state = 0, sw1_prev_state = 0;
int sw2_state = 0, sw2_prev_state = 0;
int final_hr = 0;
int final_temp = 0;
int val;
int rst=0;
float value;

//heartbeat
unsigned int reading, wait_time = 15000, count = 0;
bool state = 0, lastState = 0, flag = 0;
unsigned long previousMillis = 0;
const long interval = 15000;
int BPM = 0;
int hr_gsm=0;

//temp
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.println("Starting.....");
  pinMode(A3, INPUT); //heartbeat
  pinMode(A1, INPUT); //temp
  Serial.println("Wait few seconds...");
  delay(5000);
  Serial.println("Sistem Started...");
  sim.begin(9600);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  int sw1_read = digitalRead(sw_pin1);
  int sw2_read = digitalRead(sw_pin2);

  //switch 1
  if (sw1_read == HIGH && sw1_prev_state == 0) {
    sw1_state = !sw1_state;
  }
  sw1_prev_state = sw1_read;
  //Serial.print(sw1_state);

  //switch 2
  if (sw2_read == HIGH && sw2_prev_state == 0) {
    sw2_state = !sw2_state;
  }
  sw2_prev_state = sw2_read;
  //Serial.println(sw2_state);
  START:
  if (sw1_state == 1 && sw2_state == 0) {
    //hearbeat
    unsigned long currentMillis = millis();
    reading = analogRead(A3);
    if ( reading >= 210 )
    {
      //Serial.println("Pulse..............................................................");
      state = 1;
      if (lastState != state)
      {
        count++;
        //Serial.println(count);
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          BPM = 4 * count;
          count = 0;
          Serial.print("\n");
          Serial.println(BPM);
          //final_hr = BPM;
          if(BPM>40){
            final_hr = BPM; 
            hr_gsm = final_hr;
          }
          sw1_state = 0;
          previousMillis = millis();
          goto START;
        }
        else {
          Serial.print("|");
          Serial.print("Pulse");
          Serial.print("|");
          Serial.print(".");
        }
      }
    }
    else {
      //Serial.println("Not Pulse");
      //delay(100);
      state = 0;
    }
    lastState = state;
  }
  else if (sw1_state == 0 && sw2_state == 1) {
    //temperature
    BPM = 0;
    previousMillis = millis();
    val = analogRead(A1);
    Serial.println(val);
    value = float(val);
    float mv = ( value / 1024.0) * 5000.00;
    float cel = mv / 10.00;
    float farh = (cel * 9) / 5.00 + 32.00;
    final_temp = int(farh);

    Serial.print("TEMPRATURE = ");
    Serial.print(cel);
    Serial.print(" *C | ");
    delay(500);
    Serial.print("TEMPRATURE = ");
    Serial.print(farh);
    Serial.print(" *F");
    Serial.println();
  }
  else if (sw1_state == 1 && sw2_state == 1) {
    Serial.println("Two Process Can't Run Simultaneously");
    sw1_state = 0;
    sw2_state = 0;
    BPM = 0;
    previousMillis = millis();
  }
  else if (sw1_state == 0 && sw2_state == 0) {
    Serial.println("Two Process Off");
    BPM = 0;
    previousMillis = millis();
  }                                                                                                                                                                                                                                                                                                                                   
  lcd.setCursor(0,0);
  lcd.print("Heart Rate:");
  
  lcd.println(final_hr);
  lcd.setCursor(0,1);
  lcd.print("Temperature:");
  lcd.println(final_temp);
  if(hr_gsm>=68){
    sim.println("AT+CMGF=1");
    delay(1000);
    sim.println("AT+CMGS=\"" + number + "\"\r");
    delay(1000);
    String SMS = "Patients heart rate in critical condition.HR:"+String(hr_gsm);
    sim.println(SMS);
    sim.println("ppm");
    delay(100);
    sim.println((char)26);
    delay(1000);
    hr_gsm = 0;
  }
}
