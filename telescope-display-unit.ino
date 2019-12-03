#include <Arduino.h>
#include <LCDMenuLib2.h>
#include <U8g2lib.h>

// Chip select pin
#define CS_PIN 8
//#define BOARD_NANO
#define BOARD_MEGA

#define DEBUG_MSG

U8G2_ST7920_128X64_2_HW_SPI u8g2(U8G2_R0, CS_PIN, U8X8_PIN_NONE);     // (Arduino Nano, ...
//U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, CS_PIN, U8X8_PIN_NONE);   // (Arduino Mega and co

// settings for u8g lib and LCD
#define _LCDML_DISP_w                 128            // LCD width
#define _LCDML_DISP_h                 64             // LCD height
// font settings
#define _LCDML_DISP_font              u8g_font_5x8  // u8glib font (more fonts under u8g.h line 1520 ...)
#define _LCDML_DISP_font_w            5              // font width
#define _LCDML_DISP_font_h            8             // font height
// cursor settings
#define _LCDML_DISP_cursor_char       "X"            // cursor char
#define _LCDML_DISP_cur_space_before  2              // cursor space between
#define _LCDML_DISP_cur_space_behind  4              // cursor space between
// menu position and size
#define _LCDML_DISP_box_x0            0              // start point (x0, y0)
#define _LCDML_DISP_box_y0            0              // start point (x0, y0)
#define _LCDML_DISP_box_x1            128            // width x  (x0 + width)
#define _LCDML_DISP_box_y1            64             // hight y  (y0 + height)
#define _LCDML_DISP_draw_frame        0              // draw a box around the menu
// scrollbar width
#define _LCDML_DISP_scrollbar_w       4  // scrollbar width (if this value is < 3, the scrollbar is disabled)

// nothing change here
#define _LCDML_DISP_cols_max          ((_LCDML_DISP_box_x1-_LCDML_DISP_box_x0)/_LCDML_DISP_font_w)
#define _LCDML_DISP_rows_max          ((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0-((_LCDML_DISP_box_y1-_LCDML_DISP_box_y0)/_LCDML_DISP_font_h))/_LCDML_DISP_font_h)

// rows and cols
// when you use more rows or cols as allowed change in LCDMenuLib.h the define "_LCDML_DISP_cfg_max_rows" and "_LCDML_DISP_cfg_max_string_length"
// the program needs more ram with this changes
#define _LCDML_DISP_rows              _LCDML_DISP_rows_max  // max rows
#define _LCDML_DISP_cols              20                   // max cols

// This stores the angle of the Altitude (or Declination) axis
// To avoid having to deal with floats this stores the angle*1000
volatile long g_angleDeclination = 0;

// This stores the angle of the Azimuth (or Right Ascension) axis
// To avoid having to deal with floats this stores the angle*1000
volatile long g_angleRightAscension = 0;
// The telescope status
volatile char* g_status = "Offline";


// *********************************************************************
// Prototypes
// *********************************************************************
void lcdml_menu_display();
void lcdml_menu_clear();
void lcdml_menu_control();


// *********************************************************************
// Objects
// *********************************************************************
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, _LCDML_DISP_rows, _LCDML_DISP_cols, lcdml_menu_display, lcdml_menu_clear, lcdml_menu_control);


// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
// LCDML_0        => layer 0
// LCDML_0_X      => layer 1
// LCDML_0_X_X    => layer 2
// LCDML_0_X_X_X  => layer 3
// LCDML_0_...      => layer ...
// The API looks like this:
//          LCDML_add(id, prev_layer, new_num, condition, lang_char_array, callback_function)
// LCDML_addAdvanced (id, prev_layer, new_num, condition, lang_char_array, callback_function, parameter(0-255), _LCDML_TYPE_[dynParam|default]);


// Telescope status
LCDML_addAdvanced (0 , LCDML_0,        0 , NULL, "",       mDyn_status,       0,  _LCDML_TYPE_dynParam);  // Telescope status

