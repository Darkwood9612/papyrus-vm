#include "Structures.h"



VarValue VarValue::CastToInt() const {

	switch (this->type) {
		
	case kType_Object:
			assert(false);
		
	case kType_Identifier:
			assert(false);
		
	case kType_Integer:
		return VarValue((int32_t)this->data.i);

	case kType_Float:
		return VarValue((int32_t)this->data.f);

	case kType_Bool:
		return  VarValue((int32_t)this->data.b);
	
	default:
		assert(false);
		return VarValue();
	}
}

VarValue VarValue::CastToFloat() const {
	
	switch (this->type) {

	case kType_Object:
		assert(false);

	case kType_Identifier:
		assert(false);

	case kType_Integer:
		return VarValue((float)this->data.i);

	case kType_Float:
		return VarValue((float)this->data.f);

	case kType_Bool:
		return  VarValue((float)this->data.b);

	default:
		assert(false);
		return VarValue();
	}
}

VarValue VarValue::CastToBool() const {
	switch (this->type) {

	case kType_Object:
		if (this->data.id == nullptr)
			return  VarValue((bool)false);
		else
			return  VarValue((bool)true);

	case kType_Identifier:
		assert(false);
	case kType_String: {
		std::string str;

		if (this->data.string == str)
			return  VarValue((bool)false);
		else
			return  VarValue((bool)true);
	}
	case kType_Integer:
		return VarValue((bool)this->data.i);

	case kType_Float:
		return VarValue((bool)this->data.f);

	case kType_Bool:
		return  VarValue((bool)this->data.b);
	case kType_ObjectArray:
		if (this->pArray != nullptr) {
			if (this->pArray->size() >= 1)
				return  VarValue((bool)true);
		}
		return  VarValue((bool)false);
	case kType_StringArray:
		if (this->pArray != nullptr) {
			if (this->pArray->size() >= 1)
				return  VarValue((bool)true);
		}
		return  VarValue((bool)false);
	case kType_IntArray:
		if (this->pArray != nullptr) {
			if (this->pArray->size() >= 1)
				return  VarValue((bool)true);
		}
		return  VarValue((bool)false);
	case kType_FloatArray:
		if (this->pArray != nullptr) {
			if (this->pArray->size() >= 1)
				return  VarValue((bool)true);
		}
		return  VarValue((bool)false);
	case kType_BoolArray:
		if (this->pArray != nullptr) {
			if (this->pArray->size() >= 1)
				return  VarValue((bool)true);
		}
		return  VarValue((bool)false);
	default:
		assert(false);
		return VarValue();
	}
}



VarValue::VarValue(uint8_t type){
	static std::string emptyLine;
	switch (type) {

	case kType_Object:
		this->type = this->kType_Object;
		this->data.id = nullptr;
		break;
	case kType_Identifier:
		this->type = this->kType_Identifier;
		this->data.string = nullptr;
		break;
	case kType_Integer:
		this->type = this->kType_Integer;
		this->data.i = 0;
		break;
	case kType_Float:
		this->type = this->kType_Float;
		this->data.f = 0.0f;
		break;
	case kType_Bool:
		this->type = this->kType_Bool;
		this->data.b = false;
		break;
	case kType_String:
		this->type = this->kType_String;
		this->data.string = emptyLine.c_str();
		break;
	case kType_ObjectArray:
		this->type = this->kType_ObjectArray;
		this->pArray = nullptr;
		break;
	case kType_StringArray:
		this->type = this->kType_StringArray;
		this->pArray = nullptr;
		break;
	case kType_IntArray:
		this->type = this->kType_IntArray;
		this->pArray = nullptr;
		break;
	case kType_FloatArray:
		this->type = this->kType_FloatArray;
		this->pArray = nullptr;
		break;
	case kType_BoolArray:
		this->type = this->kType_BoolArray;
		this->pArray = nullptr;
		break;
	default:
		assert(false);
		
	}
}


VarValue::VarValue(uint8_t type, const char* value) {
	this->type = this->kType_Identifier;
	this->data.string = value;
}


VarValue::VarValue(IGameObject* object){
	this->type = this->kType_Object;
	this->data.id = object;
}

VarValue::VarValue(int32_t value){
	this->type = this->kType_Integer;
	this->data.i = value;
}

VarValue::VarValue(const char* value){
	this->type = this->kType_String;
	this->data.string = value;
}

VarValue::VarValue(float value){
	this->type = this->kType_Float;
	this->data.f = value;
}

VarValue::VarValue(bool value){
	this->type = this->kType_Bool;
	this->data.b = value;
}

