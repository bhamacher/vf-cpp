#ifndef VEINPROXYCOMP_H
#define VEINPROXYCOMP_H

#include <QObject>

#include "veinabstractcomponent.h"

namespace VfCpp {



class VeinProxyComp : public VeinAbstractComponent
{
    Q_OBJECT
public:
    explicit VeinProxyComp();

    /**
     * @brief setValueByEvent
     * This class is meant for use in generic EventSystem classes and should be used in
     * processEvent functions.
     * @param value
     */
    void setValueByEvent(QVariant value);

    QVariant getValue();
    QString getName();

public slots:
    void setValue(QVariant value); // here we have to emit event for notification
    void setError(); // here we have to emit event for error notification

signals:

};

}

#endif // VEINPROXYCOMP_H
