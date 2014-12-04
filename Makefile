
#
#   make - Configuration
#

    MAKE_BINARY:=bin
    MAKE_DOCENT:=doc
    MAKE_LIBRAR:=lib
    MAKE_SOURCE:=src

#
#   make - Modules
#

    MAKE_MODULE:=$(MAKE_LIBRAR)/libcsps $(MAKE_LIBRAR)/libfastcal

#
#   make - Enumeration
#

    MAKE_BUILDS:=$(notdir $(wildcard $(MAKE_SOURCE)/*) )

#
#   make - Targets
#

    all:make-directories make-modules make-build
    build:make-directories make-build
    modules:make-modules
    documentation:make-directories make-doc
    clean:make-clean
    clean-modules:make-clean-modules
    clean-documentation:make-clean-documentation
    install:make-install
    uninstall:make-uninstall

#
#   make - Directives
#

    make-build:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) all && cp $(MAKE_SOURCE)/$(SOFT)/$(MAKE_BINARY)/$(SOFT) $(MAKE_BINARY)/ && ) true

    make-modules:
	@$(foreach LIBS, $(MAKE_MODULE), $(MAKE) -C $(LIBS) all && ) true

    make-doc:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) documentation && ln -fs ../../$(MAKE_SOURCE)/$(SOFT)/$(MAKE_DOCENT)/html $(MAKE_DOCENT)/html/$(SOFT) && ) true

#
#   make - Cleaning
#

    make-clean:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) clean && ) true
	rm -f $(MAKE_BINARY)/*

    make-clean-modules:
	@$(foreach LIBS, $(MAKE_MODULE), $(MAKE) -C $(LIBS) clean && ) true

    make-clean-documentation:
	rm -f $(MAKE_DOCENT)/html/*

#
#   make - Implementation
#

    make-install:
	@$(foreach SOFT, $(MAKE_BUILDS), cp $(MAKE_BINARY)/$(SOFT) /usr/bin/$(SOFT) && ) true

    make-uninstall:
	@$(foreach SOFT, $(MAKE_BUILDS), rm -f /usr/bin/$(SOFT) && ) true

#
#   make - Directories
#

    make-directories:
	mkdir -p $(MAKE_BINARY) $(MAKE_DOCENT)

