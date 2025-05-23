#include <Memory.h>
#include <Quickdraw.h>
#include <QuickdrawText.h>
#include <Fonts.h>
#include <Events.h>
#include <Menus.h>
#include <Windows.h>
#include <Dialogs.h>
#include <Resources.h>
#include <OSUtils.h>
#include <TextUtils.h>
#include <QDOffscreen.h>

#include "stdio.h"
#include "string.h"
#include "global.h"
#include "tGraphics.h"
#include "math.h"
#include "dlogtool.h"
#include "keydlgs.h"
#include "sound.h"

void Draw_Some_Item ();
void load_main_screen();
Rect get_template_rect ();
void load_terrain_template();
GWorldPtr load_pict();
short terrain_in_index();
void put_terrain_in_template();
void place_location();
//void draw_cur_string();
void undo_clip();

short find_index_spot();
Boolean is_s_d();
void sort_specials();

extern outdoor_record_type current_terrain;
extern WindowPtr	mainPtr;
extern town_record_type current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern town_record_type town;
extern big_tr_type t_d;
extern template_town_type town_template;
extern short town_type;  // 0 - big 1 - ave 2 - small
extern short cur_viewing_mode,overall_mode;
extern short available_dlog_buttons[NUM_DLOG_B];
extern Boolean editing_town;
extern short max_dim[3];
extern short dungeon_font_num,geneva_font_num;
extern Rect windRect;
extern piles_of_stuff_dumping_type *data_store;
extern scenario_data_type scenario;
extern Rect world_screen;

extern Rect left_button[NLS];
extern Rect right_buttons[NRSONPAGE];
extern Rect right_scrollbar_rect;
extern Rect right_area_rect;
extern unsigned char borders[4][50];
extern ControlHandle right_sbar;

extern Boolean left_buttons_active,right_buttons_active;
extern short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, +10 - button
extern short right_button_status[NRS]; 
extern unsigned char m_pic_index[200];
extern char *button_strs[140];
extern location cur_out;

short num_ir[3] = {12,10,4};

GWorldPtr monst_gworld[10],terrain_gworld[6],small_terrain_gworld,editor_mixed,terrain_buttons_gworld;
GWorldPtr anim_gworld,field_gworld,dialog_gworld,items_gworld,tiny_obj_gworld,small_ter_gworld;
GWorldPtr spec_scen_g = NULL,dlg_buttons_gworld[NUM_BUTTONS][2],ter_draw_gworld,dlogpics_gworld,talkfaces_gworld,mixed_gworld;
PixPatHandle	bg[14];
PixPatHandle map_pat[25];
	short small_icons[24] = {0,23,37,38,39,35,33,34,30,0,
							30,26,0,0,36,0,27,28,29,20,
							21,22,0,0};


// begin new stuff
Rect blue_button_from = {112,91,126,107};						
Rect start_button_from = {112,70,119,91};
Rect base_small_button_from = {112,0,119,7};
extern Rect palette_buttons[8][6];
Rect palette_button_base = {0,0,18,25};
Rect terrain_buttons_rect = {0,0,410,294};
extern Rect left_buttons[NLS][2]; // 0 - whole, 1 - blue button
Rect left_button_base = {5,5,21,200};
Rect right_button_base = {RIGHT_AREA_UL_Y,RIGHT_AREA_UL_X,17,RIGHT_AREA_UL_Y};
Rect terrain_rect = {0,0,340,272};
Str255 current_string = "\p";
Str255 current_string2 = "\p";
extern Rect terrain_rects[256];

short map_pats[220] = {50,50,1,1,1,6,6,6,6,6,
				6,6,6,6,6,6,6,6,2,2,
				2,2,2,2,2,2,2,2,2,2,
				2,2,4,4,4,4,4,4,4,4,
				4,4,4,4,4,4,3,3,3,3,
				3,3,3,3,3,3,3,3,3,0, // 50
				0,0,0,0,0,0,0,23,23,23,
				15,15,50,50,0,0,0,0,0,7,
				7,8,8,14,14,9,9,9,5,5,
				0,0,0,0,0,0,0,0,0,0,
				18,18,0,0,0,0,0,0,0,0, // 100
				22,22,0,0,0,0,0,0,0,0,
				0,0,0,10,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,11,0,0, // 150
				0,0,0,12,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0, // 200
				0,0,0,0,0,0,0,0,0,0				
				};
				
				
unsigned char small_what_drawn[64][64];
extern Boolean small_any_drawn;

void Set_up_win ()
{
	short i,j;


	for (i = 0; i < 8; i++)
		for (j = 0; j < 6; j++) {
		palette_buttons[i][j] = palette_button_base;
		OffsetRect(&palette_buttons[i][j],i * 25, j * 17);
		if (i == 7)
			palette_buttons[i][j].right++;
		}
	for (i = 0; i < 8; i++)
		for (j = 2; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 8; i++)
		for (j = 3; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 8; i++)
		for (j = 4; j < 6; j++) 
			OffsetRect(&palette_buttons[i][j],0,3);
	
	for (i = 0; i < NLS; i++) {
		left_buttons[i][0] = left_button_base;
		OffsetRect(&left_buttons[i][0],0,i * 16);
		left_buttons[i][1] = left_buttons[i][0];
		left_buttons[i][1].top += 1;
		left_buttons[i][1].bottom -= 1;
		left_buttons[i][1].left += 0;
		left_buttons[i][1].right = left_buttons[i][1].left + 16;
		}		
	right_button_base.left = RIGHT_AREA_UL_X + 1;
	right_button_base.top = RIGHT_AREA_UL_Y + 1;
	right_button_base.bottom = right_button_base.top + 12;
	right_button_base.right = right_button_base.left + RIGHT_AREA_WIDTH - 20;
	for (i = 0; i < NRSONPAGE; i++) {
		right_buttons[i] = right_button_base;
		OffsetRect(&right_buttons[i],0,i * 12);
		}		
	load_main_screen();
}

void run_startup_g()
{
	Rect pict_rect = {0,0,480,640};
	PicHandle	pict_to_draw;

	SetPort(mainPtr);
	OffsetRect(&pict_rect,-25,-25);
	pict_to_draw = GetPicture(3002);
	DrawPicture(pict_to_draw, &pict_rect);
	ReleaseResource((Handle) pict_to_draw);	
	play_sound(95);

}

