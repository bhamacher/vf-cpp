#include "veinproxyvalidator.h"

using namespace VfCpp;

VeinProxyValidator::VeinProxyValidator(QPointer<VeinProxyComp> p_proxyComp, QObject *parent) : QValidator(parent), m_proxyComp(p_proxyComp)
{

}

void VeinProxyValidator::fixup(QString &input) const
{

}

QValidator::State VfCpp::VeinProxyValidator::validate(QString &p_input, int &p_pos) const
{
    if(!m_proxyComp.isNull()){
        m_proxyComp->setValue(QVariant(p_input));
    }
    return QValidator::State::Invalid;
}
