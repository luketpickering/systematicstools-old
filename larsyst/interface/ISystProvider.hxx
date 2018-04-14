#ifndef ISYSTPROVIDER_HXX_SEEN
#define ISYSTPROVIDER_HXX_SEEN

#include "larsyst/interface/EventResponse_product.hxx"
#include "larsyst/interface/SystMetaData_product.hxx"

#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"

#include <iostream>
#include <map>
#include <string>

namespace larsyst {

class ISystProvider {
public:
  ISystProvider() : fName(""){};
  ISystProvider(std::string const &name) : fName(name){};
  virtual SystMetaData GetSystMetaData() const = 0;
  virtual EventResponse GetEventResponse(art::Event &) = 0;

  virtual char Configure(fhicl::ParameterSet const &, char) = 0;

  std::string const &GetName() { return fName; }

  virtual ~ISystProvider(){};

protected:
  std::string fName;
};

class ISystProviderCreator {
public:
  ISystProviderCreator(std::string const &ISystProviderClassName);
  virtual ~ISystProviderCreator(){};
  virtual ISystProvider *Create() = 0;
};

template <class T>
class ISystProviderCreatorImplementation : public ISystProviderCreator {
public:
  ISystProviderCreatorImplementation<T>(
      std::string const &ISystProviderClassName)
      : ISystProviderCreator(ISystProviderClassName) {}
  ISystProvider *Create() { return new T; }
};

#define DECLARE_ISYSTPROVIDER(T)                                               \
private:                                                                       \
  static const ISystProviderCreatorImplementation<T> creator;

#define REGISTER_ISYSTPROVIDER(T)                                              \
  const ISystProviderCreatorImplementation<T> T::creator(#T);

namespace SystProviderFactory {
  void Register(std::string name, ISystProviderCreator *creator);
  ISystProvider *Create(std::string const &name);
}

} // namespace larsyst

#endif
