#include "VirtualMachine.h"

std::string VirtualMachine::GetSourceFileName(){
	return structure.source;
}

std::vector<std::string> VirtualMachine::GetObjectsListForLink() {
	std::vector<std::string> listObjects;
	bool bContinue = false;

	for (auto& object : structure.objectTable.m_data) {
		for (auto& prop : object.properties) {
			std::string type = GetStringByIndex(prop.type);

			for (auto &_type : listObjects) {
				if (_type == type)
					bContinue = true;
			}

			if (!bContinue)
				listObjects.push_back(type);
			else bContinue = false;
		}
	}

	return listObjects;
}

void VirtualMachine::FillObjectsLinks(std::vector<std::shared_ptr<VirtualMachine>> &listLinks){

	for (auto &prop : structure.objectTable.m_data[0].properties) {
		uint32_t link;
		for (auto &list : listLinks) {
			if (list->GetSourceFileName() == GetStringByIndex(prop.type)+".psc") {
				GetVariableValueByName(GetStringByIndex(prop.autoVarName))->linkObject = list;
			}
		}
	}
}



VMScript* VirtualMachine::GetScriptStructure() {
	return &structure;
}

std::pair<uint16_t, VarValue> VirtualMachine::RunHeandler(const std::string& name, int action, const std::vector<VarValue>& functionArgs) {
	FunctionInfo func;
	OpcodeContext context;

	for (auto& object : structure.objectTable.m_data) {
		for (auto prop : object.properties) {
			if(GetStringByIndex(prop.name) == name  && action == func.kFlags_Read){
				func = prop.readHandler;
				break;
			}
			if (GetStringByIndex(prop.name) == name && action == func.kFlags_Write) {
				func = prop.writeHandler;
				break;
			}
		}
	}
		
	return StartFunction(func, context, functionArgs);
}

std::pair<uint16_t, VarValue> VirtualMachine::RunFunction(const std::string &name, const std::vector<VarValue>& functionArgs){
	uint16_t index = GetStringIndex(name);
	
	assert(index < structure.stringTable.m_data.size());

	OpcodeContext context;

	FunctionInfo func = GetFunctionByName(index);
	return StartFunction(func, context, functionArgs);
}

void VirtualMachine::RegisterFunction(const std::string &name, VMNativeFunction fn) {
	this->nativeMethods[name] = fn;
}

std::string VirtualMachine::GetString(const VarValue &stringValue) {
	if (stringValue.type != VarValue::kType_String) {
		return "";
	}
	return GetStringByIndex(stringValue.data.stringRef);
}

uint16_t VirtualMachine::GetStringIndex(std::string name) {
	for (uint16_t i = 0; i < structure.stringTable.m_data.size(); ++i) {
		if (name == structure.stringTable.m_data[i]) {
			return i;
		}
	}
	return -1;
}

std::string VirtualMachine::GetStringByIndex(uint16_t index) {
	if (index < structure.stringTable.m_data.size())
		return structure.stringTable.m_data[index];
	assert(false);
	return "";
}

FunctionInfo VirtualMachine::GetFunctionByName(uint16_t name) {
	FunctionInfo function;

	for (auto &object : structure.objectTable.m_data) {
		for (auto &state : object.states) {
			for (auto &func : state.functions) {
				if (func.name == name) {
					return func.function;
				}
			}
		}
	}
	
	assert(false);
	return function;
}

VarValue& VirtualMachine::GetValue(OpcodeContext& context, VarValue &value) {
	if (value.type == VarValue::kType_Identifier) {
		return GetVariableValueByName(context, value.data.stringRef);
	}
	else return value;
}

