#pragma once

#include "api_server_interface.hpp"

#include <QtCore/QObject>
#include <QtCore/QList>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

namespace rpeditor {

class RestAPISession;

class RestAPIServer: public QObject, public APIServerInterface
{
    Q_OBJECT

public:
    explicit RestAPIServer(quint16 port, QObject* parent=nullptr);

    ~RestAPIServer(void);

    void broadcast(const std::string& json_msg) override;

    void broadcast(const rapidjson::Document& doc) override;

    void close(void);

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void on_new_connection(void);
    void process_message(QString message);
    void socket_disconnected(void);

private:
    void do_accept(void);

    QWebSocketServer* socket_server_ = nullptr;
    QList<QWebSocket*> clients_;
};

}
