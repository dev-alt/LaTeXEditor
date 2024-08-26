#include "ThemeManager.h"
#include <QApplication>

ThemeManager& ThemeManager::getInstance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager() : m_currentThemeIndex(0) {
    initializeDefaultThemes();
}

void ThemeManager::initializeDefaultThemes() {
    // Light theme
    addTheme(Theme("Light",
                   QColor(240, 240, 240), // windowColor
                   Qt::black,             // textColor
                   Qt::white,             // baseColor
                   QColor(245, 245, 245), // alternateBaseColor
                   QColor(76, 163, 224),  // highlightColor
                   Qt::white,             // highlightedTextColor
                   QColor(0, 0, 255),     // commandColor
                   QColor(128, 0, 128),   // environmentColor
                   QColor(0, 128, 0),     // bracketColor
                   QColor(128, 128, 128)  // commentColor
    ));

    // Dark theme
    addTheme(Theme("Dark",
                   QColor(53, 53, 53),    // windowColor
                   Qt::white,             // textColor
                   QColor(25, 25, 25),    // baseColor
                   QColor(40, 40, 40),    // alternateBaseColor
                   QColor(42, 130, 218),  // highlightColor
                   Qt::black,             // highlightedTextColor
                   QColor(102, 217, 239), // commandColor
                   QColor(249, 38, 114),  // environmentColor
                   QColor(166, 226, 46),  // bracketColor
                   QColor(117, 113, 94)   // commentColor
    ));
}

void ThemeManager::applyTheme(const QString &themeName) {
    for (int i = 0; i < m_themes.size(); ++i) {
        if (m_themes[i].name == themeName) {
            m_currentThemeIndex = i;
            QPalette palette = createPaletteFromTheme(m_themes[i]);
            qApp->setPalette(palette);
            emit themeChanged(m_themes[i]);
            return;
        }
    }
}

void ThemeManager::addTheme(const Theme &theme) {
    m_themes.append(theme);
}

QStringList ThemeManager::getThemeNames() const {
    QStringList names;
    for (const auto &theme : m_themes) {
        names << theme.name;
    }
    return names;
}

const Theme& ThemeManager::getCurrentTheme() const {
    return m_themes[m_currentThemeIndex];
}

QPalette ThemeManager::createPaletteFromTheme(const Theme &theme) {
    QPalette palette;
    palette.setColor(QPalette::Window, theme.windowColor);
    palette.setColor(QPalette::WindowText, theme.textColor);
    palette.setColor(QPalette::Base, theme.baseColor);
    palette.setColor(QPalette::AlternateBase, theme.alternateBaseColor);
    palette.setColor(QPalette::ToolTipBase, theme.baseColor);
    palette.setColor(QPalette::ToolTipText, theme.textColor);
    palette.setColor(QPalette::Text, theme.textColor);
    palette.setColor(QPalette::Button, theme.windowColor);
    palette.setColor(QPalette::ButtonText, theme.textColor);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, theme.highlightColor);
    palette.setColor(QPalette::Highlight, theme.highlightColor);
    palette.setColor(QPalette::HighlightedText, theme.highlightedTextColor);
    return palette;
}