void load_main_screen()
{
	Str255 fn1 = "\pGeneva";
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";
	short i,j;	
	
	GetFNum(fn1,&geneva_font_num);
	if (geneva_font_num == 0)
		GetFNum(fn3,&geneva_font_num);
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);

	NewGWorld(&terrain_buttons_gworld, 0,&terrain_buttons_rect, NIL, NIL, 0);
	
	SetPort((GrafPtr) terrain_buttons_gworld);
	TextFont(geneva_font_num);
	TextSize(10);
	TextFace(bold);
	SetPort(mainPtr);
	NewGWorld(&ter_draw_gworld, 0,&terrain_rect, NIL, NIL, 0);
	world_screen = terrain_rect;
	OffsetRect(&world_screen,TER_RECT_UL_X,TER_RECT_UL_Y);
	for (i = 0; i < NUM_BUTTONS; i++)
		for (j = 0; j < 2; j++)
			dlg_buttons_gworld[i][j] = load_pict(2000 + (2 * i) + j);
	for (i = 0; i < 14; i++) 
	    bg[i] = GetPixPat (128 + i);
	for (i = 0; i < 25; i++) 
	    map_pat[i] = GetPixPat (200 + i);
	    
	for (i = 0; i < 10; i++)
		monst_gworld[i] = load_pict(1100 + i);	
	for (i = 0; i < 6; i++)
		terrain_gworld[i] = load_pict(800 + i);	
	small_terrain_gworld = load_pict(904);
	editor_mixed = load_pict(906);
	anim_gworld = load_pict(820);
	field_gworld = load_pict(821);
	talkfaces_gworld = load_pict(860);
	items_gworld = load_pict(901);
	tiny_obj_gworld = load_pict(900);
	dlogpics_gworld = load_pict(850);
	small_ter_gworld = load_pict(904);
	mixed_gworld = load_pict(903);

	//draw_main_screen();
}

void redraw_screen()
{
	GrafPtr		old_port;
	
	
	GetPort (&old_port);
	SetPort (mainPtr);
	FillCRect(&windRect,bg[12]);
	draw_main_screen();
	if (overall_mode < 60);
		draw_terrain();
	SetPort (old_port);
}

void draw_main_screen()
{
	short i;
	Rect draw_rect;
	

	// draw left buttons (always active)
	//for (i = 0; i < NLS; i++) {
	//	draw_lb_slot(i);
	//	}
	draw_lb();
		
	// draw right buttons (only when not editing terrain)
	if (overall_mode >= 60) {
		draw_rect.left = RIGHT_AREA_UL_X;
		draw_rect.top = RIGHT_AREA_UL_Y;
		draw_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 16;
		draw_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;
		
		FrameRect(&draw_rect);
		InsetRect(&draw_rect,1,1);
		FillCRect(&draw_rect,bg[6]);
		
		draw_rb();
		Draw1Control(right_sbar);
		}
		
	// draw terrain palette
	if ((overall_mode < 60) || (overall_mode == 62)) {
		//draw_rect = terrain_buttons_rect;
		//OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
		//rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		//	terrain_buttons_gworld,draw_rect,0,1);
		place_location();
		}
	
	
}

void draw_lb()
{
	Rect temp_rect;
	short i;
	
	temp_rect = windRect;
	temp_rect.right = RIGHT_AREA_UL_X - 2;
	FillCRect(&temp_rect,bg[12]);
	for (i = 0; i < NLS; i++)
		draw_lb_slot(i,0);
}

// mode 0 normal 1 click
void draw_lb_slot (short which,short mode) 
{
	Rect text_rect,from_rect;
 
 	FillCRect(&left_buttons[which][0],bg[12]);
	if (left_button_status[which] == 0)
		return;
	text_rect = left_buttons[which][0];
	if (left_button_status[which] >= 10) {
		text_rect.left += 18;
		from_rect = blue_button_from;
		if (mode > 0)
			OffsetRect(&from_rect,from_rect.right - from_rect.left,0);
		rect_draw_some_item(editor_mixed,from_rect,editor_mixed,left_buttons[which][1],0,1);
		}
	if (left_button_status[which] % 10 == 3) 
		text_rect.left += 16;
	if (left_button_status[which] % 10 == 2) {
		TextSize(14);
		TextFace(bold);
		}
		else OffsetRect(&text_rect,0,2);
	if (mode > 0)
		ForeColor(blueColor);
	char_win_draw_string(mainPtr,text_rect,(char *)data_store->strings_ls[which],0,12);
	ForeColor(blackColor);
	
	//char_win_draw_string(mainPtr,text_rect,"FIsh.",1,12);
	TextSize(10);
	TextFace(bold);
}

void draw_rb()
{
	short i,pos;
	
 	pos = GetControlValue(right_sbar);
	for (i = pos; i < pos + NRSONPAGE; i++)
		draw_rb_slot(i,0);
}

// mode 0 normal 1 pressed
void draw_rb_slot (short which,short mode) 
{
	Rect text_rect;
 	short pos;
 	
 	pos = GetControlValue(right_sbar);
	if ((which < pos) || (which >= pos + NRSONPAGE))
		return;
		
 	FillCRect(&right_buttons[which - pos],bg[6]);
	if (right_button_status[which] == 0)
		return;
	text_rect = right_buttons[which - pos];
	
	if (mode > 0)
		ForeColor(redColor);
	char_win_draw_string(mainPtr,text_rect,(char *)data_store->strings_rs[which],0,12);
	ForeColor(blackColor);
	TextSize(10);
	TextFace(bold);
}

