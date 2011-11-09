#!/bin/bash -ex

psPlatformVersion="3.4.0"
distribution="snapshots"
upstreamVersion="1.0.2"
debianVersion="0ubuntu1"

config/autorun.sh

dch \
	--distribution "${distribution}" \
	--newversion "${upstreamVersion}-${debianVersion}-ps${psPlatformVersion}~${BUILD_NUMBER}" \
	"Continuous integration build #${BUILD_NUMBER}"

debuild -i -us -uc -b
