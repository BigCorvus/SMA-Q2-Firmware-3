void remote() {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(LCD_COLOR_BLACK);
  display.setTextSize(3);
  display.print("Remote");
  display.refresh();

initkx023();
delay(20);

  //digitalWrite(ACCEL_PWR, HIGH); //turn on the accelerometer

 
  EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin
  accelSamplingTimer.startTimer(100, sendXYZ); //sample accel and send data

  while (digitalRead(BTN_BCK)) {
    sleep();

    wdt_feed();

    if (digitalRead(BTN_UP) == LOW) {
      
      delay(100);
      ble.print("UP");
      
    }

    if (digitalRead(BTN_DOWN) == LOW) {
      
      delay(100);
      ble.print("DOWN");
      
    }

    if (digitalRead(BTN_OK) == LOW) {
      
      delay(100);
      ble.print("OK");
      
    }
    
 if (bleConnected) {
  display.setCursor(5, 40);
  display.setTextColor(LCD_COLOR_BLACK,LCD_COLOR_WHITE);
  display.setTextSize(1);
  display.print("streaming accel data");
  display.refresh();
 }else{
  display.setCursor(5, 40);
  display.setTextColor(LCD_COLOR_WHITE,LCD_COLOR_WHITE);
  display.setTextSize(1);
  display.print("streaming accel data");
  display.refresh();
 }

    while (ble.available() ) {
      int i = ble.read();
      bleInString[bleInIndx] = i;
      bleInIndx++;
    }
    if ( bleInIndx > 0) {

      display.setCursor(10, 140);
      display.setTextColor(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
      for (int i = 0; i < bleInIndx + 1; i++) display.print(" ");
      display.setCursor(10, 140);
      display.setTextSize(1);
      display.print(bleInString);
      display.refresh();
      bleInIndx = 0;
    }


  } //end menue loop

  accelSamplingTimer.stop(); //stop sampling and sending
 // digitalWrite(ACCEL_PWR, LOW); //turn off the accelerometer
 accelStdby();
  displayMenue(0);
  delay(100); //debounce
}

void sendXYZ() {
  int16_t x, y, z = 0;

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

  x = (int16_t)((res[1] << 8) | res[0]) ;
  y = (int16_t)((res[3] << 8) | res[2]) ;
  z = (int16_t)((res[5] << 8) | res[4]) ;
  if (bleConnected) {
    ble.print(x);
    ble.print(",");
    ble.print(y);
    ble.print(",");
    ble.print(z);
    ble.print(";");
  }
}
