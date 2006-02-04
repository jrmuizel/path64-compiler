#!/bin/sh
# The order in which the -s subsets are specified
# does not matter functionally, but this order
# makes the generated files differ less from the
# previous st231-based versions.
perl targ_gen.pl -s st231 -s st230 -s st220 -o $1