// Angles sub-menu
LCDML_addAdvanced (1 , LCDML_0,        1 , NULL, "Angles", NULL,              0,  _LCDML_TYPE_default);  // Angles Menu
LCDML_addAdvanced (2 , LCDML_0_1,          1 , NULL, "",   mDyn_declination_10_0,      0,  _LCDML_TYPE_dynParam);  // Altitude/Declination +/- 10.0
LCDML_addAdvanced (3 , LCDML_0_1,          2 , NULL, "",   mDyn_declination_1_0,       0,  _LCDML_TYPE_dynParam);  // Altitude/Declination +/-  1.0
LCDML_addAdvanced (4 , LCDML_0_1,          3 , NULL, "",   mDyn_declination_0_1,       0,  _LCDML_TYPE_dynParam);  // Altitude/Declination +/-  0.1
LCDML_addAdvanced (5 , LCDML_0_1,          4 , NULL, "",   mDyn_right_ascension_10_0,  0,  _LCDML_TYPE_dynParam);  // Azimuth/Right Ascension +/- 10.0
LCDML_addAdvanced (6 , LCDML_0_1,          5 , NULL, "",   mDyn_right_ascension_1_0,   0,  _LCDML_TYPE_dynParam);  // Azimuth/Right Ascension +/-  1.0
LCDML_addAdvanced (7 , LCDML_0_1,          6 , NULL, "",   mDyn_right_ascension_0_1,   0,  _LCDML_TYPE_dynParam);  // Azimuth/Right Ascension +/-  0.1
LCDML_add         (8 , LCDML_0_1,          0 , "Back",     mFunc_back);

// Main Menu Axis adjustment and display
LCDML_addAdvanced (9 , LCDML_0,        2 , NULL, "",       mDyn_declination_1_0,       0,  _LCDML_TYPE_dynParam);  // Main menu Altitude/Declination adjustment
LCDML_addAdvanced (10, LCDML_0,        3 , NULL, "",       mDyn_right_ascension_1_0,   0,  _LCDML_TYPE_dynParam);  // Main menu Azimuth/Right Ascension adjustment

// "Targets" sub-menu
LCDML_add        (11  , LCDML_0,       4 , "Set Alignment Star",    NULL);
LCDML_addAdvanced(12  , LCDML_0_4,           2 , NULL, "Albireo", mTargets_select_target, 0, _LCDML_TYPE_default);  //     - Albireo
LCDML_addAdvanced(13  , LCDML_0_4,           3 , NULL, "Vega",    mTargets_select_target, 1, _LCDML_TYPE_default);  //     - Vega
LCDML_addAdvanced(14  , LCDML_0_4,           4 , NULL, "Arktur",  mTargets_select_target, 2, _LCDML_TYPE_default);  //     - Arktur
LCDML_addAdvanced(15  , LCDML_0_4,           5 , NULL, "Polaris", mTargets_select_target, 3, _LCDML_TYPE_default);  //     - Polaris
LCDML_addAdvanced(16  , LCDML_0_4,           6 , NULL, "Zero",    mTargets_select_target, 4, _LCDML_TYPE_default);  //     - Zero
LCDML_add        (17  , LCDML_0_4,           0 , "Back",          mFunc_back);                                      //     - Back to "Set Alignment"

// "Special" sub-menu
LCDML_add        (18  , LCDML_0,       5  , "Special"          , NULL);                        // "Special" sub-menu
LCDML_add        (19  , LCDML_0_5,         1  , "Go to Root"       , mFunc_goToRootMenu);      //     - Back to root
LCDML_add        (20  , LCDML_0_5,         2  , "Jump to Time info", mFunc_jumpTo_timer_info); //     - Open timer info
LCDML_add        (21  , LCDML_0_5,         3  , "Back"             , mFunc_back);              //     - Back to root
LCDML_addAdvanced(22  , LCDML_0,       6 ,COND_hide, "screensaver" , mFunc_screensaver, 0,  _LCDML_TYPE_default); // The "screensaver" menu entry which is hidden by default

// Advanced menu (for profit) part with more settings
// Example for one function and different parameters
// It is recommend to use parameters for switching settings like, (small drink, medium drink, big drink) or (200ml, 400ml, 600ml, 800ml) ...
// the parameter change can also be released with dynParams on the next example
// LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
  

// Example for dynamic content
// 1. set the string to ""
// 2. use type  _LCDML_TYPE_dynParam   instead of    _LCDML_TYPE_default
// this function type can not be used in combination with different parameters
// LCDMenuLib_add(id, prev_layer,     new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
//LCDML_addAdvanced (24 , LCDML_0         , 5  , NULL,          ""                  , mDyn_para,                0,   _LCDML_TYPE_dynParam);                     // NULL = no menu function

