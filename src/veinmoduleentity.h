
#pragma once

#include <QList>
#include <QMap>
#include <QPointer>
#include <QValidator>
#include <ve_eventsystem.h>

#include "veinmodulecomponent.h"
#include "veinproxycomp.h"
#include "veinmodulerpc.h"
#include "veinrpcfuture.h"

namespace VfCpp {



/**
 * @brief The veinmoduleentity class
 *
 * Abstraction for vein entities.
 * Manages entities including all components, rpcs and events.
 *
 * Must be added to Eventsystem manually.
 *
 */
class VeinModuleEntity : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    typedef QSharedPointer<VeinModuleEntity> Ptr;
    typedef QWeakPointer<VeinModuleEntity> WPtr;

    VeinModuleEntity(int p_entityId,QObject *p_parent=nullptr);
    ~VeinModuleEntity();
    /**
     * @brief hasComponent checks if component is in m_componentList
     * @param [in] name: the component name
     * @return true if component was found
     */
    bool hasComponent(const QString p_name);
    /**
     * @brief createComponent creates a new vein component
     * @param[in] name: the component name
     * @param[in] initval: the initial component value
     * @param[in] p_readOnly: if true only this entit can write this component
     * @return the component handler
     *
     * it is recommended to store the handler in vfcompproxy.
     */
    cVeinModuleComponent::WPtr  createComponent(QString p_name, QVariant p_initval, cVeinModuleComponent::Direction p_direction = cVeinModuleComponent::Direction::inOut, QValidator *p_validator=nullptr);

    bool removeComponent(const QString &p_name);
    /**
     * @brief createRpc creates a vein rpc
     * @param p_object: the object handling the rpc
     * @param p_funcName: the mehtod name in this object
     * @param p_parameter: the expected parameters key : name value: type
     * @return the rpc handler
     *
     * There is no reason to use the rpc handler object
     *
     */
    cVeinModuleRpc::WPtr createRpc(QObject *p_object, QString p_funcName, QMap<QString,QString> p_parameter, bool p_thread=true);
    /**
     * @brief processEvent
     * @param t_event
     * @return
     *
     * do not use!
     */
    bool processEvent(QEvent *p_event) override;
    /**
     * @brief watchComponent
     * @param p_EntityId
     * @param p_componentName
     *
     * @todo implement proxy object as return
     */
    VeinProxyComp::WPtr createProxyComponent(int p_SubEntityId, const QString &p_SubComponentName,const QString p_proxyCompName,VeinProxyComp::TakeOver p_takeOver=VeinProxyComp::TakeOver::onNotification);
    /**
     * @brief unWatchComponent
     * @param p_EntityId
     * @param p_componentName
     * @return true on success
     */
    bool removeProxyComponent(int p_entitId,const QString &p_componentName);

    VeinRpcFuture::Ptr invokeRPC(int p_entityId,const QString &p_procedureName, const QVariantMap &p_parameters);
public:
    QMap<QString, cVeinModuleComponent::Ptr> getComponentList() const;
    void setComponentList(const QMap<QString, cVeinModuleComponent::Ptr> &p_value);
    QMap<QString, cVeinModuleRpc::Ptr> getRpcList() const;
    void setRpcList(const QMap<QString, cVeinModuleRpc::Ptr> &p_value);
    QMap<int, QMap<QString,VeinProxyComp::Ptr> > getWatchList() const;
    void setWatchList(const QMap<int, QMap<QString,VeinProxyComp::Ptr> >  &p_watchList);
    int getEntitiyId() const;
public slots:
    /**
     * @brief initModule
     * call once after constructor
     */
    void initModule();
signals:
    /**
     * @brief sigEvent
     * processEvent emits sigEvent for usecases where we want to handle
     * events the classic way. sigEvent takes a copy and not a pointer.
     * This will have some performance impact but is much safer.
     * Furthermore please note that only commandEvents will be forwarded.
     * You can still use sigSendEvent to transmit data into the system.
     * @param t_event
     */
    void sigEvent(QEvent t_event);

private:
    bool processCommandEvent(VeinEvent::CommandEvent *p_cEvent);

    bool processComponentData(VeinEvent::CommandEvent *p_cEvent);
    bool processRpcData(VeinEvent::CommandEvent *p_cEvent);
private:
    /**
     * @brief m_componentList
     * List with all registered components
     */
    QMap<QString,cVeinModuleComponent::Ptr> m_componentList;
    /**
     * @brief m_rpcList
     * List with all registered rpcs
     */
    QMap<QString,cVeinModuleRpc::Ptr> m_rpcList;
    /**
     * @brief m_watchList
     * List with all watched components (proxyComponents)
     */
    QMap<int,QMap<QString,VeinProxyComp::Ptr>> m_watchList;
    //QMap<QString,cVeinModuleRpc> m_activeObjectList;
    /**
     * @brief m_futureList
     * List with all futures bound to called RPCs
     */
    // QMap<entityId,QMap<rpcname,QMap<rpcUniqueId,RpcFutureObject>>>
    QMap<int, QMap<QString,QMap<QUuid,VeinRpcFuture::Ptr>>> m_futureList;

    /**
     * @brief m_entityId
     * The entites unique entity id
     */
    int m_entityId;


};

}
