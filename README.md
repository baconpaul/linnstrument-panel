# Prototype Linnstrument Panel 

Based on this fact

```
3) LinnStrument has an RGB LED in every one of the 200 note pads. The Note Lights setting permits you to a) turn on or off each of the 12
notes in the chromatic scale in one color, and b) turn on or off each of the 12 notes in the chromatic scale in another accent color. Clearly
12this isn’t helpful for octaves consisting of other than 12 notes. To workaround this, we allow you to individual set each of the 200 note pads
to any of 10 colors or off by sending LinnStrument the following MIDI Control Change commands:
CC20: Column number of note pad to change (control key column is 0, left play column is 1, right play column is 25)
CC21: Row number of note pad to change (bottom row is 0, top is 7)
CC22: Color to change it to (0=as set in Note Lights settings, 1=red, 2=yellow, 3=green, 4=cyan, 5=blue, 6=magenta, 7=off, 8=white, 9=
orange, 10=lime and 11=pink).
```