// Example for conditions (for example for a screensaver)
// 1. define a condition as a function of a boolean type -> return false = not displayed, return true = displayed
// 2. set the function name as callback (remove the braces '()' it gives bad errors)
//  LCDMenuLib_add(id , prev_layer, new_num, condition,   lang_char_array, callback_function, parameter (0-255), menu function type  )
// LCDML_addAdvanced (25 , LCDML_0   ,     7  , COND_hide,  "screensaver"   , mFunc_screensaver,                 0,  _LCDML_TYPE_default);       // this menu function can be found on "LCDML_display_menuFunction" tab

// menu element count - last element id
// this value MUST be the same as the ID of the last menu element
  #define _LCDML_DISP_cnt    22

  // create menu
  LCDML_createMenu(_LCDML_DISP_cnt);
#define I2C_SLAVE_ADDRESS 9

// *********************************************************************
// SETUP
// *********************************************************************
  void setup()
  {
    u8g2.begin();
    //Wire.begin();
    Serial.begin(9600);
    Serial.println("s?");
    #ifdef BOARD_MEGA
      Serial2.begin(9600);
      Serial2.println("s?");
    #endif

    // LCDMenuLib Setup
    LCDML_setup(_LCDML_DISP_cnt);

    // Enable Menu Rollover
    LCDML.MENU_enRollover();

    // Enable Screensaver (screensaver menu function, time to activate in ms)
    //LCDML.SCREEN_enable(mFunc_screensaver, 10000); // set to 10 seconds
    LCDML.SCREEN_disable();

    // Some needful methods

    // You can jump to a menu function from anywhere with
    //LCDML.OTHER_jumpToFunc(mFunc_p2); // the parameter is the function name
    randomSeed(analogRead(0));
  }

// *********************************************************************
// LOOP
// *********************************************************************
//long tim = 0;
//byte x = 0;
long last_update = 0;
void loop()
{
  recvWithEndMarker();
  storeNewData();

  // Get update every two seconds
  if(millis() - last_update >= 2000) {
    last_update = millis();
    Serial.println("s?");
    #ifdef BOARD_MEGA
      Serial2.println("s?");
    #endif
  }
  
  LCDML.loop(); 
}

// *********************************************************************
// Serial data
// *********************************************************************
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (Serial2.available() > 0 && newData == false) {
          rc = Serial2.read();
          Serial.println(rc);

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

long getAngleFromCommand(int numChars=6, int offset=3) {
  long multiplier = 1L;
  long result = 0;
  for(int i=numChars-1; i>=0; i--) {
    result += (receivedChars[offset+i]-'0') * multiplier;
    multiplier *= 10L;
  }
  return result;
}

void storeNewData() {
    if (newData == true) {
        if(receivedChars[0] == 's') {
          // Status update: s:1-3
          if(receivedChars[2] == '1') {
            g_status = "Initializing";
          } else if(receivedChars[2] == '2') {
            g_status = "Aligning";
          } else if(receivedChars[2] == '3') {
            g_status = "Tracking";
          } else {
            g_status = "Offline";
          }
        } else if(receivedChars[0] == 'r' && receivedChars[1] == 'a') {
            // Update right ascension angle
            g_angleRightAscension = getAngleFromCommand(6, 3);
            if(receivedChars[2] == '-') {
              g_angleRightAscension *= -1L;
            }
            #ifdef DEBUG_MSG
              Serial.println("Updated right ascension to ");
              Serial.println(g_angleRightAscension/1000.);
            #endif
        } else if(receivedChars[0] == 'd' && receivedChars[1] == 'c') {
            // Update declination angle
            g_angleDeclination = getAngleFromCommand(6, 3);
            if(receivedChars[2] == '-') {
              g_angleDeclination *= -1L;
            }
            #ifdef DEBUG_MSG
              Serial.println("Updated declination to ");
              Serial.println(g_angleDeclination/1000.);
            #endif
        }
        LCDML.DISP_update();
        newData = false;
    }
}
// *********************************************************************
// check some errors - do not change here anything
// *********************************************************************
# if(_LCDML_glcd_tft_box_x1 > _LCDML_glcd_tft_w)
# error _LCDML_glcd_tft_box_x1 is to big
# endif

# if(_LCDML_glcd_tft_box_y1 > _LCDML_glcd_tft_h)
# error _LCDML_glcd_tft_box_y1 is to big
# endif

#if defined(BOARD_NANO) || (defined(DEBUG_MSG) && defined(BOARD_MEGA))
  #define PRINT_SERIAL
#endif
#ifdef BOARD_MEGA
  #define PRINT_SERIAL2
#endif
