#include "OpcodesImplementation.h"

void OpcodesImplementation::strCat(VarValue &result, VarValue& s1, VarValue& s2 , StringTable &table) {

	std::string temp;
	temp = temp + (const char*)s1 + (const char*)s2;

	for (auto &str : table.m_data) {
		if (str == temp) {
			result = VarValue(str.data());
			return;
		}
	}

	size_t _size = table.m_data.size();
	table.m_data.push_back(temp);
	result = VarValue(table.m_data[_size].data());
}

void OpcodesImplementation::arrayFindElement(VarValue& array, VarValue& result, VarValue& needValue, VarValue& startIndex) {

	if ((int)startIndex < 0 || (int)startIndex >= array.pArray->size()) {
		result = VarValue(-1);
		return;
	}
		


	if (array.pArray != nullptr) {
		for (int32_t i = (int)startIndex; i < array.pArray->size(); ++i) {
			if (array.GetType() == array.kType_StringArray) {
				if (!strcmp((const char*)array.pArray->at(i), (const char*)needValue)) {
					result = VarValue(i);
					break;
				}
				else result = VarValue(-1);
			}
			else if (array.pArray->at(i) == needValue) {
				result = VarValue(i);
				break;
			}
			else result = VarValue(-1);
		}
	}
	else assert(0);
}

void OpcodesImplementation::arrayRFindElement(VarValue& array, VarValue& result, VarValue &needValue, VarValue& startIndex) {

	int32_t indexForStart = array.pArray->size() - 1;
	
	if((int)startIndex < -1)
		indexForStart = array.pArray->size() + (int)startIndex;

	if (indexForStart >= array.pArray->size() || indexForStart < 0){
		result = VarValue(-1);
		return;
	}

	if (array.pArray != nullptr) {
		for (int32_t i = indexForStart; i >= 0; --i) {
			if (array.GetType() == array.kType_StringArray) {
				if (!strcmp((const char*)array.pArray->at(i), (const char*)needValue)) {
					result = VarValue(i);
					break;
				}
				else result = VarValue(-1);
			}
			else if (array.pArray->at(i) == needValue) {
				result = VarValue(i);
				break;
			}
			else result = VarValue(-1);
		}
	}
	else assert(0);
}
