# 課題１
echo /usr/include/u*d.h

# 課題２
grep '# define errno' /usr/include/*.h

# 課題３
grep '/tcp' services.txt | grep "^[^#;]" | wc -l

# 課題４
cut -f 1 syscall_headers.txt | sort | uniq | wc -l

# 練習
kill $(ps|grep sleep|grep -v 'grep sleep'|awk '{print $1}')

# 課題５
cut -f 2 syscall_headers.txt|sort|uniq -c|sort -n -r|head -n 3

# 発展課題A
grep -Fxf <(rev words.txt)  words.txt |wc -l
# or
rev words.txt|grep -Fxf words.txt |wc -l