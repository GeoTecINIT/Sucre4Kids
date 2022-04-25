/* U8g2 library by me
 */

#include "U8g2.h"

/**
 * Constructor.
 */
U8g2::U8g2()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void U8g2::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void U8g2::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void U8g2::doit()
{
    Serial.println("called doit");
}
