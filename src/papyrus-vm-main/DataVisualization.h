#pragma once
#include "Structures.h"

class Test
{
public:

	void ShowData(VMScript structure);

	void ShowFileInByteCode(std::vector<uint8_t> arrayByte);
	void GetNeedUnion(ObjectTable::Object::VarInfo data, StringTable table);
	void ShowObjectInfo(VMScript structure);
	std::string ShowString(StringTable table, int index);
	std::string GetStringValueType(ObjectTable::Object::VarInfo var);
	void ShowHandler(bool b, FunctionInfo hendler, StringTable table);
	void ShowFunction(FunctionInfo function, StringTable table);
	std::string GetStringOpcodeType(VarValue var);
	void GetNeedUnionOp(VarValue  var, StringTable table);
private:

};
