#include "veinmoduleentity.h"
#include "vcmp_entitydata.h"
#include "vcmp_remoteproceduredata.h"

using namespace VfCpp;
using namespace VeinComponent;
using namespace VeinEvent;

veinmoduleentity::veinmoduleentity(int p_entityId,QObject *p_parent):
    VeinEvent::EventSystem(p_parent),
    m_entityId(p_entityId)
{
    //QObject::connect(this,&veinmoduleentity::sigAttached,this,&veinmoduleentity::initModule);
}

veinmoduleentity::~veinmoduleentity()
{
}

bool veinmoduleentity::hasComponent(const QString name)
{
    return m_componentList.contains(name);
}

cVeinModuleComponent::WPtr  veinmoduleentity::createComponent(QString p_name, QVariant p_initval, bool p_readOnly)
{
    if(!hasComponent(p_name)) {
        cVeinModuleComponent::Ptr tmpPtr=cVeinModuleComponent::Ptr(new cVeinModuleComponent(m_entityId,this,p_name,p_initval,p_readOnly), &QObject::deleteLater);
        m_componentList[tmpPtr->getName()]=tmpPtr;
        return tmpPtr;
    }
    else {
        qFatal("veinmoduleentity::createComponent: A component %s already exists", qPrintable(p_name));
    }
}


cVeinModuleRpc::WPtr  veinmoduleentity::createRpc(QObject *p_object, QString p_funcName, QMap<QString, QString> p_parameter, bool p_threaded)
{
    cVeinModuleRpc::Ptr tmpPtr = cVeinModuleRpc::Ptr(new cVeinModuleRpc(m_entityId,this,p_object,p_funcName,p_parameter,p_threaded),&QObject::deleteLater);
    m_rpcList[tmpPtr->rpcName()]=tmpPtr;
    return tmpPtr;
}

bool veinmoduleentity::processEvent(QEvent *t_event)
{
    bool retVal = false;
    if(t_event->type()==VeinEvent::CommandEvent::eventType())
    {
        VeinEvent::CommandEvent *cEvent = nullptr;
        VeinEvent::EventData *evData = nullptr;
        cEvent = static_cast<VeinEvent::CommandEvent *>(t_event);
        Q_ASSERT(cEvent != nullptr);

        evData = cEvent->eventData();
        Q_ASSERT(evData != nullptr);

        if(evData->entityId() == m_entityId)
        {
            // does the actual handling if event is of the correct type and
            // addresses this entity
            retVal = processCommandEvent(cEvent);
        }
    }
    return retVal;
}

VeinProxyComp::WPtr veinmoduleentity::watchComponent(int p_SubEntityId, const QString &p_SubComponentName)
{
    // check if entite already exists in watchlist
    if(!m_watchList.contains(p_SubEntityId)){
        m_watchList[p_SubEntityId]=QMap<QString,VeinProxyComp::Ptr>();
    }
    // check if component is already subscribed. Create if not.
    // If the component is already subscribed we can just return the already exyisting component
    if(!m_watchList[p_SubEntityId].contains(p_SubComponentName)){
        VeinProxyComp::Ptr tmpPtr= VeinProxyComp::Ptr(new VeinProxyComp(m_entityId,this,p_SubEntityId,p_SubComponentName));
        m_watchList[p_SubEntityId][p_SubComponentName]=tmpPtr;
    }
    return m_watchList[p_SubEntityId][p_SubComponentName];
}

bool veinmoduleentity::unWatchComponent(int p_EntityId, const QString &p_componentName)
{
    bool retVal=false;
    if(m_watchList.contains(p_EntityId)){
        if(m_watchList[p_EntityId].contains(p_componentName)){
            m_watchList[p_EntityId].remove(p_componentName);
            retVal=true;
        }
    }
    return retVal;
}

VeinRpcFuture::Ptr veinmoduleentity::invokeRPC(int p_entityId, const QString &p_procedureName, const QVariantMap &p_parameters)
{
    QUuid rpcIdentifier;
    rpcIdentifier=QUuid::createUuid();
    // create future. Rpc data will end inside the future
    VeinRpcFuture::Ptr tmpFuture=VeinRpcFuture::Ptr(new VeinRpcFuture(p_entityId,p_procedureName,rpcIdentifier));
    // Add future to futurelist
    if(!m_futureList.contains(p_entityId)){
        m_futureList[p_entityId]=QMap<QString,QMap<QUuid,VeinRpcFuture::Ptr>>();
    }
    if(!m_futureList[p_entityId].contains(p_procedureName)){
            m_futureList[p_entityId][p_procedureName]=QMap<QUuid,VeinRpcFuture::Ptr>();
    }
    m_futureList[p_entityId][p_procedureName][rpcIdentifier]=tmpFuture;

    // send rpc invoke message

    QVariantMap rpcParamData;
    rpcParamData.insert(RemoteProcedureData::s_callIdString, rpcIdentifier);
    rpcParamData.insert(RemoteProcedureData::s_parameterString, p_parameters);
    RemoteProcedureData *rpcData = new RemoteProcedureData();
    rpcData->setEntityId(m_entityId);
    rpcData->setCommand(RemoteProcedureData::Command::RPCMD_CALL);
    rpcData->setEventOrigin(ComponentData::EventOrigin::EO_LOCAL);
    rpcData->setEventTarget(ComponentData::EventTarget::ET_ALL);
    rpcData->setProcedureName(p_procedureName);
    rpcData->setInvokationData(rpcParamData);
    CommandEvent *cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, rpcData);
    emit sigSendEvent(cEvent);

    return tmpFuture;
}

