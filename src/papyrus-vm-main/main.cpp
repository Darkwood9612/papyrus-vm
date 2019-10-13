#include "VirtualMachine.h"
#include "DataVisualization.h"
#include <cstdint>
#include <ctime>
#include <map>
#include "VMLinker.h"

int main(int argc, char *argv[]) {
	try {
		constexpr int n = 2;
		if (argc < n) {
			throw std::runtime_error("At least " + std::to_string(n) + " arguments expected");
		}

		// i.e. 'C:/papyrus-vm-main.exe'
		// We can ignore it
		const std::string pathToExe = argv[0];

		// Path to a compiled script file (including '.pex' suffix)
		const std::string pscPath = argv[1];

		std::shared_ptr<VirtualMachine> vm(new VirtualMachine(pscPath));

		std::shared_ptr<int> assertId(new int(1));

		vm->RegisterFunction("Print", [=](VarValue self, std::vector<VarValue> args) {
			if (args.size() >= 1) {
				std::cout << "[!] Papyrus says: " << vm->GetString(args[0]) << std::endl;
			}
			return VarValue::None();
		});

		vm->RegisterFunction("Assert", [=](VarValue self, std::vector<VarValue> args) {
			if (args.size() >= 1) {
				bool success = (bool)args[0];
				std::string message = "Assertion " + std::string(success ? "succeed" : "failed") + " (" + std::to_string(*assertId) + ")";
				(*assertId)++;
				if (!success) {
					throw std::runtime_error(message);
				}
				std::cout << message << std::endl;
			}
			return VarValue::None();
		});

		std::pair<uint16_t, VarValue> returnValue;
		std::vector<VarValue> functionArgs;
		returnValue = vm->RunFunction("Main", functionArgs);

		return 0;
	}
	catch (std::exception &e) {
		std::cerr << "\n\n\n[!!!] Unhandled exception\n\t" << e.what() << "\n\n\n" << std::endl;
		return 1;
	}
}




