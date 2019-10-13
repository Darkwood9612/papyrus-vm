#include "DataVisualization.h"
#include <iostream>
#include <iomanip>

void Test::ShowData(VMScript structure){
	auto& table = structure.stringTable;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "Signature: " << (int)structure.header.Signature << std::endl;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "VerMajor: " << (int)structure.header.VerMajor << std::endl;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "VerMinor: " << (int)structure.header.VerMinor << std::endl;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "GameID: " << (int)structure.header.GameID << std::endl;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "BuildTime: " << (int)structure.header.BuildTime << std::endl;
	std::cout << "source: " << structure.source << std::endl;
	std::cout << "user: " << structure.user << std::endl;
	std::cout << "machine: " << structure.machine << std::endl << std::endl;

	std::cout << "StringTable Data: \n";

	std::cout << "[ ";

	for (int i = 0; i < (int)structure.stringTable.m_data.size(); i++) {
		std::cout << std::dec << i << ") <" << structure.stringTable.m_data[i] << "> " << std::endl;
	}

	std::cout << " ].\n\n";

	std::cout << std::hex << std::setfill('0') << std::setw(2) << "Flags: " << (int)structure.debugInfo.m_flags << std::endl;
	std::cout << std::hex << std::setfill('0') << std::setw(2) << "SourceModificationTime: " << (int)structure.debugInfo.m_sourceModificationTime << std::endl << std::endl;

	std::cout << "Debug Function: \n";



	for (int i = 0; i < (int)structure.debugInfo.m_data.size(); i++) {
		std::cout << "[ <ObjName = " << ShowString(table, (int)structure.debugInfo.m_data[i].objName) << ">, ";
		std::cout << " <StateName = " << ShowString(table, (int)structure.debugInfo.m_data[i].stateName) << ">, ";
		std::cout << " <FnName = " << ShowString(table, (int)structure.debugInfo.m_data[i].fnName) << ">, ";
		std::cout << " <type = " << (int)structure.debugInfo.m_data[i].type << ">, ";
		std::cout << " <LineNumbers = " << (int)structure.debugInfo.m_data[i].GetNumInstructions() << ">, ].\n";
	}

	std::cout << "\n\n";


	std::cout << "User Flags: \n";

	for (int i = 0; i < (int)structure.userFlagTable.m_data.size(); i++) {
		std::cout << "[ <Name = " << ShowString(table, (int)structure.userFlagTable.m_data[i].name) << ">, ";
		std::cout << " <Index = " << (int)structure.userFlagTable.m_data[i].idx << "> ]\n";
	}

	std::cout << std::endl;

	ShowObjectInfo(structure);
}


