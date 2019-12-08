#include "VirtualMachine.h"
#include "Reader.h"
#include <cstdint>
#include <ctime>

int main(int argc, char *argv[]) {

	std::string pexPath = "E:\\WorkSkyMP\\papyrus-vm-build\\pex\\OpcodesTest.pex";

	std::string pexPathForm = "E:\\WorkSkyMP\\papyrus-vm-build\\pex\\TestObject.pex";


	try {
		constexpr int n = 2;

		if (argc < n) {

		}	// Path to a compiled script file (including '.pex' suffix)
		else
			//pexPath = argv[1];		// Path to a compiled script file (including '.pex' suffix)
		const std::string pathToExe = argv[0];		// i.e. 'C:/papyrus-vm-main.exe' // We can ignore it

		std::vector<std::string> allPath = { pexPath , pexPathForm };

		Reader reader(allPath);

		std::vector<std::shared_ptr<PexScript>> vector = reader.GetSourceStructures();

		VirtualMachine vm(vector);

		std::shared_ptr<int> assertId(new int(1));
		vm.RegisterFunction( "", "Print", FunctionType::GlobalFunction, [=](VarValue self, const std::vector<VarValue> args) {
			if (args.size() >= 1) {
				std::string showString = (const char*)args[0];
				std::cout << std::endl <<"[!] Papyrus says: " << showString << std::endl << std::endl;
				(*assertId) = 1;
			}
			return VarValue::None();
		});

		vm.RegisterFunction("", "Assert", FunctionType::GlobalFunction ,[=](VarValue self, std::vector<VarValue> args) {
			if (args.size() >= 1) {
				bool success = (bool)args[0];
				std::string message = "\t Assertion " + std::string(success ? "succeed" : "failed") + " (" + std::to_string(*assertId) + ")";
				(*assertId)++;
				if (!success) {
					throw std::runtime_error(message);
				}
				std::cout << message << std::endl;
			}
			return VarValue::None();
		});

		class TestObject : public IGameObject {
			const std::string  MY_ID = "0x006AFF2E";

		public:
			const char* GetStringID() override { return MY_ID.c_str(); };
		};

		std::shared_ptr<IGameObject> testObject(new TestObject);









		VarForBuildActivePex vars;

		std::vector<std::pair<std::string, VarValue>> mapArgs;

		mapArgs.push_back(std::make_pair<std::string, VarValue>("OpcodeRef", VarValue(testObject.get())));

		vars["OpcodesTest"] = mapArgs;



		vm.AddObject(testObject, { "OpcodesTest", "TestObject" }, vars);

		std::vector<VarValue> functionArgs;
		vm.SendEvent(testObject,"Main", functionArgs);


		return 0;
	
	}
	catch (std::exception &e) {
		std::cerr << "\n\n\n[!!!] Unhandled exception\n\t" << e.what() << "\n\n\n" << std::endl;
		return 1;
	}
}




