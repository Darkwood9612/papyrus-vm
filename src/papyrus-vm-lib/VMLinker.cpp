#include "VMLinker.h"

void Linker::CreateLinksForSctructure(std::shared_ptr<VirtualMachine> VM){

	ListObjects.push_back(VM);

	std::vector<std::string> listNames = VM->GetObjectsListForLink();
	std::vector<std::shared_ptr<VirtualMachine>> linksList = FillListLinks(listNames);
	
	VM->FillObjectsLinks(linksList);
}

std::vector<std::shared_ptr<VirtualMachine>> Linker::FillListLinks(std::vector<std::string> &listNames){

	std::vector<std::shared_ptr<VirtualMachine>> list;

	for(auto &name : listNames){
		for (auto &object : ListObjects) {
			std::string nameObj = object->GetSourceFileName();

			if (name+".psc" == nameObj) {
				list.push_back(object);
			}
		}
	}

	return list;
}
