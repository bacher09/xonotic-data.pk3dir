#ifdef INTERFACE
CLASS(XonoticResolutionSlider) EXTENDS(XonoticTextSlider)
	METHOD(XonoticResolutionSlider, configureXonoticResolutionSlider, void(entity))
	METHOD(XonoticResolutionSlider, loadResolutions, void(entity, float))
	METHOD(XonoticResolutionSlider, addResolution, void(entity, float, float, float))
	METHOD(XonoticResolutionSlider, loadCvars, void(entity))
	METHOD(XonoticResolutionSlider, saveCvars, void(entity))
	METHOD(XonoticResolutionSlider, draw, void(entity))
	ATTRIB(XonoticResolutionSlider, vid_fullscreen, float, -1)
ENDCLASS(XonoticResolutionSlider)
entity makeXonoticResolutionSlider();
void updateConwidths(float width, float height, float pixelheight);
#endif

#ifdef IMPLEMENTATION

/* private static */ float XonoticResolutionSlider_DataHasChanged;

// Updates cvars (to be called by menu.qc at startup or on detected res change)
void updateConwidths(float width, float height, float pixelheight)
{
	vector r, c;
	float minfactor, maxfactor;
	float sz, f;

	// Save off current settings.
	cvar_set("_menu_vid_width", ftos(width));
	cvar_set("_menu_vid_height", ftos(height));
	cvar_set("_menu_vid_pixelheight", ftos(pixelheight));

	r_x = width;
	r_y = height;
	r_z = pixelheight;
	sz = cvar("menu_vid_scale");

	// calculate the base resolution
	c_z = 0;
	c_x = 800;
	c_y = c_x * r_y * r_z / r_x;
	if(c_y < 600)
	{
		c_y = 600;
		c_x = c_y * r_x / (r_y * r_z);
	}

	f = min(r_x / c_x, r_y / c_y);
	if(f < 1)
		c = c * f; // ensures that c_x <= r_x and c_y <= r_y

	minfactor = min(1, 640 / c_x);            // can be > 1 only if c_x is <640
	maxfactor = max(1, r_x / c_x, r_y / c_y); // can be < 1 only if r_x < c_x and r_y < c_y
	dprint("min factor: ", ftos(minfactor), "\n");
	dprint("max factor: ", ftos(maxfactor), "\n");

	if(sz < 0)
		f = 1 - (maxfactor - 1) * sz;
	else if(sz > 0)
		f = 1 + (minfactor - 1) * sz;
	else
		f = 1;
	c = c * f; // fteqcc fail

	cvar_set("vid_conwidth", ftos(rint(c_x)));
	cvar_set("vid_conheight", ftos(rint(c_y)));
	XonoticResolutionSlider_DataHasChanged = TRUE;
}
entity makeXonoticResolutionSlider()
{
	entity me;
	me = spawnXonoticResolutionSlider();
	me.configureXonoticResolutionSlider(me);
	return me;
}
void XonoticResolutionSlider_addResolution(entity me, float w, float h, float pixelheight)
{
	if (pixelheight != 1)
	{
		float aspect = w / (h * pixelheight);
		float bestdenom = rint(aspect);
		float bestnum = 1;
		float denom;
		for (denom = 2; denom < 10; ++denom) {
			float num = rint(aspect * denom);
			if (fabs(num / denom - aspect) < fabs(bestnum / bestdenom - aspect))
			{
				bestnum = num;
				bestdenom = denom;
			}
		}
		me.addValue(me, strzone(sprintf(_("%dx%d (%d:%d)"), w, h, bestnum, bestdenom)), strzone(strcat(ftos(w), " ", ftos(h), " ", ftos(pixelheight))));
	}
	else
		me.addValue(me, strzone(sprintf(_("%dx%d"), w, h)), strzone(strcat(ftos(w), " ", ftos(h), " ", ftos(pixelheight))));
	// FIXME (in case you ever want to dynamically instantiate this): THIS IS NEVER FREED
}
float autocvar_menu_vid_allowdualscreenresolution;
void XonoticResolutionSlider_configureXonoticResolutionSlider(entity me)
{
	me.configureXonoticTextSlider(me, "_menu_vid_width");
	me.loadResolutions(me, cvar("vid_fullscreen"));
}
void XonoticResolutionSlider_loadResolutions(entity me, float fullscreen)
{
	float i;
	vector r0, r;

	me.clearValues(me);

	if (fullscreen)
	{
		r0 = '0 0 0';
		for(i = 0;; ++i)
		{
			r = getresolution(i);
			if(r_x == 0 && r_y == 0)
				break;
			if(r_z == 0)
				r_z = 1; // compat
			if(r == r0)
				continue;
			r0 = r;
			if(r_x < 640 || r_y < 480)
				continue;
			if(r_x > 2 * r_y) // likely dualscreen resolution, skip this one
				if(autocvar_menu_vid_allowdualscreenresolution <= 0)
					continue;
				
			me.addResolution(me, r_x, r_y, r_z);
		}
	}

	if(me.nValues == 0)
	{
		me.addResolution(me, 640, 480, 1); // pc res
		me.addResolution(me, 720, 480, 1.125); // DVD NTSC 4:3
		me.addResolution(me, 720, 576, 0.9375); // DVD PAL 4:3
		me.addResolution(me, 720, 480, 0.84375); // DVD NTSC 16:9
		me.addResolution(me, 720, 576, 0.703125); // DVD PAL 16:9
		me.addResolution(me, 800, 480, 1); // 480p at 1:1 pixel aspect
		me.addResolution(me, 800, 600, 1); // pc res
		me.addResolution(me, 1024, 600, 1); // notebook res
		me.addResolution(me, 1024, 768, 1); // pc res
		me.addResolution(me, 1280, 720, 1); // 720p
		me.addResolution(me, 1280, 960, 1); // pc res
		me.addResolution(me, 1280, 1024, 1); // pc res
		me.addResolution(me, 1920, 1080, 1); // 1080p
	}

	me.vid_fullscreen = fullscreen;

	me.configureXonoticTextSliderValues(me);
}
void XonoticResolutionSlider_loadCvars(entity me)
{
	me.setValueFromIdentifier(me, strcat(cvar_string("_menu_vid_width"), " ", cvar_string("_menu_vid_height"), " ", cvar_string("_menu_vid_pixelheight")));
}
void XonoticResolutionSlider_saveCvars(entity me)
{
	if(me.value >= 0 || me.value < me.nValues)
	{
		tokenize_console(me.getIdentifier(me));
		cvar_set("_menu_vid_width", argv(0));
		cvar_set("_menu_vid_height", argv(1));
		cvar_set("_menu_vid_pixelheight", argv(2));
	}
}
void XonoticResolutionSlider_draw(entity me)
{
	if (cvar("vid_fullscreen") != me.vid_fullscreen)
	{
		me.loadResolutions(me, cvar("vid_fullscreen"));
		XonoticResolutionSlider_DataHasChanged = TRUE;
	}
	if (XonoticResolutionSlider_DataHasChanged)
	{
		XonoticResolutionSlider_DataHasChanged = FALSE;
		me.loadCvars(me);
	}
	SUPER(XonoticResolutionSlider).draw(me);
}
#endif
