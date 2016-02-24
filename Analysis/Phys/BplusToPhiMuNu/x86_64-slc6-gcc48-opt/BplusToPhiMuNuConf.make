#-- start of make_header -----------------

#====================================
#  Document BplusToPhiMuNuConf
#
#   Generated Wed Feb 24 12:24:59 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNuConf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuConf_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNuConf

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuConf = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuConf.make
cmt_local_tagfile_BplusToPhiMuNuConf = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuConf.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuConf = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuConf = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNuConf)
#-include $(cmt_local_tagfile_BplusToPhiMuNuConf)

ifdef cmt_BplusToPhiMuNuConf_has_target_tag

cmt_final_setup_BplusToPhiMuNuConf = $(bin)setup_BplusToPhiMuNuConf.make
#cmt_final_setup_BplusToPhiMuNuConf = $(bin)BplusToPhiMuNu_BplusToPhiMuNuConfsetup.make
cmt_local_BplusToPhiMuNuConf_makefile = $(bin)BplusToPhiMuNuConf.make

else

cmt_final_setup_BplusToPhiMuNuConf = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuConf = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuConf_makefile = $(bin)BplusToPhiMuNuConf.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNuConf :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNuConf'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNuConf/
#BplusToPhiMuNuConf::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/genconfig_header
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

# Use genconf.exe to create configurables python modules, then have the
# normal python install procedure take over.

.PHONY: BplusToPhiMuNuConf BplusToPhiMuNuConfclean

confpy  := BplusToPhiMuNuConf.py
conflib := $(bin)$(library_prefix)BplusToPhiMuNu.$(shlibsuffix)
confdb  := BplusToPhiMuNu.confdb
instdir := $(CMTINSTALLAREA)$(shared_install_subdir)/python/$(package)
product := $(instdir)/$(confpy)
initpy  := $(instdir)/__init__.py

ifdef GENCONF_ECHO
genconf_silent =
else
genconf_silent = $(silent)
endif

BplusToPhiMuNuConf :: BplusToPhiMuNuConfinstall

install :: BplusToPhiMuNuConfinstall

BplusToPhiMuNuConfinstall : /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu/$(confpy)
	@echo "Installing /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu in /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/InstallArea/x86_64-slc6-gcc48-opt/python" ; \
	 $(install_command) --exclude="*.py?" --exclude="__init__.py" --exclude="*.confdb" /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/InstallArea/x86_64-slc6-gcc48-opt/python ; \

/afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu/$(confpy) : $(conflib) /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu
	$(genconf_silent) $(genconfig_cmd)   -o /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu -p $(package) \
	  --configurable-module=GaudiKernel.Proxy \
	  --configurable-default-name=Configurable.DefaultName \
	  --configurable-algorithm=ConfigurableAlgorithm \
	  --configurable-algtool=ConfigurableAlgTool \
	  --configurable-auditor=ConfigurableAuditor \
          --configurable-service=ConfigurableService \
	  -i ../$(tag)/$(library_prefix)BplusToPhiMuNu.$(shlibsuffix)

/afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu:
	@ if [ ! -d /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu ] ; then mkdir -p /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu ; fi ;

BplusToPhiMuNuConfclean :: BplusToPhiMuNuConfuninstall
	$(cleanup_silent) $(remove_command) /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu/$(confpy) /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/Phys/BplusToPhiMuNu/x86_64-slc6-gcc48-opt/genConf/BplusToPhiMuNu/$(confdb)

uninstall :: BplusToPhiMuNuConfuninstall

BplusToPhiMuNuConfuninstall ::
	@$(uninstall_command) /afs/cern.ch/user/i/ismith/cmtuser/DaVinci_v38r1p1/InstallArea/x86_64-slc6-gcc48-opt/python
libBplusToPhiMuNu_so_dependencies = ../x86_64-slc6-gcc48-opt/libBplusToPhiMuNu.so
#-- start of cleanup_header --------------

clean :: BplusToPhiMuNuConfclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNuConf.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConf.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNuConf.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConf.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNuConf)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConf.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConf.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuConf.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNuConfclean ::
#-- end of cleanup_header ---------------
