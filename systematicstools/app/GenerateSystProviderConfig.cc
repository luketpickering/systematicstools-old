#include "systematicstools/interface/ISystProviderTool.hh"
#include "systematicstools/interface/SystMetaData.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/utility/ParameterAndProviderConfigurationUtility.hh"
#include "systematicstools/utility/md5.hh"
#include "systematicstools/utility/printers.hh"
#include "systematicstools/utility/string_parsers.hh"

#ifdef NO_ART
#include "systematicstools/systproviders/ExampleISystProviderTool.hh"
#endif

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#ifndef NO_ART
#include "cetlib/filepath_maker.h"
#endif

#include <fstream>
#include <iomanip>
#include <iostream>

namespace cliopts {
std::string fclname = "";
std::string outputfile = "";
std::string envvar = "FHICL_FILE_PATH";
std::string fhicl_key = "syst_providers";
#ifndef NO_ART
int lookup_policy = 1;
#endif
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
               "\t-o <output.fcl>  : fhicl file to write, stdout by default.\n"
               "\t-k <list key>    : fhicl key to look for list of providers,\n"
               "\t                   \"syst_providers\" by default.\n"
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
    } else if (std::string(argv[opt]) == "-o") {
      cliopts::outputfile = argv[++opt];
    } else if (std::string(argv[opt]) == "-k") {
      cliopts::fhicl_key = argv[++opt];
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

#ifndef NO_ART
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
    exit(1);
  }

  fhicl::ParameterSet in_ps = ReadParameterSet(argv);

  std::function<std::unique_ptr<systtools::ISystProviderTool>(
      fhicl::ParameterSet const &)>
      InstanceBuilder;
#ifndef NO_ART
  InstanceBuilder = art::make_tool<systtools::ISystProviderTool>
#else
  InstanceBuilder = [](fhicl::ParameterSet const &paramset)
      -> std::unique_ptr<systtools::ISystProviderTool> {
    std::string tool_type = "";
    paramset.get_if_present("tool_type", tool_type);
    if (tool_type == "ExampleISystProvider") {
      return std::make_unique<ExampleISystProvider>(paramset);
    } else {
      std::cerr << "[ERROR]: When build artless can only instantiate "
                   "ExampleISystProvider instances. A "
                << std::quoted(tool_type) << " instance was requested."
                << std::endl;
      throw;
    }
  };
#endif
      systtools::provider_list_t tools =
          systtools::ConfigureISystProvidersFromToolConfig(in_ps, InstanceBuilder,
                                                         cliopts::fhicl_key);

  fhicl::ParameterSet out_ps;
  std::vector<std::string> providerNames;
  for (auto &prov : tools) {
    if (!systtools::Validate(prov->GetSystMetaData(), false)) {
      throw systtools::invalid_SystMetaData()
          << "[ERROR]: A parameter handled by provider: "
          << std::quoted(prov->GetFullyQualifiedName())
          << " failed validation.";
    }
    fhicl::ParameterSet tool_ps = prov->GetParameterHeadersDocument();
    out_ps.put(prov->GetFullyQualifiedName(), tool_ps);
    providerNames.push_back(prov->GetFullyQualifiedName());
  }
  out_ps.put("syst_providers", providerNames);

  fhicl::ParameterSet wrapped_out_ps;
  wrapped_out_ps.put("generated_systematic_provider_configuration", out_ps);

  std::ostream *os(nullptr);

  if (cliopts::outputfile.size()) {
    std::ofstream *fs = new std::ofstream(cliopts::outputfile);
    if (!fs->is_open()) {
      std::cout << "[ERROR]: Failed to open " << cliopts::outputfile
                << " for writing." << std::endl;
      exit(1);
    }
    os = fs;
  } else {
    os = &std::cout;
  }

  (*os) << wrapped_out_ps.to_indented_string() << std::endl;

  if (cliopts::outputfile.size()) {
    static_cast<std::ofstream *>(os)->close();
    delete os;
  }

  std::cout << (cliopts::outputfile.size() ? "Wrote" : "Built")
            << " systematic provider configuration with md5: "
            << std::quoted(md5(out_ps.to_compact_string())) << std::flush;
  if (cliopts::outputfile.size()) {
    std::cout << " to " << std::quoted(cliopts::outputfile) << std::flush;
  }
  std::cout << std::endl;
}
