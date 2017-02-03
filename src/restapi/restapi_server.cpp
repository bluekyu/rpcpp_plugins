#include "restapi/restapi_server.hpp"

#include <boost/log/trivial.hpp>

#include <QtCore/QCoreApplication>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "restapi/resolve_message.hpp"

namespace restapi {

RestAPIServer* RestAPIServer::instance_;
std::unique_ptr<std::thread> RestAPIServer::network_thread_;

void RestAPIServer::run(void)
{
    network_thread_ = std::make_unique<std::thread>([]() {
        BOOST_LOG_TRIVIAL(info) << "Starting WebSocket server thread ...";

        int argc = 0;
        char* argv[] ={ nullptr };
        QCoreApplication app(argc, argv);

        RestAPIServer server(8888);
        QObject::connect(instance_ = &server, &RestAPIServer::closed, &app, &QCoreApplication::quit);

        app.exec();

        instance_ = nullptr;

        BOOST_LOG_TRIVIAL(info) << "WebSocket server thread is done.";
    });
}

void RestAPIServer::close(void)
{
    if (instance_)
        instance_->socket_server_->close();
    network_thread_->join();
    network_thread_.reset();
}

RestAPIServer::~RestAPIServer(void)
{
    socket_server_->close();
    qDeleteAll(clients_.begin(), clients_.end());
}

void RestAPIServer::broadcast(const std::string& json_msg)
{
    for (auto& client: clients_)
        client->sendTextMessage(QString::fromStdString(json_msg));
}

void RestAPIServer::broadcast(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    broadcast(std::string(buffer.GetString(), buffer.GetSize()));
}

void RestAPIServer::on_new_connection(void)
{
    QWebSocket* socket = socket_server_->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &RestAPIServer::process_message);
    connect(socket, &QWebSocket::disconnected, this, &RestAPIServer::socket_disconnected);

    clients_ << socket;

    BOOST_LOG_TRIVIAL(info) << "Client is connected from '" << socket->peerName().toStdString() << "'";
    BOOST_LOG_TRIVIAL(debug) << "Peer address (" << socket->peerAddress().toString().toStdString() << "), port (" << socket->peerPort() << ")";
}

void RestAPIServer::process_message(QString message)
{
    resolve_message(message.toStdString());
}

void RestAPIServer::socket_disconnected()
{
    QWebSocket* client = qobject_cast<QWebSocket *>(sender());
    if (client)
    {
        clients_.removeAll(client);
        client->deleteLater();

        BOOST_LOG_TRIVIAL(info) << "Client is disconnected from '" << client->peerName().toStdString() << "'";
    }
}

RestAPIServer::RestAPIServer(quint16 port, QObject *parent): QObject(parent),
    socket_server_(new QWebSocketServer(QStringLiteral("Echo Server"), QWebSocketServer::NonSecureMode, this))
{
    if (socket_server_->listen(QHostAddress::LocalHost, port))
    {
        connect(socket_server_, &QWebSocketServer::newConnection, this, &RestAPIServer::on_new_connection);
        connect(socket_server_, &QWebSocketServer::closed, this, &RestAPIServer::closed);
    }
}

}   // namespace restapi
