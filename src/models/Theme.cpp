#include "Theme.h"

Theme::Theme(const QString &name,
             const QColor &windowColor,
             const QColor &textColor,
             const QColor &baseColor,
             const QColor &alternateBaseColor,
             const QColor &highlightColor,
             const QColor &highlightedTextColor,
             const QColor &commandColor,
             const QColor &environmentColor,
             const QColor &bracketColor,
             const QColor &commentColor)
        : name(name),
          windowColor(windowColor),
          textColor(textColor),
          baseColor(baseColor),
          alternateBaseColor(alternateBaseColor),
          highlightColor(highlightColor),
          highlightedTextColor(highlightedTextColor),
          commandColor(commandColor),
          environmentColor(environmentColor),
          bracketColor(bracketColor),
          commentColor(commentColor)
{}