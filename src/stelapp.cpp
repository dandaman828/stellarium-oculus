//
// C++ Implementation: stelapp
//
// Description: 
//
//
// Author: Fabien Chereau <stellarium@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "stelapp.h"

StelApp::StelApp(const string& CDIR, const string& LDIR, const string& DATA_ROOT) : 
		frame(0), timefr(0), timeBase(0), fps(0), maxfps(10000.f),  FlagTimePause(0), 
		is_mouse_moving_horiz(false), is_mouse_moving_vert(false), draw_mode(StelApp::DM_NONE)
{
	configDir = CDIR;
	SelectedScript = SelectedScriptDirectory = "";
	core = new StelCore(LDIR, DATA_ROOT);
	ui = new StelUI(core, this);
	commander = new StelCommandInterface(core, this);
	scripts = new ScriptMgr(commander, core->getDataDir());
	time_multiplier = 1;
}


StelApp::~StelApp()
{
	SDL_FreeCursor(Cursor);
	delete ui;
	delete scripts;
	delete commander;
	delete core;
}

void StelApp::quit(void)
{
	static SDL_Event Q;						// Send a SDL_QUIT event
	Q.type = SDL_QUIT;						// To the SDL event queue
	if(SDL_PushEvent(&Q) == -1)				// Try to send the event
	{
		printf("SDL_QUIT event can't be pushed: %s\n", SDL_GetError() );
		exit(-1);
	}
}

void StelApp::init(void)
{
	//cout << Translator::getAvailableLanguagesCodes(LOCALEDIR) << endl;

	// Initialize video device and other sdl parameters
	InitParser conf;
	conf.load(configDir + "config.ini");
	
	// Main section
	string version = conf.get_str("main:version");
	if (version!=string(VERSION))
	{
		// The config file is too old to try an importation
		printf("The current config file is from a version too old for parameters to be imported (%s).\nIt will be replaced by the default config file.\n", version.empty() ? "<0.6.0" : version.c_str());
		system( (string("cp -f ") + core->getDataRoot() + "/data/default_config.ini " + getConfigFile()).c_str() );
	}
	
	screenW = conf.get_int("video:screen_w");
	screenH = conf.get_int("video:screen_h");
	initSDL(screenW, screenH, conf.get_int("video:bbp_mode"), conf.get_boolean("video:fullscreen"), core->getDataDir() + "/icon.bmp");	
	
	core->init(conf);
	
	setVisionModeNormal();
	if (conf.get_boolean("viewing:flag_chart")) setVisionModeChart();
	if (conf.get_boolean("viewing:flag_night")) setVisionModeNight();
	
	maxfps 				= conf.get_double ("video","maximum_fps",10000);
	string appLocaleName = conf.get_str("localization", "app_locale", "system");
	setAppLanguage(appLocaleName);
	scripts->set_allow_ui( conf.get_boolean("gui","flag_script_allow_ui",0) );

	string tmpstr = conf.get_str("projection:viewport");
	if (tmpstr=="maximized") core->setMaximizedViewport(screenW, screenH);
	else
		if (tmpstr=="square") core->setSquareViewport(screenW, screenH, conf.get_int("video:horizontal_offset"), conf.get_int("video:horizontal_offset"));
		else
		{
			if (tmpstr=="disk") core->getViewportMaskDisk();
			else
			{
				cerr << "ERROR : Unknown viewport type : " << tmpstr << endl;
				exit(-1);
			}
		}	

	// Navigation section
	PresetSkyTime 		= conf.get_double ("navigation","preset_sky_time",2451545.);
	StartupTimeMode 	= conf.get_str("navigation:startup_time_mode");	// Can be "now" or "preset"
	FlagEnableMoveMouse	= conf.get_boolean("navigation","flag_enable_move_mouse",1);
	MouseZoom			= conf.get_int("navigation","mouse_zoom",30);
	
	if (StartupTimeMode=="preset" || StartupTimeMode=="Preset")
		core->setJDay(PresetSkyTime - core->getObservatory().get_GMT_shift(PresetSkyTime) * JD_HOUR);
	
	// initialisation of the User Interface
	ui->init(conf);

	ui->init_tui();
	
	// play startup script, if available
	if(scripts) scripts->play_startup_script();
	
}

void StelApp::update(int delta_time)
{
	++frame;
	timefr+=delta_time;
	if (timefr-timeBase > 1000)
	{
		fps=frame*1000.0/(timefr-timeBase);				// Calc the FPS rate
		frame = 0;
		timeBase+=1000;
	}

	// change time rate if needed to fast forward scripts
	delta_time *= time_multiplier;

	// keep audio position updated if changing time multiplier
	if(!scripts->is_paused()) commander->update(delta_time);

	// run command from a running script
	scripts->update(delta_time);
	
	ui->gui_update_widgets(delta_time);
	ui->tui_update_widgets();

	if(!scripts->is_paused()) core->getImageMgr()->update(delta_time);
	
	core->update(delta_time);
}

