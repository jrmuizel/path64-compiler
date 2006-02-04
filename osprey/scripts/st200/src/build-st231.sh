#!/bin/sh
perl targ_gen.pl -s st231 -o $1
sed 's/st231/st220/g' $1/isa/isa_subset.cxx > tt
mv tt $1/isa/isa_subset.cxx
sed 's/st231/st220/g' $1/proc/st200_si.cxx > tt
mv tt $1/proc/st200_si.cxx
