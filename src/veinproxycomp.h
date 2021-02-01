#ifndef VEINPROXYCOMP_H
#define VEINPROXYCOMP_H

#include <QObject>

#include <vcmp_componentdata.h>
#include <ve_eventsystem.h>

#include "veinabstractcomponent.h"

namespace VfCpp {



class VeinProxyComp : public VeinAbstractComponent
{
    Q_OBJECT
public:
    typedef  QSharedPointer< VeinProxyComp > Ptr;
    typedef  QWeakPointer< VeinProxyComp > WPtr;

    enum class TakeOver{
        direct,                     // takes the value but does not create sigValueChanged
        directWithNotification,     // takes the value and creates sigValueChanged (ends up with two signals)
        onNotification              // only send the transaction and waits for confirmation
                                    // Get value will return the old value until the new value is confirmed.
                                    // That might be never!
    };

    /**
     * @brief VeinProxyComp Objects of this class represent components owned by other entities
     * @param entityId the entite id of the proxy owner
     * @param eventsystem the eventsystem handling this poxycomponent (veinmoduleentity)
     * @param subEntName the entitie where the component originates
     * @param SubCompName the name of the original component
     * @param initval the proxy component value until the first notification comes in
     */
    explicit VeinProxyComp(int p_entityId, QPointer<veinmoduleentity> p_eventsystem,int p_subEntName ,QString p_subCompName,QString p_proxyCompName, QVariant p_initval=QVariant(), TakeOver p_defaultTake=TakeOver::onNotification);
    void init();
    /**
     * @brief setValueByEvent
     * This class is meant for use in generic EventSystem classes and should be used in
     * processEvent functions.
     * @param value
     */
    void setValueByEvent(QVariant value);

    QVariant getValue() override;
    QString getName() override;

    int getSubEntityId() const;
    void setSubEntityId(int subEntityId);

public slots:
    void setValue(QVariant p_value, TakeOver p_takeOver); // here we have to emit event for notification
    void setValue(QVariant p_value) override;
    void setError(); // here we have to emit event for error notification

signals:


private:
    int m_entityId;
    QPointer<veinmoduleentity> m_pEventSystem;
    int m_subEntityId;
    QString m_subComponentName;
    QVariant m_value;
    TakeOver m_defaultTake;
    cVeinModuleComponent::WPtr m_realComponent;

protected:
    /**
     * @brief sendNotification
     * sends data to vein, if something changed
     * @param vcmd
     */
    virtual void sendTransaction(VeinComponent::ComponentData::Command vcmd, QVariant p_value);

};

}

#endif // VEINPROXYCOMP_H
