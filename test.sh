#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5;"
assert 5 "a = 3; a+2;"
assert 1 "a=22; a<25;"
assert 42 "a=2; b=4;c=10; a+b*c;"
assert 0 "a=3; b= c = 3; d = a*b; d < c;"
echo OK