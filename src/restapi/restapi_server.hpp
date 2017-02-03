#include <memory>
#include <thread>

#include <QtCore/QObject>
#include <QtCore/QList>

#include "restapi/config.hpp"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

namespace restapi {

class RestAPISession;

class RestAPIServer: public QObject
{
    Q_OBJECT

public:
    static void run(void);
    static void close(void);

    static RestAPIServer* get_instance(void);

    ~RestAPIServer(void);

    void broadcast(const std::string& json_msg);

    void broadcast(const rapidjson::Document& doc);

Q_SIGNALS:
    void closed();

    private Q_SLOTS:
    void on_new_connection(void);
    void process_message(QString message);
    void socket_disconnected(void);

private:
    explicit RestAPIServer(quint16 port, QObject* parent=nullptr);

    void do_accept(void);

    static RestAPIServer* instance_;
    static std::unique_ptr<std::thread> network_thread_;

    QWebSocketServer* socket_server_;
    QList<QWebSocket*> clients_;
};

inline RestAPIServer* RestAPIServer::get_instance(void)
{
    return instance_;
}

}   // namespace restapi
