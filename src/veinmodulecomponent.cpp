#include <QJsonObject>
#include <QJsonArray>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "veinmodulecomponent.h"
using namespace VfCpp;



cVeinModuleComponent::cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QVariant initval, Direction p_direction)
    :m_nEntityId(entityId), m_pEventSystem(eventsystem), m_sName(name), m_vValue(initval), m_direction(p_direction)
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_ADD);
}


cVeinModuleComponent::~cVeinModuleComponent()
{
    sendNotification(VeinComponent::ComponentData::Command::CCMD_REMOVE);
}


QVariant cVeinModuleComponent::getValue()
{
    return m_vValue;
}

QString cVeinModuleComponent::getName()
{
    return m_sName;
}


void cVeinModuleComponent::setValue(QVariant value)
{
    if(m_direction == Direction::out || m_direction == Direction::inOut){
        m_vValue = value;
        sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
    }
}


void cVeinModuleComponent::setError()
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(VeinComponent::ComponentData::Command::CCMD_SET);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinComponent::ErrorData *errData;

    errData = new VeinComponent::ErrorData();

    errData->setEntityId(m_nEntityId);
    errData->setErrorDescription("Invalid parameter");
    errData->setOriginalData(cData);

    VeinEvent::CommandEvent *cEvent = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, errData);
    QUuid id; // null id
    cEvent->setPeerId(id);

    m_pEventSystem->sigSendEvent(cEvent);

}

void cVeinModuleComponent::setValueByEvent(QVariant value)
{
    if(value != getValue()){
        if(m_direction == Direction::in || m_direction == Direction::inOut){
            m_vValue = value;
            sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
        }
        emit sigValueChanged(value);
    }
}


void cVeinModuleComponent::sendNotification(VeinComponent::ComponentData::Command vcmd)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(vcmd);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
    QUuid id; // null id
    event->setPeerId(id);

    emit m_pEventSystem->sigSendEvent(event);
}

