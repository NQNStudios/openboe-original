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
#include <TextEdit.h>
#include <Dialogs.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <stdio.h>
#include <QDOffscreen.h>
#include <Palettes.h>

#include "Global.h"
#include "tGraphics.h"
#include "tactions.h"
#include "tfileio.h"
#include "dlogtool.h"
#include "buttonmg.h"
#include "sound.h"
#include "townout.h"
#include "scenario.h"
#include "keydlgs.h"


/* Globals */
Rect	windRect, Drag_Rect;
Boolean Multifinder_Present, All_Done = FALSE,dialog_not_toast = FALSE,play_sounds = TRUE;
EventRecord	event;
WindowPtr	mainPtr;	
town_record_type town;
big_tr_type t_d;
Boolean diff_depth_ok = FALSE,mouse_button_held = FALSE,editing_town = FALSE;
short cur_viewing_mode = 0;
short town_type = 0;  // 0 - big 1 - ave 2 - small
short max_dim[3] = {64,48,32};
short cen_x, cen_y;
short overall_mode = 61;
Handle menu_bar_handle;
ControlHandle right_sbar;
short mode_count = 0;
short dungeon_font_num,geneva_font_num;
MenuHandle apple_menu;
outdoor_record_type current_terrain;
talking_record_type talking;
short given_password;
short user_given_password = -1;
short pixel_depth,old_depth = 8;

unsigned char border1 = 90, border2 = 90; // kludgy thing ... leave right here, before borders
unsigned char borders[4][50];

Boolean change_made = FALSE;

// Numbers of current areas being edited
short cur_town;
location cur_out;

/* Prototypes */
void main(void);
void Initialize(void);
void Set_Window_Drag_Bdry();
void Handle_One_Event();
void Handle_Activate();
void Handle_Update();
void handle_menu_choice(long choice);
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_scenario_menu(int item_hit);
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);
void handle_help_menu(int item_hit);
pascal void right_sbar_action(ControlHandle bar, short part);
void Mouse_Pressed();
void close_program();
 void ding();
 pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
void set_pixel_depth();
void restore_depth();
void find_quickdraw() ;


scenario_data_type scenario;
piles_of_stuff_dumping_type *data_store;
Rect right_sbar_rect;

// 
//	Main body of program Exileedit
//

//MW specified argument and return type.
void main(void)
{

	short j,k;
	long i;
	size_t size;
	outdoor_record_type dummy_outdoor, *store2;

	MaxApplZone();
	MoreMasters();
	MoreMasters();
	MoreMasters();
	MoreMasters();

	data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));	
	init_current_terrain();
	//create_file();
	//ExitToShell();
	cd_init_dialogs();

	cen_x = 18; cen_y = 18;
	
	Initialize();
	load_sounds();
	init_directories();
	run_startup_g();
	init_lb();
	init_rb();
	init_town(1);
	init_out();
	init_scenario();
	
	make_cursor_sword();
	
	Set_Window_Drag_Bdry();
	
	Set_up_win();
	init_screen_locs();

	//create_basic_scenario();

	menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NIL) {
		SysBeep(50); SysBeep(50); SysBeep(50);
		ExitToShell();
		}
	SetMenuBar(menu_bar_handle);
	DisposHandle(menu_bar_handle);

	apple_menu = GetMHandle(500);
	/*file_menu = GetMHandle(550);
	options_menu = GetMHandle(600);
	create_menu = GetMHandle(650);
	items_menu = GetMHandle(700);*/
	AddResMenu(apple_menu, 'DRVR');
	shut_down_menus(0);
	DrawMenuBar();
		
	//update_item_menu();
	
//	to_create = get_town_to_edit();

//	load_terrain(to_create);

//	modify_lists();
	set_up_start_screen();
	
	Multifinder_Present = (NGetTrapAddress(_WaitNextEvent, ToolTrap) != 
		NGetTrapAddress(_Unimplemented, ToolTrap));

	TextFace(bold);
	
	redraw_screen();
	
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

	Str255 tit = "  ";
	OSErr		error;
	SysEnvRec	theWorld;
		
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
	set_pixel_depth();

	//InsetRect(&windRect, 5, 34);
	InsetRect(&windRect,(windRect.right - 584) / 2,(windRect.bottom - 420) / 2);
	OffsetRect(&windRect,0,18);
	mainPtr = NewCWindow(nil, &windRect, "\pBlades of Exile Scenario Editor", true, documentProc, 
						(WindowPtr) -1, false, 0);
	windRect = mainPtr->portRect;	
	SetPort(mainPtr);						/* set window to current graf port */
	right_sbar_rect.top = RIGHT_AREA_UL_Y;
	right_sbar_rect.left = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1 - 16;
	right_sbar_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;
	right_sbar_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1;
	right_sbar = NewControl(mainPtr,&right_sbar_rect,tit,TRUE,0,0,0,scrollBarProc,1);
	 
}

