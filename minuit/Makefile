#
#
#
# A hacked Makefile for MINUIT 
#
#  Paul Eugenio  23-jun-98

#
#  To change the max number of minuit parameters modify: minuit/d506cm.inc
#

FOBJS= 	minuit.o  mncrck.o  mneval.o  mnhess.o  mnmatu.o  mnplot.o  mnsave.o \
	mntiny.o mnamin.o  mncros.o  mnexcm.o mnimpr.o  mnmigr.o  mnpout.o \
        mnscan.o mnunpt.o mnbins.o mncuve.o mnexin.o  mninex.o  mnmnos.o  \
	mnprin.o  mnseek.o  mnvers.o mncalf.o mnderi.o mnfixp.o \
	mninit.o  mnmnot.o  mnpsdf.o  mnset.o   mnvert.o mncler.o mndxdi.o \
	mnfree.o  mninpu.o mnparm.o  mnrazz.o  mnseti.o  mnwarn.o mncntr.o  \
	mneig.o mngrad.o mnintr.o mnpars.o mnread.o  mnsimp.o  mnwerr.o \
	mncomd.o  mnemat.o  mnhelp.o  mnlims.o mnpfit.o mnrn15.o \
	mnstat.o  stand.o mncont.o  mnerrs.o  mnhes1.o  mnline.o  mnpint.o  \
	mnrset.o  mnstin.o

COBJS= intrac.o

libMinuit.a: $(FOBJS) $(COBJS)
	ar -r $@ $?
	cp $@ ../

$(FOBJS): 
	gfortran  -w  -g -O2 -c $*.F

$(COBJS): 
	gcc  -w  -g -O2 -c $*.c


lib: libMinuit.a

install:lib
	mv libMinuit.a ../


clean:
	rm -rf *.o *~ core.*



# End of file

