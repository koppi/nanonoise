#include <QDebug>
#include <QStringList>

#include "X11DoTool.h"

#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
  #include <X11/extensions/XTest.h>
  #include <X11/keysymdef.h>
#elif defined (Q_WS_WIN32)
#include <windows.h>
#endif

X11DoTool::X11DoTool(QWidget *parent)
{
m_parent = parent;

#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */

  if (qgetenv("XDG_SESSION_TYPE") == QString("wayland")) {
    qDebug() << "Error: wayland not supported yet\n";
    exit(1);
  }

  dpy = QX11Info::display();

  int dummy;
  if (!XTestQueryExtension(dpy, &dummy, &dummy, &dummy, &dummy)) {
    qDebug() << "Error: No XTEST extension available on screen"
             << QX11Info::appScreen() << "\n";
    exit(1);
  }
#elif defined (Q_WS_WIN32)
  sendKeys = new CSendKeys();
#endif


#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
  symbolmap.insert("Alt", "Alt_L");
  symbolmap.insert("RAlt", "Alt_R");
  symbolmap.insert("Ctrl", "Control_L");
  symbolmap.insert("Shift", "Shift_L");
  symbolmap.insert("RShift", "Shift_R");
  symbolmap.insert("Space", "space");
  symbolmap.insert("NumpadEqual", "KP_Equal");
  symbolmap.insert("Numpad0", "KP_0");
  symbolmap.insert("Numpad1", "KP_1");
  symbolmap.insert("Numpad2", "KP_2");
  symbolmap.insert("Numpad3", "KP_3");
  symbolmap.insert("Numpad4", "KP_4");
  symbolmap.insert("Numpad5", "KP_5");
  symbolmap.insert("Numpad6", "KP_6");
  symbolmap.insert("Numpad7", "KP_7");
  symbolmap.insert("Numpad8", "KP_8");
  symbolmap.insert("Numpad9", "KP_9");
  symbolmap.insert("NumpadDivide", "KP_Divide");
  symbolmap.insert("NumpadEnter", "KP_Enter");
  symbolmap.insert("NumpadEqual", "KP_Equal");
  symbolmap.insert("NumpadMinus", "KP_Subtract");
  symbolmap.insert("NumpadMultiply", "KP_Multiply");
  symbolmap.insert("NumpadPlus", "KP_Add");
  symbolmap.insert("Comma", "comma");
  symbolmap.insert("Capital", "Caps_Lock");
  symbolmap.insert("Minus", "minus");
  symbolmap.insert("Plus", "plus");

  symbolmap.insert("Backslash", "backslash");
  symbolmap.insert("Grave", "grave");

  symbolmap.insert("Scroll", "Scroll_Lock");

  // TODO add more mappings here
  symbolmap.insert("control", "Control_L");
  symbolmap.insert("meta", "Meta_L");
  symbolmap.insert("super", "Super_L");
#elif defined (Q_WS_WIN32)
  symbolmap.insert("Apps", "{APPS}");

  symbolmap.insert("Alt", "%");
  symbolmap.insert("RAlt", "%");
  symbolmap.insert("Ctrl", "^");
  symbolmap.insert("Shift", "+");
  symbolmap.insert("RShift", "+");
  symbolmap.insert("Space", " ");
  symbolmap.insert("NumpadEqual", "=");
  symbolmap.insert("Numpad0", "{Numpad0}");
  symbolmap.insert("Numpad1", "{Numpad1}");
  symbolmap.insert("Numpad2", "{Numpad2}");
  symbolmap.insert("Numpad3", "{Numpad3}");
  symbolmap.insert("Numpad4", "{Numpad4}");
  symbolmap.insert("Numpad5", "{Numpad5}");
  symbolmap.insert("Numpad6", "{Numpad6}");
  symbolmap.insert("Numpad7", "{Numpad7}");
  symbolmap.insert("Numpad8", "{Numpad8}");
  symbolmap.insert("Numpad9", "{Numpad9}");
  symbolmap.insert("NumpadDivide", "/");
  symbolmap.insert("NumpadEnter", "~");

  symbolmap.insert("NumpadMinus", "-");
  symbolmap.insert("NumpadMultiply", "*");
  symbolmap.insert("NumpadPlus", "{PLUS}");
  symbolmap.insert("Comma", ",");
  symbolmap.insert("Capital", "{CAPSLOCK}");
  symbolmap.insert("Minus", "{SUBTRACT}");
  symbolmap.insert("Plus", "{PLUS}");

  symbolmap.insert("Backslash", "\\");
//TODO  symbolmap.insert("Grave", "grave");

//TODO  symbolmap.insert("Scroll", "Scroll_Lock");

  // TODO add more mappings here and test the rest
  symbolmap.insert("control", "^");
  symbolmap.insert("meta", "%");
  symbolmap.insert("super", "%");
#endif
}

void X11DoTool::sendKey(QString sequence, bool onoff) {

//    qDebug() << "sendKey( " << sequence << ")";

if (sequence.size() == 0) {
  return;
}

  int *keys = NULL;
  int nkeys = 0;
  int keys_len = 10;

  keys = (int*)malloc(keys_len * sizeof(int));
  int keysym;

  QStringList l = sequence.split("+", QString::SplitBehavior::SkipEmptyParts);
  for (int i = 0; i < l.size(); ++i) {
    QString tok = l.at(i);
    
    // Check if tok is an alias to another key
    if (symbolmap.contains(tok)) {
      tok = symbolmap.value(tok);
    }

    keysym = XStringToKeysym(tok.toLocal8Bit().data());
    if (keysym == NoSymbol) {
      qDebug() << "(symbol) No such key name '" << tok << "'. Ignoring it.";
    }
  
    keys[nkeys] = XKeysymToKeycode(dpy, keysym);
  
    if (keys[nkeys] == 0) {
      qDebug() << "No such key '" << tok << "'. Ignoring it.";
    }
  
    nkeys++;
    if (nkeys == keys_len) {
      keys_len *= 2;
      keys = (int *)realloc(keys, keys_len);
    }
  }
  
  if (onoff) {
    for (int i = 0; i < nkeys; i++) {
      //qDebug() << " X11DoTool sendKey " << keys[i];
      XTestFakeKeyEvent(dpy, keys[i], True, CurrentTime);
    }
  }
  
  if (!onoff) {
    for (int i = nkeys - 1; i >= 0; i--) {
      XTestFakeKeyEvent(dpy, keys[i], False, CurrentTime);
    }
  }

  free(keys);
  XFlush(dpy);
}
