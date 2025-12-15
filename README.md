# nanonoise - map midi to Renoise keystrokes

The nanonoise application is a quick hack to enable sending keystrokes to
the [Renoise tracking software](https://www.renoise.com/) triggered by midi cc messages and midi mmc
commands.

The app was originally written for the KORG nanoKONTROL midi controller,
however it should also work with other midi controller hardware. The source
code was tested to compile and run under Linux.

![nanonoise-midi-mapping-mode](https://github.com/koppi/nanonoise/raw/gh-pages/nanonoise-midi-mapping-mode.png "midi mapping mode")

![nanonoise-assign-renoise-action](https://github.com/koppi/nanonoise/raw/gh-pages/nanonoise-assign-renoise-action.png "assign Renoise keystroke")

## Features

* Maps midi cc messages and midi mmc commands to Renoise keystrokes
 - Sends a key press event triggered by a midi cc message with the value 127.
 - Sends a key release event triggered by a midi cc message with the value 0.
 - Sends a key press- and release event triggered by a midi mmc command.
 - Parses your most recent ~/.renoise/V??????/KeyBindings.xml to get the
   knowledge about Renoise keystroke actions.

* Virtual midi output port
 - Forwards only midi cc messages which are not mapped to trigger keystrokes.
   Select this virtual midi output port as a Renoise midi input port. In this
   way you can still map non-keystroke-triggering buttons, sliders and knobs
   in Renoise.

* Virtual midi input port
 - Recives midi messages of other midi gear connected to this port. In this
   way you can attach additional midi controller hardware to the application.

* Support for midi controller hardware:
 - nanoKONTROL - Scene switch support. Scene switch is triggered by the
   nanoKONTROL's scene switch sysex message.
 - Support for additional midi controller hardware can be added to the
   source code. Send your patches, please.
   
## Install and compile nanonoise from source code

First install the development packages needed to build nanonoise:

```bash
sudo apt -y install libqt5x11extras5-dev qtbase5-dev libasound2-dev libx11-dev libxtst-dev
```

Then download the application's source code from the github.com repository:

```bash
git clone git://github.com/koppi/nanonoise.git
```

(this will create the directory nanonoise/) and compile nanonoise:

```bash
cd nanonoise
qmake
make -j$(nproc)
```

Install and run nanonoise as follows:

```bash
sudo cp nanonoise /usr/local/bin
nanonoise &
```

## Supported operating systems

- [x] Linux - Ubuntu 22.04 - 26.04 LTS.
- [ ] MacOS - (not tested so far):
   Needs a bit of tweaking in the source code.
   Please send patches and give feedback!
- [ ] Windows - (not tested so far):
   Needs lot of tweaking in the source code.
   Send patches and give feedback!

# Contact

* Renoise forum support thread: > Nanonoise - Midi Control Renoise Tool
  http://www.renoise.com/board/index.php?showtopic=18679

## Thanks

* to the Renoise developers for porting Renoise to Linux.
  see http://www.renoise.com/

* to Jordan Sissel for his code to send keystrokes using X11's XTEST extension
  see http://www.semicomplete.com/projects/xdotool/

* to Rui Nuno Capela for his qtractor source code
  see http://qtractor.sourceforge.net/

* to github.com for filespace and backup
  see http://github.com/