uint8_t VirtualMachine::GetTypeByName(uint16_t typeRef)
{
	std::string name = structure.stringTable.m_data[typeRef];
	std::transform(name.begin(), name.end(), name.begin(), tolower);
	if (name == "int") {
		return VarValue::kType_Integer;
	}
	if (name == "float") {
		return VarValue::kType_Float;
	}
	if (name == "string") {
		return VarValue::kType_String;
	}
	if (name == "bool") {
		return VarValue::kType_Bool;
	}
	if (name == "identifier") {
		assert(0);
	}
	if (name == "string[]") {
		return VarValue::kType_StringArray;
	}
	if (name == "int[]") {
		return VarValue::kType_IntArray;
	}
	if (name == "float[]") {
		return VarValue::kType_FloatArray;
	}
	if (name == "bool[]") {
		return VarValue::kType_BoolArray;
	}
	if (name.find("[]") != std::string::npos) {
		return VarValue::kType_ObjectArray;
	}
	return VarValue::kType_Object;

}

 VarValue& VirtualMachine::GetVariableValueByName(OpcodeContext& context, const uint16_t name) {
	 for (auto &var : allObjectVariables) {
		 if (var.name == name) {
			 return var.value;
		 }
	 }

	for (auto &var : context.locals) {
		if (var.first == name) {
			return var.second;
		}	
	}
	assert(false);
	static VarValue _;
	return _;
}

 VarValue* VirtualMachine::GetVariableValueByName( const std::string &name) {
	 uint16_t index = GetStringIndex(name);

	 assert(index < structure.stringTable.m_data.size());

	 for (auto& var : allObjectVariables) {
		 if (var.name == index) {
			 return &var.value;
		 }
	 }


	 return nullptr;
 }

 std::pair<uint16_t, VarValue> VirtualMachine::StartFunction(FunctionInfo &function, OpcodeContext &context,const std::vector<VarValue> &functionArgs){
	std::vector<std::pair<uint16_t, VarValue>> locals;
	
	for (auto& var : function.locals) {
		locals.push_back({var.name, VarValue(GetTypeByName(var.type))});
	}

	//assert(function.params.size() >= 1);

	for (size_t i = 0; i < functionArgs.size(); ++i) {
		locals.push_back({ function.params[i].name, functionArgs[i] });
		assert(locals.back().second.type == functionArgs[i].type);
	}


		for (size_t i = functionArgs.size(); i < function.params.size(); ++i) {
			const auto& var_ = function.params[i];
			locals.push_back({ var_.name, VarValue(GetTypeByName(var_.type)) });
		}


	for (auto &var : locals) {
		var.second = GetValue(context, var.second);
	}

	std::pair<uint16_t, VarValue> data;
	auto opCode = function.code.instructions;

		for (size_t line = 0; line < opCode.size(); ++line) {
		
		OpcodeContext context_(locals, opCode[line].args);
		data = ImplementateOpcode(context_, opCode[line]);
		
		if (needJump) {
			needJump = false;
			line += data.second.data.i;
		}
		
		if (needReturn) {
			needReturn = false;
			return data;
		}

		for (auto& localVar : locals) {
			if (data.first == localVar.first)
				localVar.second = data.second;
		}
	}
	
		locals.clear();
	return data;
 }

 std::pair<uint16_t, VarValue> VirtualMachine::ImplementateOpcode(OpcodeContext &context, FunctionCode::Instruction& opCode){

	 switch (opCode.op) {
	case Opcodes::op_Nop:
		return nop(context);

	case Opcodes::op_iAdd:
		return iAdd(context);

	case Opcodes::op_fAdd:
		return fAdd(context);

	case Opcodes::op_iSub:
		return iSub(context);

	case Opcodes::op_fSub:
		return fSub(context);

	case Opcodes::op_iMul:
		return iMul(context);

	case Opcodes::op_fMul:
		return fMul(context);

	case Opcodes::op_iDiv:
		return iDiv(context);

	case Opcodes::op_fDiv:
		return fDiv(context);

	case Opcodes::op_iMod:
		return iMod(context);

	case Opcodes::op_Not:
		return _not(context);

	case Opcodes::op_iNeg:
		return iNeg(context);

	case Opcodes::op_fNeg:
		return fNeg(context);

	case Opcodes::op_Assign:
		return assign(context); 

	case Opcodes::op_Cast:
		return cast(context);

	case Opcodes::op_Cmp_eq:
		return cmp_eq(context);

	case Opcodes::op_Cmp_lt:
		return cmp_lt(context);

	case Opcodes::op_Cmp_le:
		return cmp_le(context);

	case Opcodes::op_Cmp_gt:
		return cmp_gt(context);
	case Opcodes::op_Cmp_ge:
		return cmp_ge(context);

	case Opcodes::op_Jmp:
		return jmp(context);

	case Opcodes::op_Jmpt:
		return jmpt(context);

	case Opcodes::op_Jmpf:
		return jmpf(context);

	case Opcodes::op_CallMethod:
		return callMethod(context);

	case Opcodes::op_CallParent:
		return callParent(context);

	case Opcodes::op_CallStatic:
		return callStatic(context);

	case Opcodes::op_Return:
		return _return(context);

	case Opcodes::op_StrCat:
		return 	strCat(context);

	case Opcodes::op_PropGet:
		return propGet(context);

	case Opcodes::op_PropSet:
		return propSet(context);

	case Opcodes::op_Array_Create:
		return arrayCreate(context);

	case Opcodes::op_Array_Length:
		return arrayLength(context);

	case Opcodes::op_Array_GetElement:
		return 	arrayGetElement(context);

	case Opcodes::op_Array_SetElement:
		return arraySetElement(context);

	case Opcodes::op_Array_FindElement:
		return arrayFindElement(context);

	case Opcodes::op_Array_RfindElement:
		return 	arrayRFindElement(context);
	}
	assert(0);
	return std::pair<uint16_t, VarValue>();
}

