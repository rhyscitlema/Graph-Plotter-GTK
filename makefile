# C Compiler
COMPILER = gcc

# folder containing the libraries
FOLDER = ../

# additional include files
INCLUDE_FILES  =    $(FOLDER)lib_std/*.h $(FOLDER)libmfet/*.h $(FOLDER)algorithms/*.h $(FOLDER)read_write_image_file/*.h $(FOLDER)librodt/*h

# additional include folders
INCLUDE_FOLDER = -I $(FOLDER)lib_std/ -I $(FOLDER)libmfet/ -I $(FOLDER)algorithms/ -I $(FOLDER)read_write_image_file/ -I $(FOLDER)librodt/


# GTK library
GTK = gtk+-3.0

# Compiler flags (all warnings -Wall, optimization level -O3)
COMPILER_FLAGS = `pkg-config --cflags $(GTK)` $(INCLUDE_FOLDER) -Wall -g -DGTK_API -DLIBPNG -DLIBRODT -DLIB_STD -pthread

# additional library path and library name
CUSTOM_LIBRARY = -L $(FOLDER)read_write_image_file/libjpeg -ljpeg -lpng \
                #-L $(FOLDER)read_write_image_file/libpng -lpng

# all libraries (math library is -lm)
LINKER_FLAGS = `pkg-config --libs $(GTK)` $(CUSTOM_LIBRARY) -lm -pthread


# object files
OBJECT_FILES = userinterface/main.o \
               userinterface/files.o \
               userinterface/mthread.o \
               userinterface/keyboard.o \
               userinterface/userinterface.o \
               userinterface/drawing_window.o \
               $(FOLDER)read_write_image_file/rwif.o \
               $(FOLDER)algorithms/avl.o \
               $(FOLDER)algorithms/list.o \
               $(FOLDER)lib_std/_texts.o \
               $(FOLDER)lib_std/_string.o \
               $(FOLDER)lib_std/_strfun.o \
               $(FOLDER)lib_std/_math.o \
               $(FOLDER)lib_std/_value.o \
               $(FOLDER)lib_std/_stdio.o \
               $(FOLDER)lib_std/_malloc.o \
               $(FOLDER)libmfet/operations.o \
               $(FOLDER)libmfet/structures.o \
               $(FOLDER)libmfet/expression.o \
               $(FOLDER)libmfet/component.o \
               $(FOLDER)libmfet/mfet.o \
               $(FOLDER)librodt/object.o \
               $(FOLDER)librodt/camera.o \
               $(FOLDER)librodt/surface.o \
               $(FOLDER)librodt/outsider.o \
               $(FOLDER)librodt/getimage.o \
               $(FOLDER)librodt/mouse.o \
               $(FOLDER)librodt/timer.o \
               $(FOLDER)librodt/tools.o \
               $(FOLDER)librodt/userinterface_.o

# software output path and name
PROGRAM_NAME = ./GraphPlotter3D


# Build software: link object files and library files into the final software
default: $(OBJECT_FILES)
	$(COMPILER) $(OBJECT_FILES) $(LINKER_FLAGS) -o $(PROGRAM_NAME)

# Compile .c files to .o files
%.o: %.c $(INCLUDE_FILES)
	$(COMPILER) $(COMPILER_FLAGS) -c -o $@ $<


# remove created object files only
clean:
	rm -f $(OBJECT_FILES)

# remove all created files
clean_all:
	rm -f $(OBJECT_FILES)
	rm -f $(PROGRAM_NAME)

# run software
run:
	$(PROGRAM_NAME)

