// LaTeXToHtmlConverter.h
#ifndef LATEXTOHTMLCONVERTER_H
#define LATEXTOHTMLCONVERTER_H

#include <QString>
#include <QMap>
#include <QRegularExpression>

class LaTeXToHtmlConverter {
public:
    LaTeXToHtmlConverter();

    // Main conversion method
    QString convertToHtml(const QString &latexContent, bool useCdn = false);

private:
    // Individual conversion methods
    QString processDocumentStructure(const QString &content);
    QString processSections(const QString &content);
    QString processTextFormatting(const QString &content);
    QString processLists(const QString &content);
    QString processTables(const QString &content);
    QString processEnvironments(const QString &content);
    QString processSpecialCharacters(const QString &content);
    QString processPackageCommands(const QString &content);
    QString processReferences(const QString &content);
    QString processMathEnvironments(const QString &content);
    QString processVerbatim(const QString &content);

    // HTML generation
    QString generateHtmlDocument(const QString &body, bool useCdn);
    QString getMathJaxConfig(bool useCdn);
    QString getStyles();

    // Helper methods
    QString escapeHtml(const QString &text);
    QString replaceCommand(const QString &content, const QString &command, const QString &htmlTag);

    // Conversion maps
    QMap<QString, QString> m_symbolMap;
    QMap<QString, QString> m_environmentMap;

    void initializeSymbolMap();
    void initializeEnvironmentMap();
};

#endif // LATEXTOHTMLCONVERTER_H
