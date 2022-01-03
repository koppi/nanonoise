#include <QDebug>

#include "RenoiseCategory.h"

RenoiseCategory::RenoiseCategory( QString iIdentifier = "")
{
  m_identifier = iIdentifier;
}

RenoiseCategory::RenoiseCategory( const QDomElement &e )
{
  QDomNodeList identifiers = e.elementsByTagName("Identifier");
  for (int c = 0; c < int(identifiers.length()); ++c) {
    QDomNode cNode = identifiers.item(c).firstChild();
    QDomText e = cNode.toText();
    m_identifier = e.data();
  }

  QDomNodeList keybindings = e.elementsByTagName("KeyBinding");
  for (int i = 0; i < keybindings.size(); ++i) {
    RenoiseKeyBinding b(keybindings.item(i).toElement());
    if (!b.key.isEmpty()) {// filter renoise actions without hotkeys
      QList<RenoiseKeyBinding> l = bindings.value(b.topic, QList<RenoiseKeyBinding>());
      l.append(b);
      bindings[b.topic] = l;
    }
  }
}
