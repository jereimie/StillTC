#include <PID_v1.h>
/*
 * Display Globals Settings
 */
// Wiring arduino nano is A0-A5 = Display 1-6; D2-D4 = display 7-9; D7-D12 = display 10-13
const byte PIN_TM = 17;
const byte PIN_A = 14;
const byte PIN_B = 15;
const byte PIN_C = 16;
const byte PIN_D = 18;
const byte PIN_E = 19;
const byte PIN_F = 2;
const byte PIN_G = 3;

// Here down are on PNP -> common anode.
const byte PIN_BM = 8;

// n1-n4 pins reversed from Display Anode pin order (BM Excluded) for compatability with string printing
const byte PIN_N1 = 6;
const byte PIN_N2 = 9;
const byte PIN_N3 = 7;
const byte PIN_N4 = 4;

// bit mask, least significant = 7segment a,b,c... and inverse logic for common anode PNP Transistor
// 0-9,C,F,E,' ',P,I,D in order
const byte SEG_DISPLAY_BITS[] = { 64,121,36,48,25,18,2,120,0,24, 198, 142, 134, 255, 12, 79, 33 };
const byte DISPLAY_DIGIT_COUNT = 4; // Currently using 4 digits;

/*
 * Tempature Global Settings
 */
// wiring ardiuno nano is  5v -> thermistor -> A6 + -> 9850 ohm res -> ground
const int T_R1 = 9850; // ohms, known side of termistor v divider
const byte PIN_T = A6;
char g_tempUnit = 'F';


/*
 * IO Pins
 */
const byte PIN_BTN_UP = 11;
const byte PIN_BTN_DOWN = 12;
const byte PIN_RELAY = 5;
const byte PIN_BTN_MENU = 10;

/*
 * Globals for sketch
 */
char g_sDisplay[DISPLAY_DIGIT_COUNT+1] = ""; // global for MicroController speed
double g_tTargetTemp = 160; // using F currently add code to switch between (was using byte, will play with later)

// PID vars
int g_iPidInterval = 500; // ms
unsigned long g_tPidCounter = 0;
double g_Kp = 20, g_Ki = 4, g_Kd = 40;
double g_pidInput, g_pidOutput;
PID g_myPID( &g_pidInput, &g_pidOutput, &g_tTargetTemp, g_Kp, g_Ki, g_Kd, DIRECT);

/*
 * Interupt code
 */
const int g_tDisplayDelay = 2500;
const byte g_iButtonPressDelay = 165 ;
const byte g_iActiveMenuCount = 5;
volatile byte g_iActiveMenu = 0; // 0 = not active, 1 = c/f, 2 = P, 3 = I, 4 = D
volatile unsigned long g_tDisplayTimeout = 0; // time stamp counter
volatile bool BTN_UP_PRESSED = false;
volatile bool BTN_DOWN_PRESSED = false;
volatile bool BTN_MENU_PRESSED = false;
volatile unsigned long g_iButtonPressCounter;

void setup()
{
  Serial.begin( 9600 );

  pinMode( PIN_A, OUTPUT ); // resist to cath led
  pinMode( PIN_B, OUTPUT ); // resist to cath led
  pinMode( PIN_C, OUTPUT ); // resist to cath led
  pinMode( PIN_D, OUTPUT ); // resist to cath led
  pinMode( PIN_E, OUTPUT ); // resist to cath led
  pinMode( PIN_F, OUTPUT ); // resist to cath led
  pinMode( PIN_G, OUTPUT ); // resist to cath led
  pinMode( PIN_TM, OUTPUT ); // resist to cath led
  
  pinMode( PIN_BM, OUTPUT ); // PNP Trans
  pinMode( PIN_N1, OUTPUT ); // PNP Trans
  pinMode( PIN_N2, OUTPUT ); // PNP Trans
  pinMode( PIN_N3, OUTPUT ); // PNP Trans
  pinMode( PIN_N4, OUTPUT ); // PNP Trans

  pinMode( PIN_BTN_UP, INPUT );
  pinMode( PIN_BTN_DOWN, INPUT );
  pinMode( PIN_BTN_MENU, INPUT );
  pinMode( PIN_RELAY, OUTPUT );

  segDisplayClear();

  // Interupt enable
  PCICR |= 1; // enable portB
  PCMSK0 |= 28;// pins 10,11,12

  g_myPID.SetMode( AUTOMATIC );
  g_myPID.SetTunings( g_Kp, g_Ki, g_Kd );
}

