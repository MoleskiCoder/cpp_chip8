.PHONY: all
all:
	$(MAKE) -C src/libs/libchip8 all
	$(MAKE) -C src/main all

.PHONY: clean
clean:
	$(MAKE) -C src/libs/libchip8 clean
	$(MAKE) -C src/main clean
