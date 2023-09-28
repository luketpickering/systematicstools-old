# ExampleSystProvider

This file gives an example of the suggested `ISystProviderTool` documentation layout.

## Overview

This dummy provider calculates simple event responses to illustrate how to write an `ISystProviderTool` subclass.

## Physics motivation

There isn't any, but if there was, it would go here.

## Full Tool Configuration

```
ExampleWeightProvider_multiuniverse_centershift: {
  tool_type: "ExampleISystProvider"
  instance_name: "multiuniverse_centershift"

  ## Correction-like
  # central_value: 1
  ## end Correction-like

  ## Multi-universe-like
  central_value: 1
  variation_descriptor: "{-2,2}" # optional
  rand_dist: "uniform" # optional
  number_of_throws: 10 # optional
  ## end Multi-universe-like

  ## Splineable
  # variation_descriptor: "(-2,2,0.5)"
  ## end Splineable

  # provide_lateral: false # Whether response should be a kinematic shift or an event weight.
  # is_global: false # Whether the response weight differs event-by-event or if the responses themselves can be stored in the parameter headers
  # apply_to_all: true # Whether to apply to all events or randomly select some events to produce responses for
  # param_name: my_param # Force the prettyName of the generated parameter.
}
```
