/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>

#include "kanjibrowser.h"
#include <KAboutData>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>

static const char version[] = "1.0";

int main(int argc, char **argv)
{
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kitenkanjibrowser"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kitenkanjibrowserrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kanjibrowserui.rc"));
    migrate.migrate();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kiten"), app.windowIcon()));
    KLocalizedString::setApplicationDomain("kiten");

    KAboutData about(QStringLiteral("kitenkanjibrowser"),
                     i18n("Kanji Browser"),
                     version,
                     i18n("Kiten's Kanji Browser, a KDE Japanese Reference Tool"),
                     KAboutLicense::GPL_V2,
                     i18n("(C) 2011 Daniel E. Moctezuma"),
                     QString(),
                     QStringLiteral("https://edu.kde.org/kiten"),
                     QStringLiteral("democtezuma@gmail.com"));
    about.addAuthor(i18n("Daniel E. Moctezuma"), QString(), QStringLiteral("democtezuma@gmail.com"));
    about.setOrganizationDomain("kde.org");

    KAboutData::setApplicationData(about);

#ifdef Q_OS_WIN
    QApplication::setStyle(QStringLiteral("breeze"));
#endif

    if (app.isSessionRestored()) {
        kRestoreMainWindows<KanjiBrowser>();
    } else {
        auto kanjiBrowser = new KanjiBrowser();
        kanjiBrowser->show();
    }

    return app.exec();
}
