
SetupProject_tmpfile=`python -m LbConfiguration.SetupProject --shell=sh --mktemp "$@"`
SetupProjectStatus="$?"
if [ "$SetupProjectStatus" = 0 ]; then
    . $SetupProject_tmpfile
fi
rm -f $SetupProject_tmpfile
unset SetupProject_tmpfile
