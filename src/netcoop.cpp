#include <QDateTime>
#include <QThread> // sleep
#include <netcoop.h>

NetCoop::NetCoop(QObject* parent):
    QObject(parent), _socket(this), _serverAddr(QHostAddress::Null),
    _serverPort(0), _clientAddr(QHostAddress::Null), _clientPort(0),
    _isEstablished(false)
{
    connect(&_socket, &QUdpSocket::readyRead, this, &NetCoop::readyRead);
}

NetCoop::~NetCoop()
{}

void NetCoop::reset()
{
    _socket.close();

    _serverAddr = QHostAddress::Null;
    _serverPort = 0;

    _clientAddr = QHostAddress::Null;
    _clientPort = 0;

    _isEstablished = false;
}

void NetCoop::dataWrite(Line const& line)
{
    if(_isServer)
    {
        _socket.writeDatagram((char*)&line, sizeof(Line), _clientAddr, _clientPort);
    }
    else
    {
        _socket.writeDatagram((char*)&line, sizeof(Line), _serverAddr, _serverPort);
    }
}

void NetCoop::readyRead()
{
    if(_isServer == true && _isEstablished == false)
    {
        uint32_t dataRead = -1,
            dataSend = 0;

        // До установления соединения IP адрес клиента не известен поэтому на
        // первом пакете сохраняем адрес и номер порта клиента.
        _socket.readDatagram((char*)&dataRead, sizeof(uint32_t), &_clientAddr, &_clientPort);

        dataSend = dataRead + 1;

        _socket.writeDatagram((char*)&dataSend, sizeof(uint32_t), _clientAddr, _clientPort);

        _isEstablished = true;
    }
    else
    {
        Line line;
        _socket.readDatagram((char*)&line, sizeof(Line));

        emit dataRead(line);
    }
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

    _isEstablished = dataRead == dataExpt;
    return _isEstablished;
}
