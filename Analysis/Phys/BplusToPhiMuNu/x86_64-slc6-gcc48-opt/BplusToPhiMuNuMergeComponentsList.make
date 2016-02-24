#-- start of make_header -----------------

#====================================
#  Document BplusToPhiMuNuMergeComponentsList
#
#   Generated Wed Feb 24 12:24:58 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNuMergeComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuMergeComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNuMergeComponentsList

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuMergeComponentsList.make
cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuMergeComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList)
#-include $(cmt_local_tagfile_BplusToPhiMuNuMergeComponentsList)

ifdef cmt_BplusToPhiMuNuMergeComponentsList_has_target_tag

cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)setup_BplusToPhiMuNuMergeComponentsList.make
#cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)BplusToPhiMuNu_BplusToPhiMuNuMergeComponentsListsetup.make
cmt_local_BplusToPhiMuNuMergeComponentsList_makefile = $(bin)BplusToPhiMuNuMergeComponentsList.make

else

cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuMergeComponentsList = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuMergeComponentsList_makefile = $(bin)BplusToPhiMuNuMergeComponentsList.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNuMergeComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNuMergeComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNuMergeComponentsList/
#BplusToPhiMuNuMergeComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_componentslist_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.components file into a single
# <project>.components file in the (lib) install area
# If no InstallArea is present the fragment is dummy


.PHONY: BplusToPhiMuNuMergeComponentsList BplusToPhiMuNuMergeComponentsListclean

# default is already '#'
#genmap_comment_char := "'#'"

componentsListRef    := ../$(tag)/BplusToPhiMuNu.components

ifdef CMTINSTALLAREA
componentsListDir    := ${CMTINSTALLAREA}/$(tag)/lib
mergedComponentsList := $(componentsListDir)/$(project).components
stampComponentsList  := $(componentsListRef).stamp
else
componentsListDir    := ../$(tag)
mergedComponentsList :=
stampComponentsList  :=
endif

BplusToPhiMuNuMergeComponentsList :: $(stampComponentsList) $(mergedComponentsList)
	@:

.NOTPARALLEL : $(stampComponentsList) $(mergedComponentsList)

$(stampComponentsList) $(mergedComponentsList) :: $(componentsListRef)
	@echo "Running merge_componentslist  BplusToPhiMuNuMergeComponentsList"
	$(merge_componentslist_cmd) --do-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList)

BplusToPhiMuNuMergeComponentsListclean ::
	$(cleanup_silent) $(merge_componentslist_cmd) --un-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList) ;
	$(cleanup_silent) $(remove_command) $(stampComponentsList)
libBplusToPhiMuNu_so_dependencies = ../x86_64-slc6-gcc48-opt/libBplusToPhiMuNu.so
#-- start of cleanup_header --------------

clean :: BplusToPhiMuNuMergeComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNuMergeComponentsList.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuMergeComponentsList.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNuMergeComponentsList.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuMergeComponentsList.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNuMergeComponentsList)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuMergeComponentsList.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuMergeComponentsList.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuMergeComponentsList.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNuMergeComponentsListclean ::
#-- end of cleanup_header ---------------
