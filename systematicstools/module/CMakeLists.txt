
art_dictionary()

cet_build_plugin(SystToolsEventResponse art::EDProducer
  LIBRARIES
   PUBLIC
    systematicstools::utility
    systematicstools::interface
   PRIVATE
    art::Persistency_Common
    art::Framework_Services_Registry
    art_root_io::tfile_support
    art_root_io::TFileService_service
    ROOT::Core
)

