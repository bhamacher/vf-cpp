#ifndef VEINMODULECOMPONENT_H
#define VEINMODULECOMPONENT_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QUuid>

#include <vcmp_componentdata.h>
#include <vcmp_errordata.h>
#include <ve_eventsystem.h>

namespace VfCpp {
/**
 * @brief The cVeinModuleComponent class
 *
 * Abstraction for vein components
 */
class cVeinModuleComponent: public QObject
{
    Q_OBJECT
public:

    typedef  QSharedPointer< cVeinModuleComponent > Ptr;

    cVeinModuleComponent(int entityId, VeinEvent::EventSystem *eventsystem, QString name, QVariant initval, bool readOnly=false);
    ~cVeinModuleComponent();

    void setValueByEvent(QVariant value);

    QVariant getValue();
    QString getName();

signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values
    void sigValueQuery(QVariant); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter

public slots:
    void setValue(QVariant value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification

protected:
    int m_nEntityId;
    VeinEvent::EventSystem *m_pEventSystem;
    QString m_sName;
    QVariant m_vValue;
    bool m_readOnly;

protected:

    virtual void sendNotification(VeinComponent::ComponentData::Command vcmd);
};
}

#endif // VEINMODULECOMPONENT_H
