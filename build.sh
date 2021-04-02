#!/usr/bin/env bash

third_party/gn/out/gn gen out
ninja -C out

patchelf --replace-needed libcpr.so.1 "$PWD"/third_party/cpr/build/lib/libcpr.so.1 out/kstodon
patchelf --replace-needed libcpr.so "$PWD"/third_party/cpr/build/lib/libcpr.so out/kstodon
patchelf --replace-needed libcpr.so.1 "$PWD"/third_party/cpr/build/lib/libcpr.so.1 out/libkstodon_lib.so
patchelf --replace-needed libcpr.so "$PWD"/third_party/cpr/build/lib/libcpr.so out/libkstodon_lib.so

