SRCS=main.c logger.c ttcurl.c cgi-c.c
HDRS=main.h
EXES=main ttcurl docroot/cgi-c
SCRIPTS=
TXTS=report.txt
DOCS=docroot/index.html docroot/test.html docroot/cgi-perl.pl
TESTS=
INST_URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/installers

all: $(EXES)
.PHONY: all download clean zip

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/4-websock
DLS=$(SRCS) $(HDRS) $(TXTS) $(SCRIPTS) $(DOCS) $(TESTS)
download:
	-mkdir -p docroot
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

install-commands:
	type telnet  >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.telnet.sh | sh
	type tmux    >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.tmux.sh   | sh
