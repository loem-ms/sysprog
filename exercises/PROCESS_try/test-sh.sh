#!/bin/sh
key=key
in=/dev/null
param=
detail=
verbose=
while getopts :k:i:p:dv OPT; do
  case $OPT in
    k) key="$OPTARG" ;;
    i) in="$OPTARG" ; [ "$key" = key ] && key=$(basename "$OPTARG" .txt) ;;
    p) param="$OPTARG" ;;
    d) detail="-d" ;;
    v) verbose="-v" ;;
    \?) echo "Usage: $0 [-d] [-k name] [-i infile] cmd params..." ; exit 127 ;;
  esac
done
shift `expr $OPTIND - 1`
cmd=$1
shift
args="$*"

# run cat and obtain the result
tmpout=$(mktemp)
trap "rm -f '$tmpout'" EXIT
eval "sh $args <'$in' >'$tmpout' 2>/dev/null"

eval "sh ./test.sh $detail $verbose -k '$key' -i '$in' -o '$tmpout' $cmd $param $args"
rm -f __test__*.txt
