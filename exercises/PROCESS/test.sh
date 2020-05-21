#!/bin/sh
key=key
in=/dev/null
out=/dev/null
detail=no
verbose=no
while getopts :k:i:o:dv OPT; do
  case $OPT in
    k) key="$OPTARG" ;;
    i) in="$OPTARG" ;;
    o) out="$OPTARG" ;;
    d) detail=yes ;;
    v) verbose=yes ;;
    \?) echo "Usage: $0 [-d] [-k name] [-i infile] [-o outfile] cmd..." ; exit 127 ;;
  esac
done
shift `expr $OPTIND - 1`
cmd="$*"

tmpout=$(mktemp)
tmperr=$(mktemp)
trap "rm -f '$tmpout' '$tmperr'" EXIT

eval "$cmd <'$in' >'$tmpout' 2>'$tmperr'"
status=$?


[ "$in" != /dev/null ] && cmd="$cmd < $in"

if cmp -s "$tmpout" "$out"; then
  echo "✅  OK 🎉 [$key] $cmd "
  if [ $verbose = yes ]; then
    [ "$in" != /dev/null ] && echo "--- stdin" && cat $in
    echo "--- stdout expected" && cat "$out"
    echo "--- stdout" && cat "$tmpout"
    echo "--- exit status: $status"
  fi
  exit 0
else
  echo "❌  NG 😱 [$key] $cmd "
  if [ $detail = yes ]; then
    [ "$in" != /dev/null ] && echo "--- stdin" && cat $in
    echo "--- stdout expected" && cat "$out"
    echo "--- stdout" && cat "$tmpout"
    echo "--- exit status: $status"
  fi
  exit 0   # exit 1 doesn't fit the make command...
fi
