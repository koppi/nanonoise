#ifndef RENOISE_KEYBINDINGS_H
#define RENOISE_KEYBINDINGS_H

#include <QList>

#include "RenoiseCategory.h"

class RenoiseKeyBindings
{
 public:
  RenoiseKeyBindings();

  QList<RenoiseCategory> categories;
};

#endif
