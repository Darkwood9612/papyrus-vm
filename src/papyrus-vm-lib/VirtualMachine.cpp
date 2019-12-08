#include "VirtualMachine.h"

VirtualMachine::VirtualMachine(std::vector<std::shared_ptr<PexScript>> loadedScripts){
	this->allLoadedScripts = loadedScripts;
}

void VirtualMachine::RegisterFunction(std::string className, std::string functionName, FunctionType type, NativeFunction fn) {
	
	switch (type) {
	case FunctionType::GlobalFunction:

		nativeStaticFunction[functionName] = fn;
		break;
	case FunctionType::Method:

		std::pair<std::string, NativeFunction> item(std::make_pair(functionName, fn));
		nativeFunction[className].push_back(item);
		break;
	}
}

void VirtualMachine::AddObject(std::shared_ptr<IGameObject> self, std::vector<std::string> scripts, VarForBuildActivePex vars) {
	std::vector<ActivePexInstance> scriptsForObject;

	for (auto& baseScript : allLoadedScripts) {
		for (auto& nameNeedScript : scripts) {
			if (baseScript->source == nameNeedScript) {

				ActivePexInstance scriptInstance(baseScript, vars, this, VarValue((IGameObject*)self.get()), "");
				scriptsForObject.push_back(scriptInstance);
			}
		}
	}

	gameObjects[self] =  scriptsForObject;
}

void VirtualMachine::SendEvent(std::shared_ptr<IGameObject> self, const char* eventName, std::vector<VarValue> &arguments){

	for (auto& object : gameObjects) {
		if (object.first == self) {
			for (auto& scriptInstance : object.second) {
				auto fn = scriptInstance.GetFunctionByName(eventName);
				if (fn.valid) {
					scriptInstance.StartFunction(fn, arguments, eventName);
				}
			}
		}
	}
}

void VirtualMachine::SendEvent(ActivePexInstance* instance, const char* eventName, std::vector<VarValue>& arguments) {

				auto fn = instance->GetFunctionByName(eventName);
				if (fn.valid) {
					instance->StartFunction(fn, arguments, eventName);
				}
}

VarValue VirtualMachine::CallMethod(ActivePexInstance* instance, IGameObject *self, const char* methodName, std::vector<VarValue> &arguments){
	FunctionInfo function;
	
	std::string nameGoToState = "GotoState";
		std::string nameGetState = "GetState";

	if (methodName == nameGoToState || methodName == nameGetState) {
		function = instance->GetFunctionByName(methodName, "");
	}
	else function = instance->GetFunctionByName(methodName);
	
	if (function.valid) {
		return instance->StartFunction(function, arguments, methodName);
	}
	assert(false);
	return VarValue::None();
}

VarValue VirtualMachine::CallStatic(std::string className, std::string functionName, std::vector<VarValue> &arguments){
	
	VarValue result = VarValue::None();


	if (className != "") {
		if (nativeFunction.find(className) != nativeFunction.end()) {
			for (auto& function : nativeFunction[className]) {
				if (function.first == functionName) {
					NativeFunction func = function.second;
					result = func(VarValue::None(), arguments);
				}
			}
		}
	}
	else {
		if (nativeStaticFunction.find(functionName) != nativeStaticFunction.end()) {
			NativeFunction function = nativeStaticFunction[functionName];
			result = function(VarValue::None(), arguments);
		}
	}
	

	return result;
}

void VirtualMachine::RemoveObject(std::shared_ptr<IGameObject> self) {

}