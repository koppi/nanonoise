#include <QDebug>
#include <QRegExp>

#include "RenoiseKeyBinding.h"

RenoiseKeyBinding::RenoiseKeyBinding( QString iTopic, QString iBinding, QString iKey )
{
  topic = iTopic;
  binding = iBinding;
  key = iKey;
}

RenoiseKeyBinding::RenoiseKeyBinding( const QDomElement &e )
{
  topic = e.elementsByTagName("Topic").item(0).firstChild().toText().data();
  binding = e.elementsByTagName("Binding").item(0).firstChild().toText().data();
  key = e.elementsByTagName("Key").item(0).firstChild().toText().data();

  key.replace(QRegExp(" "), "");
  key.replace(QRegExp("Alt"), "Alt");
  key.replace(QRegExp("Control"), "Ctrl");
  // key.replace(QRegExp(" Control "), "Ctrl");

  //  qDebug() << "Topic =" << topic << "Binding =" << binding << "Key =" << key << endl;

  // qDebug() << key;
}

QString RenoiseKeyBinding::getID() const
{
  return topic + "-" + binding;
}
