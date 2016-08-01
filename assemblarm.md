# Programming in Assemblarm
Controlling the 3D stage through the Java interface is done mainly through a language called Assemblarm. The language is space-delineated, and each command occupies one line. ___ indicates where a parameter must be inserted.  
Basic commands include:  
 * UP/DOWN/LEFT/RIGHT/FORWARD/BACK ___ -- When followed by a float n, these move the stage n units(cm by default) in the direction specified.  
 * VACON/VACOFF -- These turn the vacuum on and off, respectively.  
 * GOTO ___ ___ ___ -- When followed by three floats x, y, and z, this command moves the stage to the absolute coordinate (x,y,z).
 * MOVE ___ ___ ___ -- Similar to GOTO; however, this command moves the stage relative to the stage's current position. For example, the command "MOVE 1 2 3" would move the stage 1 unit to the right, 2 units forward, and 3 units up.  
 * CALIBRATE -- Calibrates the stage.  
 * WAIT ___ -- When followed by an integer n, this command pauses program execution for n milliseconds.  
 Also, other files within the directory can be called.
 * INCLUDE ___ -- When followed by a file name and possible float parameters, this command runs all code within the target file. Passed parameters can be accessed within the target program using keywords such as VAR1, VAR2, ... These variables can be used to replace floats in all parts of the target program.  
 * REQUIRE ___ -- When followed by an integer n, this command checks to see if the program has received fewer than n parameters. If so, an error message is printed and the program halts. It is recommended that every program that receives parameters include this command near the beginning.  
