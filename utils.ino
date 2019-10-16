//housekeeing stuff

// reload the watchdog timer that has been started by the bootloader and set for 10 seconds
void wdt_feed() {
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
}

void readDFUcombo(){
	  if (digitalRead(BTN_UP) == LOW && digitalRead(BTN_DOWN) == LOW && digitalRead(BTN_BCK) == LOW) {
    //press the OTA DFU button combo so the device can reset and boot into OTA DFU mode
    NRF_POWER->GPREGRET = 0x01;
    sd_nvic_SystemReset();
  }
}

//This needs to be run at least once per second
void disp_toggle() {
  extcominState = !extcominState;
  digitalWrite(EXTCOMIN, extcominState);
}

void toggleBLT() {
  bltOn = !bltOn; // toggle state
  if (bltOn) {
    digitalWrite(BCKLT, HIGH);   // turn the LED on (HIGH is the voltage level)
  } else {
    digitalWrite(BCKLT, LOW);    // turn the LED off by making the voltage LOW
  }
}

int getBatteryLevel() {
  //lp_comparator_stop();
  return map(analogRead(VBAT), 306, 355, 0, 100); //adapted for SMA-Q2 306 at 3,6v and 350 at 4,1v
  //lp_comparator_start(BTN_OK, REF_8_16Vdd, handlePinLevelChangeOK); // always triggers pin LOW first, then if pin HIGH, will trigger HIGH
  //lp_comparator_start(BTN_DOWN, REF_8_16Vdd, handlePinLevelChangeDOWN);
}

void handleConnection(BLECentral& central) {
 bleConnected=true;
 if(!insideMenue) displayTime();
}

void handleDisconnection(BLECentral& central) {
bleConnected=false;
if(!insideMenue) displayTime();
}


String GetDateTimeString() {
  String datetime = String(year());
  if (month() < 10) datetime += "0";
  datetime += String(month());
  if (day() < 10) datetime += "0";
  datetime += String(day());
  if (hour() < 10) datetime += "0";
  datetime += String(hour());
  if (minute() < 10) datetime += "0";
  datetime += String(minute());
  return datetime;
}

void SetDateTimeString(String datetime) {
  int year = datetime.substring(0, 4).toInt();
  int month = datetime.substring(4, 6).toInt();
  int day = datetime.substring(6, 8).toInt();
  int hr = datetime.substring(8, 10).toInt();
  int min = datetime.substring(10, 12).toInt();
  int sec = datetime.substring(12, 14).toInt();
  setTime( hr, min, sec, day, month, year);
}

void readAccel(){
  uint8_t res[6];
  softbeginTransmission(0x1F);
  softwrite(0x06);
  softendTransmission();
  softrequestFrom(0x1F , 6);
  res[0] = softread();
  res[1] = softread();
  res[2] = softread();
  res[3] = softread();
  res[4] = softread();
  res[5] = softread();
  byte x = (int16_t)((res[1] << 8) | res[0]) / 128;
  byte y = (int16_t)((res[3] << 8) | res[2]) / 128;
  byte z = (int16_t)((res[5] << 8) | res[4]) / 128;
}

void powerOff() {
 //define wake up logic
  NRF_GPIO->PIN_CNF[BTN_BCK] &= ~((uint32_t)GPIO_PIN_CNF_SENSE_Msk);
  NRF_GPIO->PIN_CNF[BTN_BCK] |= ((uint32_t)GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos);
  
  if (checkForSoftDevice() == 1) {
    // SoftDevice enabled
    sd_power_system_off();
  } else {
    // No SoftDevice
    NRF_POWER->SYSTEMOFF = 1;
  }
}

uint8_t checkForSoftDevice() {
  uint8_t check;
  sd_softdevice_is_enabled(&check);

  return check;
}