#pragma region CreateSctructure

void VirtualMachine::Read() {

	char temp;

	std::ifstream File(path, std::ios::binary);

	if (File.is_open()) {

		File.seekg(0, std::ios_base::end);//становимся в конец файла
		//SizeFileInBytes = File.tellg();//Получаем текущую позицию
		File.seekg(0, std::ios_base::beg);//становимся в начало файла

		//std::cout << "Размер файла (в байтах): " << SizeFileInBytes << std::endl;

		while (File.get(temp)) {
			arrayBytes.push_back(temp);
		}

	}
	else {
		throw std::runtime_error("Error open file: " + path);
	}

	File.close();
};



VirtualMachine::VirtualMachine(std::string path) {
		this->currentReadPositionInFile = 0;
		this->path = path;
		this->needJump = false;
		this->needReturn = false;
		Read();
		CreateScriptStructure(arrayBytes);
	}



	void VirtualMachine::CreateScriptStructure(std::vector<uint8_t> arrayBytes) {
		this->arrayBytes = arrayBytes;
		currentReadPositionInFile = 0;

		structure.header = FillHeader();

		structure.source = FillSource();
		structure.user = FillUser();
		structure.machine = FillMachine();

		structure.stringTable = FillStringTable();
		structure.debugInfo = FillDebugInfo();
		structure.userFlagTable = FillUserFlagTable();
		structure.objectTable = FillObjectTable();
		allObjectVariables = structure.objectTable.m_data[0].variables;
		
	}

	ScriptHeader VirtualMachine::FillHeader() {
		ScriptHeader Header;

		Header.Signature = Read32_bit();	// 00	FA57C0DE
		Header.VerMajor = Read8_bit();		// 04	03
		Header.VerMinor = Read8_bit();		// 05	01
		Header.GameID = Read16_bit();		// 06	0001
		Header.BuildTime = Read64_bit();	// 08	time_t

		return Header;
	}

	std::string VirtualMachine::FillSource() {
		std::string source;
		int SizeString = Read16_bit();
		source = ReadString(SizeString);
		return source;
	}

	std::string VirtualMachine::FillUser() {
		std::string user;

		int SizeString = Read16_bit();
		user = ReadString(SizeString);

		return user;
	}

	std::string VirtualMachine::FillMachine() {
		std::string machine;

		int SizeString = Read16_bit();
		machine = ReadString(SizeString);

		return machine;
	}




	StringTable VirtualMachine::FillStringTable() {
		StringTable stringTable;

		int SizeStringTable = Read16_bit();

		for (int i = 0; i < SizeStringTable; i++) {
			int SizeString = Read16_bit();
			stringTable.m_data.push_back(ReadString(SizeString));
		}

		return stringTable;
	}

	DebugInfo VirtualMachine::FillDebugInfo() {
		DebugInfo debugInfo;


		debugInfo.m_flags = Read8_bit();
		debugInfo.m_sourceModificationTime = Read64_bit();

		int FunctionCount = Read16_bit();

		for (int i = 0; i < FunctionCount; i++) {
			DebugInfo::DebugFunction info;
			info = FillDebugFunction();
			debugInfo.m_data.push_back(info);
		}

		return debugInfo;
	}


	DebugInfo::DebugFunction VirtualMachine::FillDebugFunction() {
		DebugInfo::DebugFunction Fdebug;

		Fdebug.objName = Read16_bit();
		Fdebug.stateName = Read16_bit();
		Fdebug.fnName = Read16_bit();
		Fdebug.type = Read8_bit();

		int InstrunctionCount = Read16_bit();
		for (int i = 0; i < InstrunctionCount; i++) {
			Fdebug.lineNumbers.push_back(Read16_bit());
		}

		return Fdebug;
	}

	UserFlagTable VirtualMachine::FillUserFlagTable() {
		UserFlagTable userFlagTable;

		int UserFlagCount = Read16_bit();

		for (int i = 0; i < UserFlagCount; i++) {
			UserFlagTable::UserFlag flag;
			flag = FillUserFlag();
			userFlagTable.m_data.push_back(flag);
		}

		return userFlagTable;
	}

	UserFlagTable::UserFlag VirtualMachine::FillUserFlag() {
		UserFlagTable::UserFlag flag;

		flag.name = Read16_bit();
		flag.idx = Read8_bit();

		return flag;
	}

	ObjectTable VirtualMachine::FillObjectTable() {
		ObjectTable objectTable;

		int ObjectCount = Read16_bit();
		
		for (int i = 0; i < ObjectCount; i++) {
			ObjectTable::Object object;
			object = FillObject();
			objectTable.m_data.push_back(object);
		}

		return objectTable;
	}

	ObjectTable::Object VirtualMachine::FillObject(){
		ObjectTable::Object object;

		object.NameIndex = Read16_bit();

		Read32_bit();//	after remove size includes itself for some reason, hence size-4

		object.parentClassName = Read16_bit();
		object.docstring = Read16_bit();
		object.userFlags = Read32_bit();
		object.autoStateName = Read16_bit();

		int numVariables = Read16_bit();

		for (int i = 0; i < numVariables; i++) {
			object.variables.push_back(FillVariable());
		}

		int	numProperties = Read16_bit();

		for (int i = 0; i < numProperties; i++) {
			object.properties.push_back(FillProperty());
		}

		int	numStates = Read16_bit();

		for (int i = 0; i < numStates; i++) {
			object.states.push_back(FillState());
		}

		return object;
	}

	ObjectTable::Object::VarInfo VirtualMachine::FillVariable(){
		ObjectTable::Object::VarInfo Var;

		Var.name = Read16_bit();
		Var.typeName = Read16_bit();
		Var.userFlags = Read32_bit();
		Var.value = FillVariableData();

		return Var;
	}
	VarValue VirtualMachine::FillVariableData() {
		VarValue Data;

		

		Data.type = Read8_bit();

		if (Data.type > 10) {
			int b = 1;
		}

		switch ((int)Data.type) {
		case Data.kType_Object:
			Data.data.id = 0;
			break;
		case Data.kType_Identifier:
			Data.data.stringRef = Read16_bit();
			break;
		case Data.kType_String: 
			 Data.data.stringRef2 = Read16_bit();
			break;
		case Data.kType_Integer: 
			Data.data.i = Read32_bit();
			break;
		case Data.kType_Float: {
			uint32_t v = Read32_bit();
			Data.data.f = *(float*)& v;
			//Data.data.f = (int)Read32_bit();
		}
			break;
		case Data.kType_Bool: 
			Data.data.b = Read8_bit();
			break;
		case VarValue::kType_ObjectArray:
			Read32_bit();
			 break;
		case VarValue::kType_StringArray:
			Read32_bit();
			break;
		case VarValue::kType_IntArray:
			Read32_bit();
			break;
		case VarValue::kType_FloatArray:
			Read32_bit();
			break;
		case VarValue::kType_BoolArray:
			Read32_bit();
			break;
		default:
			assert(false);
		}

		return Data;
	}

	FunctionInfo VirtualMachine::FillFuncInfo() {
		FunctionInfo info;

		info.returnType = Read16_bit();
		info.docstring = Read16_bit();
		info.userFlags = Read32_bit();
		info.flags = Read8_bit();

		int	CountParams = Read16_bit();

		for (int i = 0; i < CountParams; i++) {
			FunctionInfo::ParamInfo  temp;
			temp.name = Read16_bit();
			temp.type = Read16_bit();
			info.params.push_back(temp);
		}

		int CountLocals = Read16_bit();

		for (int i = 0; i < CountLocals; i++) {
			FunctionInfo::ParamInfo  temp;
			temp.name = Read16_bit();
			temp.type = Read16_bit();
			info.params.push_back(temp);
		}

		int	CountInstructions = Read16_bit();

		info.code = FillFunctionCode(CountInstructions);

		return info;
	}

	FunctionCode VirtualMachine::FillFunctionCode(int CountInstructions) {
		FunctionCode funcCode;
		for (int i = 0; i < CountInstructions; i++) {
			FunctionCode::Instruction item;
			item.op = Read8_bit();

			if (item.op > 36)
				item.op = item.op;

			int numArguments = GetCountArguments(item.op);
		

				for (int i = 0; i < numArguments; i++) {

					item.args.push_back(FillVariableData());

					if (i == numArguments - 1) {
						if (additionalArguments) {
							int numAdditionalArgs = item.args[i].data.i;
							for (int i = 0; i < numAdditionalArgs; ++i) {
								item.args.push_back(FillVariableData());
							}
						}
						additionalArguments = false;
					}
				};
			
			
			funcCode.instructions.push_back(item);
		};
		return funcCode;
	}

	uint8_t VirtualMachine::GetCountArguments(uint8_t opcode){
		
		int count = numArgumentsForOpcodes[opcode];
		
		if (count < 0) {
			additionalArguments = true;
			count *= -1;
		}
	
			return count;
	}


	ObjectTable::Object::PropInfo VirtualMachine::FillProperty(){
		ObjectTable::Object::PropInfo prop;

		prop.name = Read16_bit();
		prop.type = Read16_bit();
		prop.docstring = Read16_bit();
		prop.userFlags = Read32_bit();
		prop.flags = Read8_bit();

		if ((prop.flags & 4) != 0) {
			prop.autoVarName = Read16_bit();
		}
		else prop.autoVarName;
		


		if ((prop.flags & 5) == 1) {
			prop.readHandler = FillFuncInfo();
		}
		else prop.readHandler;

		if ((prop.flags & 6) == 2) {
			prop.writeHandler = FillFuncInfo();
		}
		else prop.writeHandler;
		

		return prop;
	}


	ObjectTable::Object::StateInfo VirtualMachine::FillState(){
		ObjectTable::Object::StateInfo stateinfo;

		stateinfo.name = Read16_bit();

		int numFunctions = Read16_bit();

		for (int i = 0; i < numFunctions; ++i) {
			stateinfo.functions.push_back(FillStateFunction());
		}

		return stateinfo;
	}

	ObjectTable::Object::StateInfo::StateFunction VirtualMachine::FillStateFunction() {
		ObjectTable::Object::StateInfo::StateFunction temp;

		temp.name = Read16_bit();
		temp.function = FillFuncInfo();

		return temp;
	}

	uint8_t VirtualMachine::Read8_bit() {
		uint8_t temp;

		temp = arrayBytes[currentReadPositionInFile];
		currentReadPositionInFile++;

		return temp;
	}

	uint16_t VirtualMachine::Read16_bit() {
		uint16_t temp = NULL;

		for (int i = 0; i < 2; i++) {
			temp = temp * 256 + arrayBytes[currentReadPositionInFile];
			currentReadPositionInFile++;
		}

		return temp;
	}

	uint32_t VirtualMachine::Read32_bit()
	{
		uint32_t temp = NULL;

		for (int i = 0; i < 4; i++) {
			temp = temp * 256 + arrayBytes[currentReadPositionInFile];
			currentReadPositionInFile++;
		}
		return temp;
	}

	uint64_t VirtualMachine::Read64_bit()
	{
		uint64_t temp = NULL;

		for (int i = 0; i < 8; i++) {
			temp = temp * 256 + arrayBytes[currentReadPositionInFile];
			currentReadPositionInFile++;
		}
		return temp;
	}

	std::string VirtualMachine::ReadString(int Size) {
		std::string temp = "";

		for (int i = 0; i < Size; i++) {
			temp += (char)arrayBytes[currentReadPositionInFile];
			currentReadPositionInFile++;
		}

		return temp;
	}

