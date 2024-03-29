/* ===================================================================== *
 *                                                                       *
 * Dynamic content                                                       *
 *                                                                       *
 * ===================================================================== *
 */


boolean shouldHandleDynamicParam(uint8_t line) {
  // check if this function is active (cursor stands on this line)
  if (line == LCDML.MENU_getCursorPos())
  {
    // make only an action when the cursor stands on this menu item
    //check Button
    if(LCDML.BT_checkAny())
    {
      if(LCDML.BT_checkEnter())
      {
        // this function checks returns the scroll disable status (0 = menu scrolling enabled, 1 = menu scrolling disabled)
        if(LCDML.MENU_getScrollDisableStatus() == 0)
        {
          // disable the menu scroll function to catch the cursor on this point
          // now it is possible to work with BT_checkUp and BT_checkDown in this function
          // this function can only be called in a menu, not in a menu function
          LCDML.MENU_disScroll();
        }
        else
        {
          // enable the normal menu scroll function
          LCDML.MENU_enScroll();
        }

        // do something
        // ...
        
        LCDML.BT_resetEnter();
      }

      return true;
    }
  }

  return false;
}



bool dynamicAngleEntry(const uint8_t line, volatile long &angle, const long stepSize, const long minimum, const long maximum) {
  bool updatedAngle = false;
  // This check have only an effect when MENU_disScroll is set
  if(LCDML.BT_checkUp())
  {
    if(angle + stepSize <= maximum) {
      angle+=stepSize;
    }
    LCDML.BT_resetUp();
    updatedAngle = true;
  }

  // This check have only an effect when MENU_disScroll is set
  if(LCDML.BT_checkDown())
  {
    if(angle - stepSize >= minimum) {
      angle-=stepSize;
    }
    LCDML.BT_resetDown();
    updatedAngle = true;
  }


  if(LCDML.BT_checkLeft())
  {
    if(angle + stepSize <= maximum) {
      angle+=stepSize;
    }
    LCDML.BT_resetLeft();
    updatedAngle = true;
  }
  
  if(LCDML.BT_checkRight())
  {
    if(angle - stepSize >= minimum) {
      angle-=stepSize;
    }
    LCDML.BT_resetRight();
    updatedAngle = true;
  }

  return updatedAngle;
}



void handleDynamicAngleEntry(const uint8_t line, const char* angle_name, const char* serial_name, const long stepSize, volatile long &angle, const long minimum, const long maximum)
// *********************************************************************
{
  bool updatedAngle = false;
  // check if this function is active (cursor stands on this line)
  if (shouldHandleDynamicParam(line)) {
    updatedAngle = dynamicAngleEntry(line, angle, stepSize, minimum, maximum);
  }

  char str_angle[7];
  dtostrf(angle/1000., 5, 1, str_angle);

  char buf[30];
  sprintf (buf, angle_name, str_angle);

  // setup function
  u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);     // the value can be changed with left or right
  
  if(updatedAngle) {
    char sbuf[10];
    sprintf(sbuf, "%06ld", abs(angle));

    #ifdef PRINT_SERIAL
      Serial.print(serial_name);
      Serial.print(angle > 0 ? "+" : "-");
      Serial.println(sbuf);
    #endif
    #ifdef PRINT_SERIAL2
      Serial2.print(serial_name);
      Serial2.print(angle > 0 ? "+" : "-");
      Serial2.println(sbuf);
    #endif
  }
}

void mDyn_status(uint8_t line) {
  char buf[30];
  sprintf (buf, "Status: %s", g_status);
  u8g2.drawStr( _LCDML_DISP_box_x0+_LCDML_DISP_font_w + _LCDML_DISP_cur_space_behind,  (_LCDML_DISP_font_h * (1+line)), buf);
}


// *********************************************************************
// ********************** Altitude / Declination ***********************
void mDyn_declination_10_0(uint8_t line) {
  handleDynamicAngleEntry(line,  "Dec.     | 10.0: %s", "dc", 10000L, g_angleDeclination, -90000L, 90000L);
}

void mDyn_declination_1_0(uint8_t line) {
  handleDynamicAngleEntry(line,  "Dec.     |  1.0: %s", "dc", 1000L, g_angleDeclination, -90000L, 90000L);
}

void mDyn_declination_0_1(uint8_t line) {
  handleDynamicAngleEntry(line,  "Dec.     |  0.1: %s", "dc", 100L, g_angleDeclination, -90000L, 90000L);
}

// *********************************************************************
// *******************Azimuth / Right Ascension ************************
void mDyn_right_ascension_10_0(uint8_t line) {
  handleDynamicAngleEntry(line,  "RightAsc | 10.0: %s", "ra", 10000L, g_angleRightAscension, 0L, 360000L);
}

void mDyn_right_ascension_1_0(uint8_t line) {
  handleDynamicAngleEntry(line,  "RightAsc |  1.0: %s", "ra", 1000L, g_angleRightAscension, 0L, 360000L);
}

void mDyn_right_ascension_0_1(uint8_t line) {
  handleDynamicAngleEntry(line,  "RightAsc |  0.1: %s", "ra", 100L, g_angleRightAscension, 0L, 360000L);
}
