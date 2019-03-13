// arg1 = pin to read thermistor from
// arg2 (optional) = format 'c/C'elsius (default) or 'f/F'arenheight or 'K'elvin
double getTemp( int pin, ... )
{
  /*
   * Original code for this function from:
   * https://computers.tutsplus.com/tutorials/how-to-read-temperatures-with-arduino--mac-53714
   * More info for later:
   * https://www.ametherm.com/thermistor/ntc-thermistors-steinhart-and-hart-equation
   */
  va_list vList;
  char tempUnit;
  double Temp;

  va_start( vList, pin );
  tempUnit = va_arg( vList, int );
  va_end( vList );

  if( 96 < tempUnit && tempUnit < 123 ) // make arg2 case independent
  {
    tempUnit -= 32;
  }

  // adjusted base resist for calibration instead of T_R1
  // some day I would like to redo coefficients but for now avg of +/- 0.5 avg degree margin of error with hack ohms, close enough for this application
  Temp = log(((10240000/analogRead(pin)) - 10800 )); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );

  if( tempUnit != 'K' )
  {
    Temp = Temp - 273.15; // Convert Kelvin to Celsius

    if( tempUnit == 'F' )
    {
      Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit
    }
  }

  return Temp;
}
