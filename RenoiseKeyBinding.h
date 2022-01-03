#ifndef RENOISE_KEYBINDING_H
#define RENOISE_KEYBINDING_H

#include <qdom.h>
#include <QString>

class RenoiseKeyBinding
{

public:
    QString topic, binding, key;

    RenoiseKeyBinding( QString iTopic = "", QString iBinding = "", QString iKey = "" );
    RenoiseKeyBinding( const QDomElement &e );

    QString getID() const;
};

#endif
