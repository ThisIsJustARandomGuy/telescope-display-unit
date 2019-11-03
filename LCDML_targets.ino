// Targets (ra*1000 / dec*1000)
const long targets[][2] = {
  { 285512, 27959 }, // Albireo
  { 279235, 38785 }, // Vega
  { 213908, 19170 }, // Arktur
  {  37962, 89264 }, // Polaris
  {      0,     0 }, // Zero
};

const char* targetNames[] = {
  "Albireo",
  "Vega",
  "Arktur",
  "Polaris",
  "Zero"  
};

// *********************************************************************
void mTargets_select_target(uint8_t param)
// *********************************************************************
{
  if(LCDML.FUNC_setup())          // ****** SETUP *********
  {
    // setup is called when the menu entry (e.g. target) is clicked
    char buf[20];
    sprintf (buf, "target: %s", targetNames[param]);

    u8g2.setFont(_LCDML_DISP_font);
    u8g2.firstPage();
    do {
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 1), buf);
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 2), "long press: ok");
      u8g2.drawStr( 0, (_LCDML_DISP_font_h * 3), "short press: back");
    } while( u8g2.nextPage() );

    // starts a trigger event for the loop function every 100 milliseconds
    LCDML.FUNC_setLoopInterval(100);
  }

  if(LCDML.FUNC_loop())           // ****** LOOP *********
  {
    // loop
    // is called when it is triggered
    // - with LCDML_DISP_triggerMenu( milliseconds )
    // - with every button status change

    // check for long press
    if(LCDML.BT_checkAny()) {
      g_angleAzimuth = targets[param][0];
      g_angleAltitude = targets[param][1];

      char sbuf[13];
      
      sprintf(sbuf, "%06d", abs(g_angleAzimuth));
      Serial.print("az");
      Serial.print(g_angleAzimuth > 0 ? "+" : "-");
      Serial.println(sbuf);
      #ifdef BOARD_MEGA
        Serial2.print("az");
        Serial2.print(g_angleAzimuth > 0 ? "+" : "-");
        Serial2.println(sbuf);
      #endif
      
      sprintf(sbuf, "%06d", abs(g_angleAltitude));
      Serial.print("al");
      Serial.print(g_angleAltitude > 0 ? "+" : "-");
      Serial.println(sbuf);
      #ifdef BOARD_MEGA
        Serial2.print("al");
        Serial2.print(g_angleAltitude > 0 ? "+" : "-");
        Serial2.println(sbuf);
      #endif
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // loop end
    // you can here reset some global vars or delete it
  }
}
