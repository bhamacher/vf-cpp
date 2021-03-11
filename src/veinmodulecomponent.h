#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <QObject>
#include <QPointer>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUuid>


#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>
#include <ve_eventsystem.h>

#include "veinabstractcomponent.h"

class QValidator;


namespace VfCpp {
/**
 * @brief The cVeinModuleComponent class
 *
 * Abstraction for vein components
 */
class cVeinModuleComponent: public VeinAbstractComponent
{
    Q_OBJECT
public:

    typedef  QSharedPointer< cVeinModuleComponent > Ptr;
    typedef  QWeakPointer< cVeinModuleComponent > WPtr;

    /**
     * @brief The Direction enum
     *
     * Defines the component direction.
     * out: write only from this enity
     * in: wirte only from extern entities
     * inOut: write from where you want
     */
    enum class Direction{
        out,
        in,
        inOut,
        constant
    };

    /**
     * @brief cVeinModuleComponent
     * creates a vein component
     * @param entityId: ID of entity this component is mapped to.
     * @param eventsystem: pointer to managing Eventsystem object
     * @param name : componet name as string
     * @param initval: initial value
     * @param readOnly: set true for readonly component
     */
    cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QVariant initval, Direction p_direction=Direction::inOut);
    ~cVeinModuleComponent();

    /**
     * @brief setValueByEvent
     * This class is meant for use in generic EventSystem classes and should be used in
     * processEvent functions.
     * @param value
     */
    void setValueByEvent(QVariant p_value);

    QVariant getValue();
    QString getName();

    QPointer<QValidator> getValidator() const;
    void setValidator(const QPointer<QValidator> &p_validator);

public slots:
    void setValue(QVariant p_value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification
    void removeComponent() override;

protected:
    int m_nEntityId;
    QPointer<VeinEvent::EventSystem> m_pEventSystem;
    QString m_sName;
    QVariant m_vValue;
    Direction m_direction;
    QPointer<QValidator> m_validator;

protected:
    /**
     * @brief sendNotification
     * sends data to vein, if something changed
     * @param vcmd
     */
    virtual void sendNotification(VeinComponent::ComponentData::Command vcmd);
};
}

#endif // VEINMODULECOMPONENT_H
