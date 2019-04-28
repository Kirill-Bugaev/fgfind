/* try to find glyphs specified in argv[2] in font specified in argv[1] */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "unicode.h"

typedef struct {
	Display *dpy;
	int scr;
} XWindow;

typedef struct {
	FcFontSet *fs;
	XftFont **fonts;
} FontList;

static size_t utf2wchar(wchar_t **, const char *);
static FontList getfontslist(void);

static XWindow xw;

size_t
utf2wchar(wchar_t **wcs, const char *s)
{
	size_t (*decode)(wchar_t *, const char *, size_t);
	size_t mbslen;

//	decode = mbstowcs;
	decode = (size_t (*)(wchar_t *, const char *, size_t)) mandecode;

	/* Calculate the length required to hold argv[2] converted to
	 * a wide character string */
	mbslen = decode(NULL, s, 0);
	if (mbslen == (size_t) -1) {
		fprintf(stderr, "decode: Can't calculate size of wide character"
				"string. Invalid UTF.\n");
//		perror("mbstowcs");
		exit(EXIT_FAILURE);
	}

	/* Allocate wide character string of the desired size.  Add 1
	 * to allow for terminating null wide character (L'\0'). */
	*wcs = calloc(mbslen + 1, sizeof(wchar_t));
	if (*wcs == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	/* Convert the multibyte character string in argv[2] to a
	 * wide character string */
	if (decode(*wcs, s, mbslen + 1) == (size_t) -1) {
		fprintf(stderr, "decode: Can't convert the multibyte character string"
			  	" to wide character string. Invalid UTF.\n");
//		perror("mbstowcs");
		exit(EXIT_FAILURE);
	}

	return mbslen;
}

FontList
getfontslist(void)
{
	unsigned i;
	FontList fl;
	FcPattern *pat, *font;

	FcPattern *emptypat = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE,
		   	(char *) 0);
	FcFontSet *fs = FcFontList(0, emptypat, os);

	if (fs) {
		fl.fs = fs;
		fl.fonts = malloc(sizeof(XftFont *) * fs->nfont);
		for (i = 0; fs && i < fs->nfont; ++i) {
			pat = FcPatternCreate();
			font = FcFontRenderPrepare(0, pat, fs->fonts[i]);
			FcConfigSubstitute(0, font, FcMatchPattern);
			FcDefaultSubstitute(font);
			fl.fonts[i] = XftFontOpenPattern(xw.dpy, font);
			FcPatternDestroy(pat);
		}
	} else
		fl.fs = NULL;

	FcObjectSetDestroy(os);
	FcPatternDestroy(emptypat);
	return fl;
}

int main(int argc, char *argv[])
{
	XftFont *font;
	FcChar8 *file, *style, *family;
	FontList fl;
	wchar_t *wcs;
	size_t len, i;
	unsigned j;
	int absent = 0;

	if (argc < 3) {
		fprintf(stderr, "Not enough arguments\n"
			"Usage: %s FONT GLYPHS\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open a connection to the X server that controls a display 
	 * defined in DISPLAY environment variable */
	if (!(xw.dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Can't open display\n");
		exit(EXIT_FAILURE);
	}
	xw.scr = XDefaultScreen(xw.dpy);

	/* Open font */
	if (!(font = XftFontOpenName(xw.dpy, xw.scr, argv[1]))) {
		fprintf(stderr, "Can't open font\n");
		exit(EXIT_FAILURE);
	}

	/* Extract font info */
	if (FcPatternGetString(font->pattern, FC_FILE, 0, &file) == FcResultMatch &&
			FcPatternGetString(font->pattern, FC_FAMILY, 0, &family) ==
	   		FcResultMatch &&
			FcPatternGetString(font->pattern, FC_STYLE, 0, &style) ==
		   	FcResultMatch)
		printf("Font matches '%s'\n"
				"File: %s\n"
				"Family: %s\n"
				"Style: %s\n\n",
				argv[1], file, family, style);
	else {
		fprintf(stderr, "Can't extract font info\n");
		exit(EXIT_FAILURE);
	}

	/* Set locale according to the environment variables */
	if (setlocale(LC_ALL, "") == NULL) {
		perror("setlocale");
		exit(EXIT_FAILURE);
	}

	len = utf2wchar(&wcs, argv[2]);

	fl = getfontslist();

	/* Check gluphs are present in font */
	for (i = 0; i < len; ++i)
		if (!XftCharIndex(xw.dpy, font, wcs[i])) {
			absent = 1;
			printf("Glyph %lc is absent in preset font.\n"
					"It is present in fonts:\n",
				   	wcs[i]);
			
			/* Find fonts where glyph is present */
			for (j = 0; fl.fs && j < fl.fs->nfont; ++j) {
				if (XftCharIndex(xw.dpy, fl.fonts[j], wcs[i]))
					if (
						FcPatternGetString(fl.fonts[j]->pattern, FC_FILE, 0,
						&file) == FcResultMatch &&
   						FcPatternGetString(fl.fonts[j]->pattern, FC_FAMILY, 0,
						&family) == FcResultMatch &&
   						FcPatternGetString(fl.fonts[j]->pattern, FC_STYLE, 0,
						&style) == FcResultMatch)
					printf("-----\n"
							"File: %s\n"
							"Family: %s\n"
							"Style: %s\n",
							file, family, style);
			}

			printf("\n");
		}

	if (!absent)
		printf("All glyphs are present in preset font.\n");

	if (fl.fs) {
		for (j = 0; j < fl.fs->nfont; ++j)
			XftFontClose(xw.dpy, fl.fonts[j]);
		if (fl.fs->nfont)
			free(fl.fonts);
		FcFontSetDestroy(fl.fs);
	}
	free(wcs);
	XftFontClose(xw.dpy, font);
	XCloseDisplay(xw.dpy);
	return EXIT_SUCCESS;
}
