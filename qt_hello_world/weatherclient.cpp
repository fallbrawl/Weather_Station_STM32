#include "weatherclient.h"
#include <QJsonDocument>
#include <QJsonObject>

WeatherClient::WeatherClient(QObject *parent) : QObject(parent) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 1234);
    connect(udpSocket, &QUdpSocket::readyRead, this, &WeatherClient::processPendingDatagrams);
}

void WeatherClient::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        auto doc = QJsonDocument::fromJson(datagram);
        if (!doc.isNull() && doc.isObject()) {
            auto obj = doc.object();
            m_temperature = obj["temp"].toDouble();
            m_humidity = obj["hum"].toDouble();
            emit dataChanged();
        }
    }
}