# vf-cpp
Easy to use api to create entities with components and rpcs

This repository improves simplicity to create vein entities including 
components and rpcs in C++.

It is desinged to be used in the modulemanger application.

## How to use

1. Create your module

```
//yourModule.h

...
#include <vfmoduleentity.h>
#include <vfcompproxy.h>
...

class yourModule : public QObject{

public:
	...
	yourModule(QObject *parent = nullptr, int entityId = <yourDefault>);
	bool vf_export::initOnce();
	...
	//"your functions"
	...
	VfCpp::veinmoduleentity *getVeinEntity() const;
	void setVeinEntity(VfCpp::veinmoduleentity *value);
	
private:
	...
	//"your attributes"
	...
	bool m_isInitalized;
	VfCpp::veinmoduleentity *m_entity;
	VfCpp::VfCompProxy<QString> m_entityName;
	VfCpp::VfCompProxy< type > yourComp;
	...
public slot:
	...
	// function must be of this type!
	QVariant RPC_YourRPC(QVariantMap p_params);
	...
	
}

```

```
//yourModule.cpp
	
...	
	yourModule::yourModule(QObject *parent, int entityId){
		m_entity=new VfCpp::veinmoduleentity(entityId);
	}
	
	bool yourModule::initOnce(){
		if(m_m_isInitalized){
			retunr false;
		}
		yourComp = m_entity->createComponent("yourComp", type());
		m_entityName= m_entity->createComponent("entityName", QString(<entityName>));;
		// note parameters are not typesafe
		// note the function will not be called if not all parameters are set
		m_entity->createRpc(this,"RPC_YourRPC", VfCpp::cVeinModuleRpc::Param({"p_par1" : "type1", "p_par1" : "type2"}));
		return true;
	}
	
	QVariant yourModule::RPC_YourRPC(QVariantMap p_params){
	type1 par1=p_params["p_par1"]
	type2 par2=p_params["p_par2"]
	QVariant ret;
	...
	// Do something
	..
	
	
	return ret;	
	}
...
{
```

2. Add entity as subsytem to vein


```
// make sure to call code after app.exec()?
yourModule *yourModuleObj=new yourModule();
yourModuleObj->addSubsystem(yourModuleObj->getVeinEntity());
yourModuleObj->initOnce();
```


## How to call from qml


Create the following qml component. Then create one of these for each rpc you want to call.
Your can get the result with onReturnValChanged signale.


```
import VeinEntity 1.0
...
Item{
	id: root
	property QObject entity
	property var rpcName : "RPC_YourRPC"
	property var inProgress : False
	property var returnVal : ""
	property var rpcTrace : undefined
	
	function rpcCalle(parameters){
		root.rpcTrace=entity.invokeRPC(rpcName, parameters)
	}


  	Connections {
            target: root.entity
            onSigRPCFinished: {
                if(t_resultData["RemoteProcedureData::errorMessage"]) {
                    console.warn("RPC error:" << t_resultData["RemoteProcedureData::errorMessage"]);
                }else if(t_identifier === root.rpcTrace){
                    root.rpcTrace = undefined;
                    if(t_resultData["RemoteProcedureData::resultCode"] === 4) { //EINTR, the search was canceled
                        root.returnVal=undefined
                    }else{
                        root.returnVal=t_resultData["RemoteProcedureData::Return"];
                    }
                }
            }
            onSigRPCProgress: {
                if(t_identifier === searchProgressId) {
               	// not implemented yet
                }
            }
        }



}
```


For more information also visit: https://github.com/ZeraGmbH/vf-qml
