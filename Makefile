
all: booga

booga:
	cd src; make
	cd tests; make
	
clean:
	cd src; make clean
	cd src; make clean
