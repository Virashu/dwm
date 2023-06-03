#include "dwm.c"
#include "config.h"


void
buttonpress(XEvent *e)
{
	unsigned int i, x, click;
	int padding = 0;
	unsigned int xc;
	Arg arg = {0};
	Client *c;
	Monitor *m;
	XButtonPressedEvent *ev = &e->xbutton;

	padding -= sp * 2;

	click = ClkRootWin;
	/* focus monitor if necessary */
	if ((m = wintomon(ev->window)) && m != selmon) {
		unfocus(selmon->sel, 1);
		selmon = m;
		focus(NULL);
	}
	if (showsystray && selmon == systraytomon(selmon))
		padding -= getsystraywidth();
	if (ev->window == selmon->barwin) {
		i = x = 0;
		x += TEXTW(buttonbar);
		if (ev->x < x) {
			click = ClkButton;
		} else {
			do {
				x += TEXTW(tags[i]);
			} while (ev->x >= x && ++i < LENGTH(tags));
			if (i < LENGTH(tags)) {
				click = ClkTagBar;
				arg.ui = 1 << i;
			} else if (ev->x < x + blw)
				click = ClkLtSymbol;
			else if (ev->x > selmon->ww - selmon->tw + padding)
			{
				click = ClkStatusText;
				xc = selmon->ww - selmon->tw + padding;
				xc += lrpad / 2;
				char *text = rawstext;
				int i = -1;
				char ch;
				statuscmdn = 0;
				while (text[++i]) {
					if ((unsigned char)text[i] < ' ') {
						ch = text[i];
						text[i] = '\0';
						xc += status2dtextlength(text);
						text[i] = ch;
						text += i+1;
						i = -1;
						if (xc >= ev->x)
							break;
						if (ch <= LENGTH(statuscmds))
							statuscmdn = ch - 1;
					}
				}
			}
		}
	} else if ((c = wintoclient(ev->window))) {
		focus(c);
		restack(selmon);
		XAllowEvents(dpy, ReplayPointer, CurrentTime);
		click = ClkClientWin;
	}
	for (i = 0; i < LENGTH(buttons); i++)
		if (click == buttons[i].click && buttons[i].func && buttons[i].button == ev->button
		&& CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state))
			buttons[i].func(click == ClkTagBar && buttons[i].arg.i == 0 ? &arg : &buttons[i].arg);
}