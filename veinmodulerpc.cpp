#include "veinmodulerpc.h"

using namespace VfCpp;



cVeinModuleRpc::cVeinModuleRpc(int entityId, VeinEvent::EventSystem *eventsystem, QObject *p_object, QString p_funcName, QMap<QString,QString> p_parameter)
    : m_object(p_object), m_function(p_funcName), m_parameter(p_parameter), m_nEntityId(entityId), m_pEventSystem(eventsystem)
{
    m_rpcName=m_function;
    m_rpcName.append("(");
    for(QString param : m_parameter.keys()){
        m_rpcName.append(m_parameter[param]);
        m_rpcName.append(" ");
        m_rpcName.append(param);
        m_rpcName.append(",");
    }
    if(m_rpcName.at(m_rpcName.size()-1) == ","){
        m_rpcName.remove(m_rpcName.size()-1,1);
    }
    m_rpcName.append(")");

    VeinComponent::RemoteProcedureData *rpcData = new VeinComponent::RemoteProcedureData();
    rpcData->setEntityId(m_nEntityId);
    rpcData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_REGISTER);
    rpcData->setProcedureName(m_rpcName);
    rpcData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    rpcData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);

    emit  m_pEventSystem->sigSendEvent(new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, rpcData));
}
cVeinModuleRpc::~cVeinModuleRpc(){

};

QString cVeinModuleRpc::rpcName() const
{
    return m_rpcName;
};

void cVeinModuleRpc::callFunction(const QUuid &p_callId,const QUuid &p_peerId, const QVariantMap &t_rpcParameters) {
    Q_UNUSED(p_callId)
    QVariantMap returnVal;
    QVariant fcnRetVal;

    // check parameters
    QStringList requiredParamList = m_parameter.keys();
    QSet<QString> requiredParamKeys(requiredParamList.begin(), requiredParamList.end());
    const QVariantMap searchParameters = t_rpcParameters.value(VeinComponent::RemoteProcedureData::s_parameterString).toMap();
    QStringList searchParamList = searchParameters.keys();
    requiredParamKeys.subtract(QSet<QString>(searchParamList.begin(), searchParamList.end()));

    if(requiredParamKeys.isEmpty())
    {
        //call actual function
        bool suc=QMetaObject::invokeMethod(m_object,m_function.toUtf8(),Qt::DirectConnection,
                                           Q_RETURN_ARG(QVariant, fcnRetVal),
                                           Q_ARG(QVariantMap,searchParameters)
                                           );

        // write return value to RemoteProcedureData::Return on success
        if(suc){
            returnVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::RPC_SUCCESS);
            returnVal.insert("RemoteProcedureData::Return",fcnRetVal);
        }else{
            returnVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::RPC_EINVAL);
            returnVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Function not implemented"));
        }


    }else{
        // write error msg on error
        returnVal=t_rpcParameters;
        returnVal.insert(VeinComponent::RemoteProcedureData::s_resultCodeString, RPCResultCodes::RPC_EINVAL);
        returnVal.insert(VeinComponent::RemoteProcedureData::s_errorMessageString, QString("Missing required parameters: [%1]").arg(requiredParamList.join(',')));
    }
    // send answer
    returnVal.insert(VeinComponent::RemoteProcedureData::s_callIdString,t_rpcParameters[VeinComponent::RemoteProcedureData::s_callIdString]);
    VeinComponent::RemoteProcedureData *resultData = new VeinComponent::RemoteProcedureData();
    resultData->setEntityId(m_nEntityId);
    resultData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    resultData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    resultData->setCommand(VeinComponent::RemoteProcedureData::Command::RPCMD_RESULT);
    resultData->setProcedureName(m_rpcName);
    resultData->setInvokationData(returnVal);


    VeinEvent::CommandEvent *rpcResultEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, resultData);
    rpcResultEvent->setPeerId(p_peerId);
    emit m_pEventSystem->sigSendEvent(rpcResultEvent);

};
