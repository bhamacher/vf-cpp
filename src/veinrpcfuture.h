#ifndef VEINRPCFUTURE_H
#define VEINRPCFUTURE_H

#include <QObject>
#include <QPointer>
#include <QVariant>
#include <QUuid>

#include "vcmp_remoteproceduredata.h"



namespace VfCpp {

class veinmoduleentity;


class VeinRpcFuture : public QObject
{
    Q_OBJECT
public:
    friend class veinmoduleentity;


    typedef QSharedPointer<VeinRpcFuture> Ptr;

    enum class RpcStatus{
        inProgress,             // rpc is in progress
        finished,               // rpc in finished and all data are there
        error                   // some kind of unexpected behaviour
    };

    explicit VeinRpcFuture(int p_rpcEntityId, QString p_rpcName, QUuid p_rpcUid);


    QVariant Return();

    int getRpcEntityId() const;

    QString getRpcName() const;

    RpcStatus getStatus() const;

    QUuid getRpcUniqueId() const;

    QVariantMap getResultData() const;

signals:
    void sigRPCFinished(VeinRpcFuture::Ptr p_future);
    void sigRPCProgress(VeinRpcFuture::Ptr p_future);
    void sigRPCError(VeinRpcFuture::Ptr p_future);
private:
    void processRpcData(VeinComponent::RemoteProcedureData *p_rpcData);
private:
    /**
     * @brief m_rpcEntityId
     * Stores the id of the Entitie where the rpc is located
     */
    int m_rpcEntityId;
    /**
     * @brief m_rpcName
     * Stores the rpc name
     */
    QString m_rpcName;
    /**
     * @brief m_resultData
     * Stores result or progress data
     */
    QVariantMap m_resultData;
    /**
     * @brief m_status
     * Stores rpc status data
     */
    RpcStatus m_status;
    /**
     * @brief m_RpcUniqueId
     * unique rpc call id
     */
    QUuid m_rpcUniqueId;

};

}

#endif // VEINRPCFUTURE_H
