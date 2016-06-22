
.PHONY: doxygen clean

all: doxygen

doxygen:
	./makehtmlfooter.sh > htmlfooter.html
	/Applications/Doxygen.app/Contents/Resources/doxygen
	rm htmlfooter.html

clean:
	rm htmlfooter.html
