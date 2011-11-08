#!/bin/bash -ex

psPlatformVersion="ps3.4.0"
distribution="lucid"
upstreamVersion="1.0.0"
debianVersion="0ubuntu1"

config/autorun.sh

dch \
	--distribution "${distribution}" \
	--force-distribution \
	--newversion "${upstreamVersion}-${debianVersion}-${psPlatformVersion}~${BUILD_NUMBER}" \
	"Continuous integration build #${BUILD_NUMBER}"

debuild -i -us -uc -b
