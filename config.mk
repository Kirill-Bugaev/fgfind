# paths
PREFIX = /usr

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

PKG_CONFIG = pkg-config

# includes and libs
INCS = -I$(X11INC) \
       `$(PKG_CONFIG) --cflags fontconfig`
LIBS = -L$(X11LIB) -lX11 -lXft \
       `$(PKG_CONFIG) --libs fontconfig`

# flags
#DEBUG = -g -Og
FGFCFLAGS = $(INCS) $(DEBUG)
FGFLDFLAGS = $(LIBS) $(DEBUG)
