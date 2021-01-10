#!/usr/bin/env bash

third_party/gn/out/gn gen out
ninja -C out
