OPTIMIZATION_FLAG = -O0
CPP_STD = -std=c++11
ADDITIONAL_FLAGS = -Wall -D ENABLE_ASSERTS -g
#ADDITIONAL_FLAGS = -Wall
COMPILER = g++

all: include/pcheaders.h.gch VP4C

include/pcheaders.h.gch: include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) include/pcheaders.h

VP4C: bin/main.o bin/lex.yy.o bin/parser.tab.o bin/CFG.o bin/Log.o bin/AST.o bin/SymbolTable.o bin/TypedefTable.o bin/TofinoMerge.o bin/V1Merge.o bin/PreProcessor.o
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o VP4C bin/*.o -L ~/.local/usr/lib -lfl $(ADDITIONAL_FLAGS)

bin/main.o: src/main.cpp include/IPLBase.hpp include/SymbolTable.h include/pcheaders.h src/parser.tab.c include/AST.h include/CFG.h include/PreProcessor.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/main.o -c src/main.cpp $(ADDITIONAL_FLAGS)

bin/PreProcessor.o: src/PreProcessor.cpp include/IPLBase.hpp include/pcheaders.h include/PreProcessor.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/PreProcessor.o -c src/PreProcessor.cpp $(ADDITIONAL_FLAGS)

bin/lex.yy.o: src/lex.yy.c
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/lex.yy.o -c src/lex.yy.c

bin/parser.tab.o: src/parser.tab.c
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/parser.tab.o -c src/parser.tab.c

bin/Log.o: include/pcheaders.h include/Log.h src/Log.cpp
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/Log.o -c src/Log.cpp $(ADDITIONAL_FLAGS)

bin/CFG.o: include/pcheaders.h include/AST.h include/SymbolTable.h include/CFG.h include/IPLBase.hpp src/CFG.cpp
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/CFG.o -c src/CFG.cpp $(ADDITIONAL_FLAGS)

bin/AST.o: src/AST.cpp src/parser.tab.c include/SymbolTable.h include/Log.h include/CFG.h include/TypedefTable.h include/TofinoMerge.h include/V1Merge.h include/AST.h include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/AST.o -c src/AST.cpp $(ADDITIONAL_FLAGS)

bin/SymbolTable.o: src/SymbolTable.cpp include/IPLBase.hpp include/SymbolTable.h include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/SymbolTable.o -c src/SymbolTable.cpp $(ADDITIONAL_FLAGS)

bin/TypedefTable.o: src/TypedefTable.cpp src/parser.tab.c include/AST.h include/TypedefTable.h include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/TypedefTable.o -c src/TypedefTable.cpp $(ADDITIONAL_FLAGS)

bin/TofinoMerge.o: src/TofinoMerge.cpp include/Log.h include/CFG.h include/AST.h include/TofinoMerge.h include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/TofinoMerge.o -c src/TofinoMerge.cpp $(ADDITIONAL_FLAGS)

bin/V1Merge.o: src/V1Merge.cpp include/Log.h include/CFG.h include/AST.h include/V1Merge.h include/pcheaders.h
	$(COMPILER) $(CPP_STD) $(OPTIMIZATION_FLAG) -o bin/V1Merge.o -c src/V1Merge.cpp $(ADDITIONAL_FLAGS)

src/lex.yy.c: src/scanner.l src/parser.tab.c
	flex -osrc/lex.yy.c src/scanner.l

src/parser.tab.c: src/parser.y include/AST.h include/TypedefTable.h
	bison -o src/parser.tab.c -d src/parser.y

clean:
	rm -f VP4C
	rm -f bin/*.o
	rm -f output/*.cc output/*.h
	rm -f include/pcheaders.h.gch
	rm -f src/lex.yy.c src/parser.tab.cpp src/parser.tab.c src/parser.tab.h src/parser.tab.hpp src/stack.hh
