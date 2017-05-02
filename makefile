# Makefile created by Rhyscitlema
# Explanation of file structure available at:
# http://rhyscitlema.com/applications/makefile.html

CALC_OUT_FILE = MFET_Calculator

GP3D_OUT_FILE = GraphPlotter3D

CALC_OBJ_FILES = userinterface/main.o \
                 userinterface/files.o \
                 userinterface/keyboard.o \
                 userinterface/userinterface.o

GP3D_OBJ_FILES = $(CALC_OBJ_FILES) \
                 userinterface/drawing_window.o
                 #userinterface/mthread.o

LIBALGO = ../algorithms
LIB_STD = ../lib_std
LIBMFET = ../libmfet
LIBRODT = ../librodt
LIBRWIF = ../read_write_image_file

#-------------------------------------------------

# C compiler
CC = gcc

# Linker
LD = gcc

# GTK library
GTK = gtk+-3.0

# C compiler flags
CC_FLAGS = `pkg-config --cflags $(GTK)` \
           -I$(LIBALGO) \
           -I$(LIB_STD) \
           -I$(LIBMFET) \
           -I$(LIBRODT) \
           -Wall \
           -pedantic \
           $(CFLAGS)

# linker flags
LD_FLAGS = -L$(LIBALGO) \
           -L$(LIB_STD) \
           -L$(LIBMFET) \
           -L$(LIBRODT) \
           -L$(LIBRWIF) \
           $(LDFLAGS)

# needed linker libs.
# note: the order below matters
CALC_LD_LIBS = -luidt \
               -lmfet \
               -l_std \
               -lalgo \
               `pkg-config --libs $(GTK)` \
               -lm \
               $(LDLIBS)

GP3D_LD_LIBS = -luidt \
               -lrodt \
               -lrwif \
               -lpng \
               $(CALC_LD_LIBS)

#-------------------------------------------------

gp3d:
	$(MAKE) gp3d_objs CFLAGS+="-DLIBRODT"
	cd $(LIBALGO) && $(MAKE) CFLAGS+="-DLIB_STD -I$(LIB_STD)"
	cd $(LIB_STD) && $(MAKE) CFLAGS+="-DCOMPLEX"
	cd $(LIBMFET) && $(MAKE)
	cd $(LIBRODT) && $(MAKE)
	cd $(LIBRWIF) && $(MAKE) lib_use_os_png
	$(LD) $(GP3D_OBJ_FILES) $(LD_FLAGS) $(GP3D_LD_LIBS) -o $(GP3D_OUT_FILE)

gp3d_objs: $(GP3D_OBJ_FILES)

calc: $(CALC_OBJ_FILES)
	cd $(LIBALGO) && $(MAKE) CFLAGS+="-DLIB_STD -I$(LIB_STD)"
	cd $(LIB_STD) && $(MAKE) CFLAGS+="-DCOMPLEX"
	cd $(LIBMFET) && $(MAKE)
	cd $(LIBRODT) && $(MAKE) uidt
	$(LD) $(CALC_OBJ_FILES) $(LD_FLAGS) $(CALC_LD_LIBS) -o $(CALC_OUT_FILE)

# remove all created files
clean:
	cd $(LIBALGO) && $(MAKE) clean
	cd $(LIB_STD) && $(MAKE) clean
	cd $(LIBMFET) && $(MAKE) clean
	cd $(LIBRODT) && $(MAKE) clean
	cd $(LIBRWIF) && $(MAKE) clean
	$(RM) userinterface/*.o $(CALC_OUT_FILE) $(GP3D_OUT_FILE)

#-------------------------------------------------

INCLUDE_FILES = $(LIBALGO)/*.h \
                $(LIB_STD)/*.h \
                $(LIBMFET)/*.h \
                $(LIBRODT)/*.h

# compile .c files to .o files
%.o: %.c $(INCLUDE_FILES)
	$(CC) $(CC_FLAGS) -c -o $@ $<

