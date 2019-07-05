# Toy-Ikea-MicrowaveController
Microwave Controller for Ikea Kitchen DUKTIG

  Toy-Ikea-MicrowaveController
  Toy Ikea Microwave Controller

  Microware Controller (Arduino Based)
  for Ikea Kitchen DUKTIG
  LEDBERG LED spotlight
  SKaccess by Simon Kong
  
  www.skaccess.com
  ATTiny841

  ------------------------------------------------------------
  - Features
  increment timer by 5 sec from 0 to 60 sec
  increment timer by 10 sec from 1min to 5 min
  increment timer by 30 sec from 5min onwards to 51min
  press the rotary knob to start/pause countdown
  light will be on when countdown is active
  low tone buzzer noise when counting down to simulate the microwave being on
  opening the door while countdown is on will pause countdown
  when the door is open, light will be on
  when the countdown finishes, will display "End", if the door is not open,
    it will do the End melody again every 2 min, untill the door is open.
    this is to remind the user that there is still food in the microwave.
  
  V 1.0 - 30/06/2019 - Initial Release
  ------------------------------------------------------------
  - PCB Board Used
  https://www.skstore.com.au/electronics/pcb/Toy-Ikea-MicrowaveController

  https://github.com/SkullKill/Toy-Ikea-MicrowaveController-PCB

  more pictures in the wiki
  https://github.com/SkullKill/Toy-Ikea-MicrowaveController-PCB/wiki
  
  ------------------------------------------------------------
  - Code Used
  https://github.com/SkullKill/Toy-Ikea-MicrowaveController
  
  ------------------------------------------------------------
  - Video
  https://youtu.be/BQZ9P_Z0Fzc

  ------------------------------------------------------------
  - Product Used (included)
  15mm Aluminium Knob
  10mm Spacer/Standoff and M3 Screws
  
  ------------------------------------------------------------
  - Product Used (not included)
  
  Ikea DUKTIG Play kitchen, birch
  Article Number : 403.199.73
  https://www.ikea.com/au/en/catalog/products/40319973/

  Ikea LEDBERG LED spotlight, white, 4 pack
  Article Number : 103.517.33
  https://www.ikea.com/au/en/catalog/products/10351733/
  Note: the cable with the Dark Lines is the -ve/gnd cable. 
  But better to double check with a multimeter.

  magnetic door sensor (reed switch) [could also use a micro switch]
  3mm Acrylic sheet

  50mm L Angle Bracket
  https://www.bunnings.com.au/zenith-50mm-zinc-plated-angle-brackets-4-pack_p2760232

  various screws and bolts 
  4G X 12mm timber screws
  washer 5/32" / M4
  Bolts & Nuts 3/16" X 15mm / M5
  
  ------------------------------------------------------------
  Library Used
  
  https://github.com/SpenceKonde/ATTinyCore
  https://miguelpynto.github.io/ShiftDisplay/
  https://www.pjrc.com/teensy/td_libs_Encoder.html

  ------------------------------------------------------------
  Sketch uses 6850 bytes (90%) of program storage space. Maximum is 7552 bytes.
  Global variables use 226 bytes (44%) of dynamic memory, leaving 286 bytes for local variables. Maximum is 512 bytes.