void set_up_terrain_buttons()
{
	short i,pic,small_i;
	Rect ter_from,ter_to,ter_from_base = {0,0,36,28};
	Rect tiny_from,tiny_to; 
	
	Rect palette_from = {0,0,0,0},palette_to;
					
 	SetPort((GrafPtr) terrain_buttons_gworld);
	FillCRect(&terrain_buttons_rect,bg[6]);
	FrameRect(&terrain_buttons_rect);
 	
 	// first make terrain buttons
	for (i = 0; i < 256; i++) {
		ter_from = ter_from_base;
		pic = scenario.ter_types[i].picture;
		if (pic >= 1000) {
			ter_from = get_custom_rect(pic % 1000);
			rect_draw_some_item(spec_scen_g,
				ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
			}
			else if (pic < 400)	{
				pic = pic % 50;
				OffsetRect(&ter_from,28 * (pic % 10), 36 * (pic / 10));
				rect_draw_some_item(terrain_gworld[scenario.ter_types[i].picture/50],
					ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
				}
				else {
					pic = pic % 50;
					ter_from.left = 112 * (pic / 5);
					ter_from.right = ter_from.left + 28;
					ter_from.top = 36 * (pic % 5);
					ter_from.bottom = ter_from.top + 36;
					rect_draw_some_item(anim_gworld,
						ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
				
					}
		small_i = small_icons[scenario.ter_types[i].special];
		if ((small_i == 30) && (scenario.ter_types[i].flag2 >= 5))
			small_i = 31;
		if ((small_i == 31) && (scenario.ter_types[i].flag2 == 10))
			small_i = 32;
		if (i == 82)
			small_i = 3;
		if (i == 83)
			small_i = 2;
		if ((i == 7) || (i == 10) || (i == 13) || (i == 16))
			small_i = 23;
		tiny_from = base_small_button_from;
		OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
		tiny_to = terrain_rects[i];
		tiny_to.top = tiny_to.bottom - 7;
		tiny_to.left = tiny_to.right - 7;
		if (small_i > 0)
			rect_draw_some_item(editor_mixed,
				tiny_from,terrain_buttons_gworld,tiny_to,0,0);
		}
	
	if (overall_mode < 60) {
		palette_to.left = 5;
		palette_to.top = terrain_rects[255].bottom + 5;
		if (editing_town == TRUE) {
			palette_from.bottom = palette_buttons[0][5].bottom;
			palette_from.right = palette_buttons[7][5].right;
			}
			else {
				palette_from.bottom = palette_buttons[0][2].bottom;
				palette_from.right = palette_buttons[7][2].right;
				}
		palette_to.right = palette_to.left + palette_from.right;
		palette_to.bottom = palette_to.top + palette_from.bottom;
		rect_draw_some_item(editor_mixed,
			palette_from,terrain_buttons_gworld,palette_to,1,0);
		}

	SetPort(mainPtr);
}

GWorldPtr load_pict(int picture_to_get)
{
	PicHandle	current_pic_handle;
	Rect	pic_rect;
	short	pic_wd,pic_hgt;
	GWorldPtr	myGWorld;
	CGrafPtr	origPort;
	GDHandle	origDev;
	QDErr		check_error;
	PixMapHandle	offPMHandle;
	char good;
			
    current_pic_handle = GetPicture (picture_to_get);
	pic_rect = ( **( current_pic_handle) ).picFrame;
	pic_wd = pic_rect.right - pic_rect.left;
	pic_hgt = pic_rect.bottom - pic_rect.top;  
	GetGWorld (&origPort, &origDev);
	check_error = NewGWorld (&myGWorld, 0,
				&pic_rect,
				NULL, NULL, 0);
	if (check_error != noErr)  {
		SysBeep(50); 
		ExitToShell();}
	
	SetGWorld(myGWorld, NULL);
	
	offPMHandle = GetGWorldPixMap (myGWorld);
	good = LockPixels (offPMHandle);
	if (good = FALSE)  {
		SysBeep(50); }
	SetRect (&pic_rect, 0, 0, pic_wd, pic_hgt);
	DrawPicture (current_pic_handle, &pic_rect);
	SetGWorld (origPort, origDev);
	UnlockPixels (offPMHandle);
	ReleaseResource ((Handle) current_pic_handle);

	return myGWorld;
}

void draw_terrain()
{
	short q,r,x,y,i,small_i;
	location which_pt,where_draw;
	Rect draw_rect,clipping_rect = {8,8,332,260};	
	unsigned char t_to_draw;
	Rect source_rect,tiny_to,tiny_to_base = {37,29,44,36},tiny_from,from_rect,to_rect;
	Rect boat_rect[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};

	if (overall_mode >= 60)
		return;
	
	OffsetRect(&boat_rect[0],61,0);
		
	if (cur_viewing_mode == 0) {
	SetPort((GrafPtr) ter_draw_gworld);
 	FillCRect(&terrain_rect,bg[6]);
	FrameRect(&terrain_rect);
	SetPort(mainPtr);
	for (q = 0; q < 9; q++) 
		for (r = 0; r < 9; r++)
			{
			where_draw.x = q; where_draw.y = r;
			if (editing_town == TRUE) {
				t_to_draw = t_d.terrain[cen_x + q - 4][cen_y + r - 4];		
				}
				else {
					if (cen_x + q - 4 == -1) 
						t_to_draw = borders[3][cen_y + r - 4];
					else if (cen_x + q - 4 == 48) 
						t_to_draw = borders[1][cen_y + r - 4];
					else if (cen_y + r - 4 == -1) 
						t_to_draw = borders[0][cen_x + q - 4];
					else if (cen_y + r - 4 == 48) 
						t_to_draw = borders[2][cen_x + q - 4];
					else t_to_draw = current_terrain.terrain[cen_x + q - 4][cen_y + r - 4];
					}
			draw_one_terrain_spot(q,r,t_to_draw);
			which_pt.x = cen_x + q - 4;
			which_pt.y =cen_y + r - 4;	

			tiny_to = tiny_to_base;	
			OffsetRect(&tiny_to,28 * q, 36 * r);
			
			// draw start icon, if starting point
			if ((editing_town == TRUE) && 
				(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
				&& (scenario.where_start.y == cen_y + r - 4)) {
				from_rect = start_button_from;
				to_rect = tiny_to;
				to_rect.left -= 14;
				rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
				OffsetRect(&tiny_to,0,-7);
				}
			if ((editing_town == FALSE)
				&& (scenario.out_sec_start.x == cur_out.x)
				&& (scenario.out_sec_start.y == cur_out.y) 
				&& (scenario.out_start.x == cen_x + q - 4)
				&& (scenario.out_start.y == cen_y + r - 4)) {
				from_rect = start_button_from;
				to_rect = tiny_to;
				to_rect.left -= 14;
				rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
				OffsetRect(&tiny_to,0,-7);
				}
			small_i = small_icons[scenario.ter_types[t_to_draw].special];
			if ((small_i == 30) && (scenario.ter_types[t_to_draw].flag2 >= 5))
				small_i = 31;
			if ((small_i == 31) && (scenario.ter_types[t_to_draw].flag2 == 10))
				small_i = 32;
			tiny_from = base_small_button_from;
			OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
			if (small_i > 0) {
				rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
				OffsetRect(&tiny_to,0,-7);
				}
				
				if (is_special(cen_x + q - 4,cen_y + r - 4) == TRUE) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (7),7 * (0));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
					}	
				if ((t_to_draw == 7) || (t_to_draw == 10) || (t_to_draw == 13) || (t_to_draw == 16)) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (3),7 * (2));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
					}	
				//if (is_s_d(cen_x + q - 4,cen_y + r - 4) == TRUE) {
				//	}	
				if (editing_town == FALSE) {
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == current_terrain.wandering_locs[i].x) &&
							(cen_y + r - 4 == current_terrain.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
							}			
				
					}
					
				if (editing_town == TRUE) {
					for (i = 0; i < 30; i++) {
						if ((scenario.scen_boats[i].which_town == cur_town) &&
							(scenario.scen_boats[i].boat_loc.x == cen_x + q - 4) &&
							(scenario.scen_boats[i].boat_loc.y == cen_y + r - 4))
								Draw_Some_Item(mixed_gworld,boat_rect[0],ter_draw_gworld,where_draw,1,0);
							
						}	
					for (i = 0; i < 30; i++) {
						source_rect = boat_rect[0];
						OffsetRect(&source_rect,0,74);
						OffsetRect(&source_rect,56,36);
						if ((scenario.scen_horses[i].which_town == cur_town) &&
							(scenario.scen_horses[i].horse_loc.x == cen_x + q - 4) &&
							(scenario.scen_horses[i].horse_loc.y == cen_y + r - 4))
								Draw_Some_Item(mixed_gworld,source_rect,ter_draw_gworld,where_draw,1,0);
							
						}	
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town.start_locs[i].x) &&
							(cen_y + r - 4 == town.start_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (6 + i),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							}			
					for (i = 0; i < 4; i++)		
						if ((cen_x + q - 4 == town.wandering_locs[i].x) &&
							(cen_y + r - 4 == town.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
							}			
					if (is_web(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					if (is_crate(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					if (is_barrel(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(2,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}	
					for (i = 0; i < 8; i++) 
						if (is_sfx(cen_x + q - 4,cen_y + r - 4,i)) {
							from_rect = calc_rect(i,3);
							Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);					
							}
					for (x = 0; x < 64; x++)
						if ((cen_x + q - 4 == town.preset_items[x].item_loc.x) &&
						 (cen_y + r - 4 == town.preset_items[x].item_loc.y) && (town.preset_items[x].item_code >= 0)) {
						 }
					for (x = 0; x < 60; x++)
						if ((cen_x + q - 4 == t_d.creatures[x].start_loc.x) &&
						 (cen_y + r - 4 == t_d.creatures[x].start_loc.y) && (t_d.creatures[x].number != 0)) {
						 }
					 
					 }
				}
	if (editing_town == TRUE) {
		draw_monsts();
		draw_items();
		}
		
	SetPort((GrafPtr) ter_draw_gworld);
	ClipRect(&clipping_rect);
	
	if (editing_town == TRUE) {
		// draw info rects
		for (i = 0; i < 16; i++)
			if (t_d.room_rect[i].left > 0) {
				draw_rect.left = 22 + 28 * (t_d.room_rect[i].left - cen_x + 4);
				draw_rect.right = 22 + 28 * (t_d.room_rect[i].right - cen_x + 4);
				draw_rect.top = 24 + 36 * (t_d.room_rect[i].top - cen_y + 4);
				draw_rect.bottom = 24 + 36 * (t_d.room_rect[i].bottom - cen_y + 4);	
				ForeColor(redColor);
				FrameRect(&draw_rect);
				ForeColor(blackColor);
				}
		// draw border rect
			draw_rect.left = 21 + 28 * (town.in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town.in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town.in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town.in_town_rect.bottom - cen_y + 4);	
			ForeColor(whiteColor);
			FrameRect(&draw_rect);
			ForeColor(blackColor);
		}
	if (editing_town == FALSE) {
		// draw info rects
		for (i = 0; i < 8; i++)
			if (current_terrain.info_rect[i].left > 0) {
				draw_rect.left = 22 + 28 * (current_terrain.info_rect[i].left - cen_x + 4);
				draw_rect.right = 22 + 28 * (current_terrain.info_rect[i].right - cen_x + 4);
				draw_rect.top = 24 + 36 * (current_terrain.info_rect[i].top - cen_y + 4);
				draw_rect.bottom = 24 + 36 * (current_terrain.info_rect[i].bottom - cen_y + 4);	
				ForeColor(redColor);
				FrameRect(&draw_rect);
				ForeColor(blackColor);
				}
		}
	ClipRect(&terrain_rect);
	SetPort(mainPtr);

	small_any_drawn = FALSE;
	//if (cur_viewing_mode == 0) 
	//	draw_frames();
	}
	
	if (cur_viewing_mode == 1) {
		SetPort((GrafPtr) ter_draw_gworld);
		if (small_any_drawn == FALSE) {
		 	FillCRect(&terrain_rect,bg[6]);
			FrameRect(&terrain_rect);
			}
		for (q = 0; q < ((editing_town == TRUE) ? max_dim[town_type] : 48); q++) 
			for (r = 0; r < ((editing_town == TRUE) ? max_dim[town_type] : 48); r++) {
				t_to_draw = (editing_town == TRUE) ? t_d.terrain[q][r] :
					current_terrain.terrain[q][r];
				if ((small_what_drawn[q][r] != t_to_draw) || (small_any_drawn == FALSE)) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw);
					small_what_drawn[q][r] = t_to_draw;
					}
			}
		SetPort(mainPtr);
		small_any_drawn = TRUE;
		}
	
	//draw_cur_string();
	place_location();
	
	//to_rect = world_screen;
	//OffsetRect(&to_rect,TER_RECT_UL_X,TER_RECT_UL_Y);
	rect_draw_some_item(ter_draw_gworld,terrain_rect,ter_draw_gworld,world_screen,0,1);
}

void draw_monsts()
{
	short i,k,width,height,m_start_pic;
	GWorldPtr from_gworld;
	Rect source_rect,draw_rect;
	location where_draw,store_loc;
	GrafPtr cur_port;
	
		for (i = 0; i < 60; i++)
		if (t_d.creatures[i].number != 0) {
				where_draw.x = t_d.creatures[i].start_loc.x - cen_x + 4;
				where_draw.y = t_d.creatures[i].start_loc.y - cen_y + 4;
				width = scenario.scen_monsters[t_d.creatures[i].number].x_width;
				height = scenario.scen_monsters[t_d.creatures[i].number].y_width;
				
				for (k = 0; k < width * height; k++) {
					store_loc = where_draw;
					if ((where_draw.x == minmax(0,8,where_draw.x)) && 
					(where_draw.y == minmax(0,8,where_draw.y)) && 
						(scenario.scen_monsters[t_d.creatures[i].number].picture_num >= 1000)) {
						source_rect = get_custom_rect((scenario.scen_monsters[t_d.creatures[i].number].picture_num + k) % 1000);
						store_loc.x += k % width;
						store_loc.y += k / width;
						Draw_Some_Item(spec_scen_g, source_rect, ter_draw_gworld, store_loc, 1, 0); 
						}
						else if (scenario.scen_monsters[t_d.creatures[i].number].picture_num < 1000) {
							m_start_pic = m_pic_index[scenario.scen_monsters[t_d.creatures[i].number].picture_num] + k;
							from_gworld = monst_gworld[m_start_pic / 20];
							m_start_pic = m_start_pic % 20;
							source_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);				
							store_loc.x += k % width;
							store_loc.y += k / width;
							Draw_Some_Item(from_gworld, source_rect, ter_draw_gworld, store_loc, 1, 0); 
							}
					}
			}
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
Rect get_item_template_rect (short type_wanted)
{
	Rect store_rect;
	
	if (type_wanted < 45) {
		store_rect.top = (type_wanted / 5) * BITMAP_HEIGHT;
		store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
		store_rect.left = (type_wanted % 5) * BITMAP_WIDTH;
		store_rect.right = store_rect.left + BITMAP_WIDTH;
		}
		else {
			store_rect.top = (type_wanted / 10) * 18;
			store_rect.bottom = store_rect.top + 18;
			store_rect.left = (type_wanted % 10) * 18;
			store_rect.right = store_rect.left + 18;		
			}
	
	return store_rect;
}

void draw_items()
{
	short i;
	Rect source_rect,dest_rect;
	location where_draw;
	short pic_num;
	GrafPtr cur_port;
	
	for (i = 0; i < 64; i++) {
		if (town.preset_items[i].item_code >= 0) {
			where_draw.x = town.preset_items[i].item_loc.x - cen_x + 4;
			where_draw.y = town.preset_items[i].item_loc.y - cen_y + 4;
			pic_num = data_store->scen_item_list.scen_items[town.preset_items[i].item_code].graphic_num;
			if ((where_draw.x >= 0) && (where_draw.x <= 8) &&
				(where_draw.y >= 0) && (where_draw.y <= 8))  {
					
					if (pic_num >= 150) {
						source_rect = get_custom_rect(pic_num - 150);
						dest_rect = calc_rect(where_draw.x,where_draw.y);
						rect_draw_some_item(spec_scen_g,
							 source_rect, ter_draw_gworld, dest_rect, 1, 0); 
						}
						else {
							source_rect = get_item_template_rect(pic_num);
							dest_rect = calc_rect(where_draw.x,where_draw.y);
							OffsetRect(&dest_rect,8,8);
							if (pic_num >= 45) {
								dest_rect.top += 9;
								dest_rect.bottom -= 9;
								dest_rect.left += 5;
								dest_rect.right -= 5;
								}
							rect_draw_some_item((pic_num < 45) ? items_gworld : tiny_obj_gworld,
							 source_rect, ter_draw_gworld, dest_rect, 1, 0); 
							}
					}
			}
		}
}


void force_tiny_redraw()
{
//	short q,r;
//	for (q = 0; q < 8; q++) 
//		for (r = 0; r < 64; r++)
//			ter_changed[q][r] = 255;

}

void redraw_selected_ter()
{
//	draw_one_terrain_spot(18,8,(unsigned char) current_terrain_type);
}

void draw_one_terrain_spot (short i,short j,unsigned char terrain_to_draw)
{
	location where_draw;
	Rect source_rect;
	short picture_wanted;
	GWorldPtr source_gworld;
	
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
//	if (picture_wanted >= 1000) {
//		terrain_to_draw = 90;
//		picture_wanted = 74;
//		}
		
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	
	if ((picture_wanted >= 1000) && (spec_scen_g != NULL)) {
		source_gworld = spec_scen_g;
		source_rect = get_custom_rect(picture_wanted % 1000);
		}
	else if (picture_wanted >= 400)	{
			source_gworld = anim_gworld;
			picture_wanted -= 400;
			source_rect.left = 112 * (picture_wanted / 5);
			source_rect.right = source_rect.left + 28;
			source_rect.top = 36 * (picture_wanted % 5);
			source_rect.bottom = source_rect.top + 36;
		}
		else {
			source_rect = get_template_rect(terrain_to_draw);
			source_gworld = terrain_gworld[picture_wanted / 50];
		}
		
	Draw_Some_Item(source_gworld, source_rect, ter_draw_gworld, where_draw, 0, 0);
}

void draw_one_tiny_terrain_spot (short i,short j,unsigned char terrain_to_draw)
{

	location where_draw;
	Rect source_rect,dest_rect = {0,0,4,4},from_rect = {0,0,4,4};
	short picture_wanted,k;
	GWorldPtr source_gworld;
				
	picture_wanted = scenario.ter_types[terrain_to_draw].picture;
	
	where_draw.x = (char) i;
	where_draw.y = (char) j;
	OffsetRect(&dest_rect,8 + 4 * i,8 + 4 * j);
	switch (picture_wanted) {
	
	case 0: case 1: case 73: case 72:
		FillCRect(&dest_rect,map_pat[0]);
		break;
	case 2: case 3: case 4:
		FillCRect(&dest_rect,map_pat[1]);
		break;			

	default:
		if ((picture_wanted < 170) && (map_pats[picture_wanted] > 0)) {
			FillCRect(&dest_rect,map_pat[map_pats[picture_wanted]]);
			}
		else if (picture_wanted >= 1000)	{
			from_rect = get_custom_rect(picture_wanted % 1000);
			rect_draw_some_item(spec_scen_g, from_rect, ter_draw_gworld, dest_rect, 0, 0);
			}
		else if (picture_wanted >= 400)	{
				source_gworld = anim_gworld;
				picture_wanted -= 400;
				if (picture_wanted == 0) FillCRect(&dest_rect,map_pat[13]);
				else if (picture_wanted == 4) FillCRect(&dest_rect,map_pat[21]);
				else if (picture_wanted == 7) FillCRect(&dest_rect,map_pat[20]);
				else if (picture_wanted == 8) FillCRect(&dest_rect,map_pat[19]);
				else if (picture_wanted == 9) FillCRect(&dest_rect,map_pat[20]);
				else if (picture_wanted == 10) FillCRect(&dest_rect,map_pat[19]);
				else {
					//source_rect.left = 112 * (picture_wanted / 5);
					//source_rect.right = source_rect.left + 28;
					//source_rect.top = 36 * (picture_wanted % 5);
					//source_rect.bottom = source_rect.top + 36;
					OffsetRect(&from_rect,(picture_wanted / 5) * 24 + 1,(picture_wanted % 5) * 6 + 1 + 156);
					rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
					//rect_draw_some_item(source_gworld, source_rect, ter_draw_gworld, dest_rect, 0, 0);
					}
			}
			else { 
				//source_rect = get_template_rect(terrain_to_draw);
				//source_gworld = terrain_gworld[picture_wanted / 50];
				OffsetRect(&from_rect,(picture_wanted % 10) * 6 + 1,(picture_wanted / 10) * 6 + 1);
				rect_draw_some_item(small_ter_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
			}
		break;
		}
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
	object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item ( GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,location target,
char masked,short main_win)
{
Rect	destrec;
PixMapHandle	test1, test2;
RGBColor	test_color, store_color;
BitMap store_dest;

	if ((target.x < 0) || (target.x > 8) || (target.y < 0) || (target.y > 8))
		return;
	destrec.left = 8 + BITMAP_WIDTH * target.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * target.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;	
	
	destrec.left = destrec.right - (src_rect.right - src_rect.left);
	destrec.top = destrec.bottom - (src_rect.bottom - src_rect.top);

	rect_draw_some_item(src_gworld,src_rect,ter_draw_gworld,destrec,masked,0);

}

void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,
char masked,short main_win)

{
	Rect	destrec;
	PixMapHandle	test1, test2;
	BitMap store_dest;
	GrafPtr cur_port;
	RGBColor	store_color;

	if (main_win == 2) {
		GetBackColor(&store_color);
		BackColor(whiteColor);
		}
		
	GetPort(&cur_port);	
	store_dest = cur_port->portBits;	
	
	test1 = src_gworld->portPixMap;

	LockPixels(test1);
	if (main_win == 0) {
		test2 = targ_gworld->portPixMap; 
		LockPixels(test2);
		if (masked == 1) 
			CopyBits ( (BitMap *) *test1 ,
					(BitMap *) *test2 ,
					&src_rect, &targ_rect, 
					 transparent , NULL);	
			else CopyBits ( (BitMap *) *test1 ,
					(BitMap *) *test2 ,
					&src_rect, &targ_rect, 
				  (masked == 10) ? addOver : 0, NULL);
		UnlockPixels(test2);
		}  
		else {
		if (masked == 1) 
			CopyBits ( (BitMap *) *test1 ,
					&store_dest ,
					&src_rect, &targ_rect, 
					 transparent , NULL);
			else CopyBits ( (BitMap *) *test1 ,
					&store_dest ,
					&src_rect, &targ_rect, 
					  (masked == 10) ? addOver : 0, NULL);
			}
	UnlockPixels(test1);

	if (main_win == 2) 
		RGBBackColor(&store_color);
}


void discard_graphics()
{
}

/* Input terrain currently trying to draw. Get back Rect in terrain template containing 
desired pixmap, or Rect to darkness if desired map not present */
Rect get_template_rect (unsigned char type_wanted)
{
	Rect store_rect;
	short i,j;
	short picture_wanted;
	
	picture_wanted = scenario.ter_types[type_wanted].picture;
	if (picture_wanted >= 1000)
		picture_wanted = 0;
	picture_wanted = picture_wanted % 50;
	store_rect.top = 0 + (picture_wanted / 10) * BITMAP_HEIGHT;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (picture_wanted % 10) * BITMAP_WIDTH;
	store_rect.right = store_rect.left + BITMAP_WIDTH;
			
	return store_rect;
}


void draw_frames()
{
	char q,r,i,k;
	location which_pt;
	Rect draw_rect;
	
	for (q = 0; q < 9; q++) {
		for (r = 0; r < 9; r++)
			{
				which_pt.x = cen_x + q - 4;
				which_pt.y = cen_y + r - 4;
						draw_rect.top = 23 + r * 36;
						draw_rect.bottom = 58 + r * 36;
						draw_rect.left = 23 + q * 28;
						draw_rect.right = 50 + q * 28;
				for (i = 0; i < 4; i++)
					if ((which_pt.x == town.wandering_locs[i].x) &&
					(which_pt.y == town.wandering_locs[i].y)) {
						
						ForeColor(redColor);
						FrameRect(&draw_rect);
						ForeColor(blackColor);
					}
				for (i = 0; i < 4; i++)
					if ((which_pt.x == town.start_locs[i].x) &&
					(which_pt.y == town.start_locs[i].y)) {
						ForeColor(magentaColor);
						FrameRect(&draw_rect);
						ForeColor(blackColor);					
					}	

					
			}
		}

}


void place_location()
{
	Rect text_rect = {367,290,384,510};
	Str255 draw_str;
	Rect from_rect,draw_rect,source_rect,erase_rect;
	short picture_wanted;
	
	/*
	from_rect = terrain_buttons_rect;
	from_rect.top = from_rect.bottom - 150;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
		terrain_buttons_gworld,draw_rect,0,1);
	*/
	
	//EraseRect(&text_rect);
	SetPort((GrafPtr) terrain_buttons_gworld);

	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	FillCRect(&erase_rect,bg[6]);
	
	MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	if (overall_mode < 60)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
		else {
			MoveTo(5 ,terrain_rects[255].top + 28);		
			sprintf((char *) draw_str,"Click terrain to edit. ",cen_x,cen_y);
			}
	c2p(draw_str);
	DrawString(draw_str);
	//draw_cur_string();

	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].bottom + 117;
	erase_rect.bottom = RIGHT_AREA_HEIGHT + 6;
	FillCRect(&erase_rect,bg[6]);
	
	if (overall_mode < 60) {
		MoveTo(5,terrain_rects[255].bottom + 129);
		DrawString(current_string);
		MoveTo(RIGHT_AREA_WIDTH / 2,terrain_rects[255].bottom + 129);
		DrawString(current_string2);
		}
		
	SetPort(mainPtr);

	draw_rect.top = palette_buttons[0][0].top + terrain_rects[255].bottom + 5;
	draw_rect.left = palette_buttons[7][0].right + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	picture_wanted = scenario.ter_types[current_terrain_type].picture;
	
	if (overall_mode < 60) {
		if (picture_wanted >= 1000)	{
			source_rect = get_custom_rect(picture_wanted % 1000);
			rect_draw_some_item(spec_scen_g,
				source_rect,terrain_buttons_gworld,draw_rect,0,0);
			}
			else if (picture_wanted >= 400)	{
				picture_wanted -= 400;
				source_rect.left = 112 * (picture_wanted / 5);
				source_rect.right = source_rect.left + 28;
				source_rect.top = 36 * (picture_wanted % 5);
				source_rect.bottom = source_rect.top + 36;
				rect_draw_some_item(anim_gworld,source_rect,terrain_buttons_gworld,draw_rect,0,0);
				}
				else {
					source_rect = get_template_rect(current_terrain_type);
					rect_draw_some_item(terrain_gworld[picture_wanted / 50],source_rect,
						terrain_buttons_gworld,draw_rect,0,0);
					}	
		}

	draw_rect = terrain_buttons_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		terrain_buttons_gworld,draw_rect,0,1);
}

// klugde for speed ...exactly like place location above, but just writes location
void place_just_location()
{
	Rect text_rect = {367,290,384,510};
	Str255 draw_str;
	Rect from_rect,draw_rect,source_rect,erase_rect;
	short picture_wanted;
	

	SetPort((GrafPtr) terrain_buttons_gworld);

	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	FillCRect(&erase_rect,bg[6]);
	
	MoveTo(terrain_rects[255].left + 20 ,terrain_rects[255].top + 12);
	if (overall_mode < 60)
		sprintf((char *) draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
		else {
			MoveTo(5 ,terrain_rects[255].top + 28);		
			sprintf((char *) draw_str,"Click terrain to edit. ",cen_x,cen_y);
			}
	c2p(draw_str);
	DrawString(draw_str);

	SetPort(mainPtr);
	

	from_rect = terrain_buttons_rect;
	from_rect.top = erase_rect.top;
	from_rect.bottom = erase_rect.bottom;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
		terrain_buttons_gworld,draw_rect,0,1);
}

void set_string(char *string,char *string2)
{
	strcpy((char *)current_string,string);
	c2p(current_string);
//	if (strlen(string2) == 0)
//		current_string2[0] = 0;
//		else 
//	sprintf((char *)current_string2,"Bob");
	strcpy((char *)current_string2,string2);
	c2p(current_string2);

	place_location();
}

/*
void draw_cur_string()
{
	Rect from_rect,draw_rect;

	from_rect = terrain_buttons_rect;
	from_rect.top = from_rect.bottom - 40;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
		terrain_buttons_gworld,draw_rect,0,1);
	MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 120);
	DrawString(current_string);
	MoveTo(RIGHT_AREA_UL_X + 5,terrain_rects[255].bottom + 132);
	DrawString(current_string2);
} */

void undo_clip()
{
	Rect c = {0,0,480,640};
	
	ClipRect(&c);
}


Boolean is_special(short i,short j)
{
	short k;
	
	if (editing_town == TRUE)
		for (k = 0; k < 50; k++)
			if ((town.special_locs[k].x == i) && (town.special_locs[k].y == j))
				return TRUE;
	if (editing_town == FALSE)
		for (k = 0; k < 18; k++)
			if ((current_terrain.special_locs[k].x == i) && (current_terrain.special_locs[k].y == j))
				return TRUE;

	return FALSE;
}

void take_special(short i,short j)
{}

void make_special(short i,short j)
{}


void sort_specials()
{}

Boolean is_field_type(short i,short j,short field_type)
{
	short k;
	
	for (k = 0; k < 50; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j))
				return TRUE;
	return FALSE;
}

