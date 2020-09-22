#pragma once
#ifndef NETCOOP_H_
#define NETCOOP_H_

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QString>
#include <stdint.h> // uint16_t
#include <line.h>

class NetCoop: public QObject
{
    Q_OBJECT

signals:
    dataRead(Line const& line);

public:
    NetCoop(QObject* parent = 0);
    virtual ~NetCoop();

    void reset();

    void dataWrite(Line const& line);

    bool hostGame(uint16_t port);
    bool joinGame(QString const& addr, uint16_t port);

protected slots:
    void readyRead();

protected:
    QUdpSocket _socket;

    QHostAddress _serverAddr;
    uint16_t _serverPort;

    QHostAddress _clientAddr;
    uint16_t _clientPort;

    bool _isServer;
    bool _isEstablished;

};

#endif
