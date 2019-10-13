#pragma once
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

struct VMScript;
class VirtualMachine;
struct VarValue {

	VarValue() {
		this->type = kType_String;
		this->data.stringRef = 0;
	}

	static VarValue None() {
		VarValue none;
		none.type = kType_Object;
		none.data.id = 0;
		return none;
	}

	explicit operator bool() const {
		return this->CastToBool().data.b;
	}

	explicit operator int32_t() const {
		return this->CastToInt().data.i;
	}

	explicit operator float() const {
		return this->CastToFloat().data.f;
	}

	explicit VarValue(uint8_t type) {
		this->type = type;
		switch (type){

		case kType_Object:
			this->data.id = 0;
			break;
		case kType_Identifier:
			this->data.stringRef = 0;
			break;
		case kType_String:
			this->data.stringRef = 0;
			break;
		case kType_Integer:
			this->data.i = 0;
			break;
		case kType_Float:
			this->data.f = 0.0f;
			break;
		case kType_Bool:
			this->data.b = 0;
			break;
		case kType_StringArray:
			
			break;
		case kType_IntArray:
			
			break;
		case kType_FloatArray:
			
			break;
		case kType_BoolArray:
			
			break;
		case kType_ObjectArray:
			
			break;
		default:
			assert(false);
			break;
		}
	}


	enum{
		kType_Object = 0,		// 0 null?
		kType_Identifier,	// 1 identifier
		kType_String,		// 2
		kType_Integer,		// 3
		kType_Float,		// 4
		kType_Bool,			// 5

		_ArraysStart = 11,
		kType_ObjectArray = 11,
		kType_StringArray = 12,
		kType_IntArray = 13,
		kType_FloatArray = 14,
		kType_BoolArray = 15,
		_ArraysEnd = 16,
	};

	uint8_t	type;

	std::shared_ptr<std::vector<VarValue>> pArray;
	std::shared_ptr<VirtualMachine> linkObject;

	union{
		uint32_t id = 0;
		uint16_t	stringRef;
		uint16_t	stringRef2;
		int32_t	i;
		float	f;
		bool	b;
	} data;

	VarValue operator + (const VarValue &argument2);
	VarValue operator - (const VarValue &argument2);
	VarValue operator * (const VarValue &argument2);
	VarValue operator / (const VarValue &argument2);
	VarValue  operator % (const VarValue &argument2);
	VarValue  operator!();
	VarValue& operator = (const VarValue& argument2);
	VarValue& operator = (bool v);

	bool operator == ( VarValue &argument2);
	bool operator != ( VarValue &argument2);
	bool operator > ( VarValue &argument2);
	bool operator >= ( VarValue &argument2);
	bool  operator < ( VarValue &argument2);
	bool operator <= ( VarValue &argument2);

	

	 VarValue CastToInt() const {
		VarValue var((uint8_t)kType_Integer);
		
		switch (this->type){

		case kType_Object:
			var.data.i = (uint32_t)this->data.id;
			return var;
		case kType_Identifier:
			var.data.i = (uint32_t)this->data.stringRef;
			return var;
		case kType_Integer:
			var.data.i = (uint32_t)this->data.i;
			return var;
		case kType_Float:
			var.data.i = (uint32_t)this->data.f;
			return var;
		case kType_Bool:
			var.data.i = (uint32_t)this->data.b;
			return var;
		default:
			assert(false);
			return var;
		}
	}

	VarValue CastToFloat() const {
		VarValue var((uint8_t)kType_Float);

		switch (this->type) {

		case kType_Object:
			var.data.f = (float)this->data.id;
			return var;
		case kType_Identifier:
			var.data.f = (float)this->data.stringRef;
			return var;
		case kType_Integer:
			var.data.f = (float)this->data.i;
			return var;
		case kType_Float:
			var.data.f = (float)this->data.f;
			return var;
		case kType_Bool:
			var.data.f = (float)this->data.b;
			return var;
		default:
			assert(false);
			return var;
		}
	}
	
	VarValue CastToBool() const {
		VarValue var((uint8_t)kType_Bool);

		switch (this->type) {

		case kType_Object:
			var.data.b = (bool)this->data.id;
			return var;
		case kType_Identifier:
			var.data.b = (bool)this->data.stringRef;
			return var;
		case kType_Integer:
			var.data.b = (bool)this->data.i;
			return var;
		case kType_Float:
			var.data.b = (bool)this->data.f;
			return var;
		case kType_Bool:
			var.data.b = (bool)this->data.b;
			return var;
		default:
			assert(false);
			return var;
		}
	}

};

inline std::ostream& operator<<(std::ostream& os, VarValue& d) {
	switch (d.type)
	{
	case VarValue::kType_Object:
		return os << "Object:" << std::dec << (d.data.id);
		
	case VarValue::kType_Identifier:
		return os << "Indetifier:" << std::dec << (d.data.stringRef);
		
	case VarValue::kType_String:
		return os << "String:" << std::dec << (d.data.stringRef2);
		
	case VarValue::kType_Integer:
		return os << "Int:" << std::dec << (d.data.i);
		
	case VarValue::kType_Float:
		return os << "Float:" << std::dec << (d.data.f);
		
	case VarValue::kType_Bool:
		return os << "Bool:" << std::dec << (d.data.b);
	
	case VarValue::kType_ObjectArray:
		return os << "ObjectArray:"  << d.pArray;

	case VarValue::kType_StringArray:
		return os << "StringArray:" << d.pArray;

	case VarValue::kType_IntArray:
		return os << "IntArray:" << d.pArray;

	case VarValue::kType_FloatArray:
		return os << "FloatArray:" << d.pArray;

	case VarValue::kType_BoolArray:
		return os << "BoolArray:" << d.pArray;

	default:
		assert(0);
		return os;
		break;
	}
}

