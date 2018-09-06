# Tool configuration

## Description

  As introduced in [Moving Parts](MovingParts.md), the 'tool configuration' is the human-writable FHiCL configuration file for a systematic provider. While
  the FHiCL table that gets passed to the `systtools::ISystProviderTool` instance is almost unstructured, there are tools provided that can speed
  development if some structure can be presumed.

  Firstly, `art::make_tool` requires that the `fhicl::ParameterSet` used to instantiate a subclass instance contains a string atom formed like `tool_type: SubclassClassName`, this is used to search for the tool factory method capable of instantiating instances of the named subclass. An optional `instance_name` string atom can be used to disambiguate multiple instances of the same `systtools::ISystProviderTool` subclass. In this example, we will use the only concrete implementation available in this package, `ExampleISystProvider`. The final necessary component in a tool configuration document is a sequence of keys that correspond to the tool instances to be configured, by default it is expected to be named `syst_providers`, but this default can be overridden if needed. An example of a minimal tool configuration then is:

  ```
    ExampleProvider_toolconfig: {
      tool_type: "ExampleISystProvider"
      instance_name: "myexample"
    }
    syst_providers: [ExampleProvider_toolconfig]
  ```

  A number of configurations of the example `systtools::ISystProviderTool`, `ExampleISystProvider` can be seen in [ExampleISystProvider_tool.Config.fcl](../fcl/ExampleISystProvider_tool.Config.fcl). For this example, we will use:

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

  The hope is that the intent of the tool configuration can be easily parsed
  by a non-expert human reader who has read the relevant documentation (which
  hopefully exists). The example here can be used to produce a parameter headers document that can be used to configure an instance of `ExampleISystProvider` that exposes a single systematic parameter; Event responses to the 10 randomly thrown parameter values (between -1 and 3) will be calculated for each event unit passed to the configured `ISystProviderTool`.

## Converting to parameter headers

The conversion between tool configuration and parameter headers is requested via the `ISystProviderTool` interface; but the implementation is fully tool-specific. See [Writing A Provider](WritingAProvider.md) for a description of the helper methods available for speeding up development of such an implementation.
An application for converting a well-formed tool configuration document (which may reference an arbitrary number of `ISystProviderTool` subclass instances) to parameter headers is provided [`GenerateSystProviderConfig`](,./app/GenerateSystProviderConfig.cc). It uses the tool configuration tables referenced from the specified sequence (named `syst_providers` by default), uses `art::make_tool` to instantiate instances of each of the relevant `ISystProviderTool`s, and then compiles the generated parameter header documents into a single FHiCL document. The application usage text reads:

```
[USAGE]: GenerateSystProviderConfig

        -?|--help        : Show this message.
        -l <policy_id>   : FHICL_FILE_PATH lookup policy:
                            0 : cet::filepath_maker
                           {1}: cet::filepath_lookup
                            2 : cet::filepath_lookup_nonabsolute
                            3 : cet::filepath_lookup_after1
        -p <envvar name> : Environment variable to use when searching for fhicl.
                           FHICL_FILE_PATH by default.
        -c <config.fcl>  : fhicl file to read.
        -o <output.fcl>  : fhicl file to write, stdout by default.
        -k <list key>    : fhicl key to look for list of providers,
                           "syst_providers" by default.
        -P               : Wrap output file in {BEGIN,END}_PROLOG.

```