void make_field_type(short i,short j,short field_type)
{
	short k;
	
	if (is_field_type(i,j,field_type) == TRUE)
		return;
	for (k = 0; k < 50; k++)
		if (town.preset_fields[k].field_type == 0) {
			town.preset_fields[k].field_loc.x = i;
			town.preset_fields[k].field_loc.y = j;
			town.preset_fields[k].field_type = field_type;
			return;
			}
	give_error("Each town can have at most 50 fields and special effects (webs, barrels, blood stains, etc.). To place more, use the eraser first.","",0);
}


void take_field_type(short i,short j,short field_type)
{
	short k;
	
	for (k = 0; k < 50; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j)) {
				town.preset_fields[k].field_type = 0;
				return;
				}
}

Boolean is_web(short i,short j)
{
	return is_field_type(i,j,3);

}
void make_web(short i,short j)
{
	make_field_type(i,j,3);
}
void take_web(short i,short j)
{
	take_field_type(i,j,3);
}

Boolean is_crate(short i,short j)
{
	return is_field_type(i,j,4);
}
void make_crate(short i,short j)
{
	make_field_type(i,j,4);

}
void take_crate(short i,short j)
{
	take_field_type(i,j,4);
}

Boolean is_barrel(short i,short j)
{
	return is_field_type(i,j,5);
}
void make_barrel(short i,short j)
{
	make_field_type(i,j,5);
}
void take_barrel(short i,short j)
{
	take_field_type(i,j,5);
}

