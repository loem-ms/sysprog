SRCS=inst0.c inst0-simple.c cg.c
HDRS=
EXES=inst0 inst0-simple mygzip
SCRIPTS=check-dot.sh install.dot.sh
TXTS=report.txt
ARCS=gzip-1.2.4.tar.gz
PICS=inst0-simple.svg inst0-simple-label.svg mygzip.svg mygzip-label.svg
TESTS=

all: $(EXES)
pics: $(PICS)
.PHONY: all pics download clean zip

URL=https://www.se.cs.titech.ac.jp/lecture/sysprog/6-instrumental
DLS=$(SRCS) $(HDRS) $(TXTS) $(ARCS) $(SCRIPTS) $(TESTS)
download:
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-tool.zip
report-tool.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror -finstrument-functions
LDFLAGS=-ldl -rdynamic

inst0: inst0.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

inst0-simple: inst0-simple.c cg.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $+
inst0-simple.svg: inst0-simple
	./inst0-simple
	sh check-dot.sh
	dot -Tsvg cg.dot > $@
inst0-simple-label.svg: inst0-simple
	env SYSPROG_LABEL=1 ./inst0-simple
	sh check-dot.sh
	dot -Tsvg cg.dot > $@

# gzip-1.2.4/configure: gzip-1.2.4.tar.gz
GZIP_MIN=gzip zip deflate trees bits unzip inflate util crypt lzw unlzw unpack unlzh getopt
GZIP_MIN2=gzip zip deflate trees bits unzip inflate util crypt lzw unlzw unlzh getopt
GZIP_SRCS=$(GZIP_MIN:%=gzip-1.2.4/%.c)
GZIP_OBJS=$(GZIP_MIN:%=gzip-1.2.4/%.o)
GZIP_SRCS2=$(GZIP_MIN2:%=gzip-1.2.4/%.c)
gzip-1.2.4/.extracted: gzip-1.2.4.tar.gz
	tar zxvf gzip-1.2.4.tar.gz
	# make static non-static
	ruby -i.bak -pe '$$_.sub!(/^local\b/,"/*local*/") if /^local .*\(/ && !/strncmp/' $(GZIP_SRCS2)
	touch $@
gzip-1.2.4/Makefile: gzip-1.2.4/.extracted
	(cd gzip-1.2.4 && env CC=$(CC) CFLAGS='-finstrument-functions' ./configure)
	# mac: -DHAVE_UNISTD_H=1 -DVOID_CLOSEDIR=1 -DRETSIGTYPE=int
	# repl.it: -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DDIRENT=1
mygzip-formal: gzip-1.2.4/Makefile cg.c
	(cd gzip-1.2.4 && make $(GZIP_MIN:%=%.o))
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(GZIP_OBJS) cg.c
gzip-1.2.4/gzip.o: gzip-1.2.4/.extracted
	(cd gzip-1.2.4 && $(CC) -c -finstrument-functions -w -DHAVE_UNISTD_H $(LDFLAGS) $(GZIP_MIN:%=%.c))
mygzip: gzip-1.2.4/gzip.o cg.c
	$(CC) -finstrument-functions -w -DHAVE_UNISTD_H $(LDFLAGS) -o $@ $(GZIP_OBJS) cg.c
mygzip.svg: mygzip
	./mygzip -c inst0-simple.c > /dev/null
	sh check-dot.sh
	dot -Tsvg cg.dot > $@
mygzip-label.svg: mygzip
	env SYSPROG_LABEL=1 ./mygzip -c inst0-simple.c > /dev/null
	sh check-dot.sh
	dot -Tsvg cg.dot > $@

clean:
	-$(RM) -rf $(EXES) $(PICS) *.dSYM *.o cg.dot gzip-1.2.4

