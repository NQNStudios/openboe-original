/*
Blades of Exile Game/Scenario Editor/Character Editor
    Copyright (C) 1998-2007, Jeff Vogel
    http://www.spiderwebsoftware.com, spidweb@spiderwebsoftware.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <Memory.h>
#include <Quickdraw.h>
#include <QuickdrawText.h>

#include <Fonts.h>
#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <stdio.h>
#include <osevents.h>
#include <GestaltEqu.h>
#include <Palettes.h>
#include <AppleEvents.h>
#include <Files.h>
#include <StandardFile.h>
#include <Controls.h>
#include <QDOffscreen.h> 


#include "global.h"

#include "gamma.h"
#include "blxgraphics.h"
#include "newgraph.h"
#include "blxfileio.h"
#include "blxactions.h"
#include "text.h"
#include "party.h"
#include "sound.h"
#include "items.h"
#include "fields.h"
#include "town.h"
#include "dlogtool.h"
#include "startup.h"
#include "dialogutils.h"
#include "Exile.sound.h"
#include "info.dialogs.h"
#include "bldsexil.h"


/* Mac stuff globals */
Rect	windRect, Drag_Rect;
Boolean Multifinder_Present, All_Done = FALSE,dialog_not_toast = FALSE;
EventRecord	event;
WindowPtr	mainPtr;	
Handle menu_bar_handle;
MenuHandle apple_menu,file_menu,extra_menu,help_menu,monster_info_menu,library_menu;
MenuHandle actions_menu,music_menu,mage_spells_menu,priest_spells_menu;
short had_text_freeze = 0,num_fonts;
Boolean in_startup_mode = TRUE,app_started_normally = FALSE;
Boolean play_sounds = TRUE,first_startup_update = TRUE;
Boolean diff_depth_ok = FALSE,first_sound_played = FALSE,spell_forced = FALSE,startup_loaded = FALSE;
Boolean save_maps = TRUE,party_in_memory = FALSE;
CGrafPtr color_graf_port;
ControlHandle text_sbar = NULL,item_sbar = NULL,shop_sbar = NULL;
Rect sbar_rect = {283,546,421,562};
Rect shop_sbar_rect = {67,258,357,274};
Rect item_sbar_rect = {146,546,253,562};
Boolean bgm_on = FALSE,bgm_init = FALSE;
short dungeon_font_num,geneva_font_num;
short dialog_answer;
Point store_anim_ul;
scenario_data_type scenario;
piles_of_stuff_dumping_type *data_store;
talking_record_type talking;

Boolean gInBackground = FALSE;
long start_time;

short on_spell_menu[2][62];
short on_monst_menu[256];

// Cursors 
short current_cursor = 120;
CursHandle arrow_curs[3][3], sword_curs, boot_curs, key_curs, target_curs,talk_curs,look_curs;


// Shareware globals
Boolean registered = TRUE,ed_reg = FALSE;
long register_flag = 0;
long ed_flag = 0,ed_key;
Boolean game_run_before = TRUE;

Boolean debug_on = FALSE,give_intro_hint = TRUE,ask_to_change_color = TRUE,in_scen_debug = FALSE;
Boolean belt_present = FALSE;

/* Adventure globals */
party_record_type party;
pc_record_type adven[6];
outdoor_record_type outdoors[2][2];
current_town_type c_town;
big_tr_type t_d;
town_item_list	t_i;
unsigned char out[96][96],out_e[96][96];
setup_save_type setup_save;
unsigned char misc_i[64][64],sfx[64][64];
////unsigned char template_terrain[64][64];
location monster_targs[T_M];
outdoor_strs_type outdoor_text[2][2];

/* Display globals */
extern short combat_posing_monster = -1, current_working_monster = -1; // 0-5 PC 100 + x - monster x
Boolean fast_bang = FALSE;
short spec_item_array[60];
short overall_mode = 45,current_spell_range;
Boolean first_update = TRUE,anim_onscreen = FALSE,frills_on = TRUE,sys_7_avail,suppress_stat_screen = FALSE;
short stat_window = 0,store_modifier;
Boolean monsters_going = FALSE,boom_anim_active = FALSE,cartoon_happening = FALSE;
short give_delays = 0;
Boolean modeless_exists[18] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
								FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,
								FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
short modeless_key[18] = {1079,1080,1081,1082,1084, 1046,1088,1089,1090,1092, 1095,1072,0,0,0,0,0,0};
DialogPtr modeless_dialogs[18] = {NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	
								NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
								NULL,	NULL,	NULL,	NULL,	NULL,	NULL};
