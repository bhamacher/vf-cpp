#ifndef VeinSharedComp_H
#define VeinSharedComp_H



#include "veinabstractcomponent.h"


namespace VfCpp {


/**
 *  VfCompProxy
 *
 *  Create Components as Attributes and share them with other
 *  classes.
 *
 *  All copys of this object will have the same value.
 * (This is only the case, if vfmoduleentity is used to create
 * the vein compontent)
 *
 * use as follows:
 * VfCompProxy<type> attribute=m_entity->createComponent("attributeName",type());
 *
 * Create new reference:
 * VfCompProxy<type> attribute2 = attribute
 *
 * Get Value
 * attribute.value()
 * or
 * *attribute
 */
template <class T> class VeinSharedComp{
public:
    VeinSharedComp():
        m_component(nullptr)
    {

    };

    VeinSharedComp(VeinAbstractComponent::WPtr obj){
        m_component=obj;
    };

    VeinSharedComp(VeinSharedComp &obj){
        m_component=obj.m_component;
    };

    ~VeinSharedComp()
    {
        m_component.clear();
    };


    T value(){
        if(m_component != nullptr){
            return m_component.toStrongRef()->getValue().value<T>();
        }
        return T();
    };

    /**
     * @brief setValue
     * @param p_val vein value of type T
     */
    void setValue(T p_val){
        if(m_component != nullptr){
            m_component.toStrongRef()->setValue(p_val);
        }
    };

    void removeComponent(){
        if(!m_component.isNull()){
            m_component.toStrongRef()->removeComponent();
        }

    }


    /**
     * @brief component
     * Returns the vein component this template class is
     * wrapped around
     * @return cVeinComponent
     */
    VeinAbstractComponent::WPtr component() const
    {
        return m_component;
    };

    /**
     * @brief operator =
     * set vein value with = operator
     * @param other: the vein Value
     * @return new VeinSharedComp
     */
    VeinSharedComp<T>& operator=(const T& other){
        if(m_component != nullptr){
            setValue(other);
        }
        return *this;
    };

    VeinSharedComp<T>& operator=(VeinAbstractComponent::WPtr other){
        m_component=other;
        return *this;
    };

    /**
     * @brief operator *
     * like function component
     * @return
     */
    VeinAbstractComponent::WPtr operator*(){
        return m_component;
    };

    bool operator!=(const T& val){
        if(val != value()){
            return true;
        }
        return false;
    };

    /**
     * @brief operator ==
     * compare vein value with variable of type T
     * @param val
     * @return true/false
     */
    bool operator==(const T& val){
        if(val == value()){
            return true;
        }else{
            return false;
        }
    };

    /**
     * @brief operator >=
     * compare vein value with variable of type T
     * @param val
     * @return true/false
     */
    bool operator>=(const T& val){
        if(val >= value()){
            return true;
        }else{
            return false;
        }
    };
    /**
     * @brief operator <=
     * compare vein value with variable of type T
     * @param val
     * @return true/false
     */
    bool operator<=(const T& val){
        if(val <= value()){
            return true;
        }else{
            return false;
        }
    };

    /**
     * @brief operator <
     * compare vein value with variable of type T
     * @param val
     * @return true/false
     */
    bool operator<(const T& val){
        if(val > value()){
            return true;
        }else{
            return false;
        }
    };

    /**
     * @brief operator >
     * compare vein value with variable of type T
     * @param val
     * @return true/false
     */
    bool operator>(const T& val){
        if(val < value()){
            return true;
        }else{
            return false;
        }
    };

    /**
     * @brief operator new
     * The new operator is deleted.
     */
    void* operator new(std::size_t size) = delete ;

private:

    VeinAbstractComponent::WPtr m_component;

};
}






#endif // VeinSharedComp_H
