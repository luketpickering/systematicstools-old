#include "ISystProvider.hxx"

#include <iostream>
#include <map>

namespace larsyst {

ISystProviderCreator::ISystProviderCreator(
    std::string const &ISystProviderClassName) {
  SystProviderFactory::Register(ISystProviderClassName, this);
}

namespace SystProviderFactory {
static std::map<std::string, ISystProviderCreator *> registeredProviders;

void ListRegistered() {
  std::cout << "[INFO]: Registered LArSyst SystProviders ("
            << registeredProviders.size() << "):" << std::endl;
  for (auto const &r : registeredProviders) {
    std::cout << "\t" << r.first << std::endl;
  }
}

void Register(std::string name, ISystProviderCreator *creator) {
  if (!registeredProviders.count(name)) {
    registeredProviders[name] = creator;
    std::cout << "[INFO]: Registered ISysProvider named: " << name << std::endl;
  } else {
    std::cout << "[ERROR]: Attempted to register ISystProvider with name: "
              << name << ", but this name has already been used." << std::endl;
    throw;
  }
}

ISystProvider *Create(std::string const &name) {
  if (!registeredProviders.count(name)) {
    std::cout << "[ERROR]: Attempted to create ISystProvider with name: "
              << name << ", but this name has not been registered."
              << std::endl;
    ListRegistered();
    throw;
  } else {
    return registeredProviders[name]->Create();
  }
}
} // namespace SystProviderFactory
} // namespace larsyst
