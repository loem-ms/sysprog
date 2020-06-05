SRCS=main.c logger.c ttcurl.c cgi-c.c
HDRS=main.h
EXES=main ttcurl docroot/cgi-c
SCRIPTS=test.sh
TXTS=report.txt
DOCS=docroot/index.html docroot/test.html docroot/cgi-perl.pl
TESTS=t/1-test-body.sh t/1-test-status.sh t/2-wait-out.txt t/2-wait.sh t/200.txt t/404.txt t/cgi-c.txt t/cgi-perl.txt
INST_URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/installers

all: $(EXES)
.PHONY: all download clean zip test test1 test2 testA

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/4-websock
DLS=$(SRCS) $(HDRS) $(TXTS) $(SCRIPTS) $(DOCS) $(TESTS)
download:
	-mkdir -p docroot t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x docroot/cgi-perl.pl

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-sock.zip
report-sock.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror
LDFLAGS=
main: main.c logger.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c logger.c
ttcurl: ttcurl.c logger.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ttcurl.c logger.c
docroot/cgi-c: cgi-c.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ cgi-c.c

clean:
	-$(RM) -f $(EXES)

TOPT=
test: test1 test2 testA
test1: $(EXES)
	@sh test.sh $(TOPT) -k 1-1test-code  -o t/200.txt          ./ttcurl -u /test.html -c
	@sh test.sh $(TOPT) -k 1-2test-body  -o docroot/test.html  ./ttcurl -u /test.html -b
	@sh test.sh $(TOPT) -k 1-3index-code -o t/200.txt          ./ttcurl -u / -c
	@sh test.sh $(TOPT) -k 1-4index-body -o docroot/index.html ./ttcurl -u / -b
	@sh test.sh $(TOPT) -k 1-5notfound   -o t/404.txt          ./ttcurl -u /nonexistent.html -c

test2: $(EXES)
	@sh test.sh $(TOPT) -k 2-wait -o t/2-wait-out.txt sh t/2-wait.sh

testA: $(EXES)
	@sh test.sh $(TOPT) -k A-1c    -o t/cgi-c.txt    ./ttcurl -u /cgi-c -b
	@sh test.sh $(TOPT) -k A-2perl -o t/cgi-perl.txt ./ttcurl -u /cgi-perl.pl -b


install-commands:
	type telnet  >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.telnet.sh | sh
	type tmux    >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.tmux.sh   | sh