Boolean is_fire_barrier(short i,short j)
{
	return is_field_type(i,j,6);
}
void make_fire_barrier(short i,short j)
{
	make_field_type(i,j,6);
}
void take_fire_barrier(short i,short j)
{
	take_field_type(i,j,6);
}

Boolean is_force_barrier(short i,short j)
{
	return is_field_type(i,j,7);
}
void make_force_barrier(short i,short j)
{
	make_field_type(i,j,7);
}
void take_force_barrier(short i,short j)
{
	take_field_type(i,j,7);

}

Boolean is_sfx(short i,short j,short type)
{
	return is_field_type(i,j,type + 14);
}
void make_sfx(short i,short j,short type)
{
	make_field_type(i,j,type + 14);
}
void take_sfx(short i,short j,short type)
{
	take_field_type(i,j,type + 14);
}


Boolean is_quickfire(short i,short j)
{
	return is_field_type(i,j,8);
}
void make_quickfire(short i,short j)
{
	make_field_type(i,j,8);
}
void take_quickfire(short i,short j)
{
	take_field_type(i,j,8);
}

Boolean container_there(location l)
{
	
	unsigned char ter;
	
	if (editing_town == FALSE)
		return FALSE;
	if (scenario.ter_types[t_d.terrain[l.x][l.y]].special == 14)
		return TRUE;
	if (is_barrel(l.x,l.y) == TRUE)
		return TRUE;
	if (is_crate(l.x,l.y) == TRUE)
		return TRUE;
	return 0;		
}

