NB. y/:y and y\:y -------------------------------------------------------

test=: 3 : 0
 t=. (/:y){y
 assert. t -: y/:y
 assert. t -: /:~ y
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)>:}:t end. 
 t=. (\:y){y
 assert. t -: y\:y
 assert. t -: \:~ y
 if. (1=#$y)*.0=1{.0$y do. assert. (}.t)<:}:t end. 
 1
)

test1=: 3 : 0
 t=. (/:"1 y){"1 y
 assert. t -: y/:"1 y
 assert. t -: /:~"1 y
 assert. t -: /:"1~ y
 t=. (\:"1 y){"1 y
 assert. t -: y\:"1 y
 assert. t -: \:~"1 y
 assert. t -: \:"1~ y
 1
)

*./ b=: test @:(]   ?@$ 2:)"0 ] 500+i.2000
*./ b=: test1@:(3&, ?@$ 2:)"0 ] 500+i.2000

test   a.{~ 1000 ?@$ #a.
test   a.{~ 1001 ?@$ #a.
test   a.{~ 1002 ?@$ #a.
test   a.{~ 1003 ?@$ #a.
test   a.{~ 1004 ?@$ #a.
test   a.{~ 1005 ?@$ #a.
test   a.{~ 1006 ?@$ #a.
test   a.{~ 1007 ?@$ #a.

test   a.{~ 1000 2 ?@$ #a.
test   a.{~ 1001 2 ?@$ #a.
test   a.{~ 1002 2 ?@$ #a.
test   a.{~ 1003 2 ?@$ #a.

test   a.{~ 7000 2 ?@$ #a.
test   a.{~ 7001 2 ?@$ #a.
test   a.{~ 7002 2 ?@$ #a.
test   a.{~ 7003 2 ?@$ #a.

test     u: 1000 ?@$ 65536
test     u: 1001 ?@$ 65536
test     u: 1002 ?@$ 65536
test     u: 1003 ?@$ 65536

test     u: 7000 ?@$ 65536
test     u: 7001 ?@$ 65536
test     u: 7002 ?@$ 65536
test     u: 7003 ?@$ 65536

test  10&u: 1000 ?@$ C4MAX
test  10&u: 1001 ?@$ C4MAX
test  10&u: 1002 ?@$ C4MAX
test  10&u: 1003 ?@$ C4MAX

test  10&u: 7000 ?@$ C4MAX
test  10&u: 7001 ?@$ C4MAX
test  10&u: 7002 ?@$ C4MAX
test  10&u: 7003 ?@$ C4MAX

test        1000 ?@$ 1e4
test        1001 ?@$ 1e4
test        1002 ?@$ 1e4
test        1003 ?@$ 1e4

test    -1+ 1000 ?@$ 1e4
test    -1+ 1001 ?@$ 1e4
test    -1+ 1002 ?@$ 1e4
test    -1+ 1003 ?@$ 1e4

test  _5e3+ 1000 ?@$ 1e4
test  _5e3+ 1001 ?@$ 1e4
test  _5e3+ 1002 ?@$ 1e4
test  _5e3+ 1003 ?@$ 1e4

test        1000 ?@$ 1e9
test        1001 ?@$ 1e9
test        1002 ?@$ 1e9
test        1003 ?@$ 1e9

test        1000 ?@$ IF64{1e9 1e18
test        1001 ?@$ IF64{1e9 1e18
test        1002 ?@$ IF64{1e9 1e18
test        1003 ?@$ IF64{1e9 1e18

test  - 1 + 1000 ?@$ IF64{1e9 1e18
test  - 1 + 1001 ?@$ IF64{1e9 1e18
test  - 1 + 1002 ?@$ IF64{1e9 1e18
test  - 1 + 1003 ?@$ IF64{1e9 1e18

test(--:n)+ 1000 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1001 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1002 ?@$ n=: IF64{1e9 1e18
test(--:n)+ 1003 ?@$ n=: IF64{1e9 1e18

test  0.01*     1000 ?@$ IF64{1e9 1e18
test  0.01*     1001 ?@$ IF64{1e9 1e18
test  0.01*     1002 ?@$ IF64{1e9 1e18
test  0.01*     1003 ?@$ IF64{1e9 1e18

test  0.01*  -1+1000 ?@$ IF64{1e9 1e18
test  0.01*  -1+1001 ?@$ IF64{1e9 1e18
test  0.01*  -1+1002 ?@$ IF64{1e9 1e18
test  0.01*  -1+1003 ?@$ IF64{1e9 1e18

test  0.01*(--:n)+ 1000 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1001 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1002 ?@$ n=: IF64{1e9 1e18
test  0.01*(--:n)+ 1003 ?@$ n=: IF64{1e9 1e18


test1  a.{~ 3 1000 ?@$ #a.
test1  a.{~ 3 1001 ?@$ #a.
test1  a.{~ 3 1002 ?@$ #a.
test1  a.{~ 3 1003 ?@$ #a.

test1  a.{~ 3 1000 2 ?@$ #a.
test1  a.{~ 3 1001 2 ?@$ #a.
test1  a.{~ 3 1002 2 ?@$ #a.
test1  a.{~ 3 1003 2 ?@$ #a.

test1  a.{~ 3 1000 2 ?@$ #a.
test1  a.{~ 3 1001 2 ?@$ #a.
test1  a.{~ 3 1002 2 ?@$ #a.
test1  a.{~ 3 1003 2 ?@$ #a.

test1    u: 3 1000 ?@$ 65536
test1    u: 3 1001 ?@$ 65536
test1    u: 3 1002 ?@$ 65536
test1    u: 3 1003 ?@$ 65536

test1    u: 3 1000 ?@$ 65536
test1    u: 3 1001 ?@$ 65536
test1    u: 3 1002 ?@$ 65536
test1    u: 3 1003 ?@$ 65536

test1 10&u: 3 1000 ?@$ C4MAX
test1 10&u: 3 1001 ?@$ C4MAX
test1 10&u: 3 1002 ?@$ C4MAX
test1 10&u: 3 1003 ?@$ C4MAX

test1 10&u: 3 1000 ?@$ C4MAX
test1 10&u: 3 1001 ?@$ C4MAX
test1 10&u: 3 1002 ?@$ C4MAX
test1 10&u: 3 1003 ?@$ C4MAX

test1       3 1000 ?@$ 1e4
test1       3 1001 ?@$ 1e4
test1       3 1002 ?@$ 1e4
test1       3 1003 ?@$ 1e4

test1   -1+ 3 1000 ?@$ 1e4
test1   -1+ 3 1001 ?@$ 1e4
test1   -1+ 3 1002 ?@$ 1e4
test1   -1+ 3 1003 ?@$ 1e4

test1 _5e4+ 3 1000 ?@$ 1e4
test1 _5e4+ 3 1001 ?@$ 1e4
test1 _5e4+ 3 1002 ?@$ 1e4
test1 _5e4+ 3 1003 ?@$ 1e4

test1       3 1000 ?@$ 1e9
test1       3 1001 ?@$ 1e9
test1       3 1002 ?@$ 1e9
test1       3 1003 ?@$ 1e9

test1       3 1000 ?@$ IF64{1e9 1e18
test1       3 1001 ?@$ IF64{1e9 1e18
test1       3 1002 ?@$ IF64{1e9 1e18
test1       3 1003 ?@$ IF64{1e9 1e18

test1   -1+ 3 1000 ?@$ 1e9
test1   -1+ 3 1001 ?@$ 1e9
test1   -1+ 3 1002 ?@$ 1e9
test1   -1+ 3 1003 ?@$ 1e9

test1   -1+ 3 1000 ?@$ IF64{1e9 1e18
test1   -1+ 3 1001 ?@$ IF64{1e9 1e18
test1   -1+ 3 1002 ?@$ IF64{1e9 1e18
test1   -1+ 3 1003 ?@$ IF64{1e9 1e18

test1 _5e8+ 3 1000 ?@$ 1e9
test1 _5e8+ 3 1001 ?@$ 1e9
test1 _5e8+ 3 1002 ?@$ 1e9
test1 _5e8+ 3 1003 ?@$ 1e9

test1 (--:n) + 3 1000 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1001 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1002 ?@$ n=: IF64{1e9 1e18
test1 (--:n) + 3 1003 ?@$ n=: IF64{1e9 1e18

test1 0.01* 3 1000 ?@$ IF64{1e9 1e18
test1 0.01* 3 1001 ?@$ IF64{1e9 1e18
test1 0.01* 3 1002 ?@$ IF64{1e9 1e18
test1 0.01* 3 1003 ?@$ IF64{1e9 1e18

test1 0.01*  -1+ 3 1000 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1001 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1002 ?@$ IF64{1e9 1e18
test1 0.01*  -1+ 3 1003 ?@$ IF64{1e9 1e18

test1 0.01*(--:n) + 3 1000 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1001 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1002 ?@$ n=: IF64{1e9 1e18
test1 0.01*(--:n) + 3 1003 ?@$ n=: IF64{1e9 1e18


4!:55 ;:'b n test test1'