void Test::ShowObjectInfo(VMScript structure) {
	for (size_t j = 0; j < structure.objectTable.m_data.size(); j++) {
		auto &data = structure.objectTable.m_data[j];
		auto &table = structure.stringTable;

		std::cout << "Object number: " << j << " [ < NameIndex = " << ShowString(table, (int)data.NameIndex) << ">, ";
		std::cout << " < parentClassName = " << ShowString(table, (int)data.parentClassName) << ">, ";
		std::cout << " < docstring = " << ShowString(table, (int)data.docstring) << ">, ";
		std::cout << " < userFlags = " << data.userFlags << ">, ";
		std::cout << " < autoStateName = " << ShowString(table, (int)data.autoStateName) << ">, ] \n\n";

		std::cout << " Variales: \n";
		for (auto var : data.variables) {
			std::cout << "[ < name = " << ShowString(table, (int)var.name) << ">, ";
			std::cout << " < typeName = " << ShowString(table, (int)var.typeName) << ">, ";
			std::cout << " < userFlags = " << var.userFlags << ">, ";
			std::cout << " < value type = " << GetStringValueType(var) << "> ]" << std::endl << std::endl;
			if(var.value.type < 11)
			std::cout << " < value = " << var.value.data.i << "> ]" << std::endl << std::endl;
			else std::cout << " < value = " << var.value.pArray << "> ]" << std::endl << std::endl;
			

			GetNeedUnion(var, structure.stringTable);
		}
		std::cout << " \n\n";

		std::cout << " Properties: \n";
		for (auto prop : data.properties) {

			std::cout << "[ < name = " << ShowString(table, (int)prop.name) << ">, ";
			std::cout << " < type = " << ShowString(table, (int)prop.type) << ">, ";
			std::cout << " < docstring = " << ShowString(table, (int)prop.docstring) << ">, ";
			std::cout << " < userFlags = " << prop.userFlags << ">, ";
			std::cout << " < flags = " << prop.flags << ">, ";
			if ((prop.flags & 4) != 0) {
				std::cout << " < autoVarName = " << ShowString(table, (int)prop.autoVarName) << "> ] \n\n";
			}
			
			ShowHandler((prop.flags & 5) == 1, prop.readHandler, table);
			ShowHandler((prop.flags & 6) == 2, prop.writeHandler, table);
			
		}
		std::cout << " \n\n";

		std::cout << " States: \n";
		auto list = structure.objectTable.m_data;
		for (auto item : list) {
			for (auto state : item.states) {
				std::cout << " < state name = " << ShowString(table, (int)state.name) << "> "<< std::endl;
					for (auto function : state.functions) {
						std::cout << " < function name = " << ShowString(table, (int)function.name) << "> " << std::endl;
						ShowFunction(function.function, table);
					}
				}
		}
		std::cout << "  \n\n";
	}
}

void Test::ShowFunction(FunctionInfo function, StringTable table) {
	
	std::cout << "\n < returnType = " << ShowString(table, (int)function.returnType) << ">, ";
	std::cout << " < docstring = " << ShowString(table, (int)function.docstring) << ">, ";
	std::cout << " < userFlags = " << function.userFlags << ">, ";
	std::cout << " < flags = " << function.flags << ">, \n\n";


	for (auto par : function.params) {
		std::cout << " < name " << ShowString(table, (int)par.name) << ">, ";
		std::cout << " < type " << ShowString(table, (int)par.type) << ">, " << std::endl << std::endl;
	}



	for (auto par : function.locals) {
		std::cout << " < name " << ShowString(table, (int)par.name) << ">, ";
		std::cout << " < type " << ShowString(table, (int)par.type) << ">, \n\n";
	}

	std::cout << " Opcodes: \n";

	auto& instructions = function.code.instructions;
	for (auto inst : instructions) {
		std::cout << " < op = " << (int)inst.op << "> = ";
		/*if (inst.op == 10 || inst.op == 11 || inst.op == 13) {
			std::cout << "\n\n\n\n\n\n\n\n\n\n";
			char tmp0;
			std::cin >> tmp0;
		}*/
		for (auto arg : inst.args) {
			std::cout << "[ < type = " << GetStringOpcodeType(arg) << ">, ";
			GetNeedUnionOp(arg, table);
			std::cout << std::endl;
		}
	}
	
}

void Test::ShowHandler(bool b, FunctionInfo hendler, StringTable table) {
	if (b) {
		std::cout << "\n < returnType = " << ShowString(table, (int)hendler.returnType) << ">, ";
		std::cout << " < docstring = " << ShowString(table, (int)hendler.docstring) << ">, ";
		std::cout << " < userFlags = " << hendler.userFlags << ">, ";
		std::cout << " < flags = " << hendler.flags << ">, \n\n";


		for (auto par : hendler.params) {
			std::cout << " < name " << ShowString(table, (int)par.name) << ">, ";
			std::cout << " < type " << ShowString(table, (int)par.type) << ">, " << std::endl << std::endl;
		}



		for (auto par : hendler.locals) {
			std::cout << " < name " << ShowString(table, (int)par.name) << ">, ";
			std::cout << " < type " << ShowString(table, (int)par.type) << ">, \n\n";
		}

		std::cout << " Opcodes: \n";

		auto& instructions = hendler.code.instructions;
		for (auto inst : instructions) {
			std::cout << " < op = " << (int)inst.op << "> = ";
			for (auto arg : inst.args) {
				std::cout << "[ < type = " << GetStringOpcodeType(arg) << ">, ";
				GetNeedUnionOp(arg, table);
				std::cout << std::endl;
			}
		}
	}
}

