## DOING

## TODO

 * add universal device inquiry

   F0 7E 7F 06 01 F7

   reply

   F0 7E 00 06 02
   42 04 01
   00 00
   22 00
   00 00
   F7

   F0 7E 7F 06 02	Universal Device Reply
   00 00 0E	Alesis Manufacturer ID
   0E 00	QS Family ID, LSB first
   0x 00	QS Family Member, LSB first
   xx xx xx xx	Software revision, ASCI (ex. 30 31 30 30  = '0100' = 1.00)
   F7	End-Of-Exclusive

 * universal device sysex dump request (?)

   F0 42 50 00 01 F7

   reply

   F0 42 50 01 00 01 04 01 00 00 22 00 00 00 F7

 * preferences
  - save window positions 
     see http://doc.trolltech.com/4.4/mainwindows-recentfiles.html
 * system tray icon
     see http://doc.trolltech.com/4.4/desktop-systray.html
 * Fix global window shortcuts
 * MidiGear.cpp MidiGear.h
  - MidiKorgNanoPAD.cpp MidiKorgNanoPAD.h
  - MidiKorgNanoKEY.cpp MidiKorgNanoKEY.h

## DONE

2009-02-09
 * Added debian/control files. Tested the build process to work on
   Debian/Lenny and Ubuntu 8.10.

2009-01-13
 * Fix resizable bug

2008-11-24
 * add MMC support
   http://en.wikipedia.org/wiki/MIDI_Machine_Control

2008-11-23
 * added README
 * added RenoiseKeyBindings.h and RenoiseKeyBindings.cpp

2008-11-20
 * application icon

2008-11-19
 * little code cleanup

2008-11-16
 * X11DoTool.cpp, X11DoTool.h
  - added more keysym mappings
 * Add shortcuts - filtered midi output port

2008-11-14
 * leds
  - support for multiple scenes

2008-11-11
 * preferences
  - save renoise keystroke mappings
 * MidiKorgKorgNANOKontrol.cpp
  - add tiny dots to show midi and renoise mappings

2008-11-10
 * preferences
  - save skin selection

2008-11-09
 * Add another indirection in Renoise actions menu
 * Check "none" midi port, if the port specified in the prefs could not be opened on startup

2008-11-08
 * preferences
  - remember last opened midi input port
  - try to auto connect midi input port if no other port is set in the prefs

2008-11-07
 * Added ShortcutForm.cpp, ShortcutForm.h, ShortcutForm.ui
 * MidiIO.cpp MidiIO.h
   - select Midi input port by name

2008-11-04
 * preferences
  - save midi mappings

2008-11-03
 * MidiKorgNanoKONTROL.cpp MidiKorgNanoKONTROL.h
  - show current Renoise mapping in top left corner
 * Use RtMidi callback function pointer instead of polling
 * Started to write X11DoTool.cpp, X11DoTool.h

2008-11-01
 * MidiIO.cpp MidiIO.h
  - MidiEvent.h
 * MidiKorgNanoKONTROL.cpp MidiKorgNanoKONTROL.h
  - toggle black and white skin
  - popup menu
  - Add reset button in midi mapping mode
  - Add help info in midi mapping mode
  - Add renoise action menu
  - Add renoise clear mapping action menu
  - Add toggle Renoise keystroke action
 * nanoKONTROL - white skin adjustments, leds
 * RenoiseKeyBinding.h, RenoiseKeyBinding.cpp XML parsing
  - contains only non-empty renoise actions
 * RenoiseCategory.h, RenoiseCategory.cpp XML parsing