void Set_Window_Drag_Bdry()
{
	Drag_Rect = qd.screenBits.bounds;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

void Handle_One_Event()
{
	short chr,chr2;
	long menu_choice;

	if (Multifinder_Present == TRUE) {
		WaitNextEvent(everyEvent, &event, SLEEP_TICKS, MOUSE_REGION);
		if ((mouse_button_held == TRUE) && (event.what != 23) && (FrontWindow() == mainPtr)) {
			GlobalToLocal(&event.where);
			handle_action(event.where,event);
			}
		}
		else
		{
			SystemTask();
			GetNextEvent( everyEvent, &event);

			// I don't know why a ding here makes the program work but the ding doesn't play,
			// but, well ... it works.
			//ding();
		}
		
	switch (event.what)
	{
		case keyDown: case autoKey:
				chr = event.message & charCodeMask;
				chr2 = (char) ((event.message & keyCodeMask) >> 8);
				if ((event.modifiers & cmdKey) != 0) {
					if (event.what != autoKey) {
						BringToFront(mainPtr);
						SetPort(mainPtr);						
						menu_choice = MenuKey(chr);
						handle_menu_choice(menu_choice);
						}
					}
					else if (chr == 'Q')
						All_Done = TRUE;
						else handle_keystroke(chr,chr2,event);
			break;
		
		case mouseDown:
			Mouse_Pressed();
			break;
		
		
		case mouseUp:
			mouse_button_held = FALSE;
			break;
			
		case activateEvt:
			Handle_Activate();
			break;
		
		case updateEvt:
			Handle_Update();
			break;
		} 
}


void Handle_Activate()
{
	restore_cursor();
}

void Handle_Update()
{
	WindowPtr the_window;
	GrafPtr		old_port;
	
	the_window = (WindowPtr) event.message;
	
	GetPort (&old_port);
	SetPort (the_window);
	
	BeginUpdate(the_window);
	
	redraw_screen();
	EndUpdate(the_window);
	
	SetPort(old_port);
	restore_cursor();
}

void handle_menu_choice(long choice)
{
	int menu,menu_item;

	if (choice != 0) {
		menu = HiWord(choice);
		menu_item = LoWord(choice);

		set_cursor(0);
		switch (menu) {
			case 500:
				handle_apple_menu(menu_item);
				break;
			case 550:
				handle_file_menu(menu_item);
				break;
			case 600: 
				handle_scenario_menu(menu_item);
				break;
			case 650:
				handle_town_menu(menu_item);
				break;
			case 651:
				handle_outdoor_menu(menu_item);
				break;
			case 675:
				handle_help_menu(menu_item);
				break;
			case 700: case 701: case 702: case 703: case 704:
				handle_item_menu(menu_item + 80 * (menu - 700) - 1);
				break;
			case 750: case 751: case 752: case 753: 
				handle_monst_menu(menu_item + 64 * (menu - 750) - 1);
				break;

			}
		} 
	HiliteMenu(0);
}

void handle_apple_menu(int item_hit)
{
	Str255 desk_acc_name;
	short desk_acc_num;
	
	switch (item_hit) {
		case 1:
			fancy_choice_dialog(1062,0);
			break;
		default:
			GetItem (apple_menu,item_hit,desk_acc_name);
			desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
		}
}


void handle_file_menu(int item_hit)
{
	short create;
	
	switch (item_hit) {
		case 1: // open
			load_scenario();
			if (overall_mode == 60) {
				update_item_menu();
				set_up_main_screen();
				}
			break;
		case 2: // save
			modify_lists();
			save_scenario();
			break;
		case 3: // new scen
			build_scenario();
			if (overall_mode == 60)
				set_up_main_screen();
			break;

		case 5: // quit
			if (save_check(869) == FALSE)
				break;
			ExitToShell();
			break;
		}
}

void handle_scenario_menu(int item_hit)
{
	short i;
	
	switch (item_hit) {
		case 1:
			if (change_made == TRUE) {
				give_error("You need to save the changes made to your scenario before you can add a new town.",
					"",0);
				return;
				}
			if (scenario.num_towns >= 200) {
				give_error("You have reached the limit of 200 towns you can have in one scenario.",
					"",0);
				return;
				}
			if (new_town(scenario.num_towns) == TRUE)
				set_up_main_screen();
			break;
		case 3:
			edit_scen_details();
			break;
		case 4:
			edit_scen_intro();
			break;
		case 5:
			set_starting_loc();
			break;
		case 6:
			if (check_p(user_given_password) == TRUE) 
				given_password = get_password(); break;
		case 9:  SetControlValue(right_sbar,0); start_special_editing(0,0); break;
		case 10:
			 SetControlValue(right_sbar,0); start_string_editing(0,0);
			break;
		case 11:
			if (change_made == TRUE) {
				give_error("You need to save the changes made to your scenario before you can add a new town.",
					"",0);
				return;
				}
			i = pick_import_town(841,0);
			if (i >= 0) {
				import_town(i);
				change_made = TRUE;
				redraw_screen();
				}
			break;
		case 12:
			edit_save_rects();
			break;
		case 13:
			edit_horses();
			break;
		case 14:
			edit_boats();
			break;
		case 15:
			edit_add_town();
			break;
		case 16:
			edit_scenario_events();
			break;
		case 17:
			edit_item_placement();
			break;
		case 18:
			if (change_made == TRUE) {
				give_error("You need to save the changes made to your scenario before you can delete a town.",
					"",0);
				return;
				}
			if (scenario.num_towns == 1) {
				give_error("You can't delete the last town in a scenario. All scenarios must have at least 1 town.",
					"",0);
				return;
				}
			if (scenario.num_towns - 1 == cur_town) {
				give_error("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.",
					"",0);
				return;
				}
			if (scenario.num_towns - 1 == scenario.which_town_start) {
				give_error("You can't delete the last town in a scenario while it's the town the party starts the scenario in. Change the parties starting point and try this again.",
					"",0);
				return;
				}
			if (fancy_choice_dialog(865,0) == 1)
				delete_last_town();
			break;
		case 19:
			if (fancy_choice_dialog(866,0) == 1)
				start_data_dump();
		case 20:
			if (change_made == TRUE) {
				give_error("You need to save the changes made to your scenario before you can delete a town.",
					"",0);
				return;
				}
			if (fancy_choice_dialog(871,0) == 1)
				scen_text_dump();
			redraw_screen();
			break;
		}
	if ((item_hit != 18) && (item_hit != 19))
		change_made = TRUE;
}

void handle_town_menu(int item_hit)
{
	short i;
	
				change_made = TRUE;
	switch (item_hit) {
		case 1: edit_town_details(); break;
		case 2: edit_town_wand(); break;
		case 3: 							overall_mode = 9;
			mode_count = 2;
			set_cursor(5);
			set_string("Set town boundary","Select upper left corner");
			break;
		case 4: frill_up_terrain(); break;
		case 5: unfrill_terrain(); break;
		case 6: edit_town_strs(); break;
		case 8: if (fancy_choice_dialog(863,0) == 2)
					break;
				place_items_in_town();
				 break; // add random
		case 9: for (i = 0; i < 64; i++)
					town.preset_items[i].property = 0;
				fancy_choice_dialog(861,0);
				draw_terrain();
				break; // set not prop
		case 10: if (fancy_choice_dialog(862,0) == 2)
					break;
				for (i = 0; i < 64; i++)
					town.preset_items[i].item_code = -1;
				draw_terrain();
				break; // clear all items
		case 13:  SetControlValue(right_sbar,0); start_special_editing(2,0); break;
		case 14:  SetControlValue(right_sbar,0); start_string_editing(2,0); break;
		case 15: edit_advanced_town(); break;
		case 16: edit_town_events(); break;
		}
}
void handle_outdoor_menu(int item_hit)
{
	short i;
	
				change_made = TRUE;
	switch (item_hit) {
		case 1: outdoor_details(); break;
		case 2: edit_out_wand(0); break;
		case 3: edit_out_wand(1); break;
		case 4: frill_up_terrain(); break;
		case 5: unfrill_terrain(); break;
		case 6: edit_out_strs(); break;
		case 8: overall_mode = 47;
			set_string("Select party starting location.",""); break;
		case 11: SetControlValue(right_sbar,0); start_special_editing(1,0); break;
		case 12: SetControlValue(right_sbar,0);  start_string_editing(1,0); break;
		}
}

void handle_help_menu(int item_hit)
{
	short i;
	
	switch (item_hit) {
		case 1: fancy_choice_dialog(986,0); break; // started
		case 2: fancy_choice_dialog(1000,0); break; // testing
		case 3: fancy_choice_dialog(1001,0); break; // distributing
		case 5: fancy_choice_dialog(1002,0); break; // contest
		}
}

void handle_item_menu(int item_hit)
{
	if (data_store->scen_item_list.scen_items[item_hit].variety == 0) {
		give_error("This item has its Variety set to No Item. You can only place items with a Variety set to an actual item type.","",0);
		return;
		}
			overall_mode = 4;
			set_string("Place the item.","Select item location");
			mode_count = item_hit;
}

void handle_monst_menu(int item_hit)
{
			overall_mode = 28;
			set_string("Place the monster.","Select monster location");
			mode_count = item_hit;
}



pascal void right_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting,max,i;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case inUpButton: new_setting--; break;
		case inDownButton: new_setting++; break;
		case inPageUp: new_setting -= NRSONPAGE - 1; break;
		case inPageDown: new_setting += NRSONPAGE - 1; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		draw_rb();
}

