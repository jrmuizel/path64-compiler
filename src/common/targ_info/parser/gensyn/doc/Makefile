# Doxygen used the dot tool to generate graphs.
# You must set your LD_LIBRARY_PATH and PATH to
# respectively:
# /home/dtools/solaris/Graphviz/lib/graphviz/lib/graphviz/
# /home/dtools/solaris/Graphviz/bin/

DOX = /home/dtools/solaris/doxygen/bin/doxygen
PERL = perl
GET_ERROR_PERL = /vob/DMD-Compil/doc/get_errors.pl

DOX_CONF_FILE = GenSyn.cfg
DOX_DOC_DIR = gensyn_doc

SOURCE_DIR = ../src/*.c ../include/*.h

ERROR_DOC_FILES = GenSynErrors.txt

help:
	@echo "Help:"
	@echo "	help	-> print this help."
	@echo "	dox 	-> build doxygen documentation."
	@echo "	err	-> build error messages documentation."
	@echo "	all	-> build dox and err targets."
	@echo "	clean	-> clean generated files."


dox: $(DOX_DOC_DIR)

$(DOX_DOC_DIR): $(SOURCE_DIR) $(DOX_CONF_FILE)
	$(DOX) $(DOX_CONF_FILE)
	cd $(DOX_DOC_DIR)/latex/; gmake;

err: $(ERROR_DOC_FILES)


all: err dox


GenSynErrors.txt: ../include/gensyn_error.h
	$(PERL) $(GET_ERROR_PERL) -o $@ -f $<

clean:
	rm -rf $(DOX_DOC_DIR)
