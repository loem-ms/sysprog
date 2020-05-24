SRCS=main.c node.c logger.c lex.yy.c y.tab.c fd.c
CC_SRCS=scan.l parse.y
HDRS=main.h y.tab.h
EXES=main fd
SCRIPTS=test.sh test-sh.sh
TXTS=report.txt
TESTS=t/1-1whoami.txt t/1-2hostname.txt t/1-3echo.txt t/1-4sleep.txt t/1-5ppid.txt t/2-1simplel.txt t/2-2simpler.txt t/2-3rev.txt t/2-4big.txt t/2-5yes.txt t/2-6fd.txt t/2-7ppid.txt t/3-1grep.txt t/3-2yes.txt t/3-3gzip.txt t/3-4fd.txt t/3-5ppid.txt t/4-1in.txt t/4-2out.txt t/4-3inout.txt t/4-4append.txt t/4-5fdin.txt t/4-6fdout.txt t/4-7fdappend.txt t/4-8ppid.txt t/A-1echo.txt t/A-2fd.txt t/A-3ppid.txt t/B-1and.txt t/B-2or.txt t/B-3mixed.txt t/B-4fdand.txt t/B-5fdor.txt t/B-6mixed2.txt t/B-7ppid.txt t/C-1echo.txt t/C-2and.txt t/C-3fd.txt
INST_URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/installers

all: $(EXES)
.PHONY: all download clean zip generate-cc test test1 test2 test3 test4 testA testB testC

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/3-procplumber
DLS=$(SRCS) $(HDRS) $(CC_SRCS) $(TXTS) $(SCRIPTS) $(TESTS)
download:
	-mkdir -p t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x $(SCRIPTS)

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-proc.zip
report-proc.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror
LDFLAGS=
main: main.c node.c logger.c lex.yy.c y.tab.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c node.c logger.c lex.yy.c y.tab.c
fd: fd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	-$(RM) -f $(EXES)

TOPT=
test: test1 test2 test3 test4 testA testB testC
test1: $(EXES)
	@for x in t/1-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
test2: $(EXES)
	@for x in t/2-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
test3: $(EXES)
	@for x in t/3-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
test4: $(EXES)
	@for x in t/4-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
testA: $(EXES)
	@for x in t/A-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
testB: $(EXES)
	@for x in t/B-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done
testC: $(EXES)
	@for x in t/C-*.txt; do sh ./test-sh.sh $(TOPT) -p '-q -p' -i $$x ./main; done

# require: bison and flex
generate-cc:
	type bison >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.bison.sh | sh
	type flex  >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.flex.sh  | sh
	flex -o lex.yy.c scan.l
	bison -t -v -d -o y.tab.c parse.y
