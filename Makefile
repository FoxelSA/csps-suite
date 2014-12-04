
#
#   make - Configuration
#

    MAKE_BINARY:=bin
    MAKE_DOCENT:=doc
    MAKE_LIBRAR:=lib
    MAKE_SOURCE:=src
    MAKE_INPATH:=/usr/bin
    MAKE_CMCOPY:=cp
    MAKE_CMREMF:=rm -f
    MAKE_CMMKDR:=mkdir -p
    MAKE_CMMKLN:=ln -sf

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
    documentation:make-directories make-documentation
    clean:make-clean
    clean-all:make-clean make-clean-modules
    clean-modules:make-clean-modules
    clean-documentation:make-clean-documentation
    install:make-install
    uninstall:make-uninstall

#
#   make - Directives
#

    make-build:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) all OPENMP=$(OPENMP) && $(MAKE_CMCOPY) $(MAKE_SOURCE)/$(SOFT)/$(MAKE_BINARY)/$(SOFT) $(MAKE_BINARY)/ && ) true

    make-modules:
	@$(foreach LIBS, $(MAKE_MODULE), $(MAKE) -C $(LIBS) all OPENMP=$(OPENMP) && ) true

    make-documentation:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) documentation && $(MAKE_CMMKLN) ../../$(MAKE_SOURCE)/$(SOFT)/$(MAKE_DOCENT)/html $(MAKE_DOCENT)/html/$(SOFT) && ) true

#
#   make - Cleaning
#

    make-clean:
	$(MAKE_CMREMF) $(MAKE_BINARY)/*
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE) -C $(MAKE_SOURCE)/$(SOFT) clean && ) true

    make-clean-modules:
	@$(foreach LIBS, $(MAKE_MODULE), $(MAKE) -C $(LIBS) clean && ) true

    make-clean-documentation:
	$(MAKE_CMREMF) $(MAKE_DOCENT)/html/*

#
#   make - Implementation
#

    make-install:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE_CMCOPY) $(MAKE_BINARY)/$(SOFT) $(MAKE_INPATH)/$(SOFT) && ) true

    make-uninstall:
	@$(foreach SOFT, $(MAKE_BUILDS), $(MAKE_CMREMF) $(MAKE_INPATH)/$(SOFT) && ) true

#
#   make - Directories
#

    make-directories:
	$(MAKE_CMMKDR) $(MAKE_BINARY) $(MAKE_DOCENT)

