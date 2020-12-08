#!/usr/bin/python3

infile = open("infile", "w")
for i in range(2000):
    infile.write(str(i) + "hello world\n")
infile.close()
