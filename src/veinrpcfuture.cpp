#include "veinrpcfuture.h"

#include "vcmp_remoteproceduredata.h"

#include "veinmoduleentity.h"

using namespace VfCpp;
using namespace VeinComponent;

VeinRpcFuture::VeinRpcFuture(int p_rpcEntityId, QString p_rpcName, QUuid p_rpcUid) :
    m_rpcEntityId(p_rpcEntityId), m_rpcName(p_rpcName), m_status(RpcStatus::inProgress), m_rpcUniqueId(p_rpcUid)

{

}

VeinRpcFuture::~VeinRpcFuture()
{

}

QVariant VeinRpcFuture::Return()
{
    if(m_resultData.contains("RemoteProcedureData::Return")){
        return m_resultData.value("RemoteProcedureData::Return");
    }
    return QVariant();

}

void VeinRpcFuture::processRpcData(RemoteProcedureData *p_rpcData)
{
    QVariantMap invokationData=p_rpcData->invokationData();
    m_resultData=p_rpcData->invokationData();
    try {
        if(!invokationData.contains(RemoteProcedureData::s_callIdString)){
            throw;
        }
        if(invokationData[RemoteProcedureData::s_callIdString] != m_rpcUniqueId){
            throw;
        }
        switch(p_rpcData->command())
        {
        case RemoteProcedureData::Command::RPCMD_RESULT:
        {
            m_status=RpcStatus::finished;
            emit sigRPCFinished(m_rpcUniqueId);
            break;
        }
        case RemoteProcedureData::Command::RPCMD_PROGRESS:
        {
            m_status=RpcStatus::inProgress;
            emit sigRPCProgress(m_rpcUniqueId);
            break;
        }
        default:
            break;
        }
    }  catch (...) {
        m_status=RpcStatus::error;
        emit sigRPCError(m_rpcUniqueId);
    }


}

QVariantMap VeinRpcFuture::getResultData() const
{
    return m_resultData;
}

int VeinRpcFuture::getRpcEntityId() const
{
    return m_rpcEntityId;
}

QString VeinRpcFuture::getRpcName() const
{
    return m_rpcName;
}

VeinRpcFuture::RpcStatus VeinRpcFuture::getStatus() const
{
    return m_status;
}

QUuid VeinRpcFuture::getRpcUniqueId() const
{
    return m_rpcUniqueId;
}
