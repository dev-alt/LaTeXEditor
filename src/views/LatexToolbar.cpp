#include "LatexToolbar.h"

LatexToolbar::LatexToolbar(QWidget *parent) : QToolBar(parent) {
    createTextFormatActions();
    createMathActions();
    createStructureActions();
    createEnvironmentActions();
    createBibTeXActions();
}

void LatexToolbar::createTextFormatActions() {
    addAction(createAction("Bold", "\\textbf{}", "Make text bold"));
    addAction(createAction("Italic", "\\textit{}", "Make text italic"));
    addAction(createAction("Underline", "\\underline{}", "Underline text"));
    addAction(createAction("Emphasize", "\\emph{}", "Emphasize text"));
    addAction(createAction("Small Caps", "\\textsc{}", "Small Caps text"));
    addAction(createAction("Typewriter", "\\texttt{}", "Typewriter text"));
    addSeparator();
}

void LatexToolbar::createMathActions() {
    QMenu *mathMenu = createMenu("Math");
    mathMenu->addAction(createAction("Inline Math", "$", "Insert inline math"));
    mathMenu->addAction(createAction("Display Math", "$$", "Insert display math"));
    mathMenu->addAction(
            createAction("Equation", "\\begin{equation}\n\n\\end{equation}", "Insert equation environment"));
    mathMenu->addAction(createAction("Align", "\\begin{align}\n\n\\end{align}", "Insert align environment"));
    mathMenu->addAction(createAction("Fraction", "\\frac{}{}", "Insert fraction"));
    mathMenu->addAction(createAction("Square Root", "\\sqrt{}", "Insert square root"));
    addAction(mathMenu->menuAction());
    addSeparator();
}

void LatexToolbar::createStructureActions() {
    QMenu *sectionMenu = createMenu("Sections");
    sectionMenu->addAction(createAction("Chapter", "\\chapter{}", "Insert chapter"));
    sectionMenu->addAction(createAction("Section", "\\section{}", "Insert section"));
    sectionMenu->addAction(createAction("Subsection", "\\subsection{}", "Insert subsection"));
    sectionMenu->addAction(createAction("Subsubsection", "\\subsubsection{}", "Insert subsubsection"));
    addAction(sectionMenu->menuAction());
    addSeparator();
}

void LatexToolbar::createEnvironmentActions() {
    QMenu *envMenu = createMenu("Environments");
    envMenu->addAction(
            createAction("Itemize", "\\begin{itemize}\n\\item \n\\end{itemize}", "Insert itemize environment"));
    envMenu->addAction(
            createAction("Enumerate", "\\begin{enumerate}\n\\item \n\\end{enumerate}", "Insert enumerate environment"));
    envMenu->addAction(createAction("Description", "\\begin{description}\n\\item[] \n\\end{description}",
                                    "Insert description environment"));
    envMenu->addAction(createAction("Table",
                                    "\\begin{table}\n\\centering\n\\begin{tabular}{}\n\n\\end{tabular}\n\\caption{}\n\\label{tab:}\n\\end{table}",
                                    "Insert table environment"));
    envMenu->addAction(createAction("Figure",
                                    "\\begin{figure}\n\\centering\n\\includegraphics[width=\\textwidth]{}\n\\caption{}\n\\label{fig:}\n\\end{figure}",
                                    "Insert figure environment"));
    addAction(envMenu->menuAction());
}

QAction *LatexToolbar::createAction(const QString &text, const QString &command, const QString &tooltip) {
    QAction *action = new QAction(text, this);
    action->setData(command);
    if (!tooltip.isEmpty()) {
        action->setToolTip(tooltip);
    }
    connect(action, &QAction::triggered, [this, action]() {
        emit commandInsertRequested(action->data().toString());
    });
    return action;
}

void LatexToolbar::createBibTeXActions() {
    QMenu *bibMenu = createMenu("BibTeX");
    bibMenu->addAction(createAction("Article",
                                    "@article{key,\n  author = {},\n  title = {},\n  journal = {},\n  year = {},\n}",
                                    "Insert article citation"));
    bibMenu->addAction(createAction("Book",
                                    "@book{key,\n  author = {},\n  title = {},\n  publisher = {},\n  year = {},\n}",
                                    "Insert book citation"));
    bibMenu->addAction(createAction("InProceedings",
                                    "@inproceedings{key,\n  author = {},\n  title = {},\n  booktitle = {},\n  year = {},\n}",
                                    "Insert conference paper citation"));
    bibMenu->addAction(createAction("Misc",
                                    "@misc{key,\n  author = {},\n  title = {},\n  howpublished = {},\n  year = {},\n}",
                                    "Insert miscellaneous citation"));
    bibMenu->addAction(createAction("Cite",
                                    "\\cite{}",
                                    "Insert citation reference"));
    bibMenu->addAction(createAction("Bibliography",
                                    "\\bibliography{references}\n\\bibliographystyle{plain}",
                                    "Insert bibliography"));
    addAction(bibMenu->menuAction());
}

QMenu *LatexToolbar::createMenu(const QString &title) {
    QMenu *menu = new QMenu(title, this);
    return menu;
}