//Rect d_rects[80];
////short d_rect_index[80];
short town_size[3] = {64,48,32};
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
Point ul = {28,10};
short display_mode = 0; // 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
long stored_key;
short pixel_depth,old_depth = 8;
short current_ground = 0,stat_screen_mode = 0;
short anim_step = -1;
 
// Spell casting globals
short store_mage = 0, store_priest = 0;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short pc_last_cast[2][6] = {{1,1,1,1,1,1},{1,1,1,1,1,1}};
short num_targets_left = 0;
location spell_targets[8];
					
/* Combat globals */
short which_combat_type,town_type;
location center;
unsigned char combat_terrain[64][64];
location pc_pos[6];
short current_pc;
short combat_active_pc;
effect_pat_type current_pat;
short monst_target[T_M]; // 0-5 target that pc   6 - no target  100 + x - target monster x
short spell_caster, missile_firer,current_monst_tactic;
short store_current_pc = 0;

////town_record_type anim_town;
tiny_tr_type anim_t_d;

stored_items_list_type stored_items[3];
stored_outdoor_maps_type o_maps;

// Special stuff booleans
Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
Boolean sleep_field;

long last_anim_time = 0;


ModalFilterUPP main_dialog_UPP;

KeyMap key_state;
Boolean fry_startup = FALSE;

// 
//	Main body of program Exile
//
#ifdef EXILE_BIG_GUNS
pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
ControlActionUPP text_sbar_UPP;
ControlActionUPP item_sbar_UPP;
ControlActionUPP shop_sbar_UPP;
#endif




//MW specified argument and return type.
void main(void)
{
	short i;

	MaxApplZone();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();

	data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));	
	start_time = TickCount();

	check_sys_7();


	Initialize();
#ifdef EXILE_BIG_GUNS
	main_dialog_UPP = NewModalFilterProc(cd_event_filter);
text_sbar_UPP = NewControlActionProc(sbar_action);
item_sbar_UPP = NewControlActionProc(item_sbar_action);
shop_sbar_UPP = NewControlActionProc(shop_sbar_action);

#endif

	load_cursors();



	Set_Window_Drag_Bdry();

	init_buf();

	load_sounds();

	if (sys_7_avail == TRUE) {
		set_up_apple_events();
		}
	//import_template_terrain();
	//import_anim_terrain(0);
	plop_fancy_startup();

	party.stuff_done[306][2] = 0;
	party.stuff_done[306][1] = 0;

	init_screen_locs();
	cd_init_dialogs();

	Multifinder_Present = (NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
		NGetTrapAddress(_Unimplemented, ToolTrap));
	
	//init_party(0);
	PSD[306][6] = 1;
	//init_anim(0);

		
	menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NIL) {
		SysBeep(2); SysBeep(2); SysBeep(2);
		ExitToShell();
		}
	SetMenuBar(menu_bar_handle);
	DisposHandle(menu_bar_handle);


	apple_menu = GetMHandle(500);
	file_menu = GetMHandle(550);
	extra_menu = GetMHandle(600);
	help_menu = GetMHandle(650);
	library_menu = GetMHandle(750);
	actions_menu = GetMHandle(800);
	music_menu = GetMHandle(850);

	AddResMenu(apple_menu, 'DRVR');
	DrawMenuBar();


	menu_activate(0);



	fancy_startup_delay();


	init_spell_menus();

	if (overall_mode == 45)
		overall_mode = 0;

	//if (fry_startup == FALSE) {
		if (game_run_before == FALSE)
			FCD(986,0);
			else if (give_intro_hint == TRUE)
				tip_of_day();
	//	}

	menu_activate(0);
	DrawMenuBar();
	create_modeless_dialog(1046);

	while (All_Done == FALSE) 
		Handle_One_Event();
	
	close_program();  
}



// 
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void Initialize(void)
{

	OSErr		error;
	SysEnvRec	theWorld;
	Str255 tit = "  ";
	short i;
	
	//
	//	Test the computer to be sure we can do color.  
	//	If not we would crash, which would be bad.  
	//	If we can�t run, just beep and exit.
	//

	error = SysEnvirons(1, &theWorld);
	if (theWorld.hasColorQD == false) {
		SysBeep(50);
		ExitToShell();					/* If no color QD, we must leave. */
	}
	
	/* Initialize all the needed managers. */
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
	
	GetKeys(key_state);
		
	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don�t for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	GetDateTime((unsigned long*) &qd.randSeed);

	//
	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//
	windRect = qd.screenBits.bounds;

	find_quickdraw();
	init_directories();

	stored_key = open_pref_file();
	if (stored_key == -100) {
		stored_key = open_pref_file();
		if (stored_key == -100) {
			Alert(983,NIL);
			ExitToShell();	
			}
		}
	set_pixel_depth();
	mainPtr = GetNewCWindow(128,NIL,IN_FRONT);
	SetPort(mainPtr);						/* set window to current graf port */
	text_sbar = NewControl(mainPtr,&sbar_rect,tit,FALSE,58,0,58,scrollBarProc,1);
	item_sbar = NewControl(mainPtr,&item_sbar_rect,tit,FALSE,0,0,16,scrollBarProc,2);
	shop_sbar = NewControl(mainPtr,&shop_sbar_rect,tit,FALSE,0,0,16,scrollBarProc,3);
	adjust_window_mode();
	
}

