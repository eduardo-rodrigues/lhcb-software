#-- start of make_header -----------------

#====================================
#  Document BplusToPhiMuNu_python_init
#
#   Generated Wed Feb 24 12:25:01 2016  by ismith
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_BplusToPhiMuNu_python_init_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_BplusToPhiMuNu_python_init_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_BplusToPhiMuNu_python_init

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNu_python_init = $(BplusToPhiMuNu_tag)_BplusToPhiMuNu_python_init.make
cmt_local_tagfile_BplusToPhiMuNu_python_init = $(bin)$(BplusToPhiMuNu_tag)_BplusToPhiMuNu_python_init.make

else

tags      = $(tag),$(CMTEXTRATAGS)

BplusToPhiMuNu_tag = $(tag)

#cmt_local_tagfile_BplusToPhiMuNu_python_init = $(BplusToPhiMuNu_tag).make
cmt_local_tagfile_BplusToPhiMuNu_python_init = $(bin)$(BplusToPhiMuNu_tag).make

endif

include $(cmt_local_tagfile_BplusToPhiMuNu_python_init)
#-include $(cmt_local_tagfile_BplusToPhiMuNu_python_init)

ifdef cmt_BplusToPhiMuNu_python_init_has_target_tag

cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)setup_BplusToPhiMuNu_python_init.make
#cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)BplusToPhiMuNu_BplusToPhiMuNu_python_initsetup.make
cmt_local_BplusToPhiMuNu_python_init_makefile = $(bin)BplusToPhiMuNu_python_init.make

else

cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)setup.make
#cmt_final_setup_BplusToPhiMuNu_python_init = $(bin)BplusToPhiMuNusetup.make
cmt_local_BplusToPhiMuNu_python_init_makefile = $(bin)BplusToPhiMuNu_python_init.make

endif

cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)BplusToPhiMuNusetup.make

#BplusToPhiMuNu_python_init :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'BplusToPhiMuNu_python_init'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = BplusToPhiMuNu_python_init/
#BplusToPhiMuNu_python_init::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of python_init_header ------

installarea = ${CMTINSTALLAREA}$(shared_install_subdir)
install_python_dir = $(installarea)/python/BplusToPhiMuNu
init_file = $(install_python_dir)/__init__.py


BplusToPhiMuNu_python_init :: BplusToPhiMuNu_python_initinstall

install :: BplusToPhiMuNu_python_initinstall

BplusToPhiMuNu_python_initinstall :: $(init_file)

$(init_file) ::
	@if [ -e $(install_python_dir) -a ! -e $(init_file) ]; then \
	  echo "Installing __init__.py file from ${GAUDIPOLICYROOT}" ; \
	  $(install_command) ${GAUDIPOLICYROOT}/cmt/fragments/__init__.py $(install_python_dir) ; \
	fi

BplusToPhiMuNu_python_initclean :: BplusToPhiMuNu_python_inituninstall

uninstall :: BplusToPhiMuNu_python_inituninstall

BplusToPhiMuNu_python_inituninstall ::
	@$(uninstall_command) $(init_file)


#-- end of python_init_header ------
#-- start of cleanup_header --------------

clean :: BplusToPhiMuNu_python_initclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(BplusToPhiMuNu_python_init.make) $@: No rule for such target" >&2
#	@echo "#CMT> Warning: $@: No rule for such target" >&2; exit
	if echo $@ | grep 'BplusToPhiMuNusetup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu_python_init.make): $@: File no longer generated" >&2; exit 0; fi
else
.DEFAULT::
	$(echo) "(BplusToPhiMuNu_python_init.make) PEDANTIC: $@: No rule for such target" >&2
	if echo $@ | grep 'BplusToPhiMuNusetup\.make$$' >/dev/null; then\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu_python_init.make): $@: File no longer generated" >&2; exit 0;\
	 elif test $@ = "$(cmt_final_setup)" -o\
	 $@ = "$(cmt_final_setup_BplusToPhiMuNu_python_init)" ; then\
	 found=n; for s in 1 2 3 4 5; do\
	 sleep $$s; test ! -f $@ || { found=y; break; }\
	 done; if test $$found = n; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu_python_init.make) PEDANTIC: $@: Seems to be missing. Ignore it for now" >&2; exit 0 ; fi;\
	 elif test `expr $@ : '.*/'` -ne 0 ; then\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu_python_init.make) PEDANTIC: $@: Seems to be a missing file. Please check" >&2; exit 2 ; \
	 else\
	 test -z "$(cmtmsg)" ||\
	 echo "$(CMTMSGPREFIX)" "(BplusToPhiMuNu_python_init.make) PEDANTIC: $@: Seems to be a fake target due to some pattern. Just ignore it" >&2 ; exit 0; fi
endif

BplusToPhiMuNu_python_initclean ::
#-- end of cleanup_header ---------------
