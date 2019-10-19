/*
  3rd version for a SMA-Q2 Arduino firmware based on the sandeepmistry nrf5 core and the pfod_lp_nrf52.zip low power core mods
  see https://www.forward.com.au/pfod/BLE/LowPower/index.html for more info

  Implements basic watch functionality with over 1 week always on and advertising standby time.
  Basic menue structure with "Apps" is implemented. Since the pfod low power core does not implement interrupts, we have to poll the buttons. The lp_comparator does not work for me.
  Press the DFU bootloader combo inside the watch screen.
  In order to set date and time send a string via the Nordic BLE Uart App (available in the App Stores) in the format yyyymmddhhmmss while you're in the main menu.

  Setup:
  1. install the sandeepmistry nrf5 core
  2. replace the hardware folder inside C:\Users\yourComputer\AppData\Local\Arduino15\packages\sandeepmistry with the provided one
  3. put the entire sketch folder into your sktch directory

  The following tricks accelerate your development cycles:
  press ctrl + alt + 'S' to compile and export hex
  C:\Users\Kauzdesktop\Documents\Arduino\SMAQ2_FW3\nrfutil dfu genpkg --application C:\Users\Kauzdesktop\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.ino.SMA_Q2.hex --sd-req 0x88 C:\Users\Kauzdesktop\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.zip
  btobex -n "Galaxy S5" "C:\Users\Kauzdesktop\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.zip

  A. Jordan 2019
*/
#include <lp_BLESerial.h>
#include <lp_comparator.h>
#include <SPI.h>
#include <stdint.h>
#include "Adafruit_GFX.h"
#include "ColorMemLCD.h"
//#include "RTCInt.h"
#include "TimeLib.h"
#include <nrf.h>
#include <nrf_nvic.h>//interrupt controller stuff
#include <nrf_sdm.h>
#include <nrf_soc.h>
#include <WInterrupts.h>
#include "i2csoft.h"
//#include <PAH8002HRmon.h>  //the pre-compiled library for the HR sensor PAH8002
//#include "PAH8002.h" //the driver from the manufacturer's application note
//#include <Wire.h>

//  SMA-Q2 pins
#define SCK 2
#define MOSI 3
#define SS 5
#define EXTCOMIN 6
#define DISP 7

#define VBAT 4 //with a 1/4 divider

#define VIBRO 30
#define BCKLT 8

#define BTN_BCK 9
#define BTN_UP 27
#define BTN_OK 28
#define BTN_DOWN 29

#define FLASH_MISO 11
#define FLASH_MOSI 13
#define FLASH_CS 12
#define FLASH_CLK 14

#define ACCEL_PWR 16
#define ACCEL_INT 15
#define ACCEL_SDA 17
#define ACCEL_SCL 18

#define HRM_SDA 19
#define HRM_SCL 20
#define HRM_INT 22
#define HRM_RST 10
#define HRM_PWR 31

#define CHG_STAT 23
#define CHG_PGOOD 24

#define UART_RXD 25
#define UART_TXD 26


bool bleConnected, charging, lastConnStat= false;
bool bltOn = false;
bool extcominState = false;

lp_timer accelSamplingTimer;
lp_timer clockTimer;
lp_timer EXCOMINtimer;
lp_timer debounceTimerOK, debounceTimerDOWN;
//RTCInt rtc;  //create an RTCInt type object
lp_BLESerial ble;
ColorMemLCD display(SCK, MOSI, SS, EXTCOMIN);
//PAH8002HRmon HRsensor; //create instance of precompiled HR sensor lib

uint32_t debounceTimeOut = 20; // mS
const unsigned long DELAY_TIME = 1000; // mS == 1sec
int lastButtonStateBCK, lastButtonStateOK, lastButtonStateUP, lastButtonStateDOWN = -1; // not set initially
int buttonStateBCK, buttonStateOK, buttonStateUP, buttonStateDOWN = -1; // not set initially
char bleInString[100];
int  bleInIndx  = 0;


long buttonTimer, buttonTimerOK = 0;
const int longPressTime = 3000;
boolean buttonActive, buttonActiveOK = false;
boolean longPressActive, longPressActiveOK = false;

bool insideMenue = false;

