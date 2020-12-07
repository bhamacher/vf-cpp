#ifndef VEINCOMPPROXY_H
#define VEINCOMPPROXY_H



#include "veinmodulecomponent.h"


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
template <class T> class VeinCompProxy{
public:
    VeinCompProxy():
        m_component(nullptr)
    {

    };

    VeinCompProxy(cVeinModuleComponent::Ptr obj){
        m_component=obj;
    };

    VeinCompProxy(VeinCompProxy &obj){
        m_component=obj.m_component;
    };

    ~VeinCompProxy()
    {
        m_component.clear();
    };


    T value(){
        if(m_component != nullptr){
            return m_component->getValue().value<T>();
        }
        return T();
    };

    /**
     * @brief setValue
     * @param p_val vein value of type T
     */
    void setValue(T p_val){
       if(m_component != nullptr){
            m_component->setValue(p_val);
       }
    };


    /**
     * @brief component
     * Returns the vein component this template class is
     * wrapped around
     * @return cVeinComponent
     */
    cVeinModuleComponent::Ptr component() const
    {
        return m_component;
    };

    /**
     * @brief operator =
     * set vein value with = operator
     * @param other: the vein Value
     * @return new VeinCompProxy
     */
    VeinCompProxy<T>& operator=(const T& other){
        if(m_component != nullptr){
            setValue(other);
        }
        return *this;
    };

    VeinCompProxy<T>& operator=(cVeinModuleComponent::Ptr other){
        m_component=other;
        return *this;
    };

    /**
     * @brief operator *
     * like function component
     * @return
     */
    cVeinModuleComponent::Ptr operator*(){
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

    cVeinModuleComponent::Ptr m_component;

};
}






#endif // VEINCOMPPROXY_H
