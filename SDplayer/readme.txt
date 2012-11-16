**************************************************************************************
How to write your own firmware for the Bright Eyes board:
**************************************************************************************

Just use the Arduino IDE! Instructions for transferring the code to the board (as we don't use USB uploading like a normal Arduino) are given below. First though, a few notes on what you CAN'T do with the Bright Eyes board.

The Bright Eyes bootloader uses EEPROM on the ATMEGA to store the size and last-modified date of the firmware file in order to avoid loading the same file at every boot. Using the same areas of EEPROM in your Arduino code will cause problems. The no-go areas of EEPROM are the last 8 bytes (as we store two DWORDS) - these are EEPROM addresses 0x3F8..0x3FF inclusive. 

If none of the above makes much sense to you, or you aren't sure if you are using EEPROM, don't worry - it's very difficult to do without realising so the chances are EXTREMELY good that you are fine! (And you won't break your Bright Eyes even if there is an issue, strange things will just happen..).

**************************************************************************************
How to transfer firmware you have written in the Arduino IDE to the Bright Eyes board:
**************************************************************************************

1. Enable verbose compilation output
------------------------------------

For Mac: With the Arduino IDE open, select Arduino -> Preferences from the Menu bar (⌘+,). Ensure the checkbox labelled "Show verbose output during: compilation" is checked.

For Windows: With the Arduino IDE open, select File -> Preferences from the Menu bar (Ctrl+,). Ensure the checkbox labelled "Show verbose output during: compilation" is checked.

2. Compile (Verify) your code
-----------------------------

Click the Check (Verify) button as opposed to the usual Arrow (Upload) button. This will cause the code to be compiled, but not uploaded, and a lot of text will appear at the bottom of the Arduino IDE window (in the console area).

3. Copy the output directory location
-------------------------------------

When compilation is complete, there will be a penultimate line in the console output which shows the location of the compiled file. For example, it might say something like:

/var/folders/g7/btk0n8zs669b453qkgy94g980000gn/T/build2705930656204672901.tmp/sketch_nov12a.cpp.hex

or

C:\Users\Stefan\AppData\Local\Temp\build8839891328849755506.tmp\sketch_nov12a.cpp.hex We need to copy just the folder, not the full file location, so select from the start of this line to the \ or / before the end and copy that text (⌘+C for Mac, Ctrl+C for Windows).

The clipboard will then contain something like: 

/var/folders/g7/btk0n8zs669b453qkgy94g980000gn/T/build2705930656204672901.tmp/

or

C:\Users\Stefan\AppData\Local\Temp\build8839891328849755506.tmp\

4. Find the compiled .hex file and copy to your SD card
-------------------------------------------------------

We now need to find the compiled file:

For Mac:

Run Terminal - you can find this in Applications -> Utilities in Finder, or by typing "Terminal" into Spotlight (⌘+Space). Once terminal is open (another black screen!), type "open" then a space and then paste in the location we copied from earlier. For example, this might read:

open /var/folders/g7/btk0n8zs669b453qkgy94g980000gn/T/build2705930656204672901.tmp/

Once you have done this, press Enter and a new Finder window should open. You can close Terminal now if you like. This folder will contain a large number of files (all of which are related in some way to compiling your code). You are looking for the only file in this directory which has the extension .hex. It will be named after your sketch name, with .cpp.hex appended to the end. You will notice that this is the same as the ending of the penultimate line of compilation (the bit we didn't copy).

You can now copy this file to your SD card. Place it in the root of the card (that is, not in any folders, just on the card itself).

For Windows:

Open the Windows Run dialog - you can click Start and then search for Run or press ⌘+R (the Windows Key, plus R). Once the dialog box is open, paste in the location we copied earlier and press enter. A new explorer window will open.

This folder will contain a large number of files (all of which are related in some way to compiling your code). You are looking for the only file in this directory which has the extension .hex. It will be named after your sketch name, with .cpp.hex appended to the end. You will notice that this is the same as the ending of the penultimate line of compilation (the bit we didn't copy).

You can now copy this file to your SD card. Place it in the root of the card (that is, not in any folders, just on the card itself).

5. Rename the .hex file to firmware.hex
---------------------------------------

On the SD card, rename your copied file to firmware.hex. This will make sure that the Bright Eyes board knows to load it when you next start.

6. Flash the board!
-------------------

With the Bright Eyes turned off, insert the SD card and then switch the board on. The red status LED should flash three times and then blink repeatedly during code flashing. Once it is done, leave the board for a while to reboot and your code should be running and ready to play with!

If for whatever reason, this doesn't work, try turning off the Bright Eyes board, reseating the SD card and turning the board on again.

It is also important to note that the Bright Eyes board stores the file size and last-modified date of the most recently flashed file. It uses this information to avoid flashing the same file at every boot, so if you are trying to flash the same file over itself (although you should NEVER need to do this), try recompiling the source to create a .hex file with a different last-modified date.


