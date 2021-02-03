#ifndef VEINPROXYVALIDATOR_H
#define VEINPROXYVALIDATOR_H

#include <QObject>
#include <QValidator>

#include "veinproxycomp.h"

namespace VfCpp {

class VeinProxyValidator : public QValidator
{
    Q_OBJECT
public:
    explicit VeinProxyValidator(QPointer<VeinProxyComp>,QObject *p_parent = nullptr);
    void fixup(QString &input) const override;
    QValidator::State validate(QString &p_input, int &p_pos) const override;
private:
    QPointer<VeinProxyComp> m_proxyComp;

};

}

#endif // VEINPROXYVALIDATOR_H
