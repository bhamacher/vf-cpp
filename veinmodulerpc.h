#ifndef VEINMODULERPC_H
#define VEINMODULERPC_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUuid>
#include <QSharedPointer>
#include <QMutexLocker>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>
#include <ve_eventsystem.h>

#include <QMetaObject>
#include <vcmp_remoteproceduredata.h>
#include <ve_commandevent.h>
#include <vcmp_errordata.h>


namespace VeinEvent
{
class EventSystem;
}

namespace VfCpp {

/**
 * @brief The cVeinModuleRpc class
 */
class cVeinModuleRpc: public QObject
{
    Q_OBJECT
public:
    typedef  QSharedPointer< cVeinModuleRpc > Ptr;
    typedef  QMap<QString, QString> Param;

    enum RPCResultCodes {
        RPC_CANCELED = -64,
        RPC_EINVAL = -EINVAL, //invalid parameters
        RPC_SUCCESS = 0
    };


    cVeinModuleRpc(int entityId, VeinEvent::EventSystem *eventsystem, QObject *p_object, QString p_funcName, QMap<QString,QString> p_parameter,bool p_threaded=true);
    ~cVeinModuleRpc();

    QString rpcName() const;

    void callFunction(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &t_rpcParameters);


private slots:
    void callFunctionPrivate(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &t_rpcParameters);
signals:
    void callFunctionPrivateSignal(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &t_rpcParameters);

private:
    QObject *m_object;
    QString m_function;
    QMap<QString,QString> m_parameter;

    QString m_rpcName;

    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;

    bool m_threaded;

    QMutex m_mutex;

    friend class VeinEvent::EventSystem;
};

}


#endif // VEINMODULERPC_H
