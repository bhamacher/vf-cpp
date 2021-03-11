#include <QJsonObject>
#include <QJsonArray>
#include <QValidator>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>

#include <ve_commandevent.h>
#include <ve_eventsystem.h>

#include "veinmodulecomponent.h"
using namespace VfCpp;



cVeinModuleComponent::cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QVariant initval, Direction p_direction)
    :m_nEntityId(entityId), m_pEventSystem(eventsystem), m_sName(name), m_vValue(initval), m_direction(p_direction), m_validator(nullptr)
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
    if(!m_pEventSystem.isNull()){
        m_pEventSystem->sigSendEvent(cEvent);
    }
}

void cVeinModuleComponent::removeComponent()
{
    // This is noot in use yet. We have code where these components are used without VeinModuleEntity.
    // The issue is, that this feature only works with VeinModuleEntity, because the component must remove itself
    // from the entity here. At the moment this component onyl knows classes of type EventSystem.
}


QPointer<QValidator> cVeinModuleComponent::getValidator() const
{
    return m_validator;
}

void cVeinModuleComponent::setValidator(const QPointer<QValidator> &p_validator)
{
    m_validator = p_validator;
    if(!m_validator.isNull()){
        m_validator->setParent(this);
    }
}

void cVeinModuleComponent::setValueByEvent(QVariant p_value)
{
    if(p_value != getValue()){
        // set only if component is an in or in/out interface
        if(m_direction == Direction::in || m_direction == Direction::inOut){
            if(!m_validator.isNull()){
                QString valValue=p_value.toString();
                int valPos=0;
                // if there is a validator set data only if validator returns true.
                if(m_validator->validate(valValue,valPos) == QValidator::State::Acceptable){
                    m_vValue = p_value;
                    emit sigValueChanged(p_value);
                    sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
                }
            }else{
                m_vValue = p_value;
                emit sigValueChanged(p_value);
                sendNotification(VeinComponent::ComponentData::Command::CCMD_SET);
            }
        }

    }
}


void cVeinModuleComponent::sendNotification(VeinComponent::ComponentData::Command p_vcmd)
{
    VeinComponent::ComponentData *cData;

    cData = new VeinComponent::ComponentData();

    cData->setEntityId(m_nEntityId);
    cData->setEventOrigin(VeinEvent::EventData::EventOrigin::EO_LOCAL);
    cData->setEventTarget(VeinEvent::EventData::EventTarget::ET_ALL);
    cData->setCommand(p_vcmd);
    cData->setComponentName(m_sName);
    cData->setNewValue(m_vValue);

    VeinEvent::CommandEvent *event;
    event = new VeinEvent::CommandEvent(VeinEvent::CommandEvent::EventSubtype::NOTIFICATION, cData);
    QUuid id; // null id
    event->setPeerId(id);
    if(!m_pEventSystem.isNull()){
        emit m_pEventSystem->sigSendEvent(event);
    }
}