void Mouse_Pressed()
{
	WindowPtr	the_window;
	short	the_part,content_part,i;
	long menu_choice;
	ControlHandle control_hit;
	
	the_part = FindWindow( event.where, &the_window);
	
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
			All_Done = TRUE;
			break;
		
		case inContent:
						SetPort(mainPtr);
						GlobalToLocal(&event.where);
						content_part = FindControl(event.where,the_window,&control_hit); // hit sbar?
						if (content_part != 0) {
							switch (content_part) {
								case inThumb:
									content_part = TrackControl(control_hit,event.where,NIL);
									if (control_hit == right_sbar)
										if (content_part == inThumb) {
											draw_rb();
											}
														
									break;
								case inUpButton: case inPageUp: case inDownButton: case inPageDown:
									if (control_hit == right_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)right_sbar_action);
									break;

								}
								
							} // a control hit
					 		else  // ordinary click
								All_Done = handle_action(event.where,event);
			break;
	}
}

void close_program()
{
	restore_depth();
}

 void ding()
 {
 	SysBeep(1);
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
		//case 958: _event_filter(item_hit); break;
		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
		case 800: edit_make_scen_1_event_filter(item_hit); break;
		case 801: edit_make_scen_2_event_filter(item_hit); break;
		case 802: user_password_filter(item_hit); break;
		case 803: edit_scen_details_event_filter(item_hit); break;
		case 804: edit_scen_intro_event_filter(item_hit); break;
		case 805: set_starting_loc_filter(item_hit); break;
		case 806: edit_spec_item_event_filter(item_hit); break;
		case 807: edit_save_rects_event_filter(item_hit); break;
		case 808: edit_horses_event_filter(item_hit); break;
		case 809: edit_boats_event_filter(item_hit); break;
		case 810: edit_add_town_event_filter(item_hit); break;
		case 811: edit_scenario_events_event_filter(item_hit); break;
		case 812: edit_item_placement_event_filter(item_hit); break;
		case 813: edit_ter_type_event_filter(item_hit); break;
		case 814: edit_monst_type_event_filter(item_hit); break;
		case 815: edit_monst_abil_event_filter(item_hit); break;
		case 816: edit_text_event_filter(item_hit); break;
		case 817: edit_talk_node_event_filter(item_hit); break;
		case 818: edit_item_type_event_filter(item_hit); break;
		case 819: choose_graphic_event_filter(item_hit); break;
		case 820: choose_text_res_event_filter(item_hit); break;
		case 821: edit_basic_dlog_event_filter(item_hit); break;
		case 822: edit_spec_enc_event_filter(item_hit); break;
		case 823: give_password_filter(item_hit); break;
		case 824: edit_item_abil_event_filter(item_hit); break;
		case 825: edit_special_num_event_filter(item_hit); break;
		case 826: edit_spec_text_event_filter(item_hit); break;
		case 830: new_town_event_filter(item_hit); break;
		case 831: edit_sign_event_filter(item_hit); break;
		case 832: edit_town_details_event_filter(item_hit); break;
		case 833: edit_town_events_event_filter(item_hit); break;
		case 834: edit_advanced_town_event_filter(item_hit); break;
		case 835: edit_town_wand_event_filter(item_hit); break;
		case 836: edit_placed_item_event_filter(item_hit); break;
		case 837: edit_placed_monst_event_filter(item_hit); break;
		case 838: edit_placed_monst_adv_event_filter(item_hit); break;
		case 839: edit_town_strs_event_filter(item_hit); break;
		case 840: edit_area_rect_event_filter(item_hit); break;
		case 841: pick_import_town_event_filter(item_hit); break;
		case 842: edit_dialog_text_event_filter(item_hit); break;
		case 850: edit_out_strs_event_filter(item_hit); break;
		case 851: outdoor_details_event_filter(item_hit); break;
		case 852: edit_out_wand_event_filter(item_hit); break;
		case 854: pick_out_event_filter(item_hit); break;
		case 855: case 856: pick_town_num_event_filter(item_hit); break;
		case 857: change_ter_event_filter(item_hit); break;
		default: fancy_choice_dialog_event_filter (item_hit); break;

		}

	if (wind_hit == -1)
		return FALSE;
		else return TRUE;
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
	
	if ((pixel_depth <= 8) && (diff_depth_ok == TRUE))
		return;
	
	if (pixel_depth != 8) {
		choice = choice_dialog(0,1071);
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