#ifndef RENOISE_CATEGORY_H
#define RENOISE_CATEGORY_H

#include <qdom.h>
#include <QString>
#include <QList>
#include <QHash>

#include "RenoiseKeyBinding.h"

class RenoiseCategory
{
public:

  QString m_identifier;
  QHash<QString, QList<RenoiseKeyBinding> > bindings;

  RenoiseCategory( QString identifier );
  RenoiseCategory( const QDomElement &e );
};

#endif
