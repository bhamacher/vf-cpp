#include "veinmoduleentity.h"
#include "vcmp_entitydata.h"

using namespace VfCpp;

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

cVeinModuleComponent::Ptr  veinmoduleentity::createComponent(QString p_name, QVariant p_initval, bool p_readOnly)
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


cVeinModuleRpc::Ptr  veinmoduleentity::createRpc(QObject *p_object, QString p_funcName, QMap<QString, QString> p_parameter)
{
    cVeinModuleRpc::Ptr tmpPtr = cVeinModuleRpc::Ptr(new cVeinModuleRpc(m_entityId,this,p_object,p_funcName,p_parameter),&QObject::deleteLater);
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

void veinmoduleentity::watchComponent(int p_EntityId, const QString &p_componentName)
{
    m_watchList[p_EntityId].insert(p_componentName);
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

   //@TODO check if notification is reachable
   //@TODO implement and handle active objects
bool veinmoduleentity::processCommandEvent(VeinEvent::CommandEvent *p_cEvent)
{
     bool retVal = false;
     // handle components
    if (p_cEvent->eventData()->type() == VeinComponent::ComponentData::dataType())
    {
        QString cName;
        int entityId;
        VeinComponent::ComponentData* cData = static_cast<VeinComponent::ComponentData*> (p_cEvent->eventData());
        cName = cData->componentName();
        entityId = cData->entityId();
        // Managed by this entity
        if (p_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::TRANSACTION)
        {
            if(cData->eventCommand() == VeinComponent::ComponentData::Command::CCMD_SET &&
                    p_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::TRANSACTION)
            {
                if(m_componentList.contains(cName) && entityId == m_entityId){
                    m_componentList[cName]->setValueByEvent(cData->newValue());
                    retVal=true;
                    p_cEvent->accept();
                }
            }
        // managed by other entites

        }else if(p_cEvent->eventSubtype() == VeinEvent::CommandEvent::EventSubtype::NOTIFICATION){
            if(m_watchList.contains(entityId)){
                if(m_watchList[entityId].contains(cName)){
                    emit sigWatchedComponentChanged(entityId,cName,cData->newValue());
                }
            }
        }
    // handle rpcs
    }else if(p_cEvent->eventData()->type() == VeinComponent::RemoteProcedureData::dataType()){
        VeinComponent::RemoteProcedureData *rpcData=nullptr;
        rpcData = static_cast<VeinComponent::RemoteProcedureData *>(p_cEvent->eventData());
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

QMap<int, QSet<QString> > veinmoduleentity::getWatchList() const
{
    return m_watchList;
}

void veinmoduleentity::setWatchList(const QMap<int, QSet<QString> > &watchList)
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
