#include "veinproxycomp.h"

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include "veinmoduleentity.h"
#include "veinproxyvalidator.h"

using namespace VfCpp;

VeinProxyComp::VeinProxyComp(int p_entityId, QPointer<VeinModuleEntity> p_eventsystem,int p_subEntId ,QString p_subCompName,QString p_proxyCompName, QVariant p_initval, TakeOver p_defaultTake)
    : m_entityId(p_entityId), m_pEventSystem(p_eventsystem), m_subEntityId(p_subEntId), m_subComponentName(p_subCompName), m_value(p_initval), m_defaultTake(p_defaultTake)
{

    // We want to see the proxy Component from outside. Without the knowing of the user a
    // real component is created showing the status of the proxy
    // @attention: The created component is out only (it is not possible to write it)
    m_realComponent=m_pEventSystem->createComponent(p_proxyCompName,p_initval,cVeinModuleComponent::Direction::inOut);
    m_realComponent.toStrongRef()->setValidator(new VeinProxyValidator(this));


    connect(this,&VeinAbstractComponent::sigValueChanged,m_realComponent.toStrongRef().data(),&VeinAbstractComponent::setValue);
    connect(m_realComponent.toStrongRef().data(),&VeinAbstractComponent::sigValueChanged,this,&VeinAbstractComponent::setValue);
}

void VeinProxyComp::init()
{
    VeinEvent::CommandEvent *cEvent = nullptr;
    VeinComponent::ComponentData *cData = nullptr;
    cData = new VeinComponent::ComponentData();
    cData->setEntityId(m_subEntityId);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);
    cData->setEventOrigin(VeinComponent::ComponentData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinComponent::ComponentData::EventTarget::ET_ALL);
    cData->setComponentName(m_subComponentName);
    cEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
    emit m_pEventSystem->sigSendEvent(cEvent);
}

void VeinProxyComp::setValueByEvent(QVariant value)
{
    if(value != getValue()){
            setValue(value);
        emit sigValueChanged(value);
    }
}

QVariant VeinProxyComp::getValue()
{
    return m_value;
}

QString VeinProxyComp::getName()
{
    return m_subComponentName;
}

void VeinProxyComp::setValue(QVariant p_value, TakeOver p_takeOver)
{
    switch(p_takeOver){
    case TakeOver::direct:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET,p_value);
        m_value=p_value;
        break;
    case TakeOver::directWithNotification:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET,p_value);
        m_value=p_value;
        emit sigValueChanged(m_value);
        break;
    case TakeOver::onNotification:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET,p_value);
        break;
    default:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET,p_value);
    }

}

void VeinProxyComp::setValue(QVariant p_value)
{
    setValue(p_value,m_defaultTake);
}

void VeinProxyComp::setError()
{

}

int VeinProxyComp::getSubEntityId() const
{
    return m_subEntityId;
}

void VeinProxyComp::setSubEntityId(int subEntityId)
{
    m_subEntityId = subEntityId;
}

void VeinProxyComp::sendTransaction(VeinComponent::ComponentData::Command vcmd, QVariant p_value)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_subEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(vcmd);
    cData->setComponentName(m_subComponentName);
    cData->setNewValue(p_value);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
    QUuid id; // null id
    event->setPeerId(id);

    emit m_pEventSystem->sigSendEvent(event);
}


