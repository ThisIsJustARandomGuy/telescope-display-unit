// Targets (ra*1000 / dec*1000)
const long targets[][2] = {
  { 285512L, 27959L }, // Albireo
  { 279235L, 38785L }, // Vega
  { 213908L, 19170L }, // Arktur
  {  37962L, 89264L }, // Polaris
  {      0L,     0L }, // Zero
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
    Serial.println("algn");
    #ifdef BOARD_MEGA
      Serial2.println("algn");
    #endif

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
      g_angleRightAscension = targets[param][0];
      g_angleDeclination = targets[param][1];

      char sbuf[13];
      sprintf(sbuf, "%06ld", abs(g_angleRightAscension));
      #ifdef PRINT_SERIAL
        Serial.print("sa"); // sa = Set Alignment
        Serial.print(g_angleRightAscension > 0 ? "+" : "-");
        Serial.print(sbuf);
      #endif
      #ifdef PRINT_SERIAL2
        Serial2.print("sa");
        Serial2.print(g_angleRightAscension > 0 ? "+" : "-");
        Serial2.print(sbuf);
      #endif
      
      sprintf(sbuf, "%06ld", abs(g_angleDeclination));
      #ifdef PRINT_SERIAL
        Serial.print(g_angleDeclination > 0 ? "+" : "-");
        Serial.println(sbuf);
      #endif
      #ifdef PRINT_SERIAL2
        Serial2.print(g_angleDeclination > 0 ? "+" : "-");
        Serial2.println(sbuf);
      #endif
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close())      // ****** STABLE END *********
  {
    // loop end
    // you can here reset some global vars or delete it
    #ifdef PRINT_SERIAL
      Serial.println("salgn");
    #endif
    #ifdef PRINT_SERIAL2
      Serial2.println("salgn");
    #endif
  }
}
