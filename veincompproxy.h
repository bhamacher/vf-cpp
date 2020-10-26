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
    VeinCompProxy(){

    }

    VeinCompProxy(QSharedPointer<cVeinModuleComponent> obj){
        m_component=obj;
    };

    VeinCompProxy(VeinCompProxy &obj){
        m_component=obj.m_component;
    };


    T value(){
        return m_component->getValue().value<T>();
    };

    void setValue(T p_val){
        m_component->setValue(p_val);
    };

    QSharedPointer<cVeinModuleComponent> component() const
    {
        return m_component;
    };

    VeinCompProxy& operator=(const T& other){
        setValue(other);
    };

    VeinCompProxy& operator=(const  QSharedPointer<cVeinModuleComponent> other){
        m_component=other;
    };

    T operator*(){
        return m_component;
    };

    bool operator!=(const T& val){
        if(val != value()){
            return true;
        }else{
            return false;
        }
    };

    bool operator==(const T& val){
        if(val == value()){
            return true;
        }else{
            return false;
        }
    };

    bool operator>=(const T& val){
        if(val >= value()){
            return true;
        }else{
            return false;
        }
    };

    bool operator<=(const T& val){
        if(val <= value()){
            return true;
        }else{
            return false;
        }
    };

    bool operator<(const T& val){
        if(val > value()){
            return true;
        }else{
            return false;
        }
    };

    bool operator>(const T& val){
        if(val < value()){
            return true;
        }else{
            return false;
        }
    };


private:
    /**
     * @brief operator new
     * @param size
     * @return
     *
     * objects of this type should not be created with new.
     */
    void* operator new(size_t size){
        return nullptr;
    }

    QSharedPointer<cVeinModuleComponent> m_component;

};
}






#endif // VEINCOMPPROXY_H