void Set_Window_Drag_Bdry()
{
	Drag_Rect = qd.screenBits.bounds;
//	Drag_Rect = (**(GrayRgn)).rgnBBox;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

void Handle_One_Event()
{
	short chr,chr2;
	long menu_choice,cur_time;
	Boolean event_in_dialog = FALSE;
	GrafPtr old_port;
	
	if (Multifinder_Present == TRUE) {
		through_sending();
		WaitNextEvent(everyEvent, &event, SLEEP_TICKS, MOUSE_REGION);
		cur_time = TickCount();
		if ((event.what != 23) && (!gInBackground) && 
			((FrontWindow() == mainPtr) || (FrontWindow() == modeless_dialogs[5]))) {
			change_cursor(event.where);
			if ((cur_time - start_time) % 5 == 0)
				draw_targeting_line(event.where);
			}

//(cur_time - last_anim_time > 42)
		if ((cur_time % 40 == 0) && (in_startup_mode == FALSE) && (anim_onscreen == TRUE) && (PSD[306][9] == 0)
			&& ((FrontWindow() == mainPtr) || (FrontWindow() == modeless_dialogs[5])) && (!gInBackground)) {
			last_anim_time = cur_time;
			initiate_redraw();
			}
		if ((cur_time - last_anim_time > 20) && (in_startup_mode == TRUE) 
			&& (app_started_normally == TRUE) && (FrontWindow() == mainPtr)) {
			last_anim_time = cur_time;
			draw_startup_anim();
			}
			
		GetPort(&old_port);				
//		if (FrontWindow() != old_port)
//			SysBeep(2);
		}
		else
		{
			through_sending();
			SystemTask();
			GetNextEvent( everyEvent, &event);
		}

	clear_sound_memory();
	
	event_in_dialog = handle_dialog_event();
	
	if (event_in_dialog == FALSE)
	switch (event.what)
	{
		case keyDown: case autoKey:
				chr = event.message & charCodeMask;
				chr2 = (char) ((event.message & keyCodeMask) >> 8);
				if ((event.modifiers & cmdKey) != 0) {
					if (event.what != autoKey) {
//						SelectWindow(mainPtr);
//						SetPort(mainPtr);
						menu_choice = MenuKey(chr);
						handle_menu_choice(menu_choice);
						}
					}
					else 
						handle_keystroke(chr,chr2,event);
						
			break;
		
		case mouseDown:
			Mouse_Pressed();
			break;
		
		case activateEvt:
			Handle_Activate();
			break;
		
		case updateEvt:
			set_pixel_depth();
			Handle_Update();
			break;

		case kHighLevelEvent: 
			AEProcessAppleEvent(&event);
			break;

		case osEvt:
		/*	1.02 - must BitAND with 0x0FF to get only low byte */
			switch ((event.message >> 24) & 0x0FF) {		/* high byte of message */
				case suspendResumeMessage:		/* suspend/resume is also an activate/deactivate */
					gInBackground = (event.message & 1) == 0;  // 1 is Resume Mask
					//switch_bg_music(gInBackground);
//					DoActivate(FrontWindow(), !gInBackground);
					current_cursor = 300;
					break;
			}
			break;
		}

}


void Handle_Activate()
{
	GrafPtr old_port;
	
	GetPort(&old_port);				
	if (FrontWindow() == mainPtr)
		SetPort(mainPtr);
}

Boolean handle_dialog_event() 
{
	Boolean event_was_dlog = FALSE;
	short i,item_hit;
	DialogPtr event_d;
	
	if (FrontWindow() != NIL) {
		if (IsDialogEvent(&event)) {
			if (DialogSelect(&event, &event_d, &item_hit))
				for (i = 0; i < 18; i++)
					if (event_d == modeless_dialogs[i])	{
						/*CloseDialog(modeless_dialogs[i]);
						modeless_exists[i] = FALSE;

						event_was_dlog = TRUE;
						
						SetPort(mainPtr);
						SelectWindow(mainPtr);
						SetPort(mainPtr); */
						}
			
			}
	
		}
	
//	if ((event.what == keyDown) && (FrontWindow() != mainPtr))
	
//		}
	
	return event_was_dlog;
}


void Handle_Update()
{
	WindowPtr the_window;
	GrafPtr		old_port;
	
	the_window = (WindowPtr) event.message;
	
	GetPort (&old_port);
	SetPort (the_window);
	
	BeginUpdate(the_window);
		
	reset_text_bar(); // Guarantees text bar gets refreshed
	
	if (the_window == mainPtr) {
		if (in_startup_mode == TRUE) {
			/*if (first_startup_update == TRUE) 
				first_startup_update = FALSE;
				else*/ draw_startup(0);
/*			if (first_sound_played == FALSE) {
				play_sound(22);
				first_sound_played = TRUE;
				}  */
			}
		else {
			if (first_update == TRUE) {
				first_update = FALSE;
				if (overall_mode == 0) 
					redraw_screen(0);
				if ((overall_mode > 0) & (overall_mode < 10))
					redraw_screen(0);
			// 1st update never combat
				}
				else refresh_screen(0);
			}
		}
		
	EndUpdate(the_window);
	
	SetPort(old_port);
}

pascal void sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting,max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case inUpButton: new_setting--; break;
		case inDownButton: new_setting++; break;
		case inPageUp: new_setting -= 11; break;
		case inPageDown: new_setting += 11; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		print_buf();
}

