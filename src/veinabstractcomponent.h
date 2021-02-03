#ifndef VEINABSTRACTCOMPONENT_H
#define VEINABSTRACTCOMPONENT_H

#include <QObject>
#include <QVariant>
#include <QWeakPointer>

class VeinAbstractComponent : public QObject
{
    Q_OBJECT
public:
    typedef  QSharedPointer< VeinAbstractComponent > Ptr;
    typedef  QWeakPointer< VeinAbstractComponent > WPtr;

    explicit VeinAbstractComponent();

    /**
     * @brief setValueByEvent
     * This class is meant for use in generic EventSystem classes and should be used in
     * processEvent functions.
     * @param value
     */
    virtual void setValueByEvent(QVariant p_value) = 0;

    virtual QVariant getValue() = 0;
    virtual QString getName() = 0;

signals:
    void sigValueChanged(QVariant); // we connect here if we want to do something on changed values
    void sigValueQuery(QVariant); // we connect here if we want to read a value before returning data from storage ...perhaps with parameter

public slots:
    virtual void setValue(QVariant p_value) = 0; // here we have to emit event for notification
    virtual void setError() = 0; // here we have to emit event for error notification
    virtual void removeComponent() = 0;


signals:

};

#endif // VEINABSTRACTCOMPONENT_H
