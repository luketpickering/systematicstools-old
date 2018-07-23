#include "systematicstools/interface/ISystProvider_tool.hh"
#include "systematicstools/interface/SystMetaData.hh"

#include "systematicstools/utility/configure_syst_providers.hh"
#include "systematicstools/utility/md5.hh"
#include "systematicstools/utility/printers.hh"
#include "systematicstools/utility/string_parsers.hh"
#include "systematicstools/utility/build_parameter_set_from_header.hh"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include "cetlib/filepath_maker.h"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace cliopts {
std::string fclname = "";
std::string envvar = "FHICL_FILE_PATH";
std::string producer_name = "";
std::string analyzer_name = "";
int lookup_policy = 1;
} // namespace cliopts

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0] << "\n" << std::endl;
  std::cout << "\t-?|--help        : Show this message.\n"
               "\t-l <policy_id>   : FHICL_FILE_PATH lookup policy:\n"
               "\t                    0 : cet::filepath_maker\n"
               "\t                   {1}: cet::filepath_lookup\n"
               "\t                    2 : cet::filepath_lookup_nonabsolute\n"
               "\t                    3 : cet::filepath_lookup_after1\n"
               "\t-p <envvar name> : Environment variable to use when searching"
               " for fhicl. \n"
               "\t                   FHICL_FILE_PATH by default.\n"
               "\t-c <config.fcl>  : fhicl file to read.\n"
               "\t-P <producer key>: fhicl key of the producer, parameter set\n"
               "\t                   expected to live at "
               "\"physics.producers.<-P>\".\n"
               "\t-A <analyzer key>: fhicl key of an analyzer. Will md5 the\n"
               "\t                   content of "
               "\"physics.analyzers.<-A>.generated_systematic_provider_"
               "configuration\"."
            << std::endl;
}

void HandleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
    } else if (std::string(argv[opt]) == "-l") {
      cliopts::lookup_policy = systtools::str2T<int>(argv[++opt]);
      if (cliopts::lookup_policy > 3 || cliopts::lookup_policy < 0) {
        std::cout << "[ERROR]: -l expected to be passed an integer between 0 "
                     "and 3."
                  << std::endl;
        SayUsage(argv);
        exit(1);
      }
    } else if (std::string(argv[opt]) == "-p") {
      cliopts::envvar = argv[++opt];
      char const *ev = getenv(cliopts::envvar.c_str());
      if (!ev) {
        std::cout << "[ERROR]: Could not read environment variable:"
                  << std::quoted(cliopts::envvar)
                  << ". Please supply a variable containing a valid path list."
                  << std::endl;
        SayUsage(argv);
        exit(1);
      }
    } else if (std::string(argv[opt]) == "-c") {
      cliopts::fclname = argv[++opt];
    } else if (std::string(argv[opt]) == "-P") {
      cliopts::producer_name = argv[++opt];
    } else if (std::string(argv[opt]) == "-A") {
      cliopts::analyzer_name = argv[++opt];
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

fhicl::ParameterSet ReadParameterSet(char const *argv[]) {
  char const *ev = nullptr;

  if (cliopts::lookup_policy != 0) {
    ev = getenv(cliopts::envvar.c_str());
    if (!ev) {
      std::cout << "[ERROR]: Could not read environment variable:\""
                << cliopts::envvar
                << "\". Please supply a variable containing a valid path list "
                   "via the -p command line option."
                << std::endl;
      SayUsage(argv);
      exit(1);
    }
  }

  std::unique_ptr<cet::filepath_maker> fm(nullptr);

  switch (cliopts::lookup_policy) {
  case 0: {
    fm = std::make_unique<cet::filepath_maker>();
    break;
  }
  case 1: {
    fm = std::make_unique<cet::filepath_lookup>(ev);
    break;
  }
  case 2: {
    fm = std::make_unique<cet::filepath_lookup_nonabsolute>(ev);

    break;
  }
  case 3: {
    fm = std::make_unique<cet::filepath_lookup_after1>(ev);
    break;
  }
  default: {}
  }
  fhicl::ParameterSet ps;
  fhicl::make_ParameterSet(cliopts::fclname, *fm, ps);
  return ps;
}

int main(int argc, char const *argv[]) {
  HandleOpts(argc, argv);
  if (!cliopts::fclname.size()) {
    std::cout << "[ERROR]: Expected to be passed a -c option." << std::endl;
    SayUsage(argv);
    exit(1);
  }

  if (!cliopts::producer_name.size() && !cliopts::analyzer_name.size()) {
    std::cout << "[ERROR]: Expected at least one of -P or -A to be passed."
              << std::endl;
    SayUsage(argv);
    exit(1);
  }

  fhicl::ParameterSet in_ps = ReadParameterSet(argv);

  std::string producer_md5{""}, analyzer_md5{""};

  if (cliopts::producer_name.size()) {
    fhicl::ParameterSet producer = in_ps.get<fhicl::ParameterSet>(
        std::string("physics.producers.") + cliopts::producer_name);

    std::string module_type = producer.get<std::string>("module_type");

    if (module_type != "SystToolsEventResponse") {
      std::cout << "[ERROR]: Passed producer is not of the expected type, "
                << std::quoted(module_type)
                << " != " << std::quoted("SystToolsEventResponse") << std::endl;
      exit(1);
    }

    fhicl::ParameterSet syst_provider_config =
        producer.get<fhicl::ParameterSet>(
            "generated_systematic_provider_configuration");

    producer_md5 = md5(syst_provider_config.to_string());
    std::cout << "[INFO]: Hashed producer config: " << std::quoted(producer_md5)
              << std::endl;
  }

  if (cliopts::analyzer_name.size()) {
    fhicl::ParameterSet producer = in_ps.get<fhicl::ParameterSet>(
        std::string("physics.analyzers.") + cliopts::analyzer_name);

    fhicl::ParameterSet syst_provider_config =
        producer.get<fhicl::ParameterSet>(
            "generated_systematic_provider_configuration");

    analyzer_md5 = md5(syst_provider_config.to_string());
    std::cout << "[INFO]: Hashed analyzer config: " << std::quoted(analyzer_md5)
              << std::endl;
  }

  if (producer_md5.size() && analyzer_md5.size()) {
    if (producer_md5 == analyzer_md5) {
      std::cout << "[INFO]: Compatible configurations!" << std::endl;
    } else {
      std::cout << "[WARN]: Incompatible configurations." << std::endl;
      exit(1);
    }
  }
  return 0;
}