VarValue VarValue::operator + (const VarValue& argument2) {
	VarValue var;
	if (this->type == argument2.type) {

		switch (this->type) {

		case VarValue::kType_String:
			assert(false);
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

VarValue VarValue::operator - (const VarValue& argument2) {
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

VarValue VarValue::operator * (const VarValue& argument2) {
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

VarValue VarValue::operator*(float value)
{
	VarValue var;

		switch (this->type) {

		case VarValue::kType_Integer:
			var.data.i = this->data.i * (int)value;
			var.type = this->kType_Integer;
			return var;
		case VarValue::kType_Float:
			var.data.f = this->data.f * value;
			var.type = this->kType_Float;
			return var;
		
	} assert(false);
	return var;
}

VarValue VarValue::operator*(int value)
{
	VarValue var;

	switch (this->type) {

	case VarValue::kType_Integer:
		var.data.i = this->data.i * value;
		var.type = this->kType_Integer;
		return var;
	case VarValue::kType_Float:
		var.data.f = this->data.f * value;
		var.type = this->kType_Float;
		return var;

	} assert(false);
	return var;
}

VarValue VarValue::operator / (const VarValue& argument2) {
	VarValue var;
	if (this->type == argument2.type) {

		switch (this->type) {

		case VarValue::kType_Integer:
			var.data.i = 1;
			if (argument2.data.i != uint32_t(0) && this->data.i != uint32_t(0))
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

VarValue VarValue::operator % (const VarValue& argument2) {
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

	switch (this->type) {

	case kType_Object:
		var.type = this->kType_Bool;
		var.data.b = (this->data.id == nullptr);
		return var;

	case kType_Identifier:
		assert(false);
		return var;

	case kType_Integer:
		var.type = this->kType_Bool;
		var.data.b = (this->data.i == 0);
		return var;

	case kType_Float:
		var.type = this->kType_Bool;
		var.data.b = (this->data.f == 0.0);
		return var;

	case kType_Bool:
		var.data.b = !this->data.b;
		var.type = this->kType_Bool;
		return var;

	case kType_String: {
		var.type = this->kType_Bool;
		static std::string emptyLine;
		var.data.b = (this->data.string == emptyLine);
		return var;
	}
	case kType_ObjectArray:
		var.type = this->kType_Bool;
		var.data.b = (this->pArray->size() < 1);
		return var;

	case kType_StringArray:
		var.type = this->kType_Bool;
		var.data.b = (this->pArray->size() < 1);
		return var;

	case kType_IntArray:
		var.type = this->kType_Bool;
		var.data.b = (this->pArray->size() < 1);
		return var;

	case kType_FloatArray:
		var.type = this->kType_Bool;
		var.data.b = (this->pArray->size() < 1);
		return var;

	case kType_BoolArray:
		var.type = this->kType_Bool;
		var.data.b = (this->pArray->size() < 1);
		return var;

	default:
		assert(false);
		return var;
	}
	
}

VarValue& VarValue::operator=(const VarValue& argument2) {
	this->data = argument2.data;
	this->type = argument2.type;

	if (argument2.type >= argument2._ArraysStart && argument2.type < argument2._ArraysEnd)
		this->pArray = argument2.pArray;

	return *this;
}


bool VarValue::operator == (VarValue& argument2) {
	
		switch (this->type) {

		case VarValue::kType_Object: {
			IGameObject* id1 = nullptr;
			IGameObject* id2 = nullptr;

			if (this->data.id != nullptr)
				id1 = this->data.id;

			if (argument2.data.id != nullptr)
				id2 = argument2.data.id;

			return id1 == id2;
		}
		case VarValue::kType_Identifier: {
			std::string s1;
			std::string s2;

			if (this->data.string != NULL)
				s1 = this->data.string;

			if (argument2.data.string != NULL)
				s2 = argument2.data.string;

			return s1 == s2;
		}
		case VarValue::kType_String: {
			std::string s1;
			std::string s2;

			if(this->data.string != NULL)
				s1 = this->data.string;
 
			if (argument2.data.string != NULL)
				s2 = argument2.data.string;
	
			return s1 == s2;
		}
		case VarValue::kType_Integer:
			return this->CastToInt().data.i == argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f == argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b == argument2.CastToBool().data.b;
		}
		assert(false);
	return false;
}

bool VarValue::operator != (VarValue& argument2) {


		switch (this->type) {

		case VarValue::kType_Object: {
			IGameObject* id1 = nullptr;
			IGameObject* id2 = nullptr;

			if (this->data.id != nullptr)
				id1 = this->data.id;

			if (argument2.data.id != nullptr)
				id2 = argument2.data.id;

			return id1 != id2;
		}
		case VarValue::kType_Identifier: {
			std::string s1;
			std::string s2;

			if (this->data.string != NULL)
				s1 = this->data.string;

			if (argument2.data.string != NULL)
				s2 = argument2.data.string;

			return s1 != s2;
		}
		case VarValue::kType_String: {
			std::string s1;
			std::string s2;

			if (this->data.string != NULL)
				s1 = this->data.string;

			if (argument2.data.string != NULL)
				s2 = argument2.data.string;

			return s1 != s2;
		}
		case VarValue::kType_Integer:
			return this->CastToInt().data.i != argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f != argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b != argument2.CastToBool().data.b;
		}
	assert(false);
	return false;
}

bool VarValue::operator > (VarValue& argument2) {


		switch (this->type) {

		case VarValue::kType_Integer:
			return this->CastToInt().data.i > argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f > argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b > argument2.CastToBool().data.b;
		}
	assert(false);
	return false;
}

bool VarValue::operator >= (VarValue& argument2) {
	

		switch (this->type) {

		case VarValue::kType_Integer:
			return this->CastToInt().data.i >= argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f >= argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b >= argument2.CastToBool().data.b;
		}

	assert(false);
	return false;
}

bool  VarValue::operator < (VarValue& argument2) {
	

		switch (this->type) {

		case VarValue::kType_Integer:
			return this->CastToInt().data.i < argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f < argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b < argument2.CastToBool().data.b;
		}
	assert(false);
	return false;
}

bool  VarValue::operator <= (VarValue& argument2) {
	

		switch (this->type) {

		case VarValue::kType_Integer:
			return this->CastToInt().data.i <= argument2.CastToInt().data.i;
		case VarValue::kType_Float:
			return this->CastToFloat().data.f <= argument2.CastToFloat().data.f;
		case VarValue::kType_Bool:
			return this->CastToBool().data.b <= argument2.CastToBool().data.b;
		}
	assert(false);
	return false;
}