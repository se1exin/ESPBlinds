# ESPBlinds - Wifi Controlled Roller Blinds

STLs are available at: https://www.thingiverse.com/thing:5321952

## Parts used
 - 2.8v 1.7A Stepper Driver (SY42STH38-1684A): https://www.pololu.com/product/2267
 - DRV8825 Stepper Motor Driver: https://www.pololu.com/product/2133
 - ESP8266 12-F: https://www.katranji.com/tocimages/files/423280-229082.pdf
 - 12v DC 50mm Fan: https://core-electronics.com.au/extruder-50mm-fan.html
 - 12v 2A DC Power Supply (no link - just make sure the amperage is higher than your stepper, with some room to breath to also run the ESP8266)
 - 4x8x3mm Bearings for the main gear - https://core-electronics.com.au/makeblock-plain-ball-bearing-4-8-3mm-10-pack.html
 - M3x8mm screws (x4) for mounting the motor
 - M3x4mm screws (x4) for mounting the lid
 - 4x wood screws to mount the whole thing to the wall - you will need to decide the best screws to use based on what your wall is made out of.

...and some generic electronics components, such as 3.3v regulator, capacitors, resistors, and NPN transistors.

See the schematic for full details

![Schematic](img/schematic.png)

**NOTE** The schematic shows the A4988 Stepper Driver, which was from a previous iteration. I am now using the DRV8255 which has a slightly different pinout.

Most notably on the DRV8255, the VDD pin is not present and instead replaced by the FAULT pin (which I am not using), and the RST and SLP pins go to 3.3v rather than being tied to each other.

## Mounting instructions
- First mount the enclosure to the wall. The motor covers one of the screw holes.
- Mount the stepper motor before adding the main gear - it covers access to 2 of the motor screw holes
- Then put in the main gear and everything else
- Then add on the lid!

### Some more pics

![Gear](img/gear.jpg)

Main gear with ball-chain gear attachment

---

![Ball-Chain attachment](img/gear1.jpg)

Ball-chain gear attachment (another angle)

---

![Control Board](img/board.jpg)

Control Board (with A4988 instead of DRV8255 stepper driver)

---

![Housing with stepper](img/stepper-only.jpg)

Housing with stepper motor and gear installed

---

![Housing with stepper and board](img/stepper-board.jpg)

Housing with stepper motorm, gears, and board installed

---

![Housing with stepper and board](img/without-lid.jpg)

Everything ready to go, just need to put the lid on
