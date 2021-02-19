#ifndef VEINLAMBDAVALIDATOR_H
#define VEINLAMBDAVALIDATOR_H

#include <QValidator>
#include <functional>

class VeinLambdaValidator : public QValidator
{
    Q_OBJECT
public:
    VeinLambdaValidator(std::function<QValidator::State (QVariant p_value)> p_func, QObject* p_parent=nullptr);

    void fixup(QString &input) const override;
    QValidator::State validate(QString &p_input, int &p_pos) const override;
private:
    std::function<QValidator::State (QVariant p_value)> m_func;

};

#endif // VEINLAMBDAVALIDATOR_H
