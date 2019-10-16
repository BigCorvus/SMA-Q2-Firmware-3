void menue() {
  insideMenue = true;
  byte menueIndex=0;
  byte maxMenueIndex=3;
  displayMenue(0);

  EXCOMINtimer.startTimer(500, disp_toggle); //this toggles the EXTCOMIN pin
  
  while (digitalRead(BTN_BCK)) { 

    sleep();
    if(digitalRead(BTN_DOWN)==LOW){
      delay(30);
      if(menueIndex>=maxMenueIndex) menueIndex = maxMenueIndex; else menueIndex++;
      displayMenue(menueIndex);
    }

    if(digitalRead(BTN_UP)==LOW){
      delay(30);
     if(menueIndex<=0) menueIndex = 0; else menueIndex--;
      displayMenue(menueIndex);
    }

    if(digitalRead(BTN_OK)==LOW){
      delay(30);
     switch(menueIndex){
      case 0:
      remote();
      break;

      case 1:
      tremor();
      break;
      
      case 2:
     
      break;
      
      case 3:
       heartRate();
      break;
      
     }
    }
    
    wdt_feed();

    while (ble.available() ) {
      int i = ble.read();
      bleInString[bleInIndx] = i;
      bleInIndx++;
    }
    if ( bleInIndx > 0) {

      SetDateTimeString(bleInString);
      clockTimer.stop();
      clockTimer.startTimer(60000, displayTime); //this is the timer for the watch face refresh
      display.setCursor(10, 140);
      display.setTextColor(LCD_COLOR_BLACK);
      display.setTextSize(1);
      display.println("date and time set to:");
      display.print(bleInString);

     

      bleInIndx = 0;
    }
  
  } //end menue loop
insideMenue = false;
  displayTime();
  
  delay(100); //debounce
}


void displayMenue(int itemIndex) {
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(50, 0);
  display.setTextColor(LCD_COLOR_BLACK);
  display.println("Menu");
  
  switch(itemIndex){
  case 0:
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_GREEN); 
  display.println("Remote");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Tremor");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Timer");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("HR");
  break;
    case 1:
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE); 
  display.println("Remote");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_GREEN);
  display.println("Tremor");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Timer");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("HR");
  break;
    case 2:
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE); 
  display.println("Remote");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Tremor");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_GREEN);
  display.println("Timer");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("HR");
  break;
    case 3:
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE); 
  display.println("Remote");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Tremor");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_WHITE);
  display.println("Timer");
  display.setTextColor(LCD_COLOR_BLUE, LCD_COLOR_GREEN);
  display.println("HR");
  break;
}
  display.refresh();
}