pascal void item_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case inUpButton: new_setting--; break;
		case inDownButton: new_setting++; break;
		case inPageUp: new_setting -= (stat_window == 7) ? 2 : 8; break;
		case inPageDown: new_setting += (stat_window == 7) ? 2 : 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		put_item_screen(stat_window,1);
}
pascal void shop_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case inUpButton: new_setting--; break;
		case inDownButton: new_setting++; break;
		case inPageUp: new_setting -= 8; break;
		case inPageDown: new_setting += 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		draw_shop_graphics(0,shop_sbar_rect);			
}

void Mouse_Pressed()
{
	WindowPtr	the_window;
	short	the_part,choice,i,content_part;
	long menu_choice;
	ControlHandle control_hit;
	
	if (had_text_freeze > 0) {
		had_text_freeze--;
		return;
		}
	
	the_part = FindWindow( event.where, &the_window);
	
	//ding();
	
	switch (the_part)
	{
	
		case inMenuBar:
			menu_choice = MenuSelect(event.where);
			handle_menu_choice(menu_choice);
			break;
		
		case inSysWindow:
			break;
		
		case inDrag:
			DragWindow(the_window, event.where, &qd.screenBits.bounds);
			break;
		
		case inGoAway:
			if (the_window == mainPtr) {
				if (in_startup_mode == TRUE) {
					All_Done = TRUE;
					break;
					}
				if (overall_mode > 1){
					choice = FCD(1067,0);
					if (choice == 1)
						return;
					}
					else {
						choice = FCD(1066,0);
						if (choice == 3)
							break;
						if (choice == 1)
							save_file(0);
						}
				All_Done = TRUE;
				}
				else {
					for (i = 0; i < 18; i++)
						if ((the_window == modeless_dialogs[i]) && (modeless_exists[i] == TRUE)) {
							//CloseDialog(modeless_dialogs[i]);
							HideWindow(modeless_dialogs[i]); 
							modeless_exists[i] = FALSE;
							SelectWindow(mainPtr);
							SetPort(mainPtr);		
							}
					}
			break;
		
		case inContent:
			if ((the_window == mainPtr) && (((modeless_exists[5] == FALSE) && (FrontWindow() != the_window)) ||
				((modeless_exists[5] == TRUE) && (the_window == mainPtr) && (FrontWindow() != modeless_dialogs[5])))) {
				if (modeless_exists[5] == TRUE) {
					SetPort(mainPtr);
					SelectWindow(mainPtr);
					SetPort(mainPtr);
					BringToFront(modeless_dialogs[5]);
					}
					else {
					SetPort(mainPtr);
					SelectWindow(mainPtr);
					SetPort(mainPtr);
					}
				}
				else  
					if (the_window == mainPtr) {
						SetPort(mainPtr);
						GlobalToLocal(&event.where);
						content_part = FindControl(event.where,the_window,&control_hit); // hit sbar?
						if (content_part != 0) {
							switch (content_part) {
								case inThumb:
									content_part = TrackControl(control_hit,event.where,NIL);
									if (control_hit == text_sbar)
										if (content_part == inThumb)
											print_buf();
									if (control_hit == item_sbar)
										if (content_part == inThumb)
											put_item_screen(stat_window,0);
									if (control_hit == shop_sbar)
										if (content_part == inThumb)
											draw_shop_graphics(0,shop_sbar_rect);			
									break;
								case inUpButton: case inPageUp: case inDownButton: case inPageDown:
#ifndef EXILE_BIG_GUNS
									if (control_hit == text_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)sbar_action);
									if (control_hit == item_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)item_sbar_action);
									if (control_hit == shop_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)shop_sbar_action);
#endif
#ifdef EXILE_BIG_GUNS
									if (control_hit == text_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)text_sbar_UPP);
									if (control_hit == item_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)item_sbar_UPP);
									if (control_hit == shop_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)shop_sbar_UPP);
