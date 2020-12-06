#!/usr/bin/python3

infile=open("infile","w")
for i in range(200000):
    infile.write(str(i)+"hello wolrd\n")
infile.close()