struct OpcodeContext {

	static auto& GetEmptyLocals() {
		static std::vector<std::pair<uint16_t, VarValue>> locals;
		return locals;
	}

	static auto& GetEmptyArgs() {
		static std::vector<VarValue> args;
		return args;
	}

	OpcodeContext() : locals(GetEmptyLocals()) , args(GetEmptyArgs()) {
		
	}

	OpcodeContext(std::vector<std::pair<uint16_t, VarValue>>& locals_, std::vector<VarValue>& args_) : locals(locals_), args(args_) {

	}

	std::vector<std::pair<uint16_t, VarValue>>& locals;
	std::vector<VarValue>& args;

	

};

struct FunctionCode{
	enum{
		kOp_Nop = 0,
		kOp_IAdd,
		kOp_FAdd,
		kOp_ISubtract,
		kOp_FSubtract,			// 4
		kOp_IMultiply,
		kOp_FMultiply,
		kOp_IDivide,
		kOp_FDivide,			// 8
		kOp_IMod,
		kOp_Not,
		kOp_INegate,
		kOp_FNegate,			// C
		kOp_Assign,
		kOp_Cast,
		kOp_CompareEQ,
		kOp_CompareLT,			// 10
		kOp_CompareLTE,
		kOp_CompareGT,
		kOp_CompareGTE,
		kOp_Jump,				// 14
		kOp_JumpT,
		kOp_JumpF,
		kOp_CallMethod,
		kOp_CallParent,			// 18
		kOp_CallStatic,
		kOp_Return,
		kOp_Strcat,
		kOp_PropGet,			// 1C
		kOp_PropSet,
		kOp_ArrayCreate,
		kOp_ArrayLength,
		kOp_ArrayGetElement,	// 20
		kOp_ArraySetElement,
		kOp_Invalid,
	};

	struct Instruction{
		uint8_t		op = 0;

		typedef std::vector<VarValue>	VarTable;
		VarTable	args;
	};

	typedef std::vector <Instruction>	InstructionList;
	InstructionList	instructions;
};

struct FunctionInfo{
	bool	valid = false;


	enum {
		kFlags_Read = 1 << 0,
		kFlags_Write = 1 << 1,
	};

	struct ParamInfo{
		uint16_t	name = 0;
		uint16_t	type = 0;
 
	};

	uint16_t	returnType = 0;
	uint16_t	docstring = 0;
	uint32_t	userFlags = 0;
	uint8_t	flags = 0;

	typedef std::vector <ParamInfo>	ParamTable;
	ParamTable	params;
	ParamTable	locals;

	FunctionCode	code;

	bool IsGlobal() const {
		return flags & (1 << 0);
	}

	bool IsNative() const {
		return flags & (1 << 1);
	}
};



struct ObjectTable{

	struct Object {

		uint16_t	NameIndex;

		uint16_t	parentClassName;
		uint16_t	docstring;
		uint32_t		userFlags;
		uint16_t	autoStateName;

			struct VarInfo {
				uint16_t		name;
				uint16_t		typeName;
				uint32_t		userFlags;
				VarValue	value;
			};

			struct PropInfo {
				enum {
					kFlags_Read = 1 << 0,
					kFlags_Write = 1 << 1,
					kFlags_AutoVar = 1 << 2,
				};

				uint16_t	name;
				uint16_t	type;
				uint16_t	docstring;
				uint32_t	userFlags;
				uint8_t	flags;	// 1 and 2 are read/write
				uint16_t	autoVarName;

				FunctionInfo	readHandler;
				FunctionInfo	writeHandler;
			};

			struct StateInfo {

				struct StateFunction {
					uint16_t			name;
					FunctionInfo	function;
				};

				uint16_t	name;

				typedef std::vector <StateFunction>	FnTable;
				FnTable	functions;
			};

			typedef std::vector <VarInfo>	VarTable;
			VarTable	variables;

			typedef std::vector <PropInfo>	PropTable;
			PropTable	properties;

			typedef std::vector <StateInfo>	StateTable;
			StateTable	states;
		};

	typedef std::vector <Object>	Storage;
	Storage	m_data;
};

struct UserFlagTable{
	
	struct UserFlag{
		uint16_t	name;
		uint8_t	idx;
	};

	typedef std::vector <UserFlag>	Storage;
	Storage	m_data;
};

struct DebugInfo{
	uint8_t	m_flags;
	uint64_t	m_sourceModificationTime;

	struct DebugFunction{
		uint16_t	objName;
		uint16_t	stateName;
		uint16_t	fnName;
		uint8_t	type;	// 0-3 valid

		std::vector <uint16_t>	lineNumbers;	// one per instruction

		uint32_t	GetNumInstructions() { return lineNumbers.size(); }
	};

	typedef std::vector <DebugFunction>	Storage;
	Storage	m_data;
};

struct StringTable{
	typedef std::vector <std::string>	Storage;
	Storage	m_data;
};

struct ScriptHeader{
	enum{
		kSignature = 0xFA57C0DE,
		kVerMajor = 0x03,
		kVerMinor = 0x01,
		kGameID = 0x0001,
	};

	uint32_t	Signature;	// 00	FA57C0DE
	uint8_t		VerMajor;	// 04	03
	uint8_t		VerMinor;	// 05	01
	uint16_t	GameID;		// 06	0001
	uint64_t		BuildTime;	// 08	uint64_t
};

struct VMScript{
	ScriptHeader	header;
	StringTable		stringTable;
	DebugInfo		debugInfo;
	UserFlagTable	userFlagTable;
	ObjectTable		objectTable;

	std::string	source;
	std::string	user;
	std::string	machine;
};
