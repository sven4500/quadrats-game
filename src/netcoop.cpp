#include <QDateTime>
#include <QThread> // sleep
#include <netcoop.h>

NetCoop::NetCoop(QObject* parent):
    QObject(parent), _socket(this), _serverPort(0), _clientPort(0),
    _isEstablished(false)
{
    connect(&_socket, &QUdpSocket::readyRead, this, &NetCoop::readyRead);
}

NetCoop::~NetCoop()
{}

void NetCoop::readyRead()
{
    if(_isServer == true && _isEstablished == false)
    {
        uint32_t dataRead = -1,
            dataSend = 0;

        _socket.readDatagram((char*)&dataRead, sizeof(uint32_t), &_clientAddr, &_clientPort);

        dataSend = dataRead + 1;

        _socket.writeDatagram((char*)&dataSend, sizeof(uint32_t), _clientAddr, _clientPort);

        _isEstablished = true;
    }

    // TODO: здесь логика приёма данных.
}

bool NetCoop::hostGame(uint16_t port)
{
    _isServer = true;

    _serverAddr = QHostAddress::AnyIPv4;
    _serverPort = port;

    return _socket.bind(QHostAddress::AnyIPv4, port);
}

bool NetCoop::joinGame(QString const& addr, uint16_t port)
{
    _isServer = false;

    _serverAddr = addr;
    _serverPort = port;

    _clientAddr = QHostAddress::AnyIPv4;
    _clientPort = 0;

    _socket.bind(_clientAddr, _clientPort, QAbstractSocket::DefaultForPlatform);

    uint32_t dataSend = QDateTime::currentDateTime().toTime_t(),
        dataRead = -1,
        dataExpt = dataSend + 1;

    // Предполагаем что хост настроен и ждёт подключение. Также на время
    // блокируем сигнал о поступлении данных чтобы не сработал слот.
    _socket.blockSignals(true);
    _socket.writeDatagram((char*)&dataSend, sizeof(uint32_t), _serverAddr, _serverPort);

    // Спим одну секунду чтобы дать серверу возможность ответить.
    QThread::sleep(1);

    _socket.readDatagram((char*)&dataRead, sizeof(uint32_t));
    _socket.blockSignals(false);

    return dataRead == dataExpt;
}
