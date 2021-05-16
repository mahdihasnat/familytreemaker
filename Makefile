all: a.out tmp.txt tmp.svg

a.out: familytreemaker.cpp
	g++ familytreemaker.cpp
tmp.txt: a.out miazi_bangla.txt
	./a.out miazi_bangla.txt hanif_miazi > tmp.txt
tmp.svg: tmp.txt
	dot tmp.txt -Tsvg -o tmp.svg
clean:
	rm -f tmp.txt 
	rm -f tmp.svg 
	rm -f a.out