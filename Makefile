#
#
#
# Makefile for PWA subsystem
#



CPP= g++ 
F77= gfortran

DEBUG = -g
COPT =  -O2
SYSLIB =	 -lm
FLIB = -L/lib -lgfortran -lstdc++ 

CERNLIBS =   -L$(CERNLIB)   -lpacklib -lmathlib -lkernlib 
FINCLUDE =  -Bstatic -fno-second-underscore


INCLUDE= -I. -I../include -DFUNCPROTO

all: pwaFit pwaFitView pwaInts


pwaFitViewDict.o:pwaFitViewDict.cc
	g++  `root-config --cflags`  -c -o pwaFitViewDict.o pwaFitViewDict.cc 

pwaFitViewDict.cc:pwaFitView_Gui.h pwaFitViewLinkDef.h 
	rootcint -f pwaFitViewDict.cc -c pwaFitView_Gui.h pwaFitViewLinkDef.h 

pwaFitView:pwaFitView.o PwaFitResults.o pwaFitView_Gui_multiplelist.o pwaFitViewDict.o
	g++  `root-config --glibs` -o pwaFitView pwaFitView.o PwaFitResults.o pwaFitView_Gui_multiplelist.o pwaFitViewDict.o

pwaFitView_Gui_multiplelist.o : pwaFitView_Gui.h pwaFitView_Gui_multiplelist.cc
	g++ `root-config --cflags` $(DEBUG) $(INCLUDE)  -c pwaFitView_Gui_multiplelist.cc

PwaFitResults.o : PwaFitResults.h PwaFitResults.cc
	 g++  -c  $(DEBUG) PwaFitResults.cc  $(INCLUDE)

pwaFitView.o : pwaFitView_Gui.h pwaFitView.cc
	 g++  $(INCLUDE) $(DEBUG)  `root-config --cflags` -c pwaFitView.cc
 
pwaFit.o : pwaFit.cc
	 g++  -c ${COPT} $(DEBUG) pwaFit.cc  $(INCLUDE)

pwaFit : pwaFit.o  fortIO.o   libMinuit.a
	g++ -o $@ $@.o fortIO.o   libMinuit.a $(FLIB) $(SYSLIB) 

fortIO.o:fortIO.f
	$(F77) -O -w $(FINCLUDE)  -g -c fortIO.f

libMinuit.a:
	cd minuit/; make -f Makefile; cd -




# clean up
clean:
	rm *.o 


# End of file