//@TODO check if notification is reachable
//@TODO implement and handle active objects
bool veinmoduleentity::processCommandEvent(VeinEvent::CommandEvent *p_cEvent)
{
    bool retVal = false;
    switch (p_cEvent->eventData()->type()) {
    case VeinComponent::ComponentData::dataType():
        retVal=processComponentData(p_cEvent);
        break;
    case VeinComponent::RemoteProcedureData::dataType():
        retVal=processRpcData(p_cEvent);
        break;
    }
    return retVal;
}

bool veinmoduleentity::processComponentData(VeinEvent::CommandEvent *p_cEvent)
{
    bool retVal;
    QString cName;
    int entityId;
    VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (p_cEvent->eventData());
    cName = cData->componentName();
    entityId = cData->entityId();
    switch (p_cEvent->eventSubtype()) {
    case VeinEvent::CommandEvent::EventSubtype::TRANSACTION: // handles components located in this entitie

        if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET)
        {
            if(m_componentList.contains(cName) && entityId == m_entityId){
                m_componentList[cName]->setValueByEvent(cData->newValue());
                retVal=true;
                p_cEvent->accept();
            }
        }else if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_FETCH){
            //Nothing to do here. This is a placeholder in case the storage will not handle FETCH
            //events in Future anymore.
        }

        break;
    case VeinEvent::CommandEvent::EventSubtype::NOTIFICATION: // handles informations about components located in other entities

        if(m_watchList.contains(entityId)){
            if(m_watchList[entityId].contains(cName)){
                if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET ||
                        cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_FETCH)
                {
                    m_watchList[entityId][cName]->setValueByEvent(cData->newValue());
                }
            }
        }

        break;
    }
    return retVal;
}

bool veinmoduleentity::processRpcData(VeinEvent::CommandEvent *p_cEvent)
{
    bool retVal;
    VeinComponent::RemoteProcedureData *rpcData=nullptr;
    rpcData = static_cast<VeinComponent::RemoteProcedureData *>(p_cEvent->eventData());
    // RPC located in this module


    switch (p_cEvent->eventSubtype()) {
    case VeinEvent::CommandEvent::EventSubtype::TRANSACTION: // Handles Rpcs located in this entity

        if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL){
            if(m_rpcList.contains(rpcData->procedureName()))
            {
                retVal = true;
                const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                Q_ASSERT(callId.isNull() == false);
                m_rpcList[rpcData->procedureName()]->callFunction(callId,p_cEvent->peerId(),rpcData->invokationData());
                p_cEvent->accept();
            }
            else //unknown procedure
            {
                retVal = true;
                qWarning() << "No remote procedure with entityId:" << m_entityId << "name:" << rpcData->procedureName();
                VF_ASSERT(false, QStringC(QString("No remote procedure with entityId: %1 name: %2").arg(m_entityId).arg(rpcData->procedureName())));
                VeinComponent::ErrorData *eData = new VeinComponent::ErrorData();
                eData->setEntityId(m_entityId);
                eData->setErrorDescription(QString("No remote procedure with name: %1").arg(rpcData->procedureName()));
                eData->setOriginalData(rpcData);
                eData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
                eData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
                VeinEvent::CommandEvent *errorEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
                errorEvent->setPeerId(p_cEvent->peerId());
                p_cEvent->accept();
                emit sigSendEvent(errorEvent);
            }
        }

        break;
    case VeinEvent::CommandEvent::EventSubtype::NOTIFICATION: // handles resutl data provided by other entities

        int tmpEntId=rpcData->entityId();
        QString tmpProcName=rpcData->procedureName();
        QUuid tmpUid= rpcData->invokationData().value(RemoteProcedureData::s_callIdString).toUuid();
        // check if data are targeting one of our futures
        if(m_futureList.contains(tmpEntId)){
            if(m_futureList[tmpEntId].contains(tmpProcName)){
                if(m_futureList[tmpEntId][tmpProcName].contains(tmpUid)){
                    // update future
                    m_futureList[tmpEntId][tmpProcName][tmpUid]->processRpcData(rpcData);
                    //Remove future if this is a result msg.
                    //The Future is a shared pointer and is only deleted from heap if no one else holds a reference.
                    //Furthermore processRpcData emits a signal of some kind with a reference to itself.
                    //Prevending the reference count to go to zero before computation is possible.
                    if(rpcData->command() == RemoteProcedureData::Command::RPCMD_RESULT){
                        m_futureList[tmpEntId][tmpProcName].remove(tmpUid);
                    }
                }
            }
        }

        break;
    }
    return retVal;
}

int veinmoduleentity::getEntitiyId() const
{
    return m_entityId;
}

void veinmoduleentity::initModule()
{
    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    eData->setEntityId(m_entityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    emit sigSendEvent(tmpEvent);
}

QMap<QString, cVeinModuleRpc::Ptr> veinmoduleentity::getRpcList() const
{
    return m_rpcList;
}

QMap<int, QMap<QString,VeinProxyComp::Ptr> > veinmoduleentity::getWatchList() const
{
    return m_watchList;
}

void veinmoduleentity::setWatchList(const QMap<int, QMap<QString,VeinProxyComp::Ptr> > &watchList)
{
    m_watchList = watchList;
}

void veinmoduleentity::setRpcList(const QMap<QString, cVeinModuleRpc::Ptr> &value)
{
    m_rpcList = value;
}

QMap<QString, cVeinModuleComponent::Ptr> veinmoduleentity::getComponentList() const
{
    return m_componentList;
}

void veinmoduleentity::setComponentList(const QMap<QString, cVeinModuleComponent::Ptr> &value)
{
    m_componentList = value;
}