void char_win_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height)
{
	Str255 store_s;
	
	strcpy((char *) store_s,str);
	win_draw_string( dest_window, dest_rect,store_s, mode, line_height);

}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height)
{
	GrafPtr old_port;
	Str255 p_str,str_to_draw,str_to_draw2,c_str;
	Str255 null_s = "                                                                                                                                                                                                                                                              ";
	short str_len,i;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
	short text_len[257];
	short total_width = 0;
	Boolean end_loop,force_skip = FALSE;
	KeyMap key_state;
	long dummy3;
	RgnHandle current_clip;
	short adjust_x = 0,adjust_y = 0;
	
	strcpy((char *) p_str,(char *) str);
	strcpy((char *) c_str,(char *) str);
	c2p(p_str);	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len);
	str_len = (short) strlen((char *)str);
	if (str_len == 0) {
		return;
		}

	GetPort(&old_port);	
	SetPort((GrafPtr) dest_window);
	
	//FrameRect(&dest_rect);
	
	//current_clip = NewRgn();
	//GetClip(current_clip);
	
	dest_rect.bottom += 5;
	//ClipRect(&dest_rect);
	dest_rect.bottom -= 5;
	
	
	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= str_len))
			total_width = text_len[i];
	if ((mode == 0) && (total_width < dest_rect.right - dest_rect.left))
		mode = 2;
	for (i = 0; i < 257; i++)
		if ((i <= str_len) && (c_str[i] == '|') && (mode == 2))
			mode = 0;
		

	switch (mode) {
		case 0: 
			MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
			for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
				  && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
				  	if (c_str[i] == '|') {
				  		c_str[i] = ' ';
				  		force_skip = TRUE;
				  		}
					sprintf((char *)str_to_draw,"%s",(char *)null_s);
					strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
					sprintf((char *)str_to_draw2," %s",str_to_draw);
					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
					DrawString(str_to_draw2);
					on_what_line++;
					MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
					last_line_break = last_word_break;
					if (force_skip == TRUE) {
						force_skip = FALSE;
						i++;
						last_line_break++;
						last_word_break++;
						}
					}
				if (c_str[i] == ' ')
					last_word_break = i + 1;
				//if (on_what_line == LINES_IN_TEXT_WIN - 1)
				//	i = 10000;
				}
	
			if (i - last_line_break > 1) {
				strcpy((char *)str_to_draw,(char *)null_s);
				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
				sprintf((char *)str_to_draw2," %s",str_to_draw);
				if (strlen((char *) str_to_draw2) > 3) {
					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
					DrawString(str_to_draw2);
					}
				}	
			break;
		case 1:
			MoveTo((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x, 
			  (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);	
			DrawString(p_str);
			break;
		case 2:
			MoveTo(dest_rect.left + 1 + adjust_x, 
			  dest_rect.top + 1 + adjust_y + 9);
			DrawString(p_str);					
			break;
		case 3:
			MoveTo(dest_rect.left + 1 + adjust_x, 
			  dest_rect.top + 1 + adjust_y + 9 + (dest_rect.bottom - dest_rect.top) / 6);
			DrawString(p_str);					
			break;
		}
	//SetClip(current_clip);
	//DisposeRgn(current_clip);
	SetPort(old_port);
}

