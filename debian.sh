#!/bin/bash -ex

psPlatformVersion="3.3.0"
distribution="lucid"
upstreamVersion="0.1"
debianVersion="0ubuntu${psPlatformVersion}"

config/autorun.sh

dch \
	--distribution "${distribution}" \
	--force-distribution \
	--newversion "${upstreamVersion}-${debianVersion}~${BUILD_NUMBER}" \
	"Continuous integration build #${BUILD_NUMBER}"

debuild -i -us -uc -b