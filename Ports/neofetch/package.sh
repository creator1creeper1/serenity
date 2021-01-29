#!/usr/bin/env -S bash ../.port_include.sh

port=neofetch
version=7.1.0
useconfigure=false
depends="bash jq"
files="https://github.com/dylanaraps/neofetch/archive/${version}.tar.gz neofetch-${version}.tar.gz"