std::string Test::GetStringOpcodeType(VarValue var) {
	switch ((int)var.type) {
	case 0:
		return " null ";
		break;
	case 1:
		return " indetifier ";
		break;
	case 2:
		return " string ";
		break;
	case 3:
		return " int ";
		break;
	case 4:
		return " float ";
		break;
	case 5:
		return " bool ";
		break;
	case 11:
		return " ObjectArray ";
		break;
	case 12:
		return " _StringArray ";
		break;
	case 13:
		return " IntArray ";
		break;
	case 14:
		return " FloatArray ";
		break;
	case 15:
		return " BoolArray ";
		break;
	default:
		return"error";
	}
}

std::string Test::GetStringValueType(ObjectTable::Object::VarInfo var) {
	switch ((int)var.value.type) {
	case 0:
		return " null ";
		break;
	case 1:
		return " indetifier ";
		break;
	case 2:
		return " string ";
		break;
	case 3:
		return " int ";
		break;
	case 4:
		return " float ";
		break;
	case 5:
		return " bool ";
		break;
	case 11:
		return " ObjectArray ";
		break;
	case 12:
		return " _StringArray ";
		break;
	case 13:
		return " IntArray ";
		break;
	case 14:
		return " FloatArray ";
		break;
	case 15:
		return " BoolArray ";
		break;
	default:
		return"error";
	}
}

std::string Test::ShowString(StringTable table, int index) {
	return table.m_data[index];
}

void Test::GetNeedUnionOp(VarValue  var, StringTable table) {
	switch ((int)var.type) {
	case 0:

		break;
	case 1:
		std::cout << " < value stringRef  = " << table.m_data[(int)var.data.stringRef] << "], " ;
		break;
	case 2:
		std::cout << " < value stringRef2  = " << table.m_data[(int)var.data.stringRef2] << "], " ;
		break;
	case 3:
		std::cout << " < value i = " << (int)var.data.i << "], " ;
		break;
	case 4:
		std::cout << " < value f  = " << (float)var.data.f << "], " ;
		break;
	case 5:
		std::cout << " < value b  = " << (int)var.data.b << "], " ;
		break;
	case 11:
		std::cout << " < value array  = " << &var.pArray << "], ";
		break;
	case 12:
		std::cout << " < value array  = " << &var.pArray << "], ";
		break;
	case 13:
		std::cout << " < value array  = " << &var.pArray << "], ";;
		break;
	case 14:
		std::cout << " < value array  = " << &var.pArray << "], ";
		break;
	case 15:
		std::cout << " < value array  = " << &var.pArray << "], ";
		break;
	}
}

void Test::GetNeedUnion(ObjectTable::Object::VarInfo var, StringTable table){
	switch ((int)var.typeName) {
	case 0:

		break;
	case 1:
		std::cout << " < value stringRef [ " << table.m_data[(int)var.value.data.stringRef] << ", " << std::endl << std::endl;
		break;
	case 2:
		std::cout << " < value stringRef2 [ " << table.m_data[(int)var.value.data.stringRef2] << ", " << std::endl << std::endl;
		break;
	case 3:
		std::cout << " < value i [ " << (int)var.value.data.i << ", " << std::endl << std::endl;
		break;
	case 4:
		std::cout << " < value f [ " << (float)var.value.data.f << ", " << std::endl << std::endl;
		break;
	case 5:
		std::cout << " < value b [ " << (int)var.value.data.b << "] \n" << std::endl << std::endl;
		break;
	}
}




void Test::ShowFileInByteCode(std::vector<uint8_t> arrayByte){
	const int goNewLine = 30;
	int line = 0;
	for (uint8_t item : arrayByte) {


		std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)item << " ";

		if (line >= goNewLine) {
			line = -1;
			std::cout << "\n";
		}

		line++;
	}
}


