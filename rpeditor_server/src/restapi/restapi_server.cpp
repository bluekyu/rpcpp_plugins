#define RAPIDJSON_HAS_STDSTRING 1
#include "restapi/restapi_server.hpp"

#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <fmt/format.h>

#include <render_pipeline/rpcore/rpobject.hpp>

#include "restapi/resolve_message.hpp"

namespace rpeditor {

RestAPIServer::~RestAPIServer(void)
{
    if (socket_server_->isListening())
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

void RestAPIServer::close(void)
{
    socket_server_->close();
}

void RestAPIServer::on_new_connection(void)
{
    QWebSocket* socket = socket_server_->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &RestAPIServer::process_message);
    connect(socket, &QWebSocket::disconnected, this, &RestAPIServer::socket_disconnected);

    clients_ << socket;

    rpcore::RPObject::global_info("plugin::" RPPLUGIN_ID_STRING, fmt::format("Client is connected from '{}'", socket->peerName().toStdString()));
    rpcore::RPObject::global_debug("plugin::" RPPLUGIN_ID_STRING, fmt::format("Peer address ({})), port ({})", socket->peerAddress().toString().toStdString(), socket->peerPort()));
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

        rpcore::RPObject::global_info("plugin::" RPPLUGIN_ID_STRING, fmt::format("Client is disconnected from '{}'", client->peerName().toStdString()));
    }
}

RestAPIServer::RestAPIServer(quint16 port, QObject *parent): QObject(parent),
    socket_server_(new QWebSocketServer(QStringLiteral("RPEditor API Server"), QWebSocketServer::NonSecureMode, this))
{
    if (socket_server_->listen(QHostAddress::LocalHost, port))
    {
        connect(socket_server_, &QWebSocketServer::newConnection, this, &RestAPIServer::on_new_connection);
        connect(socket_server_, &QWebSocketServer::closed, this, &RestAPIServer::closed);
    }
}

}   // namespace rpeditor
