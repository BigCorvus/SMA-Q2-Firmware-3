void tremor() {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(LCD_COLOR_BLACK);
  display.setTextSize(3);
  display.print("Tremor f");
  display.refresh();
  int prevX, prevX2, prevY, prevY2 = 0;
  int X = 0;
  int16_t x, y, z = 0;
  EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin
initkx023();
delay(20);
 // digitalWrite(ACCEL_PWR, HIGH); //turn on the accelerometer

  while (digitalRead(BTN_BCK)) {
    //sleep();
    wdt_feed();

    if (digitalRead(BTN_UP) == LOW) {
      delay(100);
      //ble.print("UP");

    }

    if (digitalRead(BTN_DOWN) == LOW) {
      delay(100);
      //ble.print("DOWN");
    }

    if (digitalRead(BTN_OK) == LOW) {
      delay(100);
      // ble.print("OK");
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

    //accel test


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
    
    //      display.setCursor(10, 160);
    //      display.setTextSize(1);
    //      display.setTextColor(LCD_COLOR_BLACK, LCD_COLOR_WHITE);
    //      display.print("            ");
    //      display.setCursor(10, 160);
    //      display.print(x);
    //      display.print(",");
    //      display.print(y);
    //      display.print(",");
    //      display.println(z);
    //      display.refresh();


    display.setTextColor(LCD_COLOR_BLACK);
    x = map(x, -32767, 32767, 30, 90);
    y = map(y, -32767, 32767, 91, 150);
    display.fillRect(X, 30, 6, 120, LCD_COLOR_WHITE);
    display.drawLine(X - 1, prevY, X, x, LCD_COLOR_BLACK);
    //M5.Lcd.fillRect(x, 0, 4,320,COLOR_BLACK);
    display.drawLine(X - 1, prevY2, X, y, LCD_COLOR_BLUE);
    display.refresh();
    prevY = x;
    prevY2 = y;
    X++;
    if (X >= 175) X = 0;

    delay(4);

  } //end app loop


 // digitalWrite(ACCEL_PWR, LOW); //turn on the accelerometer
accelStdby();
  displayMenue(1);
  delay(100); //debounce
}
