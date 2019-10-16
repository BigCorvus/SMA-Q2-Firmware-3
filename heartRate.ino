void heartRate() {
   digitalWrite(HRM_PWR, HIGH);
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(LCD_COLOR_BLACK);
  display.setTextSize(3);
  display.print("HR calc");
  display.refresh();


 // EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin

//pah8002_init();
//bool success = pah8002_start();

  while (digitalRead(BTN_BCK)) {
    //sleep();
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
      //ble.print("OK");
    }
    wdt_feed();

    

  } //end menue loop



  displayMenue(0);
  delay(100); //debounce
   digitalWrite(HRM_PWR, LOW);
}