void c2p(Str255 str) 
{
	Str255 str2;
	short len;
	
	len = strlen((char *) str);
	strcpy((char *) str2,(char *) str);
	str[0] = (unsigned char) len;
	strncpy((char *) (str + 1), (char *) str2,len);
}

void p2c(Str255 str)
{
	Str255 str2;
	short len;

	len = (short) str[0];
	strncpy((char *) str2,(char *) (str + 1), len);
	str2[len] = 0;
	strcpy((char *) str,(char *) str2);
}

void get_str(Str255 str,short i, short j)
{
	if (i == -1) {
		strcpy((char *) str,data_store->scen_item_list.monst_names[j]);
		return;
		}
	if (i == -2) {
		strcpy((char *) str,data_store->scen_item_list.scen_items[j].full_name);
		return;
		}
	if (i == -3) {
		strcpy((char *) str,button_strs[available_dlog_buttons[j]]);
		return;
		}
	if (i == -4) {
		strcpy((char *) str,data_store->scen_item_list.ter_names[j]);
		return;
		}
	if (i == -5) {
		get_str(str,40,j * 7 + 1);
		return;
		}
	GetIndString(str, i, j);
	p2c(str);
}

short string_length(char *str)
{
	short text_len[257];
	short total_width = 0,i,len;
	Str255 p_str;
	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	
	strcpy((char *) p_str,str);
	c2p(p_str);
	MeasureText(256,p_str,text_len);
	len = strlen((char *)str);
	
	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= len))
			total_width = text_len[i];
	return total_width;
}

Rect get_custom_rect (short which_rect) ////
{
	Rect store_rect = {0,0,36,28};

	OffsetRect(&store_rect,28 * (which_rect % 10),36 * (which_rect / 10));
	return store_rect;
}