// the setup routine runs once when you press reset:
void setup() {
  pinMode(BCKLT, OUTPUT);
  pinMode(VIBRO, OUTPUT);
  pinMode(ACCEL_PWR, OUTPUT);
  pinMode(HRM_PWR, OUTPUT);
  pinMode(CHG_STAT, INPUT);

  pinMode(BTN_BCK, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(DISP, OUTPUT);
  //attachInterrupt(BTN_UP, btn_up_isr, FALLING);
  digitalWrite(DISP, HIGH);
  digitalWrite(BCKLT, LOW);
  digitalWrite(VIBRO, LOW);
  digitalWrite(ACCEL_PWR, HIGH); //turn on the accelerometer
  digitalWrite(HRM_PWR, LOW);

  display.begin();

  delay(100);
  display.clearDisplay();
  // display.setFont(&FreeSerifItalic9pt7b);
  display.refresh();
  display.setTextSize(3);
  display.setTextColor(LCD_COLOR_RED);
  display.setCursor(20, 85);
  display.println("FW 3.0");
  display.refresh();
  delay(100);
  //Wire.begin();
 initi2c();
  delay(10);
  initkx023();
  delay(10);
  // initialize the digital pin as an output.


  ble.setName("SMA-Q2custom"); // set advertised name, default name is "Nordic BLE UART"
  ble.setConnectedHandler(handleConnection); // when a connection is made
  ble.setDisconnectedHandler(handleDisconnection); // when the connection is dropped or goes out of range
  ble.begin(); // start advertising and be ready to accept connections

  ble.setTxPower(-8);


  // comparator pins for nRF52832
  // INPUT0 P0_02
  // INPUT1 P0_03
  // INPUT2 P0_04
  // INPUT3 P0_05
  // INPUT4 P0_28  OK
  // INPUT5 P0_29  DOWN
  // INPUT6 P0_30
  // INPUT7 P0_31

  //lp comparator stuff does not work
  lp_comparator_start(BTN_OK, REF_8_16Vdd, handlePinLevelChangeOK); // always triggers pin LOW first, then if pin HIGH, will trigger HIGH
  lp_comparator_start(BTN_DOWN, REF_8_16Vdd, handlePinLevelChangeDOWN); //EIGENTLICH btn_dOWN1111
  //attachInterrupt(BTN_BCK, handlePinLevelChangeBCK, FALLING);


  clockTimer.startTimer(60000, displayTime); //this is the timer for the watch face refresh
  EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin



}

void loop() {
  wdt_feed(); //keep this in your main loop to reset automatically after 10s if something goes wrong within loop()
  sleep(); // just sleep here waiting for the timer to trigger

  readBckBtn(); //keep this in your main loop
  readOkBtn();
  readDFUcombo();

  if (digitalRead(CHG_STAT) == LOW) {
    charging = true;
    
  } else charging = false;

} //LOOP




//implements long press for power off and short press for backlight toggle
void readBckBtn() {

  if (digitalRead(BTN_BCK) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      //button has been pressed for a long time
      //set any pin and wakeup logic here.
      //  systemOff(BTN_UP, LOW); //implemented here:
      //      digitalWrite(VIBRO, HIGH);
      //      delay(60);
      //      digitalWrite(VIBRO, LOW);

      digitalWrite(DISP, LOW);
      digitalWrite(BCKLT, LOW);
      digitalWrite(VIBRO, LOW);
      digitalWrite(ACCEL_PWR, LOW); //turn on the accelerometer
      digitalWrite(HRM_PWR, LOW);
      delay(2000);
      NRF_SAADC ->ENABLE = 0; //disable ADC
      NRF_PWM0  ->ENABLE = 0; //disable all pwm instance
      NRF_PWM1  ->ENABLE = 0;
      NRF_PWM2  ->ENABLE = 0;
      //nrf_gpiote_event_clear(NRF_GPIOTE_EVENTS_PORT);
      //      nrf_drv_gpiote_in_uninit();
      powerOff(); //here the watch draws about as much as in "on" mode - not quite useful except for reset purposes!!
      while (1);
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        //button has been pressed for a short time
        toggleBLT();



      }
      buttonActive = false;
    }
  }

}


