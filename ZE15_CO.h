

/*  ZE15_CO Library  ZE15_CO.h
 *  This library facilitates the use of both UART I/O Modes provided by the the Winsen ZE015_CO Electrochemical
 *  CO Module as documented in User Manual Version:1.7 Valid from 10/19/2020, available at
 *  https://www.winsen-sensor.com/sensors/co-sensor/ze07-co.htm.
 *  
 * Much of this code is patterned after that found in ZE07-CO_Sensor library created by 
 * HarvTech
 * 
 * Created by Divija 15 June 2024
 *
*/

#ifndef __ZE15_CO_H_
#define __ZE15_CO_H_

#include <SoftwareSerial.h>
#include <arduino.h>

class ZE15_CO
{ 
  public:

//-------------------Public Functions-------------------------------------------------------------------- 
  ZE15_CO(SoftwareSerial* Serial); //Use the designated SoftwareSerial port.
  ZE15_CO(HardwareSerial* Serial); //Use the designated HardwarwwareSerial port.
  void Request_CO_data(); //Request data
  void update_data(float &CON, bool &data_ready); //Update Mode2 data and reference local params.
  
//-------------------Public Variables-----------------------------------------------------------------------
  float _concentration; //CO concentration value in ppm.
  bool read_flag = false; //Toggles when UART data received

  
  private:
  
//----------------Private Variables-------------------------------------------------------------------------  
  Stream *mySerial; //Points myRerial to a Serial port.
  byte question[9] = {255, 1, 134, 0, 0, 0, 0, 0, 121}; //"Question" command data block.
  byte answer_data[9]; //Establish receive data buffer
  byte This_Byte; //data byte read from serial buffer.
  uint8_t _j = 0; //data array index.
  float CON;
  bool ready_flag;
  byte sum;
};

#endif
