Bright Eyes
==========

Welcome to the fabulous Bright Eyes code project.
This is where you will find:

* Software to convert you're awesome videos to playback on the glasses
* Software to create simple content/frame by frame animation
* Software to convert text for use on the glasses
* Software to control the glasses (Arduino)

Do check out the READMEs on both the Processing Converter and Arduino SDPlayer for more details.
This project will be updated very soon to also include more demos and examples.

The converter uses two libraries you should download first if you plan to edit the source:
<a href="http://www.extrapixel.ch/processing/gifAnimation/">gifAnimation</a><br/>
<a href="http://www.sojamo.de/libraries/drop/">sDrop</a>

LED Layout
==========

LED ids start with the top-left LED at id 1 and continue to 87 on the left side, row by row
then continue 88 to 174 on the right lens as you can see in the image bellow:<br />
<img src="https://f.cloud.github.com/assets/2814183/111495/2cd53e14-6af8-11e2-99bf-cd549a092dfc.png"><br />

For Designers, an SVG with labelled LEDs is available <a href="https://github.com/twsu/brighteyes/blob/master/BrightEyesEditor/data/brighteyes.svg">here</a>
(alt+click to download)

For Developers, here is a lookup table for reference:
```
{0  ,0  ,0  ,1  ,2  ,3  ,4  ,5  ,6  ,7  ,8  ,9  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,0  ,0  ,0  ,
 0  ,10 ,11 ,12 ,13 ,14 ,15 ,16 ,17 ,18 ,19 ,20 ,21 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,97 ,98 ,99 ,100,101,102,103,104,105,106,107,108,0  ,
 22 ,23 ,24 ,25 ,26 ,27 ,28 ,29 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,0  ,0  ,0  ,0  ,0  ,0  ,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
 37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,45 ,46 ,47 ,48 ,49 ,50 ,51 ,0  ,0  ,0  ,0  ,0  ,0  ,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,
 52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,0  ,0  ,0  ,0  ,0  ,0  ,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,
 0  ,67 ,68 ,69 ,70 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,154,155,156,157,158,159,160,161,162,163,164,165,0  ,
 0  ,0  ,0  ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,166,167,168,169,170,171,172,173,174,0  ,0  ,0  };
```
Credits
==========
Converter and SDPlayer code developed by Stefan Dzisiewski-Smith for <a href="http://technologywillsaveus.org/" target="_blank">Technology Will Save Us</a>
Open Frameworks Port and Compiled Application were written by Ben McChesney <a href="http://http://cargocollective.com/benMcChesney</a> of Helios Interactive

