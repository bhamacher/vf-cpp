#include "veinproxyvalidator.h"

using namespace VfCpp;

VeinProxyValidator::VeinProxyValidator(QPointer<VeinProxyComp> p_proxyComp, QObject *parent) : QValidator(parent), m_proxyComp(p_proxyComp)
{

}

void VeinProxyValidator::fixup(QString &p_input) const
{
    Q_UNUSED(p_input);
}

QValidator::State VfCpp::VeinProxyValidator::validate(QString &p_input, int &p_pos) const
{
    Q_UNUSED(p_input);
    if(!m_proxyComp.isNull()){
        m_proxyComp->setValue(QVariant(p_input));
    }
    return QValidator::State::Invalid;
}
