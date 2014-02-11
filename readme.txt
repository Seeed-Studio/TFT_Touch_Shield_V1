The libraries TFT and TouchScreen for Arduino 1.0 IDE:

If you want it works on the MEGA board or MEGA2560,you should change the 29 line #define SEEEDUINO to #define MEGA.

/*******************************/
As the arduino official website said,we should avoid using other functions inside the brackets, it may lead to incorrect results.So in function drawline()in the TFT.cpp file£¬some lines that use abs() were changed.
Reference:http://arduino.cc/en/Reference/Abs

Change line "#define GREEN 0x7e00" in TFT.h to be "#define GREEN 0x07e0".

by Frankie.Chu on June 28, 2012
/********************************/