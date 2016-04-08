############################################################################### 
# General Purpose Makefile
# 
# Copyright (C) 2010, Martin Tang
############################################################################### 

# Configurations
LANGUAG=en
INCLUDE=-DDEBUG -g
ASFLAGS=$(INCLUDE)
CCFLAGS=$(INCLUDE)
CXFLAGS=$(INCLUDE)
FCFLAGS=$(INCLUDE)
LDFLAGS=-lafx -lkernel32 -lshlwapi -lgdi32 -lgdiplus -lcomctl32 -lcomdlg32 -mwindows

# Objectives
OBJECTS=Compiler/Syntax.o \
				Compiler/Lexical.o \
				Compiler/Expression.o \
				Compiler/Statement.o \
				Compiler/Compiler.o \
				Graphics/StdAfx.o \
				Graphics/CModelApp.o \
				Graphics/CModelDoc.o \
				Graphics/CModelView.o \
				Graphics/CMessageWnd.o \
				Graphics/CParameterDlg.o \
				Graphics/CAboutDlg.o \
				Graphics/CMainFrame.o \
				Graphics/CSourceEditor.o \
				Graphics/CDataScope.o \
				Graphics/CBlockUnit.o \
				Graphics/CLinkUnit.o \
				Graphics/CVariable.o \
				Resources/Resource_$(LANGUAG).o
TARGETS=Synaptics.exe
DEPENDS=$(OBJECTS:.o=.dep) 

# Build Commands
all : $(TARGETS)

$(TARGETS) : $(OBJECTS) 
	@echo [LD] $@
	@g++ -o $@ $^ $(LDFLAGS)

$(OBJECTS):

debug: all
	@echo [DB] $(TARGETS)
	@gdb -w $(TARGETS)

run: all 
	@echo [EX] $(TARGETS)
	@$(TARGETS)

clean:  
	@echo [RM] $(OBJECTS) $(TARGETS)
	@rm $(DEPENDS) $(OBJECTS) $(TARGETS)

# Standard Procedures
%.dep : %.s 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.c 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.m 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.cpp 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.f 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.rc 
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.l
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.dep : %.y
	@cpp $(INCLUDE) -MM -MT $(@:.dep=.o) -o $@ $< 

%.o : %.s 
	@echo [AS] $<
	@as $(ASFLAGS) -o $@ $< 

%.o : %.c 
	@echo [CC] $<
	@gcc $(CCFLAGS) -c -o $@ $< 

%.o : %.m 
	@echo [OC] $<
	@gcc $(CCFLAGS) -c -o $@ $< 

%.o : %.cpp 
	@echo [CX] $<
	@g++ $(CXFLAGS) -c -o $@ $< 

%.o : %.f 
	@echo [FC] $<
	@g77 $(FCFLAGS) -c -o $@ $< 

%.o : %.rc 
	@echo [RC] $<
	@windres $< $@ 

%.c : %.l
	@echo [LX] $<
	@flex --header-file=$(@:.c=.h) -o $@ $<

%.c : %.y
	@echo [BS] $<
	@bison -d -o $@ $<

-include $(DEPENDS)
