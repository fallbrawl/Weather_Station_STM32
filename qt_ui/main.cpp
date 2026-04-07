#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "weatherclient.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    WeatherClient weatherData;
    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("weatherBackend", &weatherData);

    const auto url(QStringLiteral("qrc:/qt/qml/main.qml"));
    engine.load(url);

    return app.exec();
}