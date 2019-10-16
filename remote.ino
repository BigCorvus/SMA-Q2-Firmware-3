void remote() {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(LCD_COLOR_BLACK);
  display.setTextSize(3);
  display.print("Remote");
  display.refresh();


  EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin

  while (digitalRead(BTN_BCK)) {
    sleep();
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

    wdt_feed();

    while (ble.available() ) {
      int i = ble.read();
      bleInString[bleInIndx] = i;
      bleInIndx++;
    }
    if ( bleInIndx > 0) {

      display.setCursor(10, 140);
      display.setTextColor(LCD_COLOR_BLACK,LCD_COLOR_WHITE);
      for(int i=0; i<bleInIndx+1; i++) display.print(" ");
   display.setCursor(10, 140);
      display.setTextSize(1);
      display.print(bleInString);
      display.refresh();
      bleInIndx = 0;
    }

  } //end menue loop



  displayMenue(0);
  delay(100); //debounce
}
