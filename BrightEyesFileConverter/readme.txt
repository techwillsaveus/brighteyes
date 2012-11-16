Place the input video file in the /data/ folder, named "input.mov". Input format is ideally 627x220, 30 fps, MUST be .mov.

Anything outside that size will be scaled, anything of a different FPS will be played at 30 FPS (unless SDplayer.ino, the player firmware for Bright Eyes, is modified).

The converter program itself is controlled via the keyboard:
- c converts the input file, closing the program once done
- p plays / pauses playback of the input file for review
- r resets playback of the movie to the start (this does not affect conversion)
- s shows / hides an overlay of the Bright Eyes LEDs for guidance
- t toggles text information display on or off

Output will be into the /data/ folder, with files named movieNNN.dat, sequentially numbered allowing you to convert lots of files and then transfer them to SD card for playback. When conversion finishes, the Processing console (black area at the bottom of the screen) will display the name of the most recently converted file.