#endif
									break;

								}
								
							} // a control hit
					 		else { // ordinary click
								if (in_startup_mode == FALSE)
									All_Done = handle_action(event);
									else All_Done = handle_startup_press(event.where);
							}
					}
			break;
	}

	menu_activate((in_startup_mode == TRUE) ? 0 : 1);

}

void close_program()
{
	restore_depth();
	//end_music();
}

void handle_menu_choice(long choice)
{
	int menu,menu_item;

	if (choice != 0) {
		menu = HiWord(choice);
		menu_item = LoWord(choice);

//MenuHandle 500 apple_menu,550 file_menu,600 extra_menu,
//659 help_menu,700 monster_info_menu,750 library_menu,800 actions_menu,850 music_menu;900 mage_spells_menu, 950 priest_spells_menu
		switch (menu) {
			case 500:
				handle_apple_menu(menu_item);
				break;
			case 550:
				handle_file_menu(menu_item);
				break;
			case 600: 
				handle_options_menu(menu_item);
				break;
			case 650:
				handle_help_menu(menu_item);
				break;
			case 700:
				handle_monster_info_menu(menu_item);
				break;
			case 750:
				handle_library_menu(menu_item);
				break;
			case 800:
				handle_actions_menu(menu_item);
				break;
			case 850:
				handle_music_menu(menu_item);
				break;
			case 900:
				handle_mage_spells_menu(menu_item);
				break;
			case 950:
				handle_priest_spells_menu(menu_item);
				break;
			}
		}
	menu_activate((in_startup_mode == TRUE) ? 0 : 1);
 
	HiliteMenu(0);
}

void handle_apple_menu(int item_hit)
{
	Str255 desk_acc_name;
	short desk_acc_num;
	
	switch (item_hit) {
		case 1:
			FCD(1062,0);
			break;
		default:
			GetItem (apple_menu,item_hit,desk_acc_name);
			desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
		}
}

void handle_file_menu(int item_hit)
{
	short choice,i;

	switch (item_hit) {
		case 1:
			if (in_startup_mode == TRUE)
				startup_load();
				else do_load();
			break;
		case 2:
			do_save(0);
			break;
		case 3:
			if (in_startup_mode == TRUE)
				save_file(1);
				else do_save(1);
			break;
		case 4:
			if (in_startup_mode == FALSE) {
				choice = FCD(1091,0);
				if (choice == 1)
					return;
				for (i = 0; i < 6; i++)
					adven[i].main_status = 0;
				party_in_memory = FALSE;
				reload_startup();
				in_startup_mode = TRUE;
				draw_startup(0);
				}
			start_new_game();
			update_pc_graphics();
			draw_startup(0);
			break;
		case 6:
			pick_preferences();
			break;
		case 8:

			if (in_startup_mode == TRUE) {
				All_Done = TRUE;
				break;
				}
			if (overall_mode > 1) {
				choice = FCD(1067,0);
				if (choice == 1)
					return;
				}
				else {
					choice = FCD(1066,0);
					if (choice == 3)
						break;
					if (choice == 1)
						save_file(0);
					}
			All_Done = TRUE;
			break;
		}
}

