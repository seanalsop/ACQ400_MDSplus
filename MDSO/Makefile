CXXFLAGS += -I/usr/local/mdsplus/include -L/usr/local/mdsplus/lib \
	-lMdsObjectsCppShr -fexceptions -Wl,-E -lpopt 
#	-pg

APPS := hello-mdso-world \
	test-seg-push test-seg-pull analyse-ramp \
	report-flats

`all: $(APPS)


clean:
	rm $(APPS)

install:
	cp $(APPS) /usr/local/bin

