
#pragma once

#include <QList>
#include <QMap>
#include <ve_eventsystem.h>
#include <veinmodulecomponent.h>
#include <veinmodulerpc.h>

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
class veinmoduleentity : public VeinEvent::EventSystem
{
    Q_OBJECT
public:
    typedef QSharedPointer<veinmoduleentity> Ptr;

    veinmoduleentity(int p_entityId,QObject *p_parent=nullptr);
    ~veinmoduleentity();
    /**
     * @brief hasComponent checks if component is in m_componentList
     * @param [in] name: the component name
     * @return true if component was found
     */
    bool hasComponent(const QString name);
    /**
     * @brief createComponent creates a new vein component
     * @param[in] name: the component name
     * @param[in] initval: the initial component value
     * @param[in] p_readOnly: if true only this entit can write this component
     * @return the component handler
     *
     * it is recommended to store the handler in vfcompproxy.
     */
    cVeinModuleComponent::Ptr  createComponent(QString name, QVariant initval, bool p_readOnly=false);
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
    cVeinModuleRpc::Ptr createRpc(QObject *p_object, QString p_funcName, QMap<QString,QString> p_parameter);
    /**
     * @brief processEvent
     * @param t_event
     * @return
     *
     * do not use!
     */
    bool processEvent(QEvent *t_event) override;
    /**
     * @brief watchComponent
     * @param p_EntityId
     * @param p_componentName
     *
     * @todo implement proxy object as return
     */
    void watchComponent(int p_EntityId, const QString &p_componentName);
    /**
     * @brief unWatchComponent
     * @param p_EntityId
     * @param p_componentName
     * @return
     */
    bool unWatchComponent(int p_EntityId, const QString &p_componentName);
private:
    bool processCommandEvent(VeinEvent::CommandEvent *p_cEvent);
private:
    /**
     * @brief m_componentList
     * List with all regitered comoponents
     */
    QMap<QString,cVeinModuleComponent::Ptr> m_componentList;
    /**
     * @brief m_rpcList
     * List with all registered rpcs
     */
    QMap<QString,cVeinModuleRpc::Ptr> m_rpcList;
    /**
     * @brief m_watchList
     * List with all wathced components
     */
    QMap<int,QSet<QString>> m_watchList;
    //QMap<QString,cVeinModuleRpc> m_activeObjectList;
    /**
     * @brief m_entityId
     * The entites unique entity id
     */
    int m_entityId;
public:
    QMap<QString, cVeinModuleComponent::Ptr> getComponentList() const;
    void setComponentList(const QMap<QString, cVeinModuleComponent::Ptr> &value);
    QMap<QString, cVeinModuleRpc::Ptr> getRpcList() const;
    void setRpcList(const QMap<QString, cVeinModuleRpc::Ptr> &value);
    QMap<int, QSet<QString> > getWatchList() const;
    void setWatchList(const QMap<int, QSet<QString> > &watchList);
    int getEntitiyId() const;
signals:
    void sigWatchedComponentChanged(int p_entityId,QString p_componentName,QVariant p_value);
public slots:
    void initModule();
};

}