//! Main drawinf function called at each frame
void StelApp::draw(int delta_time)
{
	// Render all the main objects of stellarium
	core->draw(delta_time);

	// Draw the Graphical ui and the Text ui
	ui->draw();
}

//! @brief Set the application locale. This apply to GUI, console messages etc..
void StelApp::setAppLanguage(const std::string& newAppLocaleName)
{
	// Update the translator with new locale name
	Translator::globalTranslator = Translator(PACKAGE, LOCALEDIR, newAppLocaleName);
	cout << "Application locale is " << Translator::globalTranslator.getLocaleName() << endl;
}

// Handle mouse clics
int StelApp::handleClick(Uint16 x, Uint16 y, S_GUI_VALUE button, S_GUI_VALUE state)
{
	return ui->handle_clic(x, y, button, state);
}

// Handle mouse move
int StelApp::handleMove(int x, int y)
{
	// Turn if the mouse is at the edge of the screen.
	// unless config asks otherwise
	if(FlagEnableMoveMouse)
	{
		if (x == 0)
		{
			core->turn_left(1);
			is_mouse_moving_horiz = true;
		}
		else if (x == core->getViewportWidth() - 1)
		{
			core->turn_right(1);
			is_mouse_moving_horiz = true;
		}
		else if (is_mouse_moving_horiz)
		{
			core->turn_left(0);
			is_mouse_moving_horiz = false;
		}

		if (y == 0)
		{
			core->turn_up(1);
			is_mouse_moving_vert = true;
		}
		else if (y == core->getViewportHeight() - 1)
		{
			core->turn_down(1);
			is_mouse_moving_vert = true;
		}
		else if (is_mouse_moving_vert)
		{
			core->turn_up(0);
			is_mouse_moving_vert = false;
		}
	}

	return ui->handle_move(x, y);

}

// Handle key press and release
int StelApp::handleKeys(Uint16 key, s_gui::S_GUI_VALUE state)
{
	s_tui::S_TUI_VALUE tuiv;
	if (state == s_gui::S_GUI_PRESSED) tuiv = s_tui::S_TUI_PRESSED;
	else tuiv = s_tui::S_TUI_RELEASED;
	if (ui->FlagShowTuiMenu)
	{

		if (state==S_GUI_PRESSED && key==SDLK_m)
		{
			// leave tui menu
			ui->FlagShowTuiMenu = false;

			// If selected a script in tui, run that now
			if(SelectedScript!="")
				commander->execute_command("script action play filename " +  SelectedScript
				                           + " path " + SelectedScriptDirectory);

			// clear out now
			SelectedScriptDirectory = SelectedScript = "";
			return 1;
		}
		if (ui->handle_keys_tui(key, tuiv)) return 1;
		return 1;
	}

	if (ui->handle_keys(key, state)) return 1;

	if (state == S_GUI_PRESSED)
	{
		// Direction and zoom deplacements
		if (key==SDLK_LEFT) core->turn_left(1);
		if (key==SDLK_RIGHT) core->turn_right(1);
		if (key==SDLK_UP)
		{
			if (SDL_GetModState() & KMOD_CTRL) core->zoom_in(1);
			else core->turn_up(1);
		}
		if (key==SDLK_DOWN)
		{
			if (SDL_GetModState() & KMOD_CTRL) core->zoom_out(1);
			else core->turn_down(1);
		}
		if (key==SDLK_PAGEUP) core->zoom_in(1);
		if (key==SDLK_PAGEDOWN) core->zoom_out(1);
	}
	else
	{
		// When a deplacement key is released stop mooving
		if (key==SDLK_LEFT) core->turn_left(0);
		if (key==SDLK_RIGHT) core->turn_right(0);
		if (SDL_GetModState() & KMOD_CTRL)
		{
			if (key==SDLK_UP) core->zoom_in(0);
			if (key==SDLK_DOWN) core->zoom_out(0);
		}
		else
		{
			if (key==SDLK_UP) core->turn_up(0);
			if (key==SDLK_DOWN) core->turn_down(0);
		}
		if (key==SDLK_PAGEUP) core->zoom_in(0);
		if (key==SDLK_PAGEDOWN) core->zoom_out(0);
	}
	return 0;
}


//! Set the drawing mode in 2D for drawing in the full screen
void StelApp::set2DfullscreenProjection(void) const
{
	glViewport(0, 0, screenW, screenH);
	glMatrixMode(GL_PROJECTION);		// projection matrix mode
    glPushMatrix();						// store previous matrix
    glLoadIdentity();
    gluOrtho2D(	0, screenW,
				0, screenH);			// set a 2D orthographic projection
	glMatrixMode(GL_MODELVIEW);			// modelview matrix mode
    glPushMatrix();
    glLoadIdentity();	
}
	
//! Restore previous projection mode
void StelApp::restoreFrom2DfullscreenProjection(void) const
{
    glMatrixMode(GL_PROJECTION);		// Restore previous matrix
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();	
}
