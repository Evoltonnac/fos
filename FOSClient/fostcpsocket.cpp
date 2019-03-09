#include "fostcpsocket.h"
#include <QNetworkInterface>
#include <QTime>
#include <QDate>
#include <QMessageBox>
QHostAddress FOSTcpSocket::s_hostAddress = QHostAddress("223.3.77.166");
int FOSTcpSocket::s_hostPort = 1234;

FOSTcpSocket::FOSTcpSocket(QObject *parent)
    :QTcpSocket(parent)
{
    m_flag = 0;
    m_isConnected = false;
    linkSignalWithSlot();
}

void FOSTcpSocket::requestConnect()
{
    if (!m_isConnected)
    {
        abort();
        connectToHost(FOSTcpSocket::s_hostAddress, FOSTcpSocket::s_hostPort);
    }
}

bool FOSTcpSocket::isConnected()
{
    return m_isConnected;
}


// 设置标识
void FOSTcpSocket::setFlag(int flag)
{
    m_flag = flag;
}

// 获取标识
int FOSTcpSocket::flag()
{
    return m_flag;
}

/*************************************************
Function Name： getIP()
Description: 获取ip地址
*************************************************/
QString FOSTcpSocket::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() == QAbstractSocket::IPv4Protocol) //我们使用IPv4地址
        {
           if(address.toString()!="127.0.0.1")
                return address.toString();
        }
    }
    return 0;
}

/*************************************************
Function Name： getCurrentDateTime()
Description: 获取当前的日期和时间
*************************************************/
QString FOSTcpSocket::getCurrentDateTime()
{
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    return QString("%1  %2").arg(date.toString(Qt::ISODate))
        .arg(time.toString(Qt::ISODate));
}


// mark: private slots---------------------------------------------------------
/*************************************************
Function Name： connectionClosedByServer()
Description: 连接被客户端关闭
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void FOSTcpSocket::connectionClosed()
{
    m_isConnected = false;
    qDebug("连接断开");
}

/*************************************************
Function Name： connectionCreate()
Description: 连接创建
Input： NULL
Output： NULL
Changes： NULL
*************************************************/
void FOSTcpSocket::connectionCreate()
{
    m_isConnected = true;
}

void FOSTcpSocket::dispalyError(QAbstractSocket::SocketError socketError)
{
    m_isConnected = false;
    switch (socketError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        emit showConnectionStatus(tr("链接失败.可能是因为服务器关闭."));
        break;
    case QAbstractSocket::HostNotFoundError:
        emit showConnectionStatus(tr("链接失败.可能是因为找不到服务器"));
        QMessageBox::information(NULL, tr("FOS Client"),
                                 tr("This host was not found.Please check the"
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit showConnectionStatus(tr("链接失败.可能是因为连接被拒绝"));
        QMessageBox::information(NULL, tr("FOS Client"),
                                 tr("The connection was refused by the peer."
                                    "Make sure the FOS server is running,"
                                    "and check that the host name and port"
                                    "settings are correct."));
        break;
    default:
        emit showConnectionStatus(tr("链接失败: %1.").arg(errorString()));
        QMessageBox::information(NULL, tr("FOS Client"),
                                 tr("The following errot occurred: %1.")
                                 .arg(errorString()));
    }
}


void FOSTcpSocket::linkSignalWithSlot()
{
    connect(this, SIGNAL(connected()), this, SLOT(connectionCreate()));
    connect(this, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(dispalyError(QAbstractSocket::SocketError)));
}