#pragma endregion


#pragma region OperatorOverload

	VarValue VarValue::operator + (const VarValue &argument2) {
		VarValue var;
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_String:
				var.data.stringRef = this->data.stringRef + argument2.data.stringRef;
				var.type = this->kType_String;
			case VarValue::kType_Integer:
				 var.data.i = this->data.i + argument2.data.i;
				 var.type = this->kType_Integer;
				 return var;
			case VarValue::kType_Float:
				var.data.f = this->data.f + argument2.data.f;
				var.type = this->kType_Float;
				return var;
			}
		} assert(false);
		return var;
	}

	VarValue VarValue::operator - (const VarValue &argument2) {
		VarValue var;
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_Integer:
				var.data.i = this->data.i - argument2.data.i;
				var.type = this->kType_Integer;
				return var;
			case VarValue::kType_Float:
				var.data.f = this->data.f - argument2.data.f;
				var.type = this->kType_Float;
				return var;
			}
		} assert(false);
		return var;
	}

	VarValue VarValue::operator * (const VarValue &argument2){
		VarValue var;
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_Integer:
				var.data.i = this->data.i * argument2.data.i;
				var.type = this->kType_Integer;
				return var;
			case VarValue::kType_Float:
				var.data.f = this->data.f * argument2.data.f;
				var.type = this->kType_Float;
				return var;
			}
		} assert(false);
		return var;
	}

	VarValue VarValue::operator / (const VarValue &argument2){
		VarValue var;
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_Integer:
				var.data.i = 1;
				if(argument2.data.i != uint32_t(0) && this->data.i != uint32_t(0))
				var.data.i = this->data.i / argument2.data.i;
				var.type = this->kType_Integer;
				return var;
			case VarValue::kType_Float:
				var.data.f = 1.0f;
				if (argument2.data.f != float(0) && this->data.f != float(0))
				var.data.f = this->data.f / argument2.data.f;
				var.type = this->kType_Float;
				return var;
			}
		} assert(false);
		return var;
	}

	VarValue VarValue::operator % (const VarValue &argument2) {
		VarValue var;
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_Integer:
				var.data.i = 0;
				if (argument2.data.i != (uint32_t)0)
				var.data.i = this->data.i % argument2.data.i;
				var.type = this->kType_Integer;
				return var;
			}
		} assert(false);
		return var;
	}

	VarValue VarValue::operator ! () {
		VarValue var;
		if (this->type == this->kType_Bool) {

			var.data.b = !this->data.b;
			var.type = this->kType_Bool;
			return var;
			
		} assert(false);
		return var;
	}

	VarValue& VarValue::operator=(const VarValue& argument2){
		this->data = argument2.data;
		this->type = argument2.type;
		
		if (argument2.type >= argument2._ArraysStart && argument2.type < argument2._ArraysEnd)
			this->pArray = argument2.pArray;

		return *this;
	}

	VarValue& VarValue::operator=(bool v){
		this->type = this->kType_Bool;
		this->data.b = v;
		return *this;
	}

	bool VarValue::operator == (VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {
			case VarValue::kType_String:
				return this->data.stringRef2 == argument2.data.stringRef2;
			case VarValue::kType_Identifier:
				return this->data.stringRef == argument2.data.stringRef;
			case VarValue::kType_Integer:
				return this->data.i == argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f == argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b == argument2.data.b;
			}
		}
		else {
			
			switch (this->type) {

			case VarValue::kType_Identifier:
				return this->data.stringRef == argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 == argument2.data.stringRef2;
			case VarValue::kType_Integer:
				return this->CastToInt().data.i == argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f == argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b == argument2.CastToBool().data.b;
			}
		}assert(false);
		return false;
	}

	bool VarValue::operator != ( VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {

			case VarValue::kType_Identifier:
				return this->data.stringRef != argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 != argument2.data.stringRef2;
			case VarValue::kType_Integer:
				return this->data.i != argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f != argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b != argument2.data.b;
			}
		}
		else {

			switch (this->type) {

			case VarValue::kType_Identifier:
				return this->data.stringRef != argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 != argument2.data.stringRef2;
			case VarValue::kType_Integer:
				return this->CastToInt().data.i != argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f != argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b != argument2.CastToBool().data.b;
			}
		}assert(false);
		return false;
	}

	bool VarValue::operator > ( VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef > argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 > argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->data.i > argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f > argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b > argument2.data.b;
			}
		}
		else {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef > argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 > argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->CastToInt().data.i > argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f > argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b > argument2.CastToBool().data.b;
			}
		}assert(false);
		return false;
	}

	bool VarValue::operator >= ( VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef >= argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 >= argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->data.i >= argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f >= argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b >= argument2.data.b;
			}
		}
		else {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef >= argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 >= argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->CastToInt().data.i >= argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f >= argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b >= argument2.CastToBool().data.b;
			}

		}assert(false);
		return false;
	}

	bool  VarValue::operator < ( VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef < argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 < argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->data.i < argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f < argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b < argument2.data.b;
			}
		}
		else {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef < argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 < argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->CastToInt().data.i < argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f < argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b < argument2.CastToBool().data.b;
			}
		}assert(false);
		return false;
	}

	bool  VarValue::operator <= ( VarValue &argument2) {
		if (this->type == argument2.type) {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef <= argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 <= argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->data.i <= argument2.data.i;
			case VarValue::kType_Float:
				return this->data.f <= argument2.data.f;
			case VarValue::kType_Bool:
				return this->data.b <= argument2.data.b;
			}
		}
		else {

			switch (this->type) {

			/*case VarValue::kType_Identifier:
				return this->data.stringRef <= argument2.data.stringRef;
			case VarValue::kType_String:
				return this->data.stringRef2 <= argument2.data.stringRef2;*/
			case VarValue::kType_Integer:
				return this->CastToInt().data.i <= argument2.CastToInt().data.i;
			case VarValue::kType_Float:
				return this->CastToFloat().data.f <= argument2.CastToFloat().data.f;
			case VarValue::kType_Bool:
				return this->CastToBool().data.b <= argument2.CastToBool().data.b;
			}
		}assert(false);
		return false;
	}

