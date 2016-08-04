# armduino
Armduino is a Java + Arduino program created for Quarknet that is used to control the 3D stage at SMU.

# assemblarm
Assemblarm is a custom-made scripting language to program the 3D stage to do sequencial steps. The main benefit of using this simple language is that you can write code while executing the code, so you can see the results in real time. By convention, an assemblarm filename ends in the extension .arm.  
Current features include:  
 * File references  
 * Variable passing (floats only)  
 * Absolute positioning  
 * Relative movement  
 * Pausing  

# Serial input
Look at Java code to see examples of serial input to the Arduino to control the motors. Strings are sent to the Arduino to give commands. Then, the Arduino sends a new line "\n" in order to indicate the completion of the command. Currently functional commands include:  
 * Send three floats x, y, and z separated by commas to move the arm relative to its current position. (e.g. "10,3,6")  
 * Send three floats x, y, and z separated by commas, followed by another commas and "abs" in order to move the arm to the absolute coordinate (x,y,z). (e.g. "10,3,6,abs")  
 * Send the string "vacon" to turn on the vacuum.  
 * Send the string "vacoff" to turn off the vacuum.  
 * Send the string "cal" to calibrate the arm. The Arduino calibrates by moving each axis in the negative direction until the sensors are tripped. The ending point is subsequently called the origin, (0,0,0).  

# Java interface
The Java code includes an interface to connect with the Arduino and to interpret assemblarm. When the main function in "armduino.java" is called, a colored window is opened up, and keypresses can be detected through this window.  
 * Up/down/left/right -- moves the arm in the x and y direction depending on the key pressed.  
 * Enter/backspace -- enter moves the arm up, and backspace moves the arm down. (z-axis movement)  
 * G -- moves the arm +5 units in each direction.  
 * C -- calibrates the arm.  
 * V -- toggles vacuum. Assumes that vacuum is off when program starts. If the vacuum is not off, simply press V twice.  

The console also accepts assemblarm commands. Use the INCLUDE keyword to read a program.  

A separate readme file for assemblarm is accessible at assemblarm.md.