void handle_options_menu(int item_hit)
{
	MenuHandle option_menu;
	short choice,i;

	switch (item_hit) {
		case 1:
			choice = char_select_pc(0,0,"New graphic for who?");
			if (choice < 6)
				pick_pc_graphic(choice,1,0);
			update_pc_graphics();
			initiate_redraw();
			break;
			
		case 4:
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					choice = char_select_pc(0,0,"Delete who?");
					if (choice < 6) {
						if ((i = FCD(1053,0)) == 2)
							kill_pc(choice,0);
						}
					update_pc_graphics();
					initiate_redraw();
					}
			break;
			
			
		case 2:
			choice = select_pc(0,0);
			if (choice < 6)
				pick_pc_name(choice,0);
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
			
		case 3:
			if (!(is_town())) {
				add_string_to_buf("Add PC: Town mode only.");
				print_buf();
				break;
				}
			for (i = 0; i < 6; i++)
				if (adven[i].main_status == 0)
					i = 20;
			if (i == 6) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				}
			if (c_town.town_num == scenario.which_town_start) {
				give_help(56,0,0);
				create_pc(6,0);
				}
				else {
					add_string_to_buf("Add PC: You can only make new");
					add_string_to_buf("  characters in the town you ");
					add_string_to_buf("  started in.");
					}
			print_buf();
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;	
			
		//case 6:
		//	journal();
		//	break;////
		case 6:	
			if (overall_mode == 20) {
				ASB("Talking notes: Can't read while talking.");
				print_buf();
				return;
				}
			talk_notes();
			break;
		case 7:
			adventure_notes();
			break;
		case 8:
			if (in_startup_mode == FALSE)
				print_party_stats();
			break;
		}
}

void handle_help_menu(int item_hit)
{
	switch (item_hit) {
		case 1: FCD(1079,0);
				break;
		case 2: FCD(1080,0); break;
		case 3: FCD(1081,0); break;
		case 4: FCD(1072,0); break; // magic barriers
		case 6: FCD(1084,0); break;
		case 7: FCD(1088,0); break;
		}
}
void handle_library_menu(int item_hit)
{
	switch (item_hit) {
		case 1: display_spells(0,100,0);
				break;
		case 2: display_spells(1,100,0);
				break;
		case 3: display_skills(100,0);
			    break;
		case 4:
				display_help(0,0);
				break;
		case 5:
			tip_of_day(); break;
		case 7:
		FCD(986,0);
				break;
		}
}

void handle_actions_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			handle_keystroke('A',0,event);
			break;
		case 2:
			handle_keystroke('w',0,event);
			break;
		case 3:	
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					give_help(62,0,0);
					display_map();
					}
			make_cursor_sword();
			break;
	}
}

void handle_mage_spells_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			give_help(209,0,0);
			break;
		default:
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else handle_menu_spell(on_spell_menu[0][item_hit - 3],0);
			break;
	}
}
void handle_priest_spells_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			give_help(209,0,0);
			break;
		default:
			if (prime_time() == FALSE) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else handle_menu_spell(on_spell_menu[1][item_hit - 3],1);
			break;
	}
}
void handle_monster_info_menu(int item_hit)
{
	display_monst(item_hit - 1,(creature_data_type *) NULL,1);
}

void handle_music_menu(int item_hit)
{
	short choice;

	switch (item_hit) {
		case 1: 
			if (bgm_on == FALSE) {
				choice = choice_dialog(0,600);
				//if (choice == 1)
				//init_bg_music();
				}
				//else end_music(1);
			if (bgm_on == TRUE)
				party.stuff_done[306][5] = 1;
				else party.stuff_done[306][5] = 0;
			break;
		}
}

void load_cursors()
{
	short i,j;
	
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			arrow_curs[i][j] = GetCursor(100 + (i - 1) + 10 * (j - 1));
	sword_curs = GetCursor(120);

//	HLock ((Handle) sword_curs);
//	SetCursor (*sword_curs);
//	HUnlock((Handle) sword_curs);

	boot_curs = GetCursor(121);
	key_curs = GetCursor(122);
	target_curs = GetCursor(124);
	talk_curs = GetCursor(126);
	look_curs = GetCursor(129);

	set_cursor(sword_curs);
	current_cursor = 124;
	
}

void set_cursor(CursHandle which_curs)
{
	HLock ((Handle) which_curs);
	SetCursor (*which_curs);
	HUnlock((Handle) which_curs);
}