void loop()
{
  double temp = getTemp( PIN_T, g_tempUnit );

  if( millis() > g_tPidCounter ) // prevents heavy process and flicker on LED
  {
    g_pidInput = temp;
    g_myPID.Compute();
    analogWrite( PIN_RELAY, g_pidOutput );

    g_tPidCounter = millis() + g_iPidInterval;
  }

  /*
   * Handling button presses
   */
  if( BTN_UP_PRESSED || BTN_DOWN_PRESSED || BTN_MENU_PRESSED )
  {
    g_tDisplayTimeout = millis()+g_tDisplayDelay;

    if( BTN_MENU_PRESSED )
    {
      g_iActiveMenu = (g_iActiveMenu+1)%g_iActiveMenuCount;
    }
    
    switch( g_iActiveMenu )
    {
      case 1: // C/F (not using K for this application)
        if( BTN_UP_PRESSED || BTN_DOWN_PRESSED )
        {
          if( g_tempUnit == 'F' )
          {
            g_tTargetTemp = 5*(g_tTargetTemp-32)/9;
            g_tempUnit = 'C';
          }
          else
          {
            g_tTargetTemp = g_tTargetTemp*1.8+32;
            g_tempUnit = 'F';
          }
        }
        sprintf( g_sDisplay, "%4c", g_tempUnit );
        break;

      case 2: // P constant
        if( BTN_UP_PRESSED )
        {
          g_Kp++;
        }
    
        if( BTN_DOWN_PRESSED )
        {
          g_Kp--;
        }
  
        sprintf( g_sDisplay, "P%3i", (int)g_Kp );
        break;

      case 3: // I constant
        if( BTN_UP_PRESSED )
        {
          g_Ki++;
        }
    
        if( BTN_DOWN_PRESSED )
        {
          g_Ki--;
        }
  
        sprintf( g_sDisplay, "I%3i", (int)g_Ki );
        break;

      case 4: // D constant
        if( BTN_UP_PRESSED )
        {
          g_Kd++;
        }
    
        if( BTN_DOWN_PRESSED )
        {
          g_Kd--;
        }
  
        sprintf( g_sDisplay, "D%3i", (int)g_Kd );
        break;

      default: // temperature (should be 0)
        if( BTN_UP_PRESSED )
        {
          g_tTargetTemp++;
        }
    
        if( BTN_DOWN_PRESSED )
        {
          g_tTargetTemp--;
        }
  
        sprintf( g_sDisplay, "%3i%c", (int)g_tTargetTemp, g_tempUnit );
        break;
    }

    BTN_UP_PRESSED = false;
    BTN_DOWN_PRESSED = false;
    BTN_MENU_PRESSED = false;
  }

  /*
   * Handling display
   */
  if( g_tDisplayTimeout < millis() )
  {
    strcpy( g_sDisplay, "" );
    g_tDisplayTimeout = 0;
    g_iActiveMenu = 0;
  }

// add refresh timer for display temp.

  if( strlen( g_sDisplay ) < 1 )
  {
    sprintf( g_sDisplay, "%3i%c", (int)temp, g_tempUnit );
  }

  segDisplayUpdate( g_sDisplay );
}

ISR(PCINT0_vect)
{
  if( millis() > g_iButtonPressCounter ) // bouncing is the devil
  {
    if( !digitalRead(PIN_BTN_UP) ) // ! for pull up resistor logic
      BTN_UP_PRESSED = true;
  
    if( !digitalRead(PIN_BTN_DOWN) ) // ! for pull up resistor logic
      BTN_DOWN_PRESSED = true;
  
    if( !digitalRead(PIN_BTN_MENU) ) // ! for pull up resistor logic
      BTN_MENU_PRESSED = true;
  
    g_iButtonPressCounter = millis()+g_iButtonPressDelay;
  }
}
