SRCS=main.c logger.c stdio-size.c
HDRS=main.h
EXES=main
TXTS=strace-base64.txt
SCRIPTS=test.sh test-cat.sh 
TESTS=t/hello.txt t/64.txt t/jugemu.txt t/null.txt t/cat.txt t/parrot.txt

all: $(EXES)
.PHONY: all download clean test test1 test2 testA benchmark

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/2-iocat
DLS=$(SRCS) $(HDRS) $(TXTS) $(SCRIPTS) $(TESTS) report.txt
download:
	-mkdir -p t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x $(SCRIPTS)

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-io.zip
report-io.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror
main: main.c logger.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c logger.c

clean:
	-rm -f $(EXES)
	-rm -f random.bin

install_strace: ~/.apt/usr/bin/strace
~/.apt/usr/bin/strace: strace-base64.txt
	mkdir -p ~/.apt/usr/bin
	base64 -d $< > $@
	chmod ugo+x $@

test: test1 test2 testA

TOPT=
test1: $(EXES)
	@sh ./test-cat.sh -k 'Hello'          $(TOPT)               ./main t/hello.txt
	@sh ./test-cat.sh -k '64b'            $(TOPT)               ./main t/64.txt
	@sh ./test-cat.sh -k 'more64b'        $(TOPT)               ./main t/jugemu.txt
	@sh ./test-cat.sh -k 'null'           $(TOPT)               ./main t/null.txt
	@sh ./test-cat.sh -k 'cat'                                  ./main t/cat.txt
	@sh ./test-cat.sh -k 'parrot'                                ./main t/parrot.txt
	@sh ./test-cat.sh -k '(40)Hello'      $(TOPT) -p '-s 40'    ./main t/hello.txt
	@sh ./test-cat.sh -k '(40)64b'        $(TOPT) -p '-s 40'    ./main t/64.txt
	@sh ./test-cat.sh -k '(40)more64b'    $(TOPT) -p '-s 40'    ./main t/jugemu.txt
	@sh ./test-cat.sh -k '(40)null'       $(TOPT) -p '-s 40'    ./main t/null.txt
	@sh ./test-cat.sh -k '(40)cat'                -p '-s 40'    ./main t/cat.txt
	@sh ./test-cat.sh -k '(40)parrot'             -p '-s 40'    ./main t/parrot.txt
test2: $(EXES)
	@sh ./test-cat.sh -k '(l)Hello'       $(TOPT) -p '-l'       ./main t/hello.txt
	@sh ./test-cat.sh -k '(l)64b'         $(TOPT) -p '-l'       ./main t/64.txt
	@sh ./test-cat.sh -k '(l)more64b'     $(TOPT) -p '-l'       ./main t/jugemu.txt
	@sh ./test-cat.sh -k '(l)null'        $(TOPT) -p '-l'       ./main t/null.txt
	@sh ./test-cat.sh -k '(l)cat'                 -p '-l'       ./main t/cat.txt
	@sh ./test-cat.sh -k '(l)parrot'              -p '-l'       ./main t/parrot.txt
	@sh ./test-cat.sh -k '(40l)Hello'     $(TOPT) -p '-l -s 40' ./main t/hello.txt
	@sh ./test-cat.sh -k '(40l)64b'       $(TOPT) -p '-l -s 40' ./main t/64.txt
	@sh ./test-cat.sh -k '(40l)more64b'   $(TOPT) -p '-l -s 40' ./main t/jugemu.txt
	@sh ./test-cat.sh -k '(40l)null'      $(TOPT) -p '-l -s 40' ./main t/null.txt
	@sh ./test-cat.sh -k '(40l)cat'               -p '-l -s 40' ./main t/cat.txt
	@sh ./test-cat.sh -k '(40l)parrot'            -p '-l -s 40' ./main t/parrot.txt
testA: $(EXES)
	@sh ./test-cat.sh -k '(A-1)stdin'  $(TOPT)         -i t/jugemu.txt ./main
	@sh ./test-cat.sh -k '(A-2)2args'  $(TOPT)                         ./main t/64.txt t/jugemu.txt
	@sh ./test-cat.sh -k '(A-3)-'      $(TOPT)         -i t/jugemu.txt ./main -
	@sh ./test-cat.sh -k '(A-3)mixed'  $(TOPT)         -i t/jugemu.txt ./main t/64.txt -
	@sh ./test-cat.sh -k '(A-1l)stdin' $(TOPT) -p '-l' -i t/jugemu.txt ./main
	@sh ./test-cat.sh -k '(A-2l)2args' $(TOPT) -p '-l'                 ./main t/64.txt t/jugemu.txt
	@sh ./test-cat.sh -k '(A-3l)-'     $(TOPT) -p '-l' -i t/jugemu.txt ./main -
	@sh ./test-cat.sh -k '(A-3l)mixed' $(TOPT) -p '-l' -i t/jugemu.txt ./main t/64.txt -

export TIMEFORMAT=real: %R, user: %U, sys: %S
SIZE=52428800 # 50MB
benchmark: $(EXES)
	@cat /dev/random | base64 | head -c $(SIZE) > random.bin
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s 4194304'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s  524288'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s   65536'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s    8192'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s    1024'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s     128'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s      16'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s       2'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s 4194304'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s  524288'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s   65536'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s    8192'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s    1024'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s     128'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s      16'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s       2'
	@-rm random.bin
