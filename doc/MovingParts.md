# Event

For full details, see `art::event`. Will be related to some data-taking unit
of the experimental apparatus. For neutrino experiments, it is likely to be
equivalent to a proton beam trigger corresponding to a spill or a bucket of
protons.

## Event unit

This is a logical subdivision of the full `art::event` into 'units' that a
given systematic variation applies to. For neutrino interaction systematic
uncertainties, this will be an MC truth neutrino interaction that leaves some
selected event signature within the detector. However, this is a generalized
subdivision, and a different sytematic might affect the PID discriminator for
all MIP-like tracks within a detector, of which there may be more than one
per `art::event`. Often, 'event' and 'event unit' will be used
interchangeably.

# Responses

## Event unit response

The 'response' that some event unit incurs in as a result of a variation of
some parameter in the simulation.

## Weight response

Often event responses can be encoded as weights, which encapsulate the
relative change in probability for an event of that class to have been seen
under the assumed simulation parameter change. For neutrino interaction
systematic variations, this is almost always the simplest method of
propagation. As all independent event weights can be applied multiplicatively,
weight responses can be used by analyses with little-to-no knowledge of the
parameter being varied, as such, weight responses are logically separate from
other classes of responses (collectively called 'lateral responses') so that
they can be simply applied at analysis time.

## Lateral response

A general response may effect a change in any event property. As the response
is fully generalized, each analysis must know a priori how to apply a given
lateral event response.

A specific example for of neutrino--nucleus interactions, would be a change
in the binding potential of the nucleus causing a shift in the distribution
of final state lepton momenta. While such a response is hard to parameterize
exactly---it would be most effective to run multiple parallel
simulations---these kind of responses can be useful analysis tools.
Furthermore, the simulation-time efficiency gained by using such an ad hoc
parameterization would be negated if these responses were applied before
detector simulation (interaction simulation is often the least CPU-intensive
part of the simulation toolchain), as a result, true responses may be applied
to fully reconstructed quantities. This is an approximation and should be  
treated with care and assessed for each such response.

For other classes of lateral response, such as a shift in the reconstructed
momenta or PID discriminator value, the response can be less approximate and
as such this technique can be a useful tool for propagating systematic
variations of the detector model.

## Spline or Response function

An analytic interpolation of some discrete set of fully-calculated event
responses. This can be implemented as a single N-dimensional polynomial fit to
the calculated responses, or a piece-wise 'spline', such as implemented by the
`ROOT` class TSpline3. This is possible for both weight and lateral responses.

## Consumer

Any downstream analysis that uses the calculated event responses. Consumers
can interpret event responses by interrogating the 'parameter header'
information.

# Systematic provider

A 'Systematic provider', 'SystProvider', or 'SystProviderTool', is an
implementation of the `systtools::ISystProviderTool` interface. It defines
parameters, accepts 'tool configuration', produces 'parameter headers', and
calculates event responses to passed `art::event`s. An example is included
with this pacakge, but the modular design of `systematictools` is based
around `art::make_tool` dynamically instantiating instances of
`systtools::ISystProviderTool` subclasses---implemented in specific
packages---at runtime. For example, systematic providers for neutrino
interaction variations exist in the `nusystematics` package, which is
neutrino-specific, but not experiment specific. More-specific packages such
as a flux uncertainty package for the BNB, or a detector systematics package
for NOvA would be in-keeping with this design.

## Tool configuration

The tool configuration is a FHiCL document that can be used to fully
configure a systematic provider. The structure of the per-provider FHiCL
tables is not specified (other than that required by `art::make_tool`), and
specific provider documentation should be consulted for the tool
configuration layout. This FHiCL is meant to be read and modified by humans
in the standard workflow.

## Parameter headers

The parameter headers describe all of the systematic parameters that have
been considered when calculating the event responses. Given the
`systtools::paramId_t` of an `systools::event_unit_response_t`, a consumer
can look up all of the meta-data about the relevant systematic parameter and
its variations in the parameter headers. The parameter header information can
serialized from the `systtools::SystParamHeader` object interface to and from
a structured FHiCL format. The FHiCL format is free to contain extra
information that may be needed to configure the systematic provider in a
deterministic way across multiple art jobs. This information is not
de-serialized to the objectified interface.

Consumers are welcome to directly interrogate the parameter header database
in either FHiCL or `systtools::SystMetaData` for
(`typedef std::vector<SystParamHeader> SystMetaData`), but a helper class
that exposes convenience methods for common work flows is provided by
`systtools::ParamHeaderHelper`. More description of the c++ and FHiCL
formats, and the use of `systtools::ParamHeaderHelper`, is given in
[Parameter headers](ParameterHeaders.md).

While it is useful that the on-disk parameter header information is
human-readable, a standard workflow would not require any human modification
of the parameter headers. Each systematic provider, configured by a
well-formed tool configuration document is required to produce the
corresponding parameter header document that can be used to both re-configure
a systematic provider for event response calculation and interpret any
calculated event responses.
