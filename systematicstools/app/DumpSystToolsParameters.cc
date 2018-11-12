#include "systematicstools/interface/types.hh"

#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#ifndef NO_ART
#include "cetlib/filepath_maker.h"
#endif

#include <iostream>
#include <string>
#include <vector>

using namespace systtools;

namespace cliopts {
std::string fclname = "";
std::string envvar = "FHICL_FILE_PATH";
std::string fhicl_key = "generated_systematic_provider_configuration";
#ifndef NO_ART
int lookup_policy = 1;
#endif
bool ShowCorrections = true;
bool ShowVariations = true;
bool ShowTweaks = false;
bool ShowDetails = false;
} // namespace cliopts

void SayUsage(char const *argv[]) {
  std::cout << "[USAGE]: " << argv[0] << "\n" << std::endl;
  std::cout << "\t-?|--help        : Show this message.\n"
#ifndef NO_ART
               "\t-l <policy_id>   : FHICL_FILE_PATH lookup policy:\n"
               "\t                    0 : cet::filepath_maker\n"
               "\t                   {1}: cet::filepath_lookup\n"
               "\t                    2 : cet::filepath_lookup_nonabsolute\n"
               "\t                    3 : cet::filepath_lookup_after1\n"
               "\t-p <envvar name> : Environment variable to use when searching"
               " for fhicl. \n"
               "\t                   FHICL_FILE_PATH by default.\n"
#endif
               "\t-c <config.fcl>  : fhicl file to read.\n"
               "\t-k <list key>    : fhicl key to look for parameter headers,\n"
               "\t                   "
               "\"generated_systematic_provider_configuration\"\n"
               "\t                   by default.\n"
               "\t-C               : Display only corrections.\n"
               "\t-V               : Display only variations.\n"
               "\t-T               : Display the configured tweaks of each\n"
               "\t                   parameter.\n"
               "\t-D               : Display details about each parameter.\n"
            << std::endl;
}

void HandleOpts(int argc, char const *argv[]) {
  int opt = 1;
  while (opt < argc) {
    if ((std::string(argv[opt]) == "-?") ||
        (std::string(argv[opt]) == "--help")) {
      SayUsage(argv);
      exit(0);
#ifndef NO_ART
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
#endif
    } else if (std::string(argv[opt]) == "-c") {
      cliopts::fclname = argv[++opt];
    } else if (std::string(argv[opt]) == "-k") {
      cliopts::fhicl_key = argv[++opt];
    } else if (std::string(argv[opt]) == "-C") {
      cliopts::ShowCorrections = true;
      cliopts::ShowVariations = false;
    } else if (std::string(argv[opt]) == "-V") {
      cliopts::ShowCorrections = false;
      cliopts::ShowVariations = true;
    } else if (std::string(argv[opt]) == "-T") {
      cliopts::ShowTweaks = true;
    } else if (std::string(argv[opt]) == "-D") {
      cliopts::ShowDetails = true;
    } else {
      std::cout << "[ERROR]: Unknown option: " << argv[opt] << std::endl;
      SayUsage(argv);
      exit(1);
    }
    opt++;
  }
}

fhicl::ParameterSet ReadParameterSet(char const *[]) {

#ifndef NO_ART
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
#endif

  fhicl::ParameterSet ps;
#ifndef NO_ART
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
  fhicl::make_ParameterSet(cliopts::fclname, *fm, ps);
#else
  ps = fhicl::make_ParameterSet(cliopts::fclname);
#endif
  return ps;
}

int main(int argc, char const *argv[]) {
  HandleOpts(argc, argv);
  if (!cliopts::fclname.size()) {
    std::cout << "[ERROR]: Expected to be passed a -c option." << std::endl;
    SayUsage(argv);
    return 1;
  }

  fhicl::ParameterSet ps = ReadParameterSet(argv);

  systtools::param_header_map_t configuredParameterHeaders =
      systtools::BuildParameterHeaders(
          ps.get<fhicl::ParameterSet>(cliopts::fhicl_key));

  if (!configuredParameterHeaders.size()) {
    std::cout << "[ERROR]: Expected systematric providers loaded from input: "
              << std::quoted(cliopts::fclname)
              << " to provide some parameter headers." << std::endl;
    return 2;
  }

  size_t found = 0;
  for (auto idhdr : configuredParameterHeaders) {
    systtools::SystParamHeader const &hdr = idhdr.second.Header;

    if (!cliopts::ShowCorrections && hdr.isCorrection) {
      continue;
    }
    if (!cliopts::ShowVariations && !hdr.isCorrection) {
      continue;
    }
    found++;
  }
  std::cout << "[INFO]: Found " << found << " parameters:" << std::endl;
  for (auto idhdr : configuredParameterHeaders) {
    systtools::SystParamHeader const &hdr = idhdr.second.Header;

    if (!cliopts::ShowCorrections && hdr.isCorrection) {
      continue;
    }
    if (!cliopts::ShowVariations && !hdr.isCorrection) {
      continue;
    }

    std::cout << hdr.prettyName;
    if (cliopts::ShowDetails || cliopts::ShowTweaks) {
      std::cout << ":" << std::endl;
    } else {
      std::cout << std::endl;
    }
    if (cliopts::ShowDetails) {

      std::cout << "\tSyst provider: " << idhdr.second.ProviderFQName
                << std::endl;
      std::cout << "\tId: " << hdr.systParamId << std::endl;
      std::cout << "\tType: "
                << (hdr.isWeightSystematicVariation ? "" : "lateral ")
                << (hdr.isCorrection ? "correction" : "variation") << " "
                << (hdr.isWeightSystematicVariation ? "weight " : "")
                << std::endl;

      if (hdr.centralParamValue != systtools::kDefaultDouble) {
        std::cout << "\tCentral value: " << hdr.centralParamValue << std::endl;
      }
      if (hdr.isResponselessParam) {
        std::cout << "\t Responds via parameter id: " << hdr.responseParamId
                  << std::endl;
      }
    }
    if (cliopts::ShowTweaks) {
      std::cout << "\tVariations: [";
      for (size_t i = 0; i < hdr.paramVariations.size(); ++i) {
        std::cout << hdr.paramVariations[i]
                  << (((i + 1) == hdr.paramVariations.size()) ? "]" : ", ");
        ;
      }
      std::cout << std::endl;
    }
    if (cliopts::ShowDetails || cliopts::ShowTweaks) {
      std::cout << std::endl;
    }
  }
}
