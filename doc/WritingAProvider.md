# Writing a `ISystProviderTool`

The `ISystProviderTool` interface is well-documented in `interface/ISystProviderTool.hh`, but some specifics are highlighted here for
clarity.

## Required

The interface defines three abstract methods that must be implemented by any subclass:
  * `SystMetaData ISystProviderTool::BuildSystMetaData(fhicl::ParameterSet const &, paramId_t)`:
    This method performs the [Tool Configuration](ToolConfiguration.md) to [Parameter Headers](ParameterHeaders.md) conversion. The passed `paramId_t` should be used as the  `SystParamHeader::systParamId` for first generated `SystParamHeader`. Any subsequent `SystParamHeader` returned `SystMetaData` should be sequentially numbered---this is checked.
  * `bool ISystProviderTool::SetupResponseCalculator(fhicl::ParameterSet const &)`:
    This method performs the final instance configuration from the [Parameter Headers](ParameterHeaders.md). After this method has been called, any setup required to calculate event responses is expected to have been performed.
    *N.B.* The parameter headers are read into the instance `SystMetaData` by non-virtual base-class methods and cannot be modified by subclasses, any `SystParamHeader` configuration must be done in `ISystProviderTool::BuildSystMetaData`.
  * `std::unique_ptr<EventResponse> ISystProviderTool::GetEventResponse(art::Event const &)`:
    Once fully configured, subclasses should be able to calculate the relevant event responses to any parameters that they handle. *N.B.* Responses to parameters that do not effect a response for a given event can be omitted. Returning an empty `EventResponse` is fully valid.

## Optional

A number of optional virtual methods are available for subclass override:
  * `fhicl::ParameterSet GetExampleToolConfiguration()`:
    Subclasses should provide an example tool configuration document that can be specialized by users when performing physics analyses. This is not an abstract method to reduce the development burden, but it's override by subclasses is very strongly encouraged.
  * `std::string AsString()`:
    Subclasses can provide a string representation of their state.
  * `fhicl::ParameterSet GetExtraToolOptions()`:
    Subclasses that use additional configuration over and above the standard [Parameter Headers](ParameterHeaders.md) format should return it here. It will automatically be built into the output parameter headers document for later initialization.

## Utilities

For a concrete example of a systematic provider, see [ExampleSystProvider](ExampleSystProvider.md) and `systproviders/ExampleISystProvider_tool.cc`. The rest of this section highlights utility methods and example process flows to simplify the process of implementing new systematic providers.

### Tool Configuration Parsing

As `ISystProviderTool` instances must be configurable from a [Parameter Headers](ParameterHeaders.md) document, the recommended method of configuration is first to translate the [Tool Configuration](ToolConfiguration.md) document to a valid parameter headers document, and then write the initialization/setup routines in terms of this document. Methods to facilitate this process flow for a number of parameter description formats are provided in [utility/FHiCLSystParamHeaderUtility](../utility/FHiCLSystParamHeaderUtility.hh). These methods assume a somewhat standardized Tool Configuration FHiCL format that allows the specification of central values and variations---both distributed for interpolation and randomly thrown. An example Tool Config is shown below:

```
  ExampleWeightProvider_multiuniverse_centershift: {
    tool_type: "ExampleISystProvider"
    instance_name: "multiuniverse_centershift"

    central_value: 1
    variation_descriptor: "{-2,2}"
    rand_dist: "uniform"

    number_of_throws: 10
  }
```

For the example above, the method `ParseFHiCLVariationDescriptor` can be used to extract the `SystParamHeader::centralParamValue` of the parameter being configured as `1`, and the `SystParamHeader::oneSigmaShifts` as `-2` and `2`. Then `MakeFHiCLDefinedRandomVariations` is used to make `10` random throws according to a uniform distribution width `2 - -2 = 4` about the central value, `1`. These thrown values are then set as the `SystParamHeader::paramVariations`. `SystParamHeader::isCorrection`, `SystParamHeader::isSplineable`, and `SystParamHeader::isRandomlyThrown` are also set to their relevant values given the nature of the parameter extracted from the tool configuration. These two helper methods can be called together for a slightly more structured document by the meta-helper: `ParseFHiCLSimpleToolConfigurationParameter`. This assumes that the `<pname>_central_value`, `<pname>_variation_descriptor`, and if relevant, `<pname>_nthrows` and `<pname>_random_distribution` keys are all named correctly for a parameter named `<pname>`. The `variation_descriptor` key can also be used to define a list of points to calculate, *e.g.* `variation_descriptor: "[-3, -2, -1, 0, 1, 2, 3]"`, for regular lists the shorthand `variation_descriptor: "(<start>,<stop>,<step>)"`, can be used. The form, random, list, regular list is chosen based upon the wrapping brackets, note that the specified list is not a FHiCL list, but a FHiCL atomic string. See the method documentation in [utility/FHiCLSystParamHeaderUtility](../utility/FHiCLSystParamHeaderUtility.hh) for more details.
