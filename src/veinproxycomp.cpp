#include "veinproxycomp.h"

#include <vcmp_componentdata.h>
#include <ve_commandevent.h>
#include "veinmoduleentity.h"
#include "veinproxyvalidator.h"

using namespace VfCpp;

VeinProxyComp::VeinProxyComp(int p_entityId, QPointer<VeinModuleEntity> p_eventsystem,int p_subEntId ,QString p_subCompName,QString p_proxyCompName, QVariant p_initval, TakeOver p_defaultTake)
    : m_entityId(p_entityId), m_pEventSystem(p_eventsystem), m_subEntityId(p_subEntId), m_subComponentName(p_subCompName), m_value(p_initval), m_defaultTake(p_defaultTake)
{

    // We want to see the proxy Component from outside. Without the user knowing about it. (mirrorcomponent)
    // A real component is created showing the status of the proxy. The real component will be removed with the proxyComponent.
    // The component is inOut. But the Validator prevetns it from taking any value. If the original component is in or inOut
    // it will send a notification. This notification will chnage th value.
    m_realComponent=m_pEventSystem->createComponent(p_proxyCompName,p_initval,cVeinModuleComponent::Direction::inOut);
    m_realComponent.toStrongRef()->setValidator(new VeinProxyValidator(this));

    // connection values coming from the original component to the hidden mirror component and dat coming from the
    // mirrorcomponent to the original component
    connect(this,&VeinAbstractComponent::sigValueChanged,m_realComponent.toStrongRef().data(),&VeinAbstractComponent::setValue);
    connect(m_realComponent.toStrongRef().data(),&VeinAbstractComponent::sigValueChanged,this,&VeinAbstractComponent::setValue);
}

VeinProxyComp::~VeinProxyComp()
{
    m_pEventSystem->removeComponent(m_realComponent.toStrongRef()->getName());
}

void VeinProxyComp::init()
{
    //Fetching data from original component
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

void VeinProxyComp::setValueByEvent(QVariant p_value)
{
    if(p_value != getValue()){
        setValue(p_value);
        emit sigValueChanged(p_value);
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

void VeinProxyComp::removeComponent()
{
    m_pEventSystem->removeProxyComponent(m_subEntityId,m_subComponentName);
}

int VeinProxyComp::getSubEntityId() const
{
    return m_subEntityId;
}

void VeinProxyComp::setSubEntityId(int p_subEntityId)
{
    m_subEntityId = p_subEntityId;
}

void VeinProxyComp::sendTransaction(VeinComponent::ComponentData::Command p_vcmd, QVariant p_value)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_subEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(p_vcmd);
    cData->setComponentName(m_subComponentName);
    cData->setNewValue(p_value);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::TRANSACTION, cData);
    QUuid id; // null id
    event->setPeerId(id);

    emit m_pEventSystem->sigSendEvent(event);
}


