.PHONY: all clean

all:
	$(MAKE) -C source

clean:
	$(MAKE) -C source clean

%:
	$(MAKE) -C source $@
