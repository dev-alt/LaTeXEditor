#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QColor>

class Theme {
public:
    Theme(const QString &name,
          const QColor &windowColor,
          const QColor &textColor,
          const QColor &baseColor,
          const QColor &alternateBaseColor,
          const QColor &highlightColor,
          const QColor &highlightedTextColor,
          const QColor &commandColor,
          const QColor &environmentColor,
          const QColor &bracketColor,
          const QColor &commentColor);

    QString name;
    QColor windowColor;
    QColor textColor;
    QColor baseColor;
    QColor alternateBaseColor;
    QColor highlightColor;
    QColor highlightedTextColor;
    QColor commandColor;
    QColor environmentColor;
    QColor bracketColor;
    QColor commentColor;
};

#endif // THEME_H