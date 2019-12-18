#detecta se o sistema é de 32 ou 64 bits
BITS_OPTION = -m64

#### define o compilador
CPPC = g++
#############################

#### opcoes de compilacao e includes
ifeq ($(DEBUG), 1)
    SPFLAGS = -g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=null -fsanitize=signed-integer-overflow
else
    SPFLAGS = -O3 -DNDEBUG
endif
CCOPT = $(BITS_OPTION) $(SPFLAGS) -fPIC -fexceptions -DIL_STD -std=c++17 -lm -Wall -Wextra
#############################

#### flags do linker
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
ifdef $(OBJDIR)
   OBJDIR = $(OBJDIR)
else
   OBJDIR = obj
endif
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
tsp: $(OBJS)
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(CCOPT) $(OBJS) -o $(OBJDIR)/$@
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) $(CCOPT) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPPC) $(CCOPT) -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #ultimas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm $(OBJDIR)/* -rf

rebuild: clean tsp