#pragma endregion


#pragma region ImplementationOppCodes



	std::pair<uint16_t, VarValue> VirtualMachine::nop(OpcodeContext& context){
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iAdd(OpcodeContext& context) {
		
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 + v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::fAdd(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 + v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iSub(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 - v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::fSub(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 - v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iMul(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 * v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::fMul(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 * v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iDiv(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 / v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::fDiv(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 / v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iMod(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 % v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::_not(OpcodeContext& context){
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);

		result = !v1;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::iNeg(OpcodeContext& context){
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue v2;
		v2.data.i = -1;
		v2.type = v2.kType_Integer;
		result = v1 * v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::fNeg(OpcodeContext& context){
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue v2;
		v2.data.f = -1.0f;
		v2.type = v2.kType_Float;
		result = v1 * v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::assign(OpcodeContext& context){
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);

		result = v1;
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cast(OpcodeContext& context){
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		
		switch (result.type) {
		case result.kType_Integer:
			result = v1.CastToInt();
			break;
		case result.kType_Float:
			result = v1.CastToFloat();
			break;
		case result.kType_Bool:
			result = v1.CastToBool();
			break;
		case result.kType_String:
			result = CastToString(v1);
			break;
		}
			
			

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	VarValue VirtualMachine::CastToString(const VarValue &var) {
		
			VarValue var_((uint8_t)var.kType_String);
			std::string str;
			StringTable& table = structure.stringTable;

			switch (var.type) {

			case var.kType_Object:
				assert(false);
				return var_;
			case var.kType_Identifier:
				var_.data.stringRef = var.data.stringRef;
				return var_;
			case var.kType_Integer:
				str = std::to_string(var.data.i);
				var_.data.stringRef = table.m_data.size();
				table.m_data.push_back(str);
				return var_;
			case var.kType_Float:
				str = var.data.f;
				var_.data.stringRef = table.m_data.size();
				table.m_data.push_back(str);
				return var_;
			case var.kType_Bool:
				str = var.data.b;
				var_.data.stringRef = table.m_data.size();
				table.m_data.push_back(str);
				return var_;
			default:
				assert(false);
				return var_;
			}
		
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cmp_eq(OpcodeContext& context) {

		VarValue& result = GetValue(context,context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);
		
		if (v1.type == v1.kType_String && v2.type == v2.kType_String) {
			result = GetStringByIndex(v1.data.stringRef) == GetStringByIndex(v2.data.stringRef);
		}
		else {
			result = v1 == v2;
		}
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cmp_lt(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 < v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cmp_le(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 <= v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cmp_gt(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 > v2;
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::cmp_ge(OpcodeContext& context) {
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& v1 = GetValue(context, context.args[1]);
		VarValue& v2 = GetValue(context, context.args[2]);

		result = v1 >= v2;
		
		std::pair<uint16_t, VarValue> arg = { uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::jmp(OpcodeContext& context) {
		
		VarValue& numSkipLine = GetValue(context, context.args[0]);
		needJump = true;

		std::pair<uint16_t, VarValue> arg = { context.args[0].data.stringRef ,numSkipLine };
		--arg.second.data.i;
		return arg; 
	}

	std::pair<uint16_t, VarValue> VirtualMachine::jmpt(OpcodeContext& context){
		
		VarValue& condition = GetValue(context, context.args[0]);
		VarValue& numSkipLine = GetValue(context, context.args[1]);
		
		std::pair<uint16_t, VarValue> arg = { context.args[0].data.stringRef ,numSkipLine };

		if (condition.data.b) {
			needJump = true;
			--arg.second.data.i;
			return arg;
		}
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::jmpf(OpcodeContext& context){
		
		VarValue& condition = GetValue(context, context.args[0]);
		VarValue& numSkipLine = GetValue(context, context.args[1]);

		std::pair<uint16_t, VarValue> arg = { context.args[0].data.stringRef ,numSkipLine };

		if(!condition.data.b) {
			--arg.second.data.i;
			needJump = true;
			return arg;
		}
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::callMethod(OpcodeContext& context){

		std::pair<uint16_t, VarValue> arg = { context.args[0].data.stringRef ,VarValue() };

		std::vector<VarValue> functionArgs;
		for (size_t i = 4; i < context.args.size(); ++i){
			functionArgs.push_back(context.args[i]);
		}
		uint16_t index = context.args[2].data.stringRef;

		FunctionInfo func = GetFunctionByName(context.args[0].data.stringRef);
		if (func.IsNative()) {
			auto methodName = GetStringByIndex(context.args[0].data.stringRef);
			auto &method = nativeMethods[methodName];
			assert(method);
			if (method != nullptr) {
				// TODO: implement self
				VarValue self;
				self.type = VarValue::kType_Object;
				self.data.id = 0;

				arg.first = context.args[0].data.stringRef;
				for (auto &arg : functionArgs) {
					arg = this->GetValue(context, arg);
				}
				arg.second = method(self, functionArgs);
				return arg;
			}
			return {};
		}
		else {
			arg = StartFunction(func, context, functionArgs);
			context.args[2] = arg.second;
			arg.first = index;
			return arg;
		}
	}

	std::pair<uint16_t, VarValue> VirtualMachine::callParent(OpcodeContext& context){
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::callStatic(OpcodeContext& context){
		
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::_return(OpcodeContext& context){
		
		VarValue& argument = GetValue(context, context.args[0]);
		needReturn = true;
		std::pair<uint16_t, VarValue> arg = { context.args[0].data.stringRef ,argument };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::strCat(OpcodeContext& context){
		StringTable& table = structure.stringTable;


		VarValue& result = GetValue(context, context.args[0]);

		
		VarValue& v1 = GetValue(context, context.args[1]);
		std::string s1 = GetStringByIndex(v1.data.stringRef);

		VarValue& v2 = GetValue(context, context.args[2]);
		std::string s2 = GetStringByIndex(v2.data.stringRef);
		
		std::string sResult = s1 + s2;

		VarValue var_((uint8_t)result.kType_String);

		var_.data.stringRef = table.m_data.size();
		table.m_data.push_back(sResult);
		
		result = var_;


		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::propGet(OpcodeContext& context){

		std::string nameProperty = GetStringByIndex(context.args[0].data.stringRef);
		VarValue& variable = GetValue(context, context.args[1]);
		VarValue& result = GetValue(context, context.args[2]);
		
		if (variable.linkObject != nullptr)
			result = variable.linkObject->RunHeandler(nameProperty, FunctionInfo::kFlags_Read ,{}).second;
		else assert(false);

		std::pair<uint16_t, VarValue> arg = { context.args[2].data.stringRef ,result };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::propSet(OpcodeContext& context){
	
		std::string nameProperty = GetStringByIndex(context.args[0].data.stringRef);
		VarValue& variable = GetValue(context, context.args[1]);
		VarValue& argument = GetValue(context, context.args[2]);

		std::pair<uint16_t, VarValue> arg = { uint16_t(-1) ,VarValue() };
		
		if (variable.linkObject != nullptr)
			arg = variable.linkObject->RunHeandler(nameProperty, FunctionInfo::kFlags_Write, {argument});
		else assert(false);

		
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arrayCreate(OpcodeContext& context){
	
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& size = GetValue(context, context.args[1]);

		result.pArray = std::shared_ptr<std::vector<VarValue>>(new std::vector<VarValue>);
		result.pArray->resize(size.data.i);

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arrayLength(OpcodeContext& context){
		
		VarValue& result = GetValue(context, context.args[0]);
		VarValue& array = GetValue(context, context.args[1]);
		
		if (array.pArray != nullptr) {
			if (result.type == result.kType_Integer)
				result.data.i = array.pArray->size();

		}else result.data.i = 0;

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arrayGetElement(OpcodeContext& context){
		
		VarValue&  result = GetValue(context, context.args[0]);
		VarValue&  array = GetValue(context, context.args[1]);
		VarValue&  index = GetValue(context, context.args[2]);

		if (array.pArray != nullptr) {
			result = array.pArray->at(index.data.i);
		}

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arraySetElement(OpcodeContext& context){
		

		VarValue& array = GetValue(context, context.args[0]);
		VarValue& size = GetValue(context, context.args[1]);
		VarValue& argument = GetValue(context, context.args[2]);

		if (array.pArray != nullptr) {
			array.pArray->at(size.data.i) = argument;
		}

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arrayFindElement(OpcodeContext& context){
	
		VarValue& array = GetValue(context, context.args[0]);
		VarValue& result = GetValue(context, context.args[1]);
		VarValue& index = GetValue(context, context.args[2]);
		VarValue& startIndex = GetValue(context, context.args[3]);

		if (startIndex.data.i < 0 || startIndex.data.i >= array.pArray->size())
			startIndex.data.i = 0;

		if (array.pArray != nullptr) {
			for (size_t i = startIndex.data.i; i < array.pArray->size(); ++i) {
				if (array.type == array.kType_StringArray) {
					if (GetStringByIndex(array.pArray->at(i).data.stringRef) == GetStringByIndex(index.data.stringRef)) {
						result.data.i = i;
						break;
					}
					else result.data.i = -1;

				}
				else if (array.pArray->at(i) == index) {
					result.data.i = i;
					break;
				}
				else result.data.i = -1;
			}
		}

		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}

	std::pair<uint16_t, VarValue> VirtualMachine::arrayRFindElement(OpcodeContext& context){
		
		VarValue& array = GetValue(context, context.args[0]);
		VarValue& result = GetValue(context, context.args[1]);
		VarValue& index = GetValue(context, context.args[2]);
		VarValue& startIndex = GetValue(context, context.args[3]);

		if (startIndex.data.i >= array.pArray->size() || startIndex.data.i < 0)
				startIndex.data.i = array.pArray->size() - 1;
		
		if (array.pArray != nullptr) {
			for (int32_t i = startIndex.data.i; i >= 0; --i) {
				if (array.type == array.kType_StringArray) {
					if (GetStringByIndex(array.pArray->at(i).data.stringRef) == GetStringByIndex(index.data.stringRef)) {
						result.data.i = i;
						break;
					}
					else result.data.i = -1;
				}
				else if (array.pArray->at(i) == index) {
					result.data.i = i;
					break;
				}
				else result.data.i = -1;
			}
		}
		std::pair<uint16_t, VarValue> arg = {uint16_t(-1) ,VarValue() };
		return arg;
	}



#pragma endregion

