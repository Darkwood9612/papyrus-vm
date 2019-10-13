#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Structures.h"
#include <cassert>
#include <algorithm>
#include <functional>
#include <map>

using VMNativeFunction = std::function<VarValue(VarValue self, std::vector<VarValue> arguments)>;

class VirtualMachine {
private:
	VMScript structure;

	enum Opcodes {
		op_Nop = 0x00,
		op_iAdd = 0x01,
		op_fAdd = 0x02,
		op_iSub = 0x03,
		op_fSub = 0x04,
		op_iMul = 0x05,
		op_fMul = 0x06,
		op_iDiv = 0x07,
		op_fDiv = 0x08,
		op_iMod = 0x09,
		op_Not = 0x0A,
		op_iNeg = 0x0B,
		op_fNeg = 0x0C,
		op_Assign = 0x0D,
		op_Cast = 0x0E,
		op_Cmp_eq = 0x0F,
		op_Cmp_lt = 0x10,
		op_Cmp_le = 0x11,
		op_Cmp_gt = 0x12,
		op_Cmp_ge = 0x13,
		op_Jmp = 0x14,
		op_Jmpt = 0x15,
		op_Jmpf = 0x16,
		op_CallMethod = 0x17,
		op_CallParent = 0x18,
		op_CallStatic = 0x19,
		op_Return = 0x1A,
		op_StrCat = 0x1B,
		op_PropGet = 0x1C,
		op_PropSet = 0x1D,
		op_Array_Create = 0x1E,
		op_Array_Length = 0x1F,
		op_Array_GetElement = 0x20,
		op_Array_SetElement = 0x21,
		op_Array_FindElement = 0x22,
		op_Array_RfindElement = 0x23
	};

	int currentReadPositionInFile;
	std::string path;
	std::vector<uint8_t> arrayBytes;
	bool additionalArguments = false;
	std::vector<int> numArgumentsForOpcodes = { 0,3,3,3,3,3,3,3,3,3,2,2,2,2,2,3,3,3,3,3,1,2,2,-4,-3,-4,1,3,3,3,2,2,3,3,4,4 };
	std::map<std::string, VMNativeFunction> nativeMethods;

	std::string FillSource();
	std::string FillUser();
	std::string FillMachine();

	ScriptHeader FillHeader();
	StringTable FillStringTable();
	DebugInfo FillDebugInfo();
	UserFlagTable FillUserFlagTable();
	ObjectTable FillObjectTable();

	DebugInfo::DebugFunction FillDebugFunction();
	UserFlagTable::UserFlag  FillUserFlag();
	ObjectTable::Object FillObject();
	ObjectTable::Object::VarInfo FillVariable();
	VarValue FillVariableData();
	ObjectTable::Object::PropInfo FillProperty();
	FunctionInfo FillFuncInfo();
	FunctionCode FillFunctionCode(int CountInstructions);
	uint8_t GetCountArguments(uint8_t item);
	ObjectTable::Object::StateInfo  FillState();
	ObjectTable::Object::StateInfo::StateFunction FillStateFunction();
	uint8_t Read8_bit();
	uint16_t Read16_bit();
	uint32_t Read32_bit();
	uint64_t Read64_bit();

	std::string ReadString(int Size);


	ObjectTable::Object::VarTable allObjectVariables;
	

	VarValue& GetVariableValueByName(OpcodeContext& context, const uint16_t name);
	std::string GetStringByIndex(uint16_t index);
	

	std::pair<uint16_t, VarValue> StartFunction(FunctionInfo& function, OpcodeContext& context, const std::vector<VarValue> &functionArgs);
	bool needJump;
	bool needReturn;
	std::pair<uint16_t, VarValue> ImplementateOpcode(OpcodeContext & context, FunctionCode::Instruction &opCode);

	std::pair<uint16_t, VarValue> nop(OpcodeContext& context);
	std::pair<uint16_t, VarValue> iAdd(OpcodeContext &context);
	std::pair<uint16_t, VarValue> fAdd(OpcodeContext &context);
	std::pair<uint16_t, VarValue> iSub(OpcodeContext &context);
	std::pair<uint16_t, VarValue> fSub(OpcodeContext &context);
	std::pair<uint16_t, VarValue> iMul(OpcodeContext &context);
	std::pair<uint16_t, VarValue> fMul(OpcodeContext &context);
	std::pair<uint16_t, VarValue> iDiv(OpcodeContext &context);
	std::pair<uint16_t, VarValue> fDiv(OpcodeContext &context);
	std::pair<uint16_t, VarValue> iMod(OpcodeContext &context);
	std::pair<uint16_t, VarValue> _not(OpcodeContext& context);
	std::pair<uint16_t, VarValue> iNeg(OpcodeContext& context);
	std::pair<uint16_t, VarValue> fNeg(OpcodeContext& context);
	std::pair<uint16_t, VarValue> assign(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cast(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cmp_eq(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cmp_lt(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cmp_le(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cmp_gt(OpcodeContext& context);
	std::pair<uint16_t, VarValue> cmp_ge(OpcodeContext& context);
	std::pair<uint16_t, VarValue> jmp(OpcodeContext& context);
	std::pair<uint16_t, VarValue> jmpt(OpcodeContext& context);
	std::pair<uint16_t, VarValue> jmpf(OpcodeContext& context);
	std::pair<uint16_t, VarValue> callMethod(OpcodeContext& context);
	std::pair<uint16_t, VarValue> callParent(OpcodeContext& context);
	std::pair<uint16_t, VarValue> callStatic(OpcodeContext& context);
	std::pair<uint16_t, VarValue> _return(OpcodeContext& context);
	std::pair<uint16_t, VarValue> strCat(OpcodeContext& context);
	std::pair<uint16_t, VarValue> propGet(OpcodeContext& context);
	std::pair<uint16_t, VarValue> propSet(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arrayCreate(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arrayLength(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arrayGetElement(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arraySetElement(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arrayFindElement(OpcodeContext& context);
	std::pair<uint16_t, VarValue> arrayRFindElement(OpcodeContext& context);


	FunctionInfo GetFunctionByName(uint16_t name) ;

	VarValue& GetValue(OpcodeContext& context, VarValue& value);

	uint8_t GetTypeByName(uint16_t typeRef);

	VarValue CastToString(const VarValue& var);

	void Read();
	void CreateScriptStructure(std::vector<uint8_t> arrayBytes);
	uint16_t GetStringIndex(std::string name);
public:
	std::string GetSourceFileName();
	std::vector<std::string> GetObjectsListForLink();
	void FillObjectsLinks(std::vector<std::shared_ptr<VirtualMachine>>& listLinks);

	VirtualMachine(std::string path);
	std::pair<uint16_t, VarValue> RunHeandler(const std::string& name, int action,const std::vector<VarValue>& functionArgs);
	std::pair<uint16_t, VarValue> RunFunction(const std::string &name, const std::vector<VarValue>& functionArgs);
	void RegisterFunction(const std::string &name, VMNativeFunction nativeFunction);
	std::string GetString(const VarValue &stringValue);
	VarValue* GetVariableValueByName(const std::string& name);
	VMScript* GetScriptStructure();

};
