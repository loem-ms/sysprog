#!/usr/bin/env bash
rm -f __test__.txt
./ttcurl -w 5 -u /nonexistent.html -c >> __test__.txt &
pid1=$!

sleep 1

./ttcurl -u /test.html -c >> __test__.txt &
pid2=$!
./ttcurl -u /test.html -c >> __test__.txt &
pid3=$!

sleep 1

kill $pid2
kill $pid3
sleep 1
kill $pid1

cat __test__.txt