void change_cursor(Point where_curs)
{
	short curs_types[50] = {0,0,126,124,122,122,0,0,0,0,
					0,124,124,124,124,122,0,0,0,0,
					120,120,0,0,0,0,0,0,0,0,
					0,0,0,0,0,129,129,129,0,0,
					0,0,0,0,0,120,0,0,0,0},cursor_needed;
	location cursor_direction;
	WindowPtr the_window;
	Rect world_screen = {23, 23, 346, 274};
	

		
	SetPort(mainPtr);
	GlobalToLocal(&where_curs);	
	
	where_curs.h -= ul.h;
	where_curs.v -= ul.v;
	
	if (PtInRect(where_curs,&world_screen) == FALSE)
		cursor_needed = 120;
		else cursor_needed = curs_types[overall_mode];
	
	if (cursor_needed == 0) {
		if ((PtInRect(where_curs,&world_screen) == FALSE) || (in_startup_mode == TRUE))
			cursor_needed = 120;
			else {
				cursor_direction = get_cur_direction(where_curs);
				cursor_needed = 100 + (cursor_direction.x) + 10 * ( cursor_direction.y);
				if (cursor_needed == 100)
					cursor_needed = 120;
				}
		}

	if (cursor_needed == current_cursor)
		return;
	
	current_cursor = cursor_needed;
	
	switch (cursor_needed) {
		case 120:
			set_cursor(sword_curs);
			break;
		case 121:
			set_cursor(boot_curs);
			break;
		case 122:
			set_cursor(key_curs);
			break;
		case 124:
			set_cursor(target_curs);
			break;
		case 126:
			set_cursor(talk_curs);
			break;
		case 129:
			set_cursor(look_curs);
			break;
					
		
		default:  // an arrow
			set_cursor(arrow_curs[cursor_direction.x + 1][cursor_direction.y + 1]);
			break;	
		}	
}

void find_quickdraw() {
	OSErr err;
	long response;
	short choice;
	
	err = Gestalt(gestaltQuickdrawVersion, &response);
	if (err == noErr) {
		if (response == 0x000) {
			choice = choice_dialog(0,1070);
			if (choice == 2)
				ExitToShell();
				else diff_depth_ok = TRUE;
			}
		}
		else  {
			SysBeep(2);
			ExitToShell();
			}
}

void set_pixel_depth() {
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	short choice;
	
	cur_device = GetGDevice();	
	
	if ((HasDepth(cur_device,8,1,1)) == 0) {
		choice_dialog(0,1070);
		ExitToShell();
		}
	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	pixel_depth = (**(screen_pixmap_handle)).pixelSize;
	
	if (diff_depth_ok == TRUE)
		return;
	
	if (pixel_depth != 8) {
		if (ask_to_change_color == TRUE)
			choice = choice_dialog(0,1071);
			else choice = 1;
		if (choice == 4) {
			choice = 1;
			ask_to_change_color = FALSE;
			save_prefs();
			}
		if (choice == 3)
			diff_depth_ok = TRUE;
		if (choice == 2)
			ExitToShell();
		if (choice == 1) {
			err = SetDepth(cur_device,8,1,1);
			old_depth = pixel_depth;
//			if (err != noErr)
//				ExitToShell();
			}
		}
}

void restore_depth()
{
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	short choice;
	
	cur_device = GetGDevice();	

	screen_pixmap_handle = (**(cur_device)).gdPMap;

	if (old_depth != 8) {

			err = SetDepth(cur_device,old_depth,1,1);

		}

}

void check_sys_7()
{
	OSErr err;
	long response;
	
	err = Gestalt(gestaltSystemVersion, &response);
	if ((err == noErr) && (response >= 0x0700))
		sys_7_avail = TRUE;
		else sys_7_avail = FALSE;
}

pascal OSErr handle_open_app(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	app_started_normally = TRUE;
	return noErr;
}

pascal OSErr handle_open_doc(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	FSSpec myFSS;
	AEDescList docList;
	OSErr myErr, ignoreErr;
	long index, itemsInList;
	Size actualSize;
	AEKeyword keywd;
	DescType returnedType;

	myErr = AEGetParamDesc(theAppleEvent,keyDirectObject, typeAEList, &docList);
	if (myErr == noErr) {
		myErr = AECountItems(&docList,&itemsInList);
		if (myErr == noErr) {	
			myErr = AEGetNthPtr(&docList,1,typeFSS, 
						&keywd,&returnedType,&myFSS,
						sizeof(myFSS),&actualSize);
			if (myErr == noErr) {
				do_apple_event_open(myFSS);
				if ((in_startup_mode == FALSE) && (startup_loaded == TRUE)) 
					end_startup();
				if (in_startup_mode == FALSE) {
					post_load();
					}
					else update_pc_graphics();

				}
			}
		}
	
	
	return noErr;
}

