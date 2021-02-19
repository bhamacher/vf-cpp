#include "veinmoduleentity.h"
#include "vcmp_entitydata.h"
#include "vcmp_remoteproceduredata.h"
#include "ve_eventhandler.h"

using namespace VfCpp;
using namespace VeinComponent;
using namespace VeinEvent;

//@TODO create init signal. It might be unsafe to use sigAttach from the outside
VeinModuleEntity::VeinModuleEntity(int p_entityId,QObject *p_parent):
    VeinEvent::EventSystem(p_parent),
    m_entityId(p_entityId)
{
    // init enitie, when it is attached to the eventhandler.
    QObject::connect(this,&VeinModuleEntity::sigAttached,this,&VeinModuleEntity::initModule);
}

VeinModuleEntity::~VeinModuleEntity()
{
    // remove all components. Some of them need to
    // send events before the entitie is removed
    m_watchList.clear();
    m_componentList.clear();
    m_rpcList.clear();

    // unregister entity
    if(m_attached){
        VeinComponent::EntityData *eData = new VeinComponent::EntityData();
        eData->setCommand(VeinComponent::EntityData::Command::ECMD_REMOVE);
        eData->setEntityId(m_entityId);
        VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
        emit sigSendEvent(tmpEvent);
    }
    // remove entitie from eventhandler
    m_eventHandler->removeSubsystem(this);
}

bool VeinModuleEntity::hasComponent(const QString p_name)
{
    return m_componentList.contains(p_name);
}

cVeinModuleComponent::WPtr  VeinModuleEntity::createComponent(QString p_name, QVariant p_initval, cVeinModuleComponent::Direction p_direction,QValidator* p_validator)
{
    if(!hasComponent(p_name)) {
        cVeinModuleComponent::Ptr tmpPtr=cVeinModuleComponent::Ptr(new cVeinModuleComponent(m_entityId,this,p_name,p_initval,p_direction));
        m_componentList[tmpPtr->getName()]=tmpPtr;
        if(p_validator != nullptr){
            tmpPtr->setValidator(p_validator);
        }
        return tmpPtr;
    }
    else {
        qFatal("veinmoduleentity::createComponent: A component %s already exists", qPrintable(p_name));
    }
}

bool VeinModuleEntity::removeComponent(const QString &p_name)
{
    if(m_componentList.contains(p_name)){
        m_componentList[p_name].clear();
        m_componentList.remove(p_name);
        return true;
    }
    return false;
}

//@TODO prevent regiterig two rpcs with the same name (parameters wont have any impact on it anymore)
cVeinModuleRpc::WPtr  VeinModuleEntity::createRpc(QObject *p_object, QString p_funcName, QMap<QString, QString> p_parameter, bool p_threaded)
{
    cVeinModuleRpc::Ptr tmpPtr = cVeinModuleRpc::Ptr(new cVeinModuleRpc(m_entityId,this,p_object,p_funcName,p_parameter,p_threaded),&QObject::deleteLater);
    // As key we do only save the function name without parameters
    m_rpcList[tmpPtr->rpcName().split("(")[0]]=tmpPtr;
    return tmpPtr;
}

bool VeinModuleEntity::processEvent(QEvent *p_event)
{
    bool retVal = false;
    // We only process CommandEvents. Everthing else is not VEIN
    if(p_event->type()==VeinEvent::CommandEvent::eventType())
    {
        VeinEvent::CommandEvent *cEvent = nullptr;
        VeinEvent::EventData *evData = nullptr;
        cEvent = static_cast<VeinEvent::CommandEvent *>(p_event);
        Q_ASSERT(cEvent != nullptr);

        evData = cEvent->eventData();
        Q_ASSERT(evData != nullptr);
        retVal = processCommandEvent(cEvent);

        // forword signal for manual computation
        emit sigEvent(*p_event);

    }
    return retVal;
}

