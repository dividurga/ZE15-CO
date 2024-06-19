
/*  ZE15_CO Library ZE15_CO.cpp
 *  Facilitates the use of UART I/O Modes provided by the the Winsen ZE015_CO Electrochemical
 *  CO Module as documented in User Manual Version:1.1 Valid from 12 April, 2018, available at
 *  https://www.winsen-sensor.com/d/files/ZE15-CO.pdf
 *
 * Much of this code is patterned after that found in ZE07-CO_Sensor library created by
 * HarvTech https://github.com/HarvTech/ZE07_CO_UART_Library *
 * Created by Divija 17 June, 2024
 */

#include "ZE15_CO.h"

// Point mySerial to the Software Serial port.
ZE15_CO::ZE15_CO(SoftwareSerial *Serial)
{
  mySerial = Serial;
}

// Point mySerial to the Hardwareware Serial port.
ZE15_CO::ZE15_CO(HardwareSerial *Serial)
{
  mySerial = Serial;
}

// Request data.
void ZE15_CO::Request_CO_data()
{
  // writing question automatically switches it to QnA mode
  for (int i = 0; i <= 8; i++)
  {
    mySerial->write(question[i]);
  }
}

// Update Mode2 data and reference params.
void ZE15_CO::update_data(float &CON, bool &data_ready)
{
  if (mySerial->available() > 0)
  {
    This_Byte = mySerial->read(); // Read a data Byte.
    // When Start Byte0 is received.
    if (This_Byte == 255)
    {
      _j = 0;  // Assign it as byte index 0.
      sum = 0; // Initialize Sum check.
    }
    answer_data[_j] = This_Byte; // Enter byte in data array in Byte order.
    if ((_j != 0) && (_j != 8))  // Exclude Byte0 and Byte8.
    {
      sum = sum + answer_data[_j]; // Accumulate Sum check.
    }
    _j++; // Increment byte index.

    // When last data Byte has been received.
    if (_j > 8)
    {
      sum = ~sum + 1;            // Sum check complete.
      if (sum == answer_data[8]) // Sum check = Check sum?
      {
        // checking for sensor read failure by seeing is msb is 1 or 0
        // mask is 0b10000000 or 0x80

        if (answer_data[2] & 0x80)
        {
          Serial.println("Error: MSB of answer_data[2] is 1, indicating a sensor read failure.");
        }
        else
        {
          // Mask to isolate the last 5 bits (00011111 in binary)
          uint8_t mask = 0x1F;
          // Use the & operator to isolate the last 5 bits
          uint8_t result = answer_data[2] & mask;
          CON = float(float(result << 8 | answer_data[3])) / 10;
          data_ready = !data_ready;
        }
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------
