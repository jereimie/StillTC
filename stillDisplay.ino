// arg1 = sText expected to be length < DISPLAY_DIGIT_COUNT
// Updates segment display with sText or the first characters that fit on the screen
void segDisplayUpdate( char * sText )
{
  for( byte i=0; i < DISPLAY_DIGIT_COUNT; i++ )
  {
    if( sText[i] == '\0' )
      break;

    segDisplayClear();
    segDisplayShowVal( i, sText[i] );
  }
}

// arg1 = Position on display (left to right)
// arg2 = value for position; expecting 0-9,'0'-'9','E/e','C/c','F/f'
// arg3 (optional) = delay time on display (default 5ms) 0-255ms
// Sets the pins on the LED display to show arg2 at arg1 position and delays arg3 or (5ms default)
void segDisplayShowVal( byte bPos, byte iValue, ... )
{
  va_list vList;
  int delayTime = -1;

  va_start( vList, iValue );
  delayTime = va_arg( vList, int );
  va_end( vList );

  if( delayTime < 0 || delayTime > 255 )
  {
    delayTime = 5;
  }

  segDisplaySetPins( bPos, getDigitBits((byte)iValue)); // cast not required here but put in for clarity of types
  delay( delayTime );
}

// valid input is 0-9 OR ascii 48-57 ('0'-'9') OR ascii 67/99 (C) 70/102 (F) OR ' ' (for clear digit)
// returns segment display bits for bDigit or bits for ' ' as default
byte getDigitBits( byte bDigit )
{
  byte bBitPosition;

  if( 47 < bDigit && bDigit < 58 ) // if it's '0'-'9' reduce to (int) 0-9
  {
    bDigit -= 48;
  }

  switch( bDigit ) // probably switch to if statements if used on a display that supports A-Z
  {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      bBitPosition = bDigit; 
      break;

    case 67: // 'C'
    case 99: // 'c'
      bBitPosition = 10; // look up in table;
      break;

    case 69: // 'E'
    case 101: // 'e'
      bBitPosition = 12; // look up in table;
      break;

    case 70:  // 'F'
    case 102: // 'f'
      bBitPosition = 11; // look up in table;
      break;

    case 80:  // 'P'
    case 112: // 'p'
      bBitPosition = 14; // look up in table;
      break;

    case 73:  // 'I'
    case 105: // 'i'
      bBitPosition = 15; // look up in table;
      break;

    case 68:  // 'D'
    case 100: // 'd'
      bBitPosition = 16; // look up in table;
      break;

    default:
      bBitPosition = 13; // ' ' in table;
      break;
  }

  return SEG_DISPLAY_BITS[bBitPosition];
}

// Uses bit mask bits value and sets corresponding pin states.
void segDisplaySetPins( byte pos, byte bits )
{
  byte targetAnodePin = 0;

  switch( pos )
  {
    case 1: targetAnodePin = PIN_N2; break;
    case 2: targetAnodePin = PIN_N3; break;
    case 3: targetAnodePin = PIN_N4; break;
    case 4: targetAnodePin = PIN_BM; break;
    default: targetAnodePin = PIN_N1; break;
  }

  digitalWrite( targetAnodePin, LOW );
  digitalWrite( PIN_A, bits&1 );
  digitalWrite( PIN_B, bits&2 );
  digitalWrite( PIN_C, bits&4 );
  digitalWrite( PIN_D, bits&8 );
  digitalWrite( PIN_E, bits&16 );
  digitalWrite( PIN_F, bits&32 );
  digitalWrite( PIN_G, bits&64 );
}

// Sets all pin values to display blank
void segDisplayClear()
{
  digitalWrite( PIN_BM, HIGH );
  digitalWrite( PIN_N1, HIGH );
  digitalWrite( PIN_N2, HIGH );
  digitalWrite( PIN_N3, HIGH );
  digitalWrite( PIN_N4, HIGH );

  digitalWrite( PIN_A, HIGH );
  digitalWrite( PIN_B, HIGH );
  digitalWrite( PIN_C, HIGH );
  digitalWrite( PIN_D, HIGH );
  digitalWrite( PIN_E, HIGH );
  digitalWrite( PIN_F, HIGH );
  digitalWrite( PIN_G, HIGH );
  digitalWrite( PIN_TM, HIGH );
}
