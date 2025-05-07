1  ' This is a part of Konpu (see: https://github.com/polijan/konpu)
2  ' I place this Locomotive BASIC source in the Public Domain.
3  '
4  ' This program retrieves the default ROM font of an Amstrad CPC.
5  ' This will print it as lines of Glyph64 to the printer ...
6  ' which should be convenient to export using a CPC emulator.
7  '
10 SYMBOL AFTER 0:' <-- After this, character symbols are found after HIMEM
11 PRINT "Printing the CPC font on the printer ..."
12 FOR ch = 0 TO 255
13     PRINT ch;:' <-- on screen, as progress indicator
14     FOR i = 7 TO 0 STEP -1
15         row = PEEK(HIMEM + ch * 8 + i + 1)
16         rev = 0:' Reverse the bits of "row" into "rev":
17         FOR j = 0 TO 7
18             IF (row AND (2^j)) <> 0 THEN rev = rev + 2^(7 - j)
19         NEXT j
20         PRINT #8,HEX$(rev,2);
21     NEXT i
22     PRINT #8,USING "  ###";ch
23 NEXT ch
