.PHONY: all
all:
	$(MAKE) -C src/libs/libchip8 all
	$(MAKE) -C src/main all

.PHONY: test
test:
	$(MAKE) -C src/libs/libchip8 all
	$(MAKE) -C src/testchip8 all
	src/testchip8/testchip8

.PHONY: clean
clean:
	$(MAKE) -C src/libs/libchip8 clean
	$(MAKE) -C src/main clean
	$(MAKE) -C src/testchip8 clean
