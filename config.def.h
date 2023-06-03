/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include "selfrestart.c"
/* appearance */
static unsigned int borderpx                = 1;        /* border pixel of windows */
static const Gap default_gap                = {.isgap = 1, .realgap = 10, .gappx = 10};
static const unsigned int snap              = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 2;   /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static int showbar            				= 1;     /* 0 means no bar */
static int topbar             				= 1;     /* 0 means bottom bar */
static const int horizpadbar = 2;
static const int vertpadbar  = 12;
static int vertpad		 					= 4;
static int sidepad 							= 4;
static const int blockpadding = 0;
static char font[]			 				= "MesloLGS NF:size=10";
//static const char *fonts[]          		= { font, "FiraCode Nerd Font", "FiraCode Mono Nerd Font", "NotoMono Nerd Font", "Hack Nerd Font Mono", "MesloLGS NF", "JetBrainsMono Nerd Font" };
//static char font[]			 				= "FiraCode Nerd Font:size=10";
static const char *fonts[]          		= { font, "FiraCode Nerd Font", "FiraCode Mono Nerd Font", "NotoMono Nerd Font", "Hack Nerd Font Mono", "MesloLGS NF", "JetBrainsMono Nerd Font", "Kochi Gothic", "Kochi Mincho" };
static const char dmenufont[]       		= "monospace:size=10";
static char normbgcolor[]       		 	= "#222222";
static char normbordercolor[]       		= "#444444";
static char normfgcolor[]       			= "#bbbbbb";
static char selfgcolor[]       				= "#eeeeee";
static char selbgcolor[]        			= "#005577";
static char selbordercolor[]        		= "#005577";
static char *colors[][3]      				= {
	/*               fg         bg         border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor, selbgcolor,  selbordercolor  },
};

/* tagging */
//static const char *tags[] = { "", "", "﬏", "", "", "" };
//static const char *tags[] = { "一", "二", "三", "四", "五", "六" };

static const char *tags[] = { "", "", "", "", "", "" };
static const char *alttags[] = { "", "", "", "", "", "" };

static const char buttonbar[]       = "󰍜";

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     iscentred   isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            0,		1,           -1 },
	{ "firefox",  NULL,       NULL,       1 << 5,       0,		0,           -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	//{ "[]=",      tile },    /* first entry is default */
	//{ "><>",      NULL },    /* no layout function means floating behavior */
	//{ "[M]",      monocle },
	{ "",		tile },
	{ "",		NULL },
	{ "",		monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *alttermcmd[] = { "st", NULL };
static const char *browsercmd[] = { "firefox", NULL };
static const char *fmcmd[] = { "nautilus", NULL };

static const StatusCmd statuscmds[] = {
	{ "notify-send Mouse$BUTTON", 1 },
	{ "bash /home/virashu/scripts/volume_brightness.sh notify_volume", 2 },
	{ "bash /home/virashu/scripts/volume_brightness.sh notify_brightness", 3 },
	{ "bash /home/virashu/scripts/volume_brightness.sh notify_battery", 4 },
};

static const char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

/* OLD
static const char *upvol[]    = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",  NULL};
static const char *downvol[]  = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",  NULL};
static const char *mutevol[]  = { "/usr/bin/pactl", "set-sink-mute", "0", "toggle", NULL};

static const char *light_up[] = { "/usr/bin/light", "-A", "5", NULL };
static const char *light_down[] = { "/usr/bin/light", "-U", "5", NULL};
*/

/* NEW */
static const char *upvol[]      = { "/usr/bin/bash", "/home/virashu/scripts/volume_brightness.sh", "volume_up", NULL };
static const char *downvol[]    = { "/usr/bin/bash", "/home/virashu/scripts/volume_brightness.sh", "volume_down", NULL };
static const char *mutevol[]    = { "/usr/bin/bash", "/home/virashu/scripts/volume_brightness.sh", "volume_mute", NULL };
static const char *light_up[]   = { "/usr/bin/bash", "/home/virashu/scripts/volume_brightness.sh", "brightness_up", NULL };
static const char *light_down[] = { "/usr/bin/bash", "/home/virashu/scripts/volume_brightness.sh", "brightness_down", NULL };

static const char *menubutton[] = { "/usr/bin/bash", "/home/virashu/scripts/xmenu.sh", NULL };
static const char *menubuttonalt[] = { "xdg-xmenu", NULL};

#define DWMPRINT 0x0000ff61

/*
static const char *scrotcmd[] = { "/usr/bin/scrot", "-d3", "/home/virashu/Pictures/screenshots/%Y-%m-%d-%s_$wx$h.jpg", NULL};
static const char *scrotfoccmd[] = { "/usr/bin/scrot", "--focused", NULL };
static const char *scrotselcmd[] = { "/usr/bin/scrot", "-d3", "/home/virashu/Pictures/screenshots/%Y-%m-%d-%s_$wx$h.jpg", "--select", NULL};
*/
/* TODO: Make a script for screenshots and notifying! */
/*
static const char *scrotcmd[] = { "/usr/bin/scrot", "-d0", "/home/virashu/Pictures/screenshots/%Y-%m-%d-%s_$wx$h.jpg", ";", "dunstify", "'screenshot taken'", NULL};
static const char *scrotfoccmd[] = { "/usr/bin/scrot", "--focused", NULL };
static const char *scrotselcmd[] = { "/usr/bin/scrot", "-d0", "/home/virashu/Pictures/screenshots/%Y-%m-%d-%s_$wx$h.jpg", "--select", NULL};
*/

static const char *scrotcmd[] = { "/bin/bash", "/home/virashu/scripts/screenshot.sh", "normal", NULL };
static const char *scrotfoccmd[] = { "/bin/bash", "/home/virashu/scripts/screenshot.sh", "focused", NULL };
static const char *scrotselcmd[] = { "/bin/bash", "/home/virashu/scripts/screenshot.sh", "selection", NULL};

/* XKB layouts */
static const char *kblayout_us[] = { "setxkbmap", "us", NULL };
static const char *kblayout_ru[] = { "setxkbmap", "ru", NULL };
static const char *kblayout_jp[] = { "setxkbmap", "jp", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",       	FLOAT,   &mfact },
		{ "vertpad",		INTEGER, &vertpad },
		{ "sidepad", 		INTEGER, &sidepad },
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_grave,  spawn,          {.v = menubutton } },
	{ MODKEY|ShiftMask,	            XK_grave,  spawn,          {.v = menubuttonalt } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask|ControlMask,	XK_Return, spawn, 	   {.v = alttermcmd } },
	{ MODKEY|ShiftMask,		XK_b,	   spawn,	   {.v = browsercmd } },
	{ MODKEY|ShiftMask,		XK_n,	   spawn,	   {.v = fmcmd} },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_bracketleft,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_bracketright, focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_comma,  view_adjacent,       {.i = -1 } },
	{ MODKEY,                       XK_period, view_adjacent,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = GAP_RESET } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = GAP_TOGGLE} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
  { MODKEY|ShiftMask,             XK_r,      self_restart,   {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ 0, 				                    XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
	{ 0, 				                    XF86XK_AudioRaiseVolume, spawn, {.v = upvol } },
	{ 0, 				                    XF86XK_AudioMute, spawn,   {.v = mutevol } },
	{ MODKEY, 			                XK_F1,     spawn,          {.v = mutevol} },
	{ 0,				XF86XK_MonBrightnessUp,	       spawn,					 {.v = light_up} },
	{ 0,				XF86XK_MonBrightnessDown,      spawn,          {.v = light_down} },
	{ MODKEY|ShiftMask,		          XK_s,      spawn,	         {.v = scrotcmd} },
	//{ MODKEY|ShiftMask|ControlMask,XK_s,     spawn,          {.v = scrotfoccmd} },
	//{ MODKEY|ShiftMask|ControlMask,XK_s,     spawn,          SHCMD("sleep 1s;scrot --select") },
	{ MODKEY|ShiftMask|ControlMask,	XK_s,      spawn,	         { .v = scrotselcmd } },
	{ Mod1Mask|ShiftMask,           XK_1,      spawn,          { .v = kblayout_us } },
  { Mod1Mask|ShiftMask,           XK_2,      spawn,          { .v = kblayout_ru } },
  { Mod1Mask|ShiftMask,           XK_3,      spawn,          { .v = kblayout_jp } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkButton,            0,              Button1,        spawn,          {.v = menubutton } },
	{ ClkButton,            0,              Button3,        spawn,          {.v = menubuttonalt } },
	{ ClkButton,            MODKEY,         Button1,        spawn,          {.v = menubuttonalt } },
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = statuscmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
