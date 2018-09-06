# Writing a provider


# Tool configuration

As `systtools::ISystProviderTool` instances must be configurable from a 'parameter header' document, the recommended method of configuration is first to translate the tool-specific tool configuration to parameter headers, and then write the tool configuration routines in terms of this document. Methods to facilitate this for a number of parameter description formats are provided in (utility/FHiCLSystParamHeaderUtility)[../utility/FHiCLSystParamHeaderUtility.hh].

For the example above, the method `systtools::ParseFHiCLVariationDescriptor` is used to extract the `systtools::SystParamHeader::centralParamValue` of the parameter being configured as `1`, and the `systtools::SystParamHeader::oneSigmaShifts` as `-2` and `2`. Then `systtools::MakeFHiCLDefinedRandomVariations` is used to make `10` random throws according to a uniform distribution width `2 - -2 = 4` about the central value, `1`. These thrown values are then set as the `systtools::SystParamHeader::paramVariations`. `systtools::SystParamHeader::isCorrection`, `systtools::SystParamHeader::isSplineable`, and `systtools::SystParamHeader::isRandomlyThrown` are also set to their relevant values given the nature of the parameter extracted from the tool configuration. These two helper methods can be called together for a slightly more structured document by the meta-helper: `systtools::ParseFHiCLSimpleToolConfigurationParameter`. This assumes that the `<pname>_central_value`, `<pname>_variation_descriptor`, and if relevant, `<pname>_nthrows` and `<pname>_random_distribution` keys are all named correctly for a parameter named `<pname>`.

# Parameter header configuration
