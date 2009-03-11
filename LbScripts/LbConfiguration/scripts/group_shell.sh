# test file. The real file has to be generated by the script generateLogin

if [ -e ${HOME}/.newLHCBLoginscript ]; then
  if [ -n "$LBLOGIN_DONE" ]; then
    . /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh --silent
  else
    export LB_BANNER=`mktemp -t LbLogin_banner.XXXXXXXX`
    . /afs/cern.ch/lhcb/software/releases/LBSCRIPTS/prod/InstallArea/scripts/LbLogin.sh --quiet >! ${LB_BANNER}
  fi     
fi 
