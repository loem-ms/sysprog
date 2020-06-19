#!/bin/sh
if type dot >/dev/null 2>&1; then
  true
else
  if [ "_$HOME" = "_/home/runner" ]; then
    # repl.it
    sh install.dot.sh
  else
    echo '`dot` not found. Please install Graphviz.'
    echo '   For Mac users: `brew install graphviz`'
    echo '   For Ubuntu users: `apt-get install graphviz`'
    false
  fi
fi