pascal OSErr handle_quit(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
{
	short choice;
	
	if ((overall_mode > 40) || (in_startup_mode == TRUE)) {
		All_Done = TRUE;
		return noErr;
		}

	if (overall_mode < 2) {
		choice = FCD(1066,0);
		if (choice == 3)
			return userCanceledErr;
		if (choice == 1)
			save_file(0);
		}
		else {
				choice = FCD(1067,0);
				if (choice == 1)
					return userCanceledErr;		
			}
			
	All_Done = TRUE;
	return noErr;
}

void set_up_apple_events()
{
	OSErr myErr;
	AEEventHandlerUPP event_UPP1,event_UPP2,event_UPP3;

#ifndef EXILE_BIG_GUNS
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenApplication,
		(AEEventHandlerProcPtr) handle_open_app, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,
		(AEEventHandlerProcPtr) handle_open_doc, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
		(AEEventHandlerProcPtr) handle_quit, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);
#endif		
#ifdef EXILE_BIG_GUNS
	event_UPP1 = NewAEEventHandlerProc(handle_open_app);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenApplication,
		event_UPP1, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);

	event_UPP2 = NewAEEventHandlerProc(handle_open_doc);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,
		event_UPP2, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);

	event_UPP3 = NewAEEventHandlerProc(handle_quit);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
		event_UPP3, 0, FALSE);
			
	if (myErr != noErr)
		SysBeep(2);
#endif		

}

pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit)
{	
	char chr,chr2;
	short the_type,wind_hit,item_hit;
	Handle the_handle = NULL;
	Rect the_rect,button_rect;
	Point the_point;
	CWindowPeek w;
	
	dummy_item_hit = 0;
	
	switch (event->what) {
		case updateEvt:
		w = (CWindowPeek) hDlg;
		if (EmptyRgn(w->updateRgn) == TRUE) {
			return TRUE;
			}
		BeginUpdate(hDlg);
		cd_redraw(hDlg);
		EndUpdate(hDlg);
		DrawDialog(hDlg);
		return FALSE;
		break;
		
		case keyDown:
		chr = event->message & charCodeMask;
		chr2 = (char) ((event->message & keyCodeMask) >> 8);
		switch (chr2) {
			case 126: chr = 22; break;
			case 124: chr = 21; break;
			case 123: chr = 20; break;
			case 125: chr = 23; break;
			case 53: chr = 24; break;
			case 36: chr = 31; break;
			case 76: chr = 31; break;
			}
					// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
					// 25-30  ctrl 1-6  31 - return

		wind_hit = cd_process_keystroke(hDlg,chr,&item_hit);
		break;
	
		case mouseDown:
		the_point = event->where;
		GlobalToLocal(&the_point);	
		wind_hit = cd_process_click(hDlg,the_point, event->modifiers,&item_hit);
		break;

		default: wind_hit = -1; break;
		}
	switch (wind_hit) {
		case -1: break;
		case 823: give_password_filter(item_hit); break;
		case 869: pick_prefab_scen_event_filter(item_hit); break;
		case 947: pick_a_scen_event_filter(item_hit); break;
		case 958: tip_of_day_event_filter(item_hit); break;
		case 960: talk_notes_event_filter(item_hit); break;
		case 961: adventure_notes_event_filter(item_hit); break;
		case 962: journal_event_filter(item_hit); break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
		case 987: display_item_event_filter(item_hit); break;
		case 988: pick_trapped_monst_event_filter(item_hit); break;
		case 989: edit_party_event_filter(item_hit); break;
		case 991: display_pc_event_filter(item_hit); break;
		case 996: display_alchemy_event_filter(item_hit); break;
		case 997: display_help_event_filter(item_hit); break;
		case 998: display_pc_item_event_filter(item_hit); break;
		case 999: display_monst_event_filter(item_hit); break;
		case 1010: spend_xp_event_filter (item_hit); break;
		case 1012: get_num_of_items_event_filter (item_hit); break;
		case 1013: pick_race_abil_event_filter (item_hit); break;
		case 1014: sign_event_filter (item_hit); break;
		case 1017: case 873: get_text_response_event_filter (item_hit); break;
		case 1018: select_pc_event_filter (item_hit); break;
		case 1019: give_pc_info_event_filter(item_hit); break;
		case 1047: alch_choice_event_filter(item_hit); break;
		case 1050: pc_graphic_event_filter(item_hit); break;
		case 1051: pc_name_event_filter(item_hit); break;
		case 1073: give_reg_info_event_filter (item_hit); break;
		case 1075: do_registration_event_filter (item_hit); break;
		case 1096: display_spells_event_filter (item_hit); break;
		case 1097: display_skills_event_filter (item_hit); break;
		case 1098: pick_spell_event_filter (item_hit); break;
		case 1099: prefs_event_filter (item_hit); break;
		default: fancy_choice_dialog_event_filter (item_hit); break;
		}

	if (wind_hit == -1)
		return FALSE;
		else return TRUE;
}



