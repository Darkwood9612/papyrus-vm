#pragma once
#include "VirtualMachine.h"

class Linker{

public:
	void CreateLinksForSctructure(std::shared_ptr<VirtualMachine>structure);

private:
	std::vector<std::shared_ptr<VirtualMachine>> ListObjects;

	std::vector<std::shared_ptr<VirtualMachine>> FillListLinks(std::vector<std::string> &listNames);
};

