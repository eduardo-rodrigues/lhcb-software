#-- start of make_header -----------------

#====================================
#  Document BplusToPhiMuNuComponentsList
#
#   Generated Wed Feb 24 12:24:57 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNuComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNuComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNuComponentsList

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(BplusToPhiMuNu_tag)_BplusToPhiMuNuComponentsList.make
cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNuComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNuComponentsList = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNuComponentsList)
#-include $(cmt_local_tagfile_BplusToPhiMuNuComponentsList)

ifdef cmt_BplusToPhiMuNuComponentsList_has_target_tag

cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)setup_BplusToPhiMuNuComponentsList.make
#cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)BplusToPhiMuNu_BplusToPhiMuNuComponentsListsetup.make
cmt_local_BplusToPhiMuNuComponentsList_makefile = $(bin)BplusToPhiMuNuComponentsList.make

else

cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNuComponentsList = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNuComponentsList_makefile = $(bin)BplusToPhiMuNuComponentsList.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNuComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNuComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNuComponentsList/
#BplusToPhiMuNuComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
##
componentslistfile = BplusToPhiMuNu.components
COMPONENTSLIST_DIR = ../$(tag)
fulllibname = libBplusToPhiMuNu.$(shlibsuffix)

BplusToPhiMuNuComponentsList :: ${COMPONENTSLIST_DIR}/$(componentslistfile)
	@:

${COMPONENTSLIST_DIR}/$(componentslistfile) :: $(bin)$(fulllibname)
	@echo 'Generating componentslist file for $(fulllibname)'
	cd ../$(tag);$(listcomponents_cmd) --output ${COMPONENTSLIST_DIR}/$(componentslistfile) $(fulllibname)

install :: BplusToPhiMuNuComponentsListinstall
BplusToPhiMuNuComponentsListinstall :: BplusToPhiMuNuComponentsList

uninstall :: BplusToPhiMuNuComponentsListuninstall
BplusToPhiMuNuComponentsListuninstall :: BplusToPhiMuNuComponentsListclean

BplusToPhiMuNuComponentsListclean ::
	@echo 'Deleting $(componentslistfile)'
	@rm -f ${COMPONENTSLIST_DIR}/$(componentslistfile)

#-- start of cleanup_header --------------

clean :: BplusToPhiMuNuComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNuComponentsList.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuComponentsList.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNuComponentsList.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep '$(package)setup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuComponentsList.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNuComponentsList)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuComponentsList.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuComponentsList.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNuComponentsList.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNuComponentsListclean ::
#-- end of cleanup_header ---------------
