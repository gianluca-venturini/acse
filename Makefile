objects = executor asm compiler
dirs:=mace assembler acse tests


all : bin $(objects) test_bin

bin :
	mkdir -p bin

tests : 
	cd ./tests && $(MAKE)

executor :
	cd ./mace && $(MAKE)

asm :
	cd ./assembler && $(MAKE)

compiler :
	cd ./acse && $(MAKE)

clean :
	for i in $(dirs) ; do cd $$i && $(MAKE) clean; cd .. ; done
	rm -rf bin

.PHONY : all clean tests executor asm compiler


test_bin :
	cd ./bin && ./acse test.src
	cd ./bin && ./asm output.asm
	cd ./bin &&  ./mace output.o