VeinProxyComp::WPtr VeinModuleEntity::createProxyComponent(int p_SubEntityId, const QString &p_SubComponentName, const QString p_proxyCompName, VeinProxyComp::TakeOver p_takeOver)
{
    // check if entite already exists in watchlist
    if(!m_watchList.contains(p_SubEntityId)){
        m_watchList[p_SubEntityId]=QMap<QString,VeinProxyComp::Ptr>();
    }
    // check if component is already subscribed. Create if not.
    // If the component is already subscribed we can just return the already exyisting component
    if(!m_watchList[p_SubEntityId].contains(p_SubComponentName)){
        VeinProxyComp::Ptr tmpPtr= VeinProxyComp::Ptr(new VeinProxyComp(m_entityId,this,p_SubEntityId,p_SubComponentName,p_proxyCompName,QVariant(""),p_takeOver));
        m_watchList[p_SubEntityId][p_SubComponentName]=tmpPtr;
        tmpPtr->init();
    }
    return m_watchList[p_SubEntityId][p_SubComponentName];
}

bool VeinModuleEntity::removeProxyComponent(int p_entityId, const QString &p_componentName)
{
    bool retVal=false;
    if(m_watchList.contains(p_entityId)){
        if(m_watchList[p_entityId].contains(p_componentName)){
            m_watchList[p_entityId].remove(p_componentName);
            retVal=true;
        }
    }
    return retVal;
}

VeinRpcFuture::Ptr VeinModuleEntity::invokeRPC(int p_entityId, const QString &p_procedureName, const QVariantMap &p_parameters)
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

//@TODO implement and handle active objects
bool VeinModuleEntity::processCommandEvent(VeinEvent::CommandEvent *p_cEvent)
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

bool VeinModuleEntity::processComponentData(VeinEvent::CommandEvent *p_cEvent)
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

bool VeinModuleEntity::processRpcData(VeinEvent::CommandEvent *p_cEvent)
{
    bool retVal;
    VeinComponent::RemoteProcedureData *rpcData=nullptr;
    rpcData = static_cast<VeinComponent::RemoteProcedureData *>(p_cEvent->eventData());
    // RPC located in this module


    switch (p_cEvent->eventSubtype()) {
    case VeinEvent::CommandEvent::EventSubtype::TRANSACTION: // Handles Rpcs located in this entity
        if(rpcData->entityId() == m_entityId){
            if(rpcData->command() == VeinComponent::RemoteProcedureData::Command::RPCMD_CALL){
                // Comparing drpc name with available rpc list.
                // Comparing only the function name wihtout parameters.
                if(m_rpcList.contains(rpcData->procedureName().split("(")[0]))
                {
                    retVal = true;
                    const QUuid callId = rpcData->invokationData().value(VeinComponent::RemoteProcedureData::s_callIdString).toUuid();
                    Q_ASSERT(callId.isNull() == false);
                    m_rpcList[rpcData->procedureName().split("(")[0]]->callFunction(callId,p_cEvent->peerId(),rpcData->invokationData());
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
        }
        break;
    case VeinEvent::CommandEvent::EventSubtype::NOTIFICATION: // handles result data provided by other entities

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

int VeinModuleEntity::getEntitiyId() const
{
    return m_entityId;
}

void VeinModuleEntity::initModule()
{
    VeinComponent::EntityData *eData = new VeinComponent::EntityData();
    eData->setCommand(VeinComponent::EntityData::Command::ECMD_ADD);
    eData->setEntityId(m_entityId);
    VeinEvent::CommandEvent *tmpEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, eData);
    emit sigSendEvent(tmpEvent);
}

QMap<QString, cVeinModuleRpc::Ptr> VeinModuleEntity::getRpcList() const
{
    return m_rpcList;
}

QMap<int, QMap<QString,VeinProxyComp::Ptr> > VeinModuleEntity::getWatchList() const
{
    return m_watchList;
}

void VeinModuleEntity::setWatchList(const QMap<int, QMap<QString,VeinProxyComp::Ptr> > &p_watchList)
{
    m_watchList = p_watchList;
}

void VeinModuleEntity::setRpcList(const QMap<QString, cVeinModuleRpc::Ptr> &p_value)
{
    m_rpcList = p_value;
}

QMap<QString, cVeinModuleComponent::Ptr> VeinModuleEntity::getComponentList() const
{
    return m_componentList;
}

void VeinModuleEntity::setComponentList(const QMap<QString, cVeinModuleComponent::Ptr> &p_value)
{
    m_componentList = p_value;
}