//implements long press for power off and short press for backlight toggle
void readOkBtn() {

  if (digitalRead(BTN_OK) == LOW) {
    if (buttonActiveOK == false) {
      buttonActiveOK = true;
      buttonTimerOK = millis();
    }
    if ((millis() - buttonTimerOK > longPressTime) && (longPressActiveOK == false)) {
      longPressActiveOK = true;
      //button has been pressed for a long time
      //set any pin and wakeup logic here.
      //  systemOff(BTN_UP, LOW); //implemented here:
      digitalWrite(VIBRO, HIGH);
      delay(60);
      digitalWrite(VIBRO, LOW);
    }
  } else {
    if (buttonActiveOK == true) {
      if (longPressActiveOK == true) {
        longPressActiveOK = false;
      } else {
        //button has been pressed for a short time
        menue();

      }
      buttonActiveOK = false;
    }
  }

}



void displayTime() {
  if (!insideMenue) {
    display.clearDisplay();
    display.fillRect(0, 40, 176, 176, LCD_COLOR_YELLOW);
    display.setTextSize(5);
    display.setTextColor(LCD_COLOR_BLUE);
    //display.setFont(&DSEG7_dig_only);
    display.setCursor(15, 70);
    if (hour() < 10) display.print("0");
    display.print(hour());
    display.setTextColor(LCD_COLOR_GREEN);
    display.print(":");
    //display.setCursor(4, 50);
    //display.setCursor(10, 45);

    display.setTextColor(LCD_COLOR_MAGENTA);
    if (minute() < 10) display.print("0");
    display.print(minute());
    //display.print(":");
    //display.setFont(NULL);
    //display.setCursor(4, 70);
    //  if (second() < 10) display.print("0");
    //  display.print(second());
    //  display.print(" ");

    display.setCursor(40, 150);
    display.setTextSize(2);
    display.setTextColor(LCD_COLOR_RED);
    if (day() < 10) display.print("0");
    display.print(day());
    display.print(".");
    if (month() < 10) display.print("0");
    display.print(month());
    display.print(".");
    display.println(year());
    display.setTextColor(LCD_COLOR_BLACK);
    display.setTextSize(1);
    display.setCursor(0, 11);
    display.print(getBatteryLevel());
    display.println("%");
    display.setCursor(168, 11);
    if (bleConnected) display.print("B"); else display.print(" ");
    display.setCursor(65, 11);
    if (charging) display.print("charging"); else display.print("        ");
    display.refresh();
  }
}




//button handling stuff
// called when pin changes state, pinState is state detected, HIGH or LOW
void handlePinLevelChangeOK(int pinStateOK) {

  if (pinStateOK != lastButtonStateOK) {
    lastButtonStateOK = pinStateOK;
    debounceTimerOK.stop(); // stop last timer if any
    debounceTimerOK.startDelay(debounceTimeOut, handleDebounceTimeoutOK);
  }
}


void handlePinLevelChangeDOWN(int pinStateDOWN) {

  if (pinStateDOWN != lastButtonStateDOWN) {
    lastButtonStateDOWN = pinStateDOWN;
    debounceTimerDOWN.stop(); // stop last timer if any
    debounceTimerDOWN.startDelay(debounceTimeOut, handleDebounceTimeoutDOWN);
  }
}

void handleDebounceTimeoutOK() {
  buttonStateOK = lastButtonStateOK;   // input has settled
  // ble.print("maxQ:"); ble.print(app_sched_queue_utilization_get()); //needs #define SCHEDULER_PROFILER in utility/app_schedule.h
  //ble.print(' '); ble.print((buttonState == HIGH) ? 'H' : 'L');
  //digitalWrite(led, buttonState);   // turn the LED on when input HIGH
  //menue();
  //toggleBLT();
}

void handleDebounceTimeoutDOWN() {
  buttonStateDOWN = lastButtonStateDOWN;   // input has settled
  // ble.print("maxQ:"); ble.print(app_sched_queue_utilization_get()); //needs #define SCHEDULER_PROFILER in utility/app_schedule.h
  //ble.print(' '); ble.print((buttonState == HIGH) ? 'H' : 'L');
  //digitalWrite(led, buttonState);   // turn the LED on when input HIGH

  toggleBLT();
}
