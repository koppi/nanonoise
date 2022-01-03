#ifndef X11DOTOOL_H
#define X11DOTOOL_H

#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
#include <QX11Info>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include "SendKeys.h"
#endif
#include <QString>
#include <QHash>
#include <QWidget>

class X11DoTool
{

public:

  X11DoTool(QWidget *parent);

  void sendKey(QString sequence, bool onoff);

private:
#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
        Display *dpy;
#elif defined(Q_WS_WIN32)
  CSendKeys *sendKeys;
#endif
  QWidget *m_parent;

  QHash<QString, QString> symbolmap;
};

#endif
