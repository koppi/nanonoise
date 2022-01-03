#include <QtGui>

#include "MidiKorgNanoKONTROL.h"

#include <QAction>
#include <QApplication>
#include <QMenu>

MidiKorgNanoKONTROL::MidiKorgNanoKONTROL(QWidget *parent) : QMainWindow(parent)
{

  setAttribute(Qt::WA_OpaquePaintEvent);

  x11do = new X11DoTool(this);

  setWindowIcon(QIcon(":images/icon.png"));
  setWindowTitle("nanonoise");

  scene = 0;

  NanoKONTROLstate = None;
  selectedRect = -1;
  skin = White;

  initRenoiseMenu();

  for (int j = 0; j <= 3; ++j) {
    rects[j].append(QRect(18, 118, 50, 30)); //fbackward
    rects[j].append(QRect(70, 118, 50, 30)); //play
    rects[j].append(QRect(118, 118, 50, 30)); // fforward
    rects[j].append(QRect(18, 147, 50, 30)); // repeat
    rects[j].append(QRect(70, 147, 50, 30)); // stop
    rects[j].append(QRect(118, 147, 50, 30)); //record
    
    for (int i = 1; i <= 6; ++i) {
      midiController[j].append(0);
      midiControllerValue[j].append(0);
      renoiseKeystrokes[j].append("");
    }
    
    for (int i = 0; i <=8; ++i) {
      int off = (int)(i * 77.5);
      rects[j].append(QRect(181+off, 100, 207-181, 29)); // button 1
      rects[j].append(QRect(181+off, 162, 207-181, 29)); // button 2
      rects[j].append(QRect(208+off, 23, 249-208, 64-23)); // knop
      rects[j].append(QRect(215+off, 75, 242-215, 219-75)); // fader
      
      midiController[j].append(0);
      midiController[j].append(0);
      midiController[j].append(0);
      midiController[j].append(0);

      midiControllerValue[j].append(0);
      midiControllerValue[j].append(0);
      midiControllerValue[j].append(0);
      midiControllerValue[j].append(0);

      renoiseKeystrokes[j].append("");
      renoiseKeystrokes[j].append("");
      renoiseKeystrokes[j].append("");
      renoiseKeystrokes[j].append("");
    }
  }

  resize(899, 231);
  setFixedSize(899, 231);

  clearMidiMappingAct = new QAction(tr("&Clear MIDI mapping"), this);
  clearMidiMappingAct->setShortcut(tr("Ctrl+C"));
  clearMidiMappingAct->setStatusTip(tr("Clear MIDI mapping of selected controller"));
  connect(clearMidiMappingAct, SIGNAL(triggered()), this, SLOT(clearMIDIMapping()));
  
  clearReMappingAct = new QAction(tr("Clear Renoise action"), this);
  clearReMappingAct->setShortcut(tr("Ctrl+X"));
  clearReMappingAct->setStatusTip(tr("Clear Renoise action of selected controller"));
  connect(clearReMappingAct, SIGNAL(triggered()), this, SLOT(clearReMapping()));
  
  midiMappingAct = new QAction(tr("Map &MIDI"), this);
  midiMappingAct->setShortcut(tr("Ctrl+M"));
  midiMappingAct->setStatusTip(tr("Map MIDI controller numbers"));
  midiMappingAct->setCheckable(true);
  connect(midiMappingAct, SIGNAL(triggered()), this, SLOT(toggleMIDIMapping()));
  reMappingAct = new QAction(tr("Map &Renoise"), this);
  reMappingAct->setShortcut(tr("Ctrl+R"));
  reMappingAct->setStatusTip(tr("Map controller to Renoise action"));
  reMappingAct->setCheckable(true);
  connect(reMappingAct, SIGNAL(triggered()), this, SLOT(toggleReMapping()));
  
  toggleSkinAct = new QAction(tr("&Toggle Skin"), this);
  toggleSkinAct->setShortcut(tr("Ctrl+S"));
  toggleSkinAct->setStatusTip(tr("Toggles skin black and white"));
  connect(toggleSkinAct, SIGNAL(triggered()), this, SLOT(toggleSkin()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
  
  setMouseTracking(true);

  connect(&mio, SIGNAL(midiRecived(MidiEvent *)),
	  this, SLOT(midiRecived(MidiEvent *)));

  loadPrefs();

  background = QPixmap(width(), height());

  mio.start();
}

MidiKorgNanoKONTROL::~MidiKorgNanoKONTROL()
{
  savePrefs();
}

void MidiKorgNanoKONTROL::loadPrefs()
{
  QSettings s;
  for (int j = 0; j <= 3; ++j) {
    QStringList mC = s.value("nanoKONTROLmidiCC" + QString("%1").arg(j)).toStringList();
    for (int i = 0; i < midiController[j].size(); ++i) {
      if (i < mC.size())
	midiController[j].replace(i, mC.at(i).toInt());  
    }
  }

  QString portName = s.value("nanoKONTROLmidiInPortName", "nanoKONTROL MIDI 1").toString();
  selectMIDIInputPort(portName);

  int skinName = s.value("nanoKONTROLskin", 1).toInt();
  if (skinName == 0) {
    skin = Black;
  } else {
    skin = White;
  }

  loadSkin();

  for (int j = 0; j <= 3; ++j) {
    QStringList rK = s.value("nanoKONTROLrenoiseKeystrokes" + QString("%1").arg(j)).toStringList();
    for (int i = 0; i < renoiseKeystrokes[j].size(); ++i) {
      if (i < rK.size())
	renoiseKeystrokes[j].replace(i, rK.at(i));  
    }
  }

  // window settings
  s.beginGroup("mainwindow");
  restoreGeometry(s.value("geometry", saveGeometry() ).toByteArray());
  restoreState(s.value("savestate", saveState()).toByteArray());
  move(s.value("pos", pos() ).toPoint());
  resize(s.value("size", size() ).toSize());

  if ( s.value("maximized", isMaximized()).toBool()) {
    showMaximized();
  }

  s.endGroup();
}

void MidiKorgNanoKONTROL::savePrefs()
{
  QSettings s;

  for (int j = 0; j <= 3; ++j) {
    QStringList mC;
    for (int i = 0; i < midiController[j].size(); ++i) {
      mC.append(QString::number(midiController[j].at(i)));
    }
    s.setValue("nanoKONTROLmidiCC" + QString("%1").arg(j), mC);
  }

  s.setValue("nanoKONTROLmidiInPortName", midiInPortName);

  s.setValue("nanoKONTROLskin", skin == Black ? 0 : 1);

  for (int j = 0; j <= 3; ++j) {
    QStringList rK;
    for (int i = 0; i < renoiseKeystrokes[j].size(); ++i) {
      rK.append(renoiseKeystrokes[j].at(i));
    }
    s.setValue("nanoKONTROLrenoiseKeystrokes" + QString("%1").arg(j), rK);
  }

  s.beginGroup("mainwindow");

  s.setValue("geometry", saveGeometry());
  s.setValue("savestate", saveState());
  s.setValue("maximized", isMaximized());

  if (!isMaximized()) {
    s.setValue("pos", pos());
    s.setValue("size", size());
  }

  s.endGroup();

  //  qDebug() << "Prefs saved" << endl;
}

void MidiKorgNanoKONTROL::initRenoiseMenu()
{
  reKeyBindings = new RenoiseKeyBindings();

  QFont f = QFont(QApplication::font().family(), 10);
    
  renoiseMenu = new QMenu("Renoise actions", this);
  //renoiseMenu->setFont(f);

  for (int c = 0; c < reKeyBindings->categories.size(); ++c) {
    RenoiseCategory cat = reKeyBindings->categories.at(c);

    QMenu *a = new QMenu(cat.m_identifier, this);
    //a->setFont(f);

    QHashIterator<QString, QList<RenoiseKeyBinding> > i(cat.bindings);
    while (i.hasNext()) {
      i.next();
      QString topic = i.key();
      QList<RenoiseKeyBinding> l = i.value();

      QMenu *topicMenu = new QMenu(topic, this);
      a->addMenu(topicMenu);

      for (int j = 0; j < l.size(); ++j) {
	QAction *b = new QAction(l.at(j).binding, this);
	b->setData(l.at(j).getID());
	//b->setShortcut(QKeySequence::fromString(l.at(j).key, QKeySequence::PortableText));
        //b->setFont(f);
	connect(b, SIGNAL(triggered()), this, SLOT(setReKeystroke()));
	renoiseActions.append(b);
	topicMenu->addAction(b);

	renoiseToKeystrokes[l.at(j).getID()].append(l.at(j).key);
      }
    }

    renoiseMenu->addMenu(a);
    //a->setShortcut(tr("Ctrl+C"));
    //a->setStatusTip(tr("Clear MIDI mapping of selected controller"));
  }

  midiInMenu = new QMenu(tr("MIDI in port"), this);
  QAction *midiNone = new QAction(tr("none"), this);
  midiNone->setData("none");
  midiNone->setCheckable(true);
  midiNone->setChecked(true);
  connect(midiNone, SIGNAL(triggered()), this, SLOT(selectMIDIInputPort()));
  midiInputPortActions.append(midiNone);

  midiInMenu->addAction(midiNone);
  midiInMenu->addSeparator();

  QStringList inputs = mio.getInputPorts();
  for (int i = 0; i < inputs.size(); ++i) {
      QAction *b = new QAction(inputs.at(i), this);
      b->setData(inputs.at(i));
      b->setCheckable(true);
      connect(b, SIGNAL(triggered()), this, SLOT(selectMIDIInputPort()));
      midiInMenu->addAction(b);
      midiInputPortActions.append(b);
  }
}

void MidiKorgNanoKONTROL::selectMIDIInputPort(QString name)
{
  int found = 0;

  midiInPortName = name;
  mio.openInputPortByName(name);

  QListIterator<QAction *> i(midiInputPortActions);
  while(i.hasNext()) {
    QAction *a = i.next();
    if (a->data().value<QString>() == name) {
      a->setChecked(true);
      found = 1;
    } else {
      a->setChecked(false);
    }
  }

  if (!found) {
    midiInputPortActions.at(0)->setChecked(true);
  }

  update();
}

void MidiKorgNanoKONTROL::selectMIDIInputPort()
{
  QAction* action = dynamic_cast<QAction*>(sender());

  if (action==0)
    return;

  QString port = action->data().value<QString>();
  selectMIDIInputPort(port);
}

void MidiKorgNanoKONTROL::setReKeystroke()
{
  QAction* action = dynamic_cast<QAction*>(sender());

  if (action==0)
    return;

  QString renoiseKeystrokeID = action->data().value<QString>();

  //qDebug() << renoiseKeystrokeID << endl;

  if (selectedRect != -1)  {
    renoiseKeystrokes[scene].replace(selectedRect, renoiseKeystrokeID);
  }

  update();
}

void MidiKorgNanoKONTROL::loadSkin()
{
  QString fileName;
  QString ledOnName;
  QString ledOffName;
  QString sliderName;
  QString knobName;
  QString buttonName;
  QString buttonPressedName;

  if (skin == White) {
    fileName = ":/images/nanokontrol-white.png";
    ledOnName = ":/images/nanokontrol-white-led-on.png";
    ledOffName = ":/images/nanokontrol-white-led-off.png";
    sliderName = ":/images/nanokontrol-white-slider.png";
    knobName = ":/images/nanokontrol-white-knob.png";
    buttonName = ":/images/nanokontrol-white-button.png";
    buttonPressedName = ":/images/nanokontrol-white-button-pressed.png";
  } else {
    fileName = ":/images/nanokontrol-black.png";
    ledOnName = ":/images/nanokontrol-black-led-on.png";
    ledOffName = ":/images/nanokontrol-black-led-off.png";
    sliderName = ":/images/nanokontrol-black-slider.png";
    knobName = ":/images/nanokontrol-black-knob.png";
    buttonName = ":/images/nanokontrol-black-button.png";
    buttonPressedName = ":/images/nanokontrol-black-button-pressed.png";
  }

  {
    QImage image = QImage();
    image.load(fileName);
    pm[0] = QPixmap::fromImage(image.scaled(QSize(width(), height())));
  }

  {
    QImage ledOn = QImage();
    ledOn.load(ledOnName);
    pm[1] = QPixmap::fromImage(ledOn);
  }

  {
    QImage ledOff = QImage();
    ledOff.load(ledOffName);
    pm[2] = QPixmap::fromImage(ledOff);
  }

  {
    QImage button1 = QImage();
    button1.load(buttonName);
    pmButton[0] = QPixmap::fromImage(button1);
  }

  {
    QImage button2 = QImage();
    button2.load(buttonPressedName);
    pmButton[1] = QPixmap::fromImage(button2);
  }

  {
    QImage knob = QImage();
    knob.load(knobName);
    pmKnob = QPixmap::fromImage(knob);
  }

  {
    QImage slider = QImage();
    slider.load(sliderName);
    pmSlider = QPixmap::fromImage(slider);
  }
}

void MidiKorgNanoKONTROL::toggleSkin()
{
  if (skin == Black) {
    skin = White;
  } else {
    skin = Black;
  }

  loadSkin();

  update();
}

void MidiKorgNanoKONTROL::clearMIDIMapping()
{
  if (selectedRect != -1)  {
    midiController[scene].replace(selectedRect, 0);
  }

  update();
}

void MidiKorgNanoKONTROL::clearReMapping()
{
  if (selectedRect != -1)  {
    renoiseKeystrokes[scene].replace(selectedRect, "");
  }

  update();
}

void MidiKorgNanoKONTROL::toggleReMapping()
{
  if (NanoKONTROLstate == ReMapping) {
    NanoKONTROLstate = None;
    selectedRect = -1;
    midiMappingAct->setChecked(false);
    reMappingAct->setChecked(false);
  } else if (NanoKONTROLstate == None) {
    midiMappingAct->setChecked(false);
    reMappingAct->setChecked(true);
    NanoKONTROLstate = ReMapping;
  } else if (NanoKONTROLstate == MIDIMapping) {
    midiMappingAct->setChecked(false);
    reMappingAct->setChecked(true);
    NanoKONTROLstate = ReMapping;
  }

  update();
}

void MidiKorgNanoKONTROL::toggleMIDIMapping()
{

  if (NanoKONTROLstate == MIDIMapping) {
    NanoKONTROLstate = None;
    selectedRect = -1;
    midiMappingAct->setChecked(false);
    reMappingAct->setChecked(false);
  } else if (NanoKONTROLstate == ReMapping) {
    NanoKONTROLstate = MIDIMapping;
    midiMappingAct->setChecked(true);
    reMappingAct->setChecked(false);
  } else {
    NanoKONTROLstate = MIDIMapping;
    midiMappingAct->setChecked(true);
    reMappingAct->setChecked(false);
  }

  update();
}

void MidiKorgNanoKONTROL::contextMenuEvent(QContextMenuEvent *)
{
}

void MidiKorgNanoKONTROL::paintEvent(QPaintEvent *)
{
  QPainter p(&background);
  
  //p.setRenderHint(QPainter::Antialiasing);
  
  QRect r(QPoint(0, 0), pm[0].size());
  p.drawPixmap(r, pm[0]);

  for (int i = 0; i <= 3; ++i) {
    QPoint pled;
    if (skin == White) {
      pled = QPoint(80+i*21, 186);
    } else {
      pled = QPoint(72+i*21, 178);
    }

    if ( scene != i ) {
      QRect rled(pled, pm[2].size());
      p.drawPixmap(rled, pm[2]);
    } else {
      QRect rled(pled, pm[1].size());
      p.drawPixmap(rled, pm[1]);
    }
  }

  for (int i = 0; i <= 8; ++i) {
    int off = (int)(i * 77.5);
    QRect sl(QPoint(215+off, (int)(75 + 81 - midiControllerValue[scene].at(6 + i*4 + 3) * 0.67)), pmSlider.size());
    p.drawPixmap(sl, pmSlider);

    QRect bt(QPoint(181+off, 100), pmButton[0].size());
    p.drawPixmap(bt, pmButton[midiControllerValue[scene].at(6 + i*4 + 0) == 0x7f ? 1 : 0]);

    QRect bt1(QPoint(181+off, 162), pmButton[0].size());
    p.drawPixmap(bt1, pmButton[midiControllerValue[scene].at(6 + i*4 + 1) == 0x7f ? 1 : 0]);

    p.save();
    p.translate(QPoint(208+off + 9 + 12, 23 + 6 + 12));
    p.rotate(2.2 * midiControllerValue[scene].at(6 + i*4 + 2) - 90.0 - 45.0);
    QRect kn(QPoint(-12, -12), pmKnob.size());
    p.drawPixmap(kn, pmKnob);
    p.restore();
  }
  
  QColor orange = QColor(193, 119, 52, 255);
  QColor orange2 = QColor(193, 109, 52, 200);
  QColor orange3 = QColor(253, 159, 92, 200);

  QColor green = QColor(152, 251, 152, 255);
  QColor green2 = QColor(102, 201, 102, 200);
  QColor green3 = QColor(152, 251, 152, 200);

  QColor gray = QColor(200, 200, 200, 240);
  QColor black = QColor(0, 0, 0, 255);
  
  switch (NanoKONTROLstate) {
  case None:
    break;
  case MIDIMapping:
    {
    p.setPen (QPen (orange, 2));
    for (int i = 0; i < rects[scene].size(); ++i) {
      if (midiController[scene].at(i) != 0) {
	p.setBrush(orange3);
      } else {
	p.setBrush(orange2);
      }
      p.drawRect(rects[scene].at(i));
    }
    
    if (selectedRect != - 1) {
      p.setBrush(orange3);
      p.setPen (QPen (black, 2));
      p.drawRect(rects[scene].at(selectedRect));
    }

    for (int i = 0; i < rects[scene].size(); ++i) {
      if (midiController[scene].at(i) != 0) {
	QString txt;
        if (midiController[scene].at(i) > 0) {
          txt = QString("%1").arg(midiController[scene].at(i));
        } else if (midiController[scene].at(i) < 0) {
          txt = mio.getMMCName(-midiController[scene].at(i));
          //qDebug() << -midiController[scene].at(i);
	}
	p.setPen (QPen (black, 2));
	p.drawText(rects[scene].at(i),Qt::AlignCenter, txt);
	p.setPen (QPen (orange3, 2));
	p.setBrush(orange3);
        p.drawRect(QRect(rects[scene].at(i).x()+3, rects[scene].at(i).y()+3, 4, 4));
      } else {
	p.setPen (QPen (black, 2));
	p.setBrush(orange2);
	p.drawText(rects[scene].at(i),Qt::AlignCenter, "-");
      }

      if (renoiseKeystrokes[scene].at(i) != "") {
	p.setPen (QPen (green2, 2));
	p.setBrush(green2);
	p.drawRect(QRect(rects[scene].at(i).right()-6, rects[scene].at(i).top()+3, 4, 4));
      }

    }

    QRect help = QRect(18,18,180,70);
    p.setPen (QPen (orange, 2));
    p.setBrush(gray);
    p.drawRect(help);
    p.setPen (QPen (black, 2));
    QFont f("Helvetica", 10);
    p.setFont(f);
    p.drawText(help, Qt::AlignCenter,
	       tr("Select an orange knob, button\n or fader. Then move a\nnanoKONTROL knob, button\nor fader to auto assign."));

    }
    break;
  case ReMapping:
    {
    p.setPen (QPen (green, 2));
    for (int i = 0; i < rects[scene].size(); ++i) {
      if (renoiseKeystrokes[scene].at(i) != "") {
	p.setBrush(green3);
      } else {
	p.setBrush(green2);
      }
      p.drawRect(rects[scene].at(i));
    }
    
    QRect help = QRect(18,18,180,70);
    p.setPen (QPen (green, 2));
    p.setBrush(gray);
    p.drawRect(help);
    p.setPen (QPen (black, 2));

    QFont f("Helvetica", 10);
    p.setFont(f);
    if (selectedRect != - 1) {
      QString txt = renoiseKeystrokes[scene].at(selectedRect);
      txt.replace(QRegExp("-"), "\n");

      p.drawText(help, Qt::AlignCenter, txt);
      p.setBrush(green);
      p.setPen (QPen (black, 2));
      p.drawRect(rects[scene].at(selectedRect));
    } else {
      p.drawText(help, Qt::AlignCenter,
                 tr("Select a green knob, button or\nfader and right-click to assign\n a Renoise action\nfrom the popup menu."));
    }
    p.setBrush(QColor(3,119,52, 200));
    for (int i = 0; i < rects[scene].size(); ++i) {
      if (renoiseKeystrokes[scene].at(i) != "") {
	// QString txt = QString("%1").arg(renoiseKeystrokes.at(i));
	p.setPen (QPen (black, 2));
	p.drawText(rects[scene].at(i),Qt::AlignCenter, "x");
	p.setPen (QPen (green2, 2));
	p.setBrush(green2);
	p.drawRect(QRect(rects[scene].at(i).right()-6, rects[scene].at(i).top()+3, 4, 4));
      } else {
	p.setPen (QPen (black, 2));
	p.drawText(rects[scene].at(i),Qt::AlignCenter, "-");
      }

      p.setPen (QPen (orange2, 2));
      p.setBrush(orange2);
      if (midiController[scene].at(i) != 0) {
	p.drawRect(QRect(rects[scene].at(i).x()+3, rects[scene].at(i).y()+3, 4, 4));
      }
    }

    }
    break;
  }
  
  QPainter p1(this);
  p1.drawPixmap(QRectF(0, 0, width(), height()), background, QRectF(0, 0, width(), height()));
}

void MidiKorgNanoKONTROL::resizeEvent(QResizeEvent *) {
  background = QPixmap(width(), height());
  update();
}

void MidiKorgNanoKONTROL::mouseMoveEvent(QMouseEvent *)
{
}

void MidiKorgNanoKONTROL::mousePressEvent(QMouseEvent *e)
{
  QPoint p = e->pos();

  int found = 0;

  if ((e->button() == Qt::LeftButton && NanoKONTROLstate != None) ||
      (e->button() == Qt::RightButton && NanoKONTROLstate != None)) {
    for (int i = 0; i < rects[scene].size(); ++i) {
      if (rects[scene].at(i).contains(p)) {
	selectedRect = i;
	found = 1;
	break;
      }
    }
    
    if (!found) {
      selectedRect = -1;
    }
  }

  if (e->button() == Qt::LeftButton) {
    update();
  } else if (e->button() == Qt::RightButton) {
    QMenu menu(this);
    if (NanoKONTROLstate == MIDIMapping) {
      menu.addAction(clearMidiMappingAct);
      menu.addSeparator();
      if (selectedRect != -1 && midiController[scene].at(selectedRect) != 0 ) {
	clearMidiMappingAct->setEnabled(true);
      } else {
	clearMidiMappingAct->setEnabled(false);
      }
    } else if (NanoKONTROLstate == ReMapping) {
      menu.addMenu(renoiseMenu);
      menu.addAction(clearReMappingAct);
      menu.addSeparator();

      if (selectedRect != -1 && renoiseKeystrokes[scene].at(selectedRect) != "" ) {
	clearReMappingAct->setEnabled(true);
      } else {
	clearReMappingAct->setEnabled(false);
      }
    }
    menu.addAction(midiMappingAct);
    menu.addAction(reMappingAct);
    menu.addSeparator();
    menu.addMenu(midiInMenu);
    menu.addAction(toggleSkinAct);
    menu.addSeparator();
    menu.addAction(exitAct);
    menu.exec(e->globalPos());
  }
}

void MidiKorgNanoKONTROL::midiRecived(MidiEvent *e)
{
  QString m = QString(e->message);

  const char16_t c0[] = {0xF0, 0x42, 0x40, 0x00, 0x01, 0x04, 0x00, 0x5F, 0x4F};
  const char16_t c1[] = {0xF0, 0x42, 0x40, 0x00, 0x01, 0x04, 0x00, 0x5F, 0x4F};
  const char16_t c2[] = {0xF0, 0x42, 0x4F, 0x00, 0x01, 0x04, 0x00, 0x5F, 0x4F};
  QString s0 = QString("%1").arg(c0);
  QString s1 = QString("%1").arg(c1);
  QString s2 = QString("%1").arg(c2);

  if (m.startsWith(s0) || m.startsWith(s1) || m.startsWith(s2)) {
    scene = (int)(m.at(9).toLatin1());
    update();
    return;
  }

  MidiIO::ControlType lastMovedControllerType = MidiIO::None;
  int lastMovedControllerNr = -1;
  int lastMMCCmd = 0;

  if (m.size() == 3) {
    lastMovedControllerType = MidiIO::MidiCC;
    lastMovedControllerNr = m.at(1).toLatin1();
  } else if (m.size() == 6) {
    if (m.at(0) == 0xF0 && m.at(1) == 0x7F && m.at(3) == 0x06
        && m.at(5) == 0xF7) {
      lastMovedControllerType = MidiIO::MidiMMC;
      lastMMCCmd = m.at(4).toLatin1();
    }
  } else {
    lastMovedControllerType = MidiIO::None;
  }

  if (NanoKONTROLstate == MIDIMapping && selectedRect != -1) {

    for (int i = 0; i < midiController[scene].size(); ++i) {
      if (midiController[scene].at(i) == lastMovedControllerNr) {
	midiController[scene].replace(i, 0);
      }
    }

    if (lastMovedControllerType == MidiIO::MidiCC) {
      midiController[scene].replace(selectedRect,lastMovedControllerNr);
    } else if (lastMovedControllerType == MidiIO::MidiMMC) {
      midiController[scene].replace(selectedRect, -lastMMCCmd);

      //qDebug() << -lastMMCCmd;
    }
    update();
  }

  int controller = -1;
  for (int i = 0; i < midiController[scene].size(); ++i) {
    if (midiController[scene].at(i) == lastMovedControllerNr) {
      controller = i;
      break;
    }
  }

  if (controller != -1 && m.size() == 3 &&
      midiControllerValue[scene].at(controller) != m.at(2).toLatin1()) {
    midiControllerValue[scene].replace(controller, m.at(2).toLatin1());
    update();
  }

  if (controller != -1 && m.size() == 3 && m.at(2) == 0x7F) {
    //qDebug() << renoiseKeystrokes[scene].at(controller);
    x11do->sendKey(renoiseToKeystrokes.value(renoiseKeystrokes[scene].at(controller)), true);
  } else if (controller != -1 && m.size() == 3 && m.at(2) == 0x00) {
    //qDebug() << renoiseKeystrokes[scene].at(controller);
    x11do->sendKey(renoiseToKeystrokes.value(renoiseKeystrokes[scene].at(controller)), false);
  }

  if (lastMovedControllerType == MidiIO::MidiMMC) {
    int co = -1;
    for (int i = 0; i < midiController[scene].size(); ++i) {
      if (midiController[scene].at(i) == -lastMMCCmd) {
	co = i;
	break;
      }
    }

    if (co != -1 && co < midiController[scene].size()) {
      QString key = renoiseToKeystrokes.value(renoiseKeystrokes[scene].at(co));
      x11do->sendKey(key, true);
      x11do->sendKey(key, false);
    }
  }

  if (controller == -1) {
      //TODO fix this under windows
      // mio.send(m); // send non key-mapped midi commands to the filtered midi out port
  }
}
