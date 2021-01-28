#include "veinproxycomp.h"

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>

using namespace VfCpp;

VeinProxyComp::VeinProxyComp(int p_entityId, VeinEvent::EventSystem *p_eventsystem,int p_subEntId ,QString p_subCompName, QVariant p_initval, TakeOver p_defaultTake)
    : m_entityId(p_entityId), m_pEventSystem(p_eventsystem), m_subEntityId(p_subEntId), m_subComponentName(p_subCompName), m_value(p_initval), m_defaultTake(p_defaultTake)
{
    VeinEvent::CommandEvent *cEvent = nullptr;
    VeinComponent::ComponentData *cData = nullptr;
    cData = new VeinComponent::ComponentData();
    cData->setEntityId(p_subEntId);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_FETCH);
    cData->setEventOrigin(VeinComponent::ComponentData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinComponent::ComponentData::EventTarget::ET_ALL);
    cData->setComponentName(p_subCompName);
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
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET);
        m_value=p_value;
        break;
    case TakeOver::directWithNotification:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET);
        setValueByEvent(p_value);
        break;
    case TakeOver::onNotification:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET);
        break;
    default:
        sendTransaction(VeinComponent::ComponentData::Command::CCMD_SET);
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

void VeinProxyComp::sendTransaction(VeinComponent::ComponentData::Command vcmd)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_subEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(vcmd);
    cData->setComponentName(m_subComponentName);
    cData->setNewValue(m_value);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
    QUuid id; // null id
    event->setPeerId(id);

    emit m_pEventSystem->sigSendEvent(event);
}

