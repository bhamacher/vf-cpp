#include "veinlambdavalidator.h"

VeinLambdaValidator::VeinLambdaValidator(std::function<QValidator::State (QVariant p_value)> p_func, QObject* p_parent): QValidator(p_parent),
    m_func(p_func)
{

}

void VeinLambdaValidator::fixup(QString &input) const
{

}

QValidator::State VeinLambdaValidator::validate(QString &p_input, int &p_pos) const
{
    return m_func(p_input);
}
