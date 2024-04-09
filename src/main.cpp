// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; If not, see <https://www.gnu.org/licenses/>.
//

#include "./include/qmlwindow.h"
#include "./include/config_utility.h"
#include "./include/log_utility.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQuick>
#include <QQmlContext>
#include <QQmlFileSelector>
#include <QWindow>
#include "../deps/singleapplication/singleapplication.h"

int main(int argc, char *argv[])
{
    // install log handler
    qInstallMessageHandler(logMessageHandler);

    // enables the virtual keyboard by setting the QT_IM_MODULE environment variable before loading the .qml file
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    SingleApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // add custom virtual keyboard styles import path
    engine.addImportPath("qrc:/kbstyles");
    // assign custom virtual keyboard style
    qputenv("QT_VIRTUALKEYBOARD_STYLE", QByteArray(APP_THEME_GREEN));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", "qrc:/kbstyles/QtQuick/VirtualKeyboard/layouts");
#endif
    engine.addImportPath("qrc:/qml/imports");

    QStringList selectors;
    QQmlFileSelector fs(&engine);
    // read theme config
    if (ConfigUtility::get_ui_theme().compare(APP_THEME_ORANGE) == 0) {
        selectors << APP_THEME_ORANGE;
        qputenv("QT_VIRTUALKEYBOARD_STYLE", QByteArray(APP_THEME_ORANGE));
    }
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    // qt6 qml is not compatible with qt5
    // if qt version is 6.0.0 above replace with qt6 qml file path
    selectors << "qt6";
    // qt5 qt quick controls style name is Default (defined in qtquickcontrols2.conf)
    // qt6 qt quick controls style name is Basic
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
#endif
    // set path for QQmlFileSelector
    fs.setExtraSelectors(selectors);
    engine.load(QUrl(QStringLiteral("qrc:/qml/content/App.qml")));
    QObject *rootObject = engine.rootObjects().value(0);

    // init window object handler and value
    QMLWindow qmlwin(&app);
    qmlwin.initWindow(rootObject);

    QWindow *qml_window = qobject_cast<QWindow *>(rootObject);
    // second instance event
    QObject::connect( &app, &SingleApplication::instanceStarted, [ qml_window ]() {
        // bring instance to foreground on Wayland
        // NOTE: this is not working
        //qml_window->raise();
        // NOTE: this is not working now. And it would print warning log.
        // Warning: Wayland does not support QWindow::requestActivate()
        //qml_window->requestActivate();
        // NOTE: this is workaround
        qml_window->close();
        qml_window->showFullScreen();
    });
    // close window event
    QObject::connect( &qmlwin, SIGNAL(closeWindow()), &app, SLOT(quit()));

    return app.exec();
}
