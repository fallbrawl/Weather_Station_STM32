#ifndef WEATHERCLIENT_H
#define WEATHERCLIENT_H

#include <QObject>
#include <QUdpSocket>

class WeatherClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(float temperature READ temperature NOTIFY dataChanged)
    Q_PROPERTY(float humidity READ humidity NOTIFY dataChanged)

public:
    explicit WeatherClient(QObject *parent = nullptr);
    float temperature() const { return m_temperature; }
    float humidity() const { return m_humidity; }

signals:
    void dataChanged();

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    float m_temperature = 0.0;
    float m_humidity = 0.0;
};

#endif