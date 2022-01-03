#ifndef MIDIKORGNANOKONTROL_H
#define MIDIKORGNANOKONTROL_H

#include "MidiIO.h"
#include "MidiEvent.h"
#include "X11DoTool.h"

#include "RenoiseKeyBindings.h"

#include <QtGui>
#include <QMainWindow>
#include <QHash>

class MidiKorgNanoKONTROL : public QMainWindow {
  Q_OBJECT
    
 public:

  MidiKorgNanoKONTROL(QWidget *parent = 0);
  ~MidiKorgNanoKONTROL();
  
 protected:

  // gui
  void loadSkin();

  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

  void contextMenuEvent(QContextMenuEvent *);

  void initRenoiseMenu();

  // control/data
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void selectMIDIInputPort(QString name);

  void loadPrefs();
  void savePrefs();

 private slots:
  void selectMIDIInputPort();
  void toggleMIDIMapping();
  void clearMIDIMapping();
  void clearReMapping();
  void toggleReMapping();
  void toggleSkin();
  void midiRecived(MidiEvent *);
  void setReKeystroke();

 private:

  // gui
  QPixmap background;

  enum Skin {Black=0, White=1};
  Skin skin;
  QPixmap pm[3];
  QPixmap pmSlider;
  QPixmap pmKnob;
  QPixmap pmButton[2];

  // control
  enum NanoKONTROLState {None=0, MIDIMapping=1, ReMapping=2};
  NanoKONTROLState NanoKONTROLstate;
  QString midiInPortName;
  int scene;

  MidiIO mio;

  QVector<QRect> rects[4];
  QVector<int> midiController[4];
  QVector<int> midiControllerValue[4];
  int selectedRect;

  QMenu *midiInMenu;
  QAction *midiInputPortAct;
  QList<QAction *> midiInputPortActions;

  QAction *midiMappingAct;
  QAction *reMappingAct;
  QAction *clearMidiMappingAct;
  QAction *clearReMappingAct;
  QAction *toggleSkinAct;
  QAction *exitAct;
  QAction *shortcutsAct;

  QMenu *renoiseMenu;
  QList<QAction *> renoiseActions;
  QVector<QString> renoiseKeystrokes[4];

  QHash<QString, QString> renoiseToKeystrokes;

  X11DoTool *x11do;

  RenoiseKeyBindings *reKeyBindings;
};

#endif
