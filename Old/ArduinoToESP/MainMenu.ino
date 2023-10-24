void runMenu() {
  unsigned long currentTime = millis();

  encoder.tick();
  signed char pos = encoder.getPosition();
  static signed char lastPos = pos;

  int buttonState = digitalRead(ENCODER_BUTTON);
  if (buttonState == LOW) {
    menuSelection = (menuSelection + 1) % 4;
    delay(200);
  }

  if (menuSelection == 0 && pos != lastPos) {
    xStartPosition += (pos > lastPos) ? 1 : -1;
    xStartPosition = max(xStartPosition, 0);
    lastPos = pos;
  }

  if (menuSelection == 1 && pos != lastPos) {
    yStartPosition += (pos > lastPos) ? 1 : -1;
    yStartPosition = max(yStartPosition, 0);
    lastPos = pos;
  }

  if (menuSelection == 2 && pos != lastPos) {
    direction = (direction == 'X') ? 'Y' : 'X';
    lastPos = pos;
  }

  if (menuSelection == 3 && pos != lastPos) {
    isStarted = true;
    lastPos = pos;
  }

  if (currentTime - lastLCDChange >= LCDRefreshRate) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("INIT " + String(menuSelection == 0 ? ">" : " ") + "X: " + String(xStartPosition) + " " + String(menuSelection == 1 ? ">" : " ") + "Y:" + String(yStartPosition));
    lcd.setCursor(0, 1);
    lcd.print(String(menuSelection == 2 ? ">DIR:" : " DIR:") + direction + "  " + String(menuSelection == 3 ? ">START" : " START"));
    lastLCDChange = currentTime;
  }
}