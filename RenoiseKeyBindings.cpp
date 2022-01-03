#include <QDebug>
#include <QDir>

#include "RenoiseKeyBindings.h"

RenoiseKeyBindings::RenoiseKeyBindings()
{
  QString filename;

  QDir d = QDir(QDir::homePath());
  QStringList sl;
  if (d.cd(".renoise")) {
    d.setFilter(QDir::AllDirs);
    d.setSorting(QDir::Time);

    QFileInfoList l = d.entryInfoList();
    for (int i = 0; i < l.size(); ++i) {
      if (l.at(i).fileName() != ".." && l.at(i).fileName() != ".")
        sl.append(l.at(i).fileName());
    }

    // qDebug() << sl;
  }

  if (!sl.isEmpty()) {
    d.cd(sl.at(0));
    filename = d.absoluteFilePath("KeyBindings.xml");

    if (sl.size() > 1) {
      qDebug() << "Loading most recent key bindings file\n   "  << filename;
    }
  } else {
    filename = ":re-191.xml";
  }

  QFile file( filename );
  QDomDocument doc( "KeyboardBindings" );
  if ( !file.open(QIODevice::ReadOnly) ) {
    qDebug() << "unable to open file for reading: " << filename;
  }
  if( !doc.setContent( &file ) ) {
    qDebug() << "unable to open file for reading: " << filename;
  }
  file.close();
  
  QDomNodeList cat = doc.documentElement().elementsByTagName("Category");
  for (int c = 0; c < int(cat.length()); ++c) {
    QDomElement e = cat.item(c).toElement();
    RenoiseCategory rc(e);
    categories.append(rc);
  }
}
