# Parameter headers

## Motivation

The two levels of 'configuration' may seem clumsy at first, so the motivation will be described here.

Design principles:
  * The calculated responses of each 'event unit' to a configured set of systematic parameter variations should be stored in the `art::event` object.
  * These responses need meta-data to be correctly interpreted, *i.e.*: 'What parameter value does the response at index 3 correspond to?'
  * This meta-data does not change event by event, or file by file and it is therefore inefficient to carry it around event by event.
  * For the very vast majority of systematic parameters a fixed form of meta-data can be used to fully describe their usage, however, some form of extensibility of the meta-data format should be included.
  * As art jobs are designed to be distributed, a per-systematic provider configuration must be able to deterministically produce correctly synchronised event responses: *i.e.* If 100 randomly chosen variations of a parameter should be calculated, the same set of 100 variations must be used on each compute node that processes an art file.
  * The structure of configuration of individual systematic providers should not be fully specified to allow extensibility and generality of implementations, it should also be easily human readable, parse-able, and editable.

It was decided that the best fit for this was a staged configuration: firstly, the human-readable, per-`ISystProviderTool` configuration would be read and converted to the second, per-job configuration format, which can also be used to full interpret the calculated event responses. The [Tool Configuration](ToolConfiguration.md) is extensible and easy to read and edit. The second, per-job configuration file, the *Parameter Headers* serves two purposes, it allows concise, per-event vectors of doubles to be correctly interpreted as a wide range of event responses, and can also be used to configure an instance of a `ISystProviderTool` to determininstcally calculate the requested responses. Any extra `FHiCL` required for this configuration on top of just the serialized `systtools::SystParamHeader`s is called the `tool_options`.

The on-disk format of the parameter headers could have been anything, but `FHiCL` was chosen because the files in general will not be too large and any `tool_options` must be generally extensible. The human-readability of `FHiCL` is useful even it is not strictly necessary for the standard work flow. These format decisions could be revised in the future but would require updates to the `ISystProviderTool` subclasses that rely on extra `tool_options`.

## Format

The example introduced in [Tool Configuration: Description](ToolConfiguration.md) is converted by an instance of [`ExampleISystProvider`](../systproviders/ExampleISystProvider_tool.hh) to :

```
BEGIN_PROLOG
generated_systematic_provider_configuration: {
   ExampleISystProvider_multiuniverse_centershift: {
      ExampleSystToolsEventByEventLateral_all: {
         centralParamValue: 1
         isRandomlyThrown: true
         oneSigmaShifts: [
            -2,
            2
         ]
         paramVariations: [
            -2.42704620044744023843e-1,
            6.72229221295147727488e-1,
            -2.87007100337361986675e-1,
            -5.20900882170851398101e-1,
            4.2953405500459318489e-1,
            -9.3603670495511770433e-1,
            -8.81928152648805196989e-1,
            -6.56415778953291617626e-1,
            2.25181981400627417855,
            2.42714699282725065999
         ]
         prettyName: "ExampleSystToolsEventByEventLateral_all"
         systParamId: 0
      }
      instance_name: "multiuniverse_centershift"
      parameter_headers: [
         "ExampleSystToolsEventByEventLateral_all"
      ]
      tool_options: {
         apply_to_all: true
      }
      tool_type: "ExampleISystProvider"
   }
   syst_providers: [
      "ExampleISystProvider_multiuniverse_centershift"
   ]
}
END_PROLOG
```

by an invocation of [`GenerateSystProviderConfig`](,./app/GenerateSystProviderConfig.cc) ()*c.f.* [Tool Configuration: Converting to parameter headers](ToolConfiguration.md)). It should hopefully be more clear why these documents are not intended to be frequently human-modified. The structure of this document is as follows:
The outer-most element, `generated_systematic_provider_configuration` is a wrapping `FHiCL` table that can be passed in it's entirety to helper methods that will instantiate and fully configure all described `ISystProviderTool` subclasses. The `syst_providers` element, much like the equivalent element in the tool configuration document contains a list of keys, each of which correspond to a single `ISystProviderTool` subclass instance. The `tool_type` and `instance_name` atoms are also used in the same way as in the tool configuration. The `parameter_headers` sequence contains a list of keys that correspond to the `FHiCL` tables describing the systematic parameters exposed by the fully configured systematic provider. In this example, a single parameter, named *ExampleSystToolsEventByEventLateral_all*, is described. This FHiCL table is directly de-serialized to a `systtools::SystParamHeader` instance, which can be used by 'consumers' to interpret any calculated responses. Finally, the `tool_options` table can contain arbitrary `FHiCL` that should be used by the `ISystProviderTool` subclass instance to perform any extra configuration that cannot be described by the parameter `FHiCL` tables.

## `systtools::SystParamHeader`

In the example above, the `FHiCL` table:

```
  centralParamValue: 1
  isRandomlyThrown: true
  oneSigmaShifts: [
     -2,
     2
  ]
  paramVariations: [
     -2.42704620044744023843e-1,
     6.72229221295147727488e-1,
     -2.87007100337361986675e-1,
     -5.20900882170851398101e-1,
     4.2953405500459318489e-1,
     -9.3603670495511770433e-1,
     -8.81928152648805196989e-1,
     -6.56415778953291617626e-1,
     2.25181981400627417855,
     2.42714699282725065999
  ]
  prettyName: "ExampleSystToolsEventByEventLateral_all"
  systParamId: 0
```

describes a systematic parameter named *ExampleSystToolsEventByEventLateral_all*, with unique Id `0`, that will respond to relevant event units with ten responses to ten randomly thrown parameter variations between -1 and 3. The serialization to, and de-serialization from, `systtools::SystParamHeader` instances can be performed by helper methods found in [interface/FHiCLSystParamHeaderConverters.hh](../interface/FHiCLSystParamHeaderConverters.hh). Any members left in their default state are not written to `FHiCL` during serialization; more meta-data members than those shown in the above example exist and are well-documented in [interface/SystParamHeader.hh](../interface/SystParamHeader.hh).

The nature of the `FHiCL` format and c++ bindings mean that reading and writing parameter headers documents is well defined programatically. They are somewhat fragile with respect to manual modification by non-experts, but a number of validity checks are applied to the de-serialized vectors of `systtools::SystParamHeader` objects. These `Validate` methods can be found in [interface/SystMetaData.hh](../interface/SystMetaData.hh) and [interface/SystParamHeader.hh](../interface/SystParamHeader.hh).

## Interpreting responses

The `ISystProviderTool` interface specifies that subclasses provide event responses in a format described in [interface/EventResponse_product.hh](../interface/EventResponse_product.hh).
Herein inverted commas are used to specify that these words do not have their usual c++ STL meaning. The format is simply a 'list' of 'pairs' of parameter unique Ids and vectors of responses---in the form of double precision floating point numbers . For a given *event unit*, the Id contained in the 'pair' can be matched to the `systtools::SystParamHeader::systParamId` in a vector of `systtools::SystParamHeader`s (which is typedef'd as `systtools::SystMetaData`).
The matched `systtools::SystParamHeader` can then be used to interpret the response vector.

A helper class is provided to expose a simple API to a 'list' of `systtools::SystParamHeader`s instance created from the parsing of a parameter headers document by helper methods found in [utility/ParameterAndProviderConfigurationUtility.hh](../utility/ParameterAndProviderConfigurationUtility.hh). The helper class definition is well documented and can be found in [interpreters/ParamHeaderHelper.hh](../interpreters/ParamHeaderHelper.hh).
