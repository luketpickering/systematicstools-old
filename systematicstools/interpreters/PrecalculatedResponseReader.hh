#ifndef SYSTTOOLS_INTERPRETERS_PRECALCULATEDRESPONSEHELPER_SEEN
#define SYSTTOOLS_INTERPRETERS_PRECALCULATEDRESPONSEHELPER_SEEN

#include "systematicstools/interface/EventResponse_product.hh"
#include "systematicstools/interface/types.hh"

#include "systematicstools/interpreters/PolyResponse.hh"

#include "systematicstools/utility/exceptions.hh"

#include "TFile.h"
#include "TTree.h"

#include <iomanip>
#include <vector>

namespace systtools {

NEW_SYSTTOOLS_EXCEPT(invalid_tfile_name);
NEW_SYSTTOOLS_EXCEPT(invalid_ttree_name);

template <size_t Order> class PrecalculatedResponseReader {
public:
  NEW_SYSTTOOLS_EXCEPT(in_wrong_mode);
  NEW_SYSTTOOLS_EXCEPT(entry_overflow);
  NEW_SYSTTOOLS_EXCEPT(missing_TBranches);
  NEW_SYSTTOOLS_EXCEPT(too_many_headers);

private:
  TFile *file;
  TTree *tree;

  static const size_t NCoeffs = (Order + 1);

  param_header_map_t fHeaders;
  Int_t NIds;
  std::vector<Int_t> ids;
  /// Tree variable to hold responses.
  ///
  ///\note This is a 1D vector that is passed to the TTree as a 2D object, array
  /// stacking follows C standard for stack-allocated two dimensional arrays.
  std::vector<Double_t> coeffs_1D;

  void AllocateVectors(size_t NHeaders) {
    ids.clear();
    coeffs_1D.clear();

    std::fill_n(std::back_inserter(ids), NHeaders, 0);
    std::fill_n(std::back_inserter(coeffs_1D), NHeaders * NCoeffs, 0);
  }

  void SetBranchAddresses(TTree *tree) {
    if (tree->SetBranchAddress("nids", &NIds) ||
        tree->SetBranchAddress("ids", ids.data()) ||
        tree->SetBranchAddress("responses", coeffs_1D.data())) {
      throw missing_TBranches()
          << "[ERROR]: When trying to read precalculated response tree, failed "
             "to load all branches.";
    }
  }

public:
  PrecalculatedResponseReader() : file(nullptr), tree(nullptr) {}

  ///\brief Constructor for instantiating a PrecalculatedResponseReader in read
  /// mode
  PrecalculatedResponseReader(std::string const &file_name,
                              std::string const &tree_name, size_t NHeaders) {

    file = TFile::Open(file_name.c_str());
    if (!file || !file->IsOpen()) {
      throw invalid_tfile_name() << "[ERROR]: Failed to open input file named: "
                                 << std::quoted(file_name);
    }

    tree = dynamic_cast<TTree *>(file->Get(tree_name.c_str()));
    if (!tree) {
      throw invalid_ttree_name()
          << "[ERROR]: Failed to get TTree named: " << std::quoted(tree_name)
          << " from file named: " << std::quoted(file_name);
    }

    AllocateVectors(NHeaders);
    SetBranchAddresses(tree);
  }

  /// Gets the number of entries in an input tree when in read mode.
  size_t GetEntries() {
    if (!file || !tree) {
      throw in_wrong_mode()
          << "[ERROR]: Attempted to get number of entries from a "
             "PrecalculatedResponseReader instantiated by "
             "PrecalculatedResponseReader::MakeTreeWriter.";
    }
    return tree->GetEntries();
  }

  struct ParamPolyResponses {
    systtools::paramId_t pid;
    systtools::PolyResponse<Order> resp;
  };

  ///\brief Gets the parameterized, precalculated event responses for all
  /// relevant parameters for event number entry
  std::vector<ParamPolyResponses> GetEventResponse(size_t entry) {
    if (!file || !tree) {
      throw in_wrong_mode() << "[ERROR]: Attempted to get event response from "
                               "a PrecalculatedResponseReader instantiated by "
                               "PrecalculatedResponseReader::MakeTreeWriter.";
    }
    if (entry >= GetEntries()) {
      throw entry_overflow()
          << "[ERROR]: Requested event response for entry: " << entry
          << ", but this input file only has " << GetEntries() << " entries.";
    }

    tree->GetEntry(entry);
    std::vector<ParamPolyResponses> evresps;
    for (size_t p = 0; p < NIds; ++p) {
      evresps.push_back(ParamPolyResponses{
          systtools::paramId_t(ids[p]),
          systtools::PolyResponse<Order>(&coeffs_1D[p * NCoeffs])});
    }
    return evresps;
  }

  ///\brief Instantiator for a PrecalculatedResponseReader in write mode.
  ///
  ///\note The tree ownership is not passed. The caller is responsible for
  /// proper storage and writing of the TTree.
  static std::unique_ptr<PrecalculatedResponseReader<Order>>
  MakeTreeWriter(param_header_map_t headers, TTree *tree) {

    std::unique_ptr<PrecalculatedResponseReader<Order>> wrtr =
        std::make_unique<PrecalculatedResponseReader<Order>>();

    wrtr->fHeaders = headers;

    wrtr->AllocateVectors(headers.size());
    wrtr->tree = tree;

    wrtr->tree->Branch("nids", &wrtr->NIds, "nids/I");
    wrtr->tree->Branch("ids", wrtr->ids.data(), "ids[nids]/I");
    std::string rspb = std::string("responses[nids][") +
                       std::to_string(NCoeffs) + "]/D";
    wrtr->tree->Branch("responses", wrtr->coeffs_1D.data(), rspb.c_str());

    return wrtr;
  }
  ///\brief Converts discrete, splineable event responses to parameterized
  /// response functions and fills them to the tree.
  void AddEventResponses(event_unit_response_t eur) {
    if (file) {
      throw in_wrong_mode()
          << "[ERROR]: Attempted to fill event response from "
             "a PrecalculatedResponseReader not instantiated by "
             "PrecalculatedResponseReader::MakeTreeWriter.";
    }

    std::fill_n(coeffs_1D.data(), fHeaders.size() * NCoeffs, 0);

    ScrubUnityEventResponses(eur);
    NIds = 0;
    for (auto const &pr : eur) {
      if (fHeaders.find(pr.pid) == fHeaders.end()) {
        throw invalid_parameter_Id()
            << "[ERROR]: When trying to stash event responses with "
               "PrecalculatedResponseReader, couldn't find header for "
               "paramId_t: "
            << pr.pid;
      }
      SystParamHeader const &hdr = fHeaders[pr.pid].Header;

      ids[NIds] = pr.pid;
      std::array<double, NCoeffs> const &poly =
          PolyResponse<Order>(hdr.paramVariations, pr.responses);

      std::copy_n(&poly[0], NCoeffs, &coeffs_1D[NIds * NCoeffs]);
      NIds++;
    }
    tree->Fill();
  }
};
} // namespace systtools

#endif
