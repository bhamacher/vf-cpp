#ifndef VEINMODULERPC_H
#define VEINMODULERPC_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUuid>
#include <QPointer>
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
    typedef  QWeakPointer< cVeinModuleRpc > WPtr;
    typedef  QMap<QString, QString> Param;

    enum RPCResultCodes {
        RPC_CANCELED = -64,
        RPC_EINVAL = -EINVAL, //invalid parameters
        RPC_SUCCESS = 0
    };

    /**
     * @brief cVeinModuleRpc
     * Creates vein rpc. The actual function must be
     * member of a class inherting from QObject.
     * RPC functions can not be overloaded.
     * @param entityId: ID of entity this component is mapped to.
     * @param eventsystem: pointer to managing Eventsystem object
     * @param p_object: object where the actual memeber function is located
     * @param p_funcName: function name as string. Only name no brackets
     * @param p_parameter: function parameters (not typesafe)
     * @param p_threaded: set false to process function in main thread.
     *
     * More information is class description
     */
    cVeinModuleRpc(int entityId, VeinEvent::EventSystem *eventsystem, QObject *p_object, QString p_funcName, QMap<QString,QString> p_parameter,bool p_threaded=true);
    ~cVeinModuleRpc();

    QString rpcName() const;

    /**
     * @brief callFunction
     * This function is meant to use in EventSystem processEvent function.
     * Examples are in veinmoduleentity class.
     * @param p_callId: vein callid
     * @param p_peerId: vein peerid
     * @param t_rpcParameters: parameterlist
     */
    void callFunction(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &p_rpcParameters);


private slots:
    /**
     * @brief callFunctionPrivate
     * Same as call function. Only use of this slot is to queue rpc processing until qt main thread has time.
     */
    void callFunctionPrivate(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &p_rpcParameters);
signals:
    /**
     * @brief callFunctionPrivateSignal
     * Triggers callFunctionPrivatSlot. Is emited in callFunction function.
     */
    void callFunctionPrivateSignal(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &p_rpcParameters);

private:
    QObject *m_object;
    QString m_function;
    QMap<QString,QString> m_parameter;

    QString m_rpcName;

    int m_nEntityId;
    QPointer<VeinEvent::EventSystem> m_pEventSystem;

    bool m_threaded;

    QMutex m_mutex;

    friend class VeinEvent::EventSystem;
};

}


#endif // VEINMODULERPC_H
