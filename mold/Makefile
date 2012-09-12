target=Debug
#target=Logpool
#target=Release
.PHONY : all
all:
	make -C $(target) -j8
.PHONY : clean
clean:
	make -C $(target) clean
.PHONY : install
install: $(target)/libmolder.a
	make -C $(target) install
.PHONY : test
test: $(target)/libmolder.a
	make -C $(target) test
