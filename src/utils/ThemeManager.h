#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QVector>
#include "../models/Theme.h"

class ThemeManager : public QObject {
Q_OBJECT

public:
    static ThemeManager& getInstance();

    const Theme& getCurrentTheme() const;
    void applyTheme(const QString &themeName);
    QStringList getThemeNames() const;

signals:
    void themeChanged(const Theme &newTheme);

private:
    ThemeManager();
    void initializeDefaultThemes();
    void addTheme(const Theme &theme);

    QVector<Theme> m_themes;
    int m_currentThemeIndex;
};

#endif // THEMEMANAGER_H