#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QVector>
#include "../models/Theme.h"

class ThemeManager : public QObject {
Q_OBJECT

public:
    static ThemeManager& getInstance();

    void applyTheme(const QString &themeName);
    void addTheme(const Theme &theme);
    QStringList getThemeNames() const;
    const Theme& getCurrentTheme() const;

signals:
    void themeChanged(const Theme &newTheme);

private:
    ThemeManager();
    QVector<Theme> m_themes;
    int m_currentThemeIndex;

    void initializeDefaultThemes();
};

#endif // THEMEMANAGER_H