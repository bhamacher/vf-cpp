#ifndef VEINPROXYCOMP_H
#define VEINPROXYCOMP_H

#include <QObject>
#include <QPointer>

#include <vcmp_componentdata.h>

#include "veinabstractcomponent.h"
#include "veinmodulecomponent.h"

namespace VfCpp {

#ifdef ModTest
#define d_direct 0
#define d_directWithNotification 2
#define d_onNotification 1
#else
#define d_direct 0
#define d_directWithNotification 1
#define d_onNotification 2
#endif

class VeinModuleEntity;

class VeinProxyComp : public VeinAbstractComponent
{
    Q_OBJECT
public:
    typedef  QSharedPointer< VeinProxyComp > Ptr;
    typedef  QWeakPointer< VeinProxyComp > WPtr;

    enum class TakeOver{
        direct=d_direct,                     // takes the value but does not create sigValueChanged
        directWithNotification=d_directWithNotification,     // takes the value and creates sigValueChanged (ends up with two signals)
        onNotification=d_onNotification             // only send the transaction and waits for confirmation
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
    explicit VeinProxyComp(int p_entityId, QPointer<VeinModuleEntity> p_eventsystem,int p_subEntName ,QString p_subCompName,QString p_proxyCompName, QVariant p_initval=QVariant(), TakeOver p_defaultTake=TakeOver::onNotification);
    ~VeinProxyComp();
    void init();
    /**
     * @brief setValueByEvent
     * This class is meant for use in generic EventSystem classes and should be used in
     * processEvent functions.
     * @param value
     */
    void setValueByEvent(QVariant p_value);

    QVariant getValue() override;
    QString getName() override;

    int getSubEntityId() const;
    void setSubEntityId(int p_subEntityId);

public slots:
    void setValue(QVariant p_value, TakeOver p_takeOver); // here we have to emit event for notification
    void setValue(QVariant p_value) override;
    void setError(); // here we have to emit event for error notification
    void removeComponent();

signals:


private:
    int m_entityId;
    QPointer<VeinModuleEntity> m_pEventSystem;
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
    virtual void sendTransaction(VeinComponent::ComponentData::Command p_vcmd, QVariant p_value);

};

}

#endif // VEINPROXYCOMP_H
