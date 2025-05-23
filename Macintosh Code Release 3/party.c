#include <Dialogs.h>
#include <TextUtils.h>
#include <Windows.h>
#include <Quickdraw.h>
#include <Events.h>
#include <TextUtils.h>
#include <QDOffscreen.h>
#include "stdio.h"

#include "global.h"

#include "blxfileio.h"
#include "blxgraphics.h"
#include "blx.g.utils.h"
#include "newgraph.h"
#include "specials.h"
#include "item_data.h"
#include "info.dialogs.h"
#include "items.h"
#include "string.h"
#include "party.h"
#include "monster.h"
#include "dlogtool.h"
#include "town.h"
#include "combat.h"
#include "loc_utils.h"
#include "fields.h"
#include "text.h"
#include "Exile.sound.h"

short skill_cost[20] = {3,3,3,2,2,2, 1,2,2,6,
						5, 1,2,4,2,1, 4,2,5,0};
extern short skill_max[20] ;
short skill_g_cost[20] = {50,50,50,40,40,40,30,50,40,250,
						250,25,100,200,30,20,100,80,0,0};
short skill_bonus[21] = {-3,-3,-2,-1,0,0,1,1,1,2,
							2,2,3,3,3,3,4,4,4,5,5};

short spell_level[62] = {1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,3,3,
						4,4,4,4,4,4,4,4, 5,5,5,5,5,5,5,5, 6,6,6,6,6,6,6,6, 7,7,7,7,7,7,7,7};
short spell_cost[2][62] = {{1,1,1,1,1,2,50,2,1,3, 2,3,2,2,2,2,4,4,2,6, 3,3,5,3,3,5,6,4,6,4,
							4,5,4,8,30,-1,8,6, 5,8,8,6,9,10,6,6, 7,6,8,7,12,10,12,20, 12,8,20,10,14,10,50,10},
							{1,1,1,2,1,1,3,5,50,1, 2,2,2,2,3,5,8,6,4,2, 3,4,3,3,3,10,5,3,4,6,
							 5,5,5,15,6,5,5,8, 6,7,25,8,10,12,12,6, 8,7,8,8,14,17,8,7, 10,10,35,10,12,12,30,10}};
char *mage_s_name[] = {"Light","Spark","Minor Haste","Strength","Scare",
							"Flame Cloud","Identify","Scry Monster","Goo","True Sight",
						"Minor Poison","Flame","Slow","Dumbfound","Envenom",
							"Stinking Cloud","Summon Beast","Conflagration","Dispel Field","Sleep Cloud",
						"Unlock","Haste","Fireball","Long Light","Fear",
							"Wall of Force","Weak Summoning","Flame Arrows","Web","Resist Magic",
						"Poison","Ice Bolt","Slow Group","Magic Map",
							"Capture Soul","Simulacrum","Venom Arrows","Wall of Ice",
						"Stealth","Major Haste","Fire Storm","D. Barrier",
							"Fire Barrier","Summoning","Shockstorm","Spray Fields",
						"Major Poison","Group Fear","Kill","Paralysis",
							"Daemon","Antimagic Cloud","MindDuel","Flight",
						"Shockwave","M. Blessing","Mass Paralysis","Protection",
							"Major Summon","Force Barrier","Quickfire","Death Arrows"};
char *priest_s_name[] = {"Minor Bless","Minor Heal","Weaken Poison","Turn Undead","Location",
							"Sanctuary","Symbiosis","Minor Manna","Ritual - Sanctify","Stumble",
						"Bless","Cure Poison","Curse","Light","Wound",
							"Summon Spirit","Move Mountains","Charm Foe","Disease","Awaken",
						"Heal","Light Heal All","Holy Scourge","Detect Life","Cure Paralysis",
							"Manna","Forcefield","Cure Disease","Restore Mind","Smite",
						"Cure Party","Curse All","Dispel Undead","Remove Curse",
							"Sticks to Snakes","Martyr's Shield","Cleanse","Firewalk",
						"Bless Party","Major Heal","Raise Dead","Flamestrike",
							"Mass Sanctuary","Summon Host","Shatter","Dispel Fields",
						"Heal All","Revive","Hyperactivity","Destone",
							"Guardian","Mass Charm","Protective Circle","Pestilence",
						"Revive All","Ravage Spirit","Resurrect","Divine Thud",
							"Avatar","Wall of Blades","Word of Recall","Major Cleansing"};
	char *alch_names[] = {"Weak Curing Potion (1)","Weak Healing Potion (1)","Weak Poison (1)",
	"Weak Speed Potion (3)","Medium Poison (3)",
		"Medium Heal Potion (4)","Strong Curing (5)","Medium Speed Potion (5)",
		"Graymold Salve (7)","Weak Energy Potion (9)",
		"Potion of Clarity (9)","Strong Poison (10)","Strong Heal Potion (12)","Killer Poison (12)",
		"Resurrection Balm (9)","Medium Energy Ptn. (14)","Knowledge Brew (19)"	,
		"Strong Strength (10)","Bliss (16)","Strong Power (20)"
		};
	char *alch_names_short[] = {"Weak Curing Potion","Weak Healing Potion","Weak Poison",
	"Weak Speed Potion","Medium Poison",
		"Medium Heal Potion","Strong Curing","Medium Speed Potion",
		"Graymold Salve","Weak Energy Potion",
		"Potion of Clarity","Strong Poison","Strong Heal Potion","Killer Poison",
		"Resurrection Bal","Medium Energy Ptn.","Knowledge Brew"	,
		"Strong Strength","Bliss","Strong Power"
		};
	short spell_w_cast[2][62] = {{0,1,1,1,1,1,3,4,1,2, 1,1,1,1,1,1,4,1,4,1, 2,1,1,0,1,1,4,1,1,0,
							1,1,1,2,4,1,1,1, 2,1,1,2,4,4,1,1, 1,1,1,1,4,4,1,5, 1,4,1,4,4,4,4,1},
							{1,0,0,1,3,1,1,3,2,1, 1,0,1,0,1,4,2,1,1,0, 0,0,1,2,0,3,1,0,0,1,
							0,1,1,3,4,1,0,0, 1,0,3,1,1,4,2,4, 0,0,0,3,4,1,1,1, 0,1,3,1,4,1,5,0}};
// 0 - everywhere 1 - combat only 2 - town only 3 - town & outdoor only 4 - town & combat only  5 - outdoor only
Boolean get_mage[30] = {1,1,1,1,1,1,0,1,1,0, 1,1,1,1,1,1,0,0,1,1, 1,1,1,1,1,0,0,0,1,1};
Boolean get_priest[30] = {1,1,1,1,1,1,0,0,0,1, 1,1,1,1,1,0,0,0,1,1, 1,0,1,1,0,0,0,1,0,0};
short combat_percent[20] = {150,120,100,90,80,80,80,70,70,70,
								70,70,67,62,57,52,47,42,40,40};

short town_spell,who_cast,which_pc_displayed;
Boolean spell_button_active[90];
extern item_record_type start_items[6];

Str255 empty_string = "                                           ";

extern Boolean fast_bang;
extern party_record_type	party;
extern pc_record_type adven[6];
extern short stat_window,overall_mode,current_pc,town_size[3],town_type;
extern current_town_type	c_town;
extern big_tr_type t_d;
extern unsigned char out[96][96],out_e[96][96];
extern location pc_pos[6],center;
extern WindowPtr mainPtr;
extern Boolean in_startup_mode,spell_forced,save_maps,suppress_stat_screen,boom_anim_active;
extern stored_items_list_type stored_items[3];
extern CursHandle sword_curs;
extern setup_save_type setup_save;
extern short store_mage, store_priest;
extern short store_mage_lev, store_priest_lev;
extern short store_spell_target,pc_casting,stat_screen_mode;
extern short pc_last_cast[2][6];
extern effect_pat_type null_pat,single,t,square,rad2,rad3;
extern effect_pat_type current_pat;
extern short current_spell_range;
extern short hit_chance[21],pc_parry[6],pc_moves[6],combat_active_pc;
extern short boom_gr[8];
extern	unsigned char beasts[5];
extern	unsigned char m1[20];
extern	unsigned char m2[16];
extern	unsigned char m3[16];
extern stored_town_maps_type maps;
extern stored_outdoor_maps_type o_maps;
extern short current_ground,dialog_answer;
extern short on_spell_menu[2][62];
extern pascal Boolean cd_event_filter();
extern Boolean dialog_not_toast;
extern short mage_need_select[62];
extern short priest_need_select[62];
extern short pc_marked_damage[6];
extern short monst_marked_damage[T_M];
extern location golem_m_locs[16];
extern town_item_list t_i;
extern scenario_data_type scenario;
extern piles_of_stuff_dumping_type *data_store;

char c_line[60];

// Variables for spell selection
	short store_situation,store_last_target_darkened,on_which_spell_page = 0;
	short store_last_cast_mage = 6,store_last_cast_priest = 6;
	short buttons_on[38] = {1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};
		// buttons_on determines which buttons can be hit when on the second spell page
	short spell_index[38] = {38,39,40,41,42,43,44,45,90,90,46,47,48,49,50,51,52,53,90,90,
							54,55,56,57,58,59,60,61,90,90, 90,90,90,90,90,90,90,90};
		// Says which buttons hit which spells on second spell page, 90 means no button
	Boolean can_choose_caster;

// Variables for spending xp
	Boolean talk_done = FALSE;
	long val_for_text;
	Boolean keep_change = FALSE;
	short store_skills[20],store_h,store_sp,i,store_skp,which_skill;
	long store_g;
	short store_train_mode,store_train_pc;
	
extern GWorldPtr pcs_gworld;
extern ModalFilterUPP main_dialog_UPP;

// Dialog vars
short store_mage_store ;
short store_priest_store ;
short store_store_target;
short store_graphic_pc_num ;
short store_graphic_mode ;
short store_pc_graphic;


void draw_caster_buttons();

void draw_spell_info();
void draw_spell_pc_info();

void put_pc_caster_buttons();
void put_pc_target_buttons();
void put_spell_led_buttons();
void put_spell_list();
void put_pick_spell_graphics();


//mode; // 0 - prefab 1 - regular
void init_party(short mode)
{
	short i,j,k,l;
	Boolean item_check;
	Str255 pal_str = "\pGeneva";
		
	boat_record_type null_boat = {{0,0},{0,0},{0,0},200,FALSE};
	horse_record_type null_horse = {{0,0},{0,0},{0,0},200,FALSE};
	
	party.age = 0;
	party.gold = 200;
	party.food = 100;
	for (i = 0; i < 310; i++)
		for (j = 0; j < 10; j++)
			party.stuff_done[i][j] = 0;
	party.light_level = 0;
	party.outdoor_corner.x = 7;
	party.outdoor_corner.y = 8;
	party.i_w_c.x = 1;
	party.i_w_c.y = 1;
	party.loc_in_sec.x = 36;
	party.loc_in_sec.y = 36;
	party.p_loc.x = 84;
	party.p_loc.y = 84;
	for (i = 0; i < 30; i++)
		party.boats[i] = null_boat;
	for (i = 0; i < 30; i++)
		party.horses[i] = null_horse;
	 party.in_boat = -1;
	 party.in_horse = -1;
	for (i = 0; i < 4; i++)
		party.creature_save[i].which_town = 200;
	for (i = 0; i < 10; i++)
		 party.out_c[i].exists = FALSE;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++)
			 party.magic_store_items[i][j].variety = 0;
	for (i = 0; i < 4; i++)
	 party.imprisoned_monst[i] = 0;
	for (i = 0; i < 256; i++)
	 party.m_seen[i] = 0;
	for (i = 0; i < 50; i++) 
	 party.journal_str[i] = -1;
	for (i = 0; i < 140; i++)
		for (j = 0; j < 2; j++)
	 	party.special_notes_str[i][j] = 0;	 
	for (i = 0; i < 120; i++)
		 party.talk_save[i].personality = -1;
		 	 
	 party.total_m_killed = 0;
	party.total_dam_done = 0;
	party.total_xp_gained = 0;
	party.total_dam_taken = 0;
	 party.direction = 0;
	party.at_which_save_slot = 0;
	for (i = 0; i < 20; i++)
		 party.alchemy[i] = 0;
	for (i = 0; i < 200; i++)
		party.can_find_town[i] = 0;
	for (i = 0; i < 20; i++)
	 party.key_times[i] = 30000;
	for (i = 0; i < 30; i++)
	 party.party_event_timers[i] = 0;
	for (i = 0; i < 50; i++)
		party.spec_items[i] = 0;
	for (i = 0; i < 120; i++) {
	 party.help_received[i] = 0;
	 }
	for (i = 0; i < 200; i++)
	 party.m_killed[i] = 0;
	sprintf((char *) party.scen_name,"");

	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			party.item_taken[i][j] = 0;


	refresh_store_items();
	
		for (i = 0; i < 6; i++) {
//			adven[i] = create_debug_pc(i);
			adven[i] = return_dummy_pc();
			if (mode != 1)
				//adven[i] = create_debug_pc(i);
				adven[i] = create_prefab_pc(i);
			}
		
	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;


	for (i = 0; i < 3;i++)
		for (j = 0; j < NUM_TOWN_ITEMS; j++) {
			stored_items[i].items[j] = return_dummy_item();  
			}

	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 64; k++)
				data_store->town_maps.town_maps[i][j][k] = 0;

	for (i = 0; i < 100; i++)
			for (k = 0; k < 6; k++)
				for (l = 0; l < 48; l++)
					o_maps.outdoor_maps[i][k][l] = 0;
					
	// Default is save maps
	party.stuff_done[306][0] = 0;
	save_maps = TRUE;	
			

	// NOT DEBUG
	build_outdoors();
	update_pc_graphics();

}

// This is only called after a scenario is loaded and the party is put into it.
// Until that time, the party scen vals are uninited
// Then, it inits the party properly for starting the scenario based
// on the loaded scenario
void init_party_scen_data()
{
	short i,j,k,l;
	Boolean stored_item = FALSE;
	short store_help;
	
	party.age = 0;
	store_help = PSD[306][4];
	for (i = 0; i < 310; i++)
		for (j = 0; j < 10; j++)
			party.stuff_done[i][j] = 0;
	PSD[306][4] = store_help;
	party.light_level = 0;
	party.outdoor_corner.x = scenario.out_sec_start.x;
	party.outdoor_corner.y = scenario.out_sec_start.y;
	party.i_w_c.x = 0;
	party.i_w_c.y = 0;
	party.loc_in_sec.x = scenario.out_start.x;
	party.loc_in_sec.y = scenario.out_start.y;
	party.p_loc.x = scenario.out_start.x;
	party.p_loc.y = scenario.out_start.y;
	for (i = 0; i < 30; i++)
		party.boats[i] = scenario.scen_boats[i];
	for (i = 0; i < 30; i++)
		party.horses[i] = scenario.scen_horses[i];
	for (i = 0; i < 30; i++) {
		if ((scenario.scen_boats[i].which_town >= 0) && (scenario.scen_boats[i].boat_loc.x >= 0)) {
			if (party.boats[i].exists == FALSE) {
				party.boats[i] = scenario.scen_boats[i];
				party.boats[i].exists = TRUE;
				}
			}
		if ((scenario.scen_horses[i].which_town >= 0) && (scenario.scen_horses[i].horse_loc.x >= 0)) {
			if (party.horses[i].exists == FALSE) {
				party.horses[i] = scenario.scen_horses[i];
				party.horses[i].exists = TRUE;
				}
			}
		}
	 party.in_boat = -1;
	 party.in_horse = -1;
	for (i = 0; i < 4; i++)
		party.creature_save[i].which_town = 200;
	for (i = 0; i < 10; i++)
		 party.out_c[i].exists = FALSE;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++)
			 party.magic_store_items[i][j].variety = 0;
	for (i = 0; i < 4; i++)
	 party.imprisoned_monst[i] = 0;
	for (i = 0; i < 256; i++)
	 party.m_seen[i] = 0;
	for (i = 0; i < 50; i++) 
	 party.journal_str[i] = -1;
	for (i = 0; i < 140; i++)
		for (j = 0; j < 2; j++)
	 	party.special_notes_str[i][j] = 0;	 
	for (i = 0; i < 120; i++)
		 party.talk_save[i].personality = -1;
		 	 
	 party.direction = 0;
	party.at_which_save_slot = 0;
	for (i = 0; i < 200; i++)
		party.can_find_town[i] = 1 - scenario.town_hidden[i];
	for (i = 0; i < 20; i++)
	 	party.key_times[i] = 30000;
	for (i = 0; i < 30; i++)
	 	party.party_event_timers[i] = 0;
	for (i = 0; i < 50; i++)
		party.spec_items[i] = (scenario.special_items[i] >= 10) ? 1 : 0;

	for (i = 0; i < 200; i++)
	 party.m_killed[i] = 0;

	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			party.item_taken[i][j] = 0;


	refresh_store_items();
		
	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++)
			out_e[i][j] = 0;

	for (i = 0; i < 3;i++)
		for (j = 0; j < NUM_TOWN_ITEMS; j++) 
			if (stored_items[i].items[j].variety != 0)
				stored_item = TRUE;
	if (stored_item == TRUE)
		if (FCD(911,0) == 1) {
			for (i = 0; i < 3;i++)
				for (j = 0; j < NUM_TOWN_ITEMS; j++) 
					if (stored_items[i].items[j].variety != 0)
						if (give_to_party(stored_items[i].items[j],FALSE) == FALSE) {
							i = 20; j = NUM_TOWN_ITEMS + 1;
							}
			}
	for (i = 0; i < 3;i++)
		for (j = 0; j < NUM_TOWN_ITEMS; j++) {
			stored_items[i].items[j] = return_dummy_item();  
			}

	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 64; k++)
				data_store->town_maps.town_maps[i][j][k] = 0;

	for (i = 0; i < 100; i++)
			for (k = 0; k < 6; k++)
				for (l = 0; l < 48; l++)
					o_maps.outdoor_maps[i][k][l] = 0;

}

// When the party is placed into a scen from the startinbg screen, this is called to put the game into game 
// mode and load in the scen and init the party info
// party record already contains scen name
void put_party_in_scen()
{
	short i,j,k;
	Str255 strs[6] = {"","","","","",""};
	short buttons[3] = {-1,-1,-1};
	Boolean item_took = FALSE;
	
	for (j = 0; j < 6; j++)
	 	for (i = 0; i < 15; i++)
 			adven[j].status[i] = 0;
	for (j = 0; j < 6; j++) {
		if (adven[j].main_status >= 10)
			adven[j].main_status -= 10;
		adven[j].cur_health = adven[j].max_health; 
 		adven[j].cur_sp = adven[j].max_sp; 
		}
	for (j = 0; j < 6; j++)
	 	for (i = 23; i >= 0; i--) {
			adven[j].items[i].special_class = 0;
			if (adven[j].items[i].graphic_num >= 150) {
				take_item(j,i + 30); // strip away special items
				item_took = TRUE;
				}
			if (adven[j].items[i].ability == 119) {
				take_item(j,i + 30); // strip away summoning items
				item_took = TRUE;
				}
			if (adven[j].items[i].ability == 120) {
				take_item(j,i + 30); // strip away summoning items
				item_took = TRUE;
				}
			}
	if (item_took == TRUE)
		FCD(910,0);
	party.age = 0;
	for (i = 0; i < 200; i++)
		party.m_killed[i] = 0;
	for (i = 0; i < 30; i++)
	 	party.party_event_timers[i] = 0;
		
	if (load_scenario() == FALSE)
		return;
	init_party_scen_data();


	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();
	in_startup_mode = FALSE;
	
	set_up_ter_pics();
	
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y + 1,1,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y + 1,0,1,0,0,NULL);
	load_outdoors(party.outdoor_corner.x + 1,party.outdoor_corner.y,1,0,0,0,NULL);
	load_outdoors(party.outdoor_corner.x,party.outdoor_corner.y,0,0,0,0,NULL);
	stat_screen_mode = 0;
	build_outdoors();
	erase_out_specials();
	update_pc_graphics();
		
	current_pc = first_active_pc();
	force_town_enter(scenario.which_town_start,scenario.where_start);
	start_town_mode(scenario.which_town_start,9);
	center = scenario.where_start;
	update_explored(scenario.where_start);
	overall_mode = 1;
	load_area_graphics();
	create_clip_region();
	redraw_screen(0);
	set_stat_window(0);
	adjust_spell_menus();
	adjust_monst_menu();

	// Throw up intro dialog
	buttons[0] = 1;
	for (j = 0; j < 6; j++)
		if (strlen(	data_store->scen_strs[4 + j]) > 0) {
			for (i = 0; i < 6; i++)	
				strcpy((char *) strs[i],data_store->scen_strs[4 + i]);
			custom_choice_dialog(strs,-1 * (1600 + scenario.intro_pic),buttons) ;
			j = 6;
		}
	give_help(1,2,0);
	
	// this is kludgy, put here to prevent problems
	for (i = 0; i < 50; i++)
		party.spec_items[i] = (scenario.special_items[i] >= 10) ? 1 : 0;
}


pc_record_type return_dummy_pc()
{
	pc_record_type	dummy_pc;
	short i;

	dummy_pc.main_status = 0;
	sprintf	((char *) dummy_pc.name, "\n");
	
	for (i = 0; i < 30; i++)
		dummy_pc.skills[i] = (i < 3) ? 1 : 0;
	dummy_pc.cur_health = 6; 
	dummy_pc.max_health = 6; 
 	dummy_pc.cur_sp = 0; 
 	dummy_pc.max_sp = 0; 
 	dummy_pc.experience = 0; 
	dummy_pc.skill_pts = 60; 
	dummy_pc.level = 1;
 	for (i = 0; i < 15; i++)
 		dummy_pc.status[i] = 0;
	for (i = 0; i < 24; i++)
		dummy_pc.items[i] = return_dummy_item();
	for (i = 0; i < 24; i++)
		dummy_pc.equip[i] = FALSE;

 	for (i = 0; i < 62; i++) {
		dummy_pc.priest_spells[i] = (i < 30) ? TRUE : FALSE;
		dummy_pc.mage_spells[i] = (i < 30) ? TRUE : FALSE;
		}
	dummy_pc.which_graphic = 0;
	dummy_pc.weap_poisoned = 24;

	for (i = 0; i < 15; i++) {
		dummy_pc.advan[i] = FALSE;
		dummy_pc.traits[i] = FALSE;	
		}		
	dummy_pc.race = 0;
	dummy_pc.exp_adj = 100;
	dummy_pc.direction = 0; 	
	
	return dummy_pc;
}

pc_record_type create_debug_pc(short num)
{
	pc_record_type	dummy_pc;
	short i;
	
	dummy_pc.main_status = 1;
	switch (num) {
		case 0:	strncpy	((char *) dummy_pc.name, "Gunther", (size_t) 20);
				break;
		case 1:	strncpy	((char *) dummy_pc.name, "Yanni", (size_t) 20);
				break;
		case 2:	strncpy	((char *) dummy_pc.name, "Mandolin", (size_t) 20);
				break;
		case 3:	strncpy	((char *) dummy_pc.name, "Pete", (size_t) 20);
				break;
		case 4:	strncpy	((char *) dummy_pc.name, "Vraiment", (size_t) 20);
				break;
		case 5:	strncpy	((char *) dummy_pc.name, "Goo", (size_t) 20);
				break;
		
		}

	for (i = 0; i < 30; i++)
		dummy_pc.skills[i] = (i < 3) ? 20 : 8;
	dummy_pc.cur_health = 60; 
	dummy_pc.max_health = 60; 
 	dummy_pc.cur_sp = 90; 
 	dummy_pc.max_sp = 90; 
 	dummy_pc.experience = 0; 
	dummy_pc.skill_pts = 60; 
	dummy_pc.level = 1;
 	for (i = 0; i < 15; i++)
 		dummy_pc.status[i] = 0;
	for (i = 0; i < 24; i++)
		dummy_pc.items[i] = return_dummy_item();
	for (i = 0; i < 24; i++)
		dummy_pc.equip[i] = FALSE;

 	for (i = 0; i < 62; i++) {
		dummy_pc.priest_spells[i] = TRUE;
		dummy_pc.mage_spells[i] = TRUE;
		}
//	dummy_pc.which_graphic = num * 3 + 1;
	dummy_pc.which_graphic = num + 4;
	dummy_pc.weap_poisoned = 16;

	for (i = 0; i < 15; i++) {
		dummy_pc.advan[i] = FALSE;
		dummy_pc.traits[i] = FALSE;	
		}		

	dummy_pc.race = 0;
	dummy_pc.exp_adj = 100;
	dummy_pc.direction = 0; 	
	
	return dummy_pc;
}

pc_record_type create_prefab_pc(short num)
{

	pc_record_type	dummy_pc;
	short i;
	short pc_stats[6][19] = {{8,6,2, 6,0,0,0,0,0, 0,0,0,0,1, 0,0,2,0,0},
						{8,7,2, 0,0,6,3,0,3, 0,0,0,0,0, 0,0,0,2,0},
						{8,6,2, 3,3,0,0,2,0, 0,0,0,0,0, 4,4,0,2,1},
						{3,2,6, 2,0,0,2,0,0, 3,0,3,0,1, 0,0,0,0,0},
						{2,2,6, 3,0,0,2,0,0,  2,1,4,0,0, 0,0,0,0,1},
						{2,2,6, 0,2,0,2,0,1, 0,3,3,2,0, 0,0,0,0,0}};
	short pc_health[6] = {22,24,24,16,16,18};
	short pc_sp[6] = {0,0,0,20,20,21};
	short pc_graphics[6] = {3,32,29,16,23,14};
	short pc_race[6] = {0,2,1,0,0,0};
	short pc_t[6][15] = {{0,0,1,0,0,0,1,0,0,0, 0,1,0,0,0},		
						{1,0,0,0,0,1,0,0,0,0, 1,0,0,0,0},	
						{0,0,0,1,0,0,0,0,0,0, 0,0,1,0,0},	
						{0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0},	
						{0,0,0,0,1,0,1,1,0,0, 0,0,0,0,1},	
						{0,1,0,0,0,0,0,0,0,0, 0,0,0,0,0}};

							
	dummy_pc.main_status = 1;
	switch (num) {
		case 0:	strncpy	((char *) dummy_pc.name, "Jenneke", (size_t) 20);
				break;
		case 1:	strncpy	((char *) dummy_pc.name, "Thissa", (size_t) 20);
				break;
		case 2:	strncpy	((char *) dummy_pc.name, "Frrrrrr", (size_t) 20);
				break;
		case 3:	strncpy	((char *) dummy_pc.name, "Adrianna", (size_t) 20);
				break;
		case 4:	strncpy	((char *) dummy_pc.name, "Feodoric", (size_t) 20);
				break;
		case 5:	strncpy	((char *) dummy_pc.name, "Michael", (size_t) 20);
				break;
		
		}

	for (i = 0; i < 19; i++)
		dummy_pc.skills[i] = pc_stats[num][i];
	dummy_pc.cur_health = pc_health[num]; 
	dummy_pc.max_health = pc_health[num]; 
	dummy_pc.experience = 0; 
	dummy_pc.skill_pts = 0; 
	dummy_pc.level = 1;

 	for (i = 0; i < 15; i++)
 		dummy_pc.status[i] = 0;
	for (i = 0; i < 24; i++)
		dummy_pc.items[i] = return_dummy_item();
	for (i = 0; i < 24; i++)
		dummy_pc.equip[i] = FALSE;
 	dummy_pc.cur_sp = pc_sp[num]; 
 	dummy_pc.max_sp = pc_sp[num]; 
 	for (i = 0; i < 62; i++) {
		dummy_pc.priest_spells[i] = (i < 30) ? TRUE : FALSE;////
		dummy_pc.mage_spells[i] = (i < 30) ? TRUE : FALSE;
		}
 	for (i = 0; i < 15; i++) {
		dummy_pc.traits[i] = pc_t[num][i];
		dummy_pc.advan[i] = FALSE;		
		}
		
	dummy_pc.race = pc_race[num];
 	dummy_pc.exp_adj = 100;
	dummy_pc.direction = 0; 	
		
	dummy_pc.which_graphic = pc_graphics[num];
	
	return dummy_pc;
}





Boolean create_pc(short spot,short parent_num)
//spot; // if spot is 6, find one
{
	Boolean still_ok = TRUE;
	short i;

	if (spot == 6) {
		for (spot = 0; spot < 6; spot++)
			if (adven[spot].main_status == 0)
				break;
		}
	if (spot == 6)
		return FALSE;

	adven[spot] = return_dummy_pc();

	pick_race_abil(&adven[spot],0,parent_num);
	
	if (parent_num != 0)
		cd_initial_draw(989);
	
	still_ok = spend_xp(spot,0,parent_num);
	if (still_ok == FALSE)
		return FALSE;
	adven[spot].cur_health = adven[spot].max_health;
	adven[spot].cur_sp = adven[spot].max_sp;
	if (parent_num != 0)
		cd_initial_draw(989);
	
	pick_pc_graphic(spot,0,parent_num);

	if (parent_num != 0)
		cd_initial_draw(989);
	pick_pc_name(spot,parent_num);

	adven[spot].main_status = 1;
	
	if (in_startup_mode == FALSE) {
			adven[spot].items[0] = start_items[adven[spot].race * 2];
			adven[spot].equip[0] = TRUE;
			adven[spot].items[1] = start_items[adven[spot].race * 2 + 1];
			adven[spot].equip[1] = TRUE;

			// Do stat adjs for selected race.
			if (adven[spot].race == 1)
				adven[spot].skills[1] += 2;
			if (adven[spot].race == 2) {
				adven[spot].skills[0] += 2;
				adven[spot].skills[2] += 1;	
				}
			adven[spot].max_sp += adven[spot].skills[9] * 3 + adven[spot].skills[10] * 3;
			adven[spot].cur_sp = adven[spot].max_sp;
			}
	
	update_pc_graphics();
	return TRUE;
}

Boolean take_sp(short pc_num,short amt)
{
	if (adven[pc_num].cur_sp < amt)
		return FALSE;
	adven[pc_num].cur_sp -= amt;
	return TRUE;
}




void heal_pc(short pc_num,short amt)
{
	if (adven[pc_num].cur_health > adven[pc_num].max_health)
		return;
	if (adven[pc_num].main_status != 1)
		return;
	adven[pc_num].cur_health += amt;
	if (adven[pc_num].cur_health > adven[pc_num].max_health)
		adven[pc_num].cur_health = adven[pc_num].max_health;

}

void heal_party(short amt)
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			heal_pc(i,amt);
}

void cure_pc(short pc_num,short amt)
{
	if (adven[pc_num].main_status != 1)
		return;
	if (adven[pc_num].status[2] <= amt)
		adven[pc_num].status[2] = 0;
		else adven[pc_num].status[2] -= amt;
	one_sound(51);
}

void cure_party(short amt)
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			cure_pc(i,amt);

}

void curse_pc(short which_pc,short how_much)
{
	if (adven[which_pc].main_status != 1)
		return;
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[1] = max(adven[which_pc].status[1] - how_much,-8);
		sprintf ((char *) c_line, "  %s cursed.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		}
	put_pc_screen();
		give_help(59,0,0);
}

void dumbfound_pc(short which_pc,short how_much)
{
	short r1;

	if (adven[which_pc].main_status != 1)
		return;
	r1 = get_ran(1,0,90);
	if (pc_has_abil_equip(which_pc,53) < 24) {////
		add_string_to_buf("  Ring of Will glows.");
		r1 -= 10;
		}
	if (r1 < adven[which_pc].level)
		how_much -= 2;
	if (how_much <= 0) {
		sprintf ((char *) c_line, "  %s saved.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		return;
		}
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[9] = min(adven[which_pc].status[9] + how_much,8);
		sprintf ((char *) c_line, "  %s dumbfounded.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		}
	one_sound(67);
	put_pc_screen();
	adjust_spell_menus();
	give_help(28,0,0);
}
void disease_pc(short which_pc,short how_much)
{
	short r1,level;

	if (adven[which_pc].main_status != 1)
		return;
	r1 = get_ran(1,0,100);
	if (r1 < adven[which_pc].level * 2)
		how_much -= 2;
	if (how_much <= 0) {
		sprintf ((char *) c_line, "  %s saved.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		return;
		}
	if ((level = get_prot_level(which_pc,62)) > 0)////
		how_much -= level / 2;
	if ((adven[which_pc].traits[12] == TRUE) && 
		(how_much > 1))
			how_much++;
	if ((adven[which_pc].traits[12] == TRUE) && 
		(how_much == 1) && (get_ran(1,0,1) == 0))
			how_much++;
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[7] = min(adven[which_pc].status[7] + how_much,8);
		sprintf ((char *) c_line, "  %s diseased.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		}
	one_sound(66);
		put_pc_screen();
	give_help(29,0,0);
}

void sleep_pc(short which_pc,short how_much,short what_type,short adjust)
// higher adjust, less chance of saving
{
	short r1,level;
	if (adven[which_pc].main_status != 1)
		return;
	if (how_much == 0)
		return;
	if ((what_type == 11) || (what_type == 12)) { ////
		if ((level = get_prot_level(which_pc,53)) > 0)
			how_much -= level / 2;
		if ((level = get_prot_level(which_pc,54)) > 0)
			how_much -= (what_type == 11) ? level : level * 300;

		}
		
	r1 = get_ran(1,0,100) + adjust;
	if (r1 < 30 + adven[which_pc].level * 2)
		how_much = -1;
	if ((what_type == 11) && ((adven[which_pc].traits[7] > 0) || (adven[which_pc].status[11] < 0)))
		how_much = -1;
	if (how_much <= 0) {
		sprintf ((char *) c_line, "  %s resisted.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		return;
		}
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[what_type] = how_much;
		if (what_type == 11)
			sprintf ((char *) c_line, "  %s falls asleep.",(char *) adven[which_pc].name);
			else sprintf ((char *) c_line, "  %s paralyzed.",(char *) adven[which_pc].name);
		if (what_type == 11)
			play_sound(96);
			else play_sound(90);
		add_string_to_buf((char *) c_line);
		pc_moves[which_pc] = 0;
		}
	put_pc_screen();
	if (what_type == 11)
		give_help(30,0,0);
		else give_help(32,0,0);

}

// if how_much < 0, haste
void slow_pc(short which_pc,short how_much)////
{
	if (adven[which_pc].main_status != 1)
		return;
	if (adven[which_pc].main_status == 1) {

		adven[which_pc].status[3] = minmax(-8,8,adven[which_pc].status[3] - how_much);
		if (how_much < 0)
			sprintf ((char *) c_line, "  %s hasted.",(char *) adven[which_pc].name);
			else sprintf ((char *) c_line, "  %s slowed.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		}
	put_pc_screen();
	if (how_much < 0)
		give_help(35,0,0);
}

void web_pc(short which_pc,short how_much)
{
	if (adven[which_pc].main_status != 1)
		return;
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[6] = min(adven[which_pc].status[6] + how_much,8);
		sprintf ((char *) c_line, "  %s webbed.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		one_sound(17);
		}
		put_pc_screen();
	give_help(31,0,0);
}

void acid_pc(short which_pc,short how_much)
{
	if (adven[which_pc].main_status != 1)
		return;
	if (pc_has_abil_equip(which_pc,36) < 24) {
		sprintf ((char *) c_line, "  %s resists acid.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		return;
		}
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].status[13] += how_much;
		sprintf ((char *) c_line, "  %s covered with acid!",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		one_sound(42);
		}
		put_pc_screen();
}

void increase_light(short amt)
{
	short i;
	location where;

	party.light_level += amt;
	if (is_combat()) {
		for (i = 0; i < 6; i++)
			if (adven[i].main_status == 1) {
				update_explored(pc_pos[i]);
				}
		}
		else {
			where = get_cur_loc();
			update_explored(where);
			}
	put_pc_screen();
}

void restore_sp_pc(short pc_num,short amt)
{
	if (adven[pc_num].cur_sp > adven[pc_num].max_sp)
		return;
	adven[pc_num].cur_sp += amt;
	if (adven[pc_num].cur_sp > adven[pc_num].max_sp)
		adven[pc_num].cur_sp = adven[pc_num].max_sp;
}

void restore_sp_party(short amt)
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			restore_sp_pc(i,amt);
}

void award_party_xp(short amt)
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			award_xp(i,amt);
}

void award_xp(short pc_num,short amt)
{
	short adjust,add_hp;
	short xp_percent[30] = {150,120,100,90,80,70,60,50,50,50,
								45,40,40,40,40,35,30,25,23,20,
								15,15,15,15,15,15,15,15,15,15};	
	if (adven[pc_num].level > 49) {
		adven[pc_num].level = 50;
		return;
		}
	if (amt > 200) { // debug
		SysBeep(50); SysBeep(50);
		ASB("Oops! Too much xp!");
		ASB("Report this!");
		return;
		}
	if (amt < 0) { // debug
		SysBeep(50); SysBeep(50);
		ASB("Oops! Negative xp!");
		ASB("Report this!");
		return;
		}
	if (adven[pc_num].main_status != 1)
		return;

	if (adven[pc_num].level >= 40)
		adjust = 15;
		else adjust = xp_percent[adven[pc_num].level / 2];

	if ((amt > 0) && (adven[pc_num].level > 7)) {
		if (get_ran(1,0,100) < xp_percent[adven[pc_num].level / 2])
			amt--;
		}
	if (amt <= 0)
		return;


//	adven[pc_num].experience += (max(((amt * adjust) / 100), 0) * adven[pc_num].exp_adj) / 100;
//	party.total_xp_gained += (max(((amt * adjust) / 100), 0) * adven[pc_num].exp_adj) / 100;
	adven[pc_num].experience += (max(((amt * adjust) / 100), 0) * 100) / 100;
	party.total_xp_gained += (max(((amt * adjust) / 100), 0) * 100) / 100;


	
	if (adven[pc_num].experience < 0) {
		SysBeep(50); SysBeep(50);
		ASB("Oops! Xp became negative somehow!");
		ASB("Report this!");
		adven[pc_num].experience = adven[pc_num].level * (get_tnl(&adven[pc_num])) - 1;
		return;
		}
	if (adven[pc_num].experience > 15000) {
		adven[pc_num].experience = 15000;
		return;
		}
	
	while (adven[pc_num].experience >= (adven[pc_num].level * (get_tnl(&adven[pc_num])))) {
			play_sound(7);
			adven[pc_num].level++;
			sprintf ((char *) c_line, "  %s is level %d!  ",(char *) adven[pc_num].name,adven[pc_num].level);
			add_string_to_buf((char *) c_line);		
			adven[pc_num].skill_pts += (adven[pc_num].level < 20) ? 5 : 4;
			add_hp = (adven[pc_num].level < 26) ? get_ran(1,2,6) + skill_bonus[adven[pc_num].skills[0]]
			   : max (skill_bonus[adven[pc_num].skills[0]],0);
			if (add_hp < 0)
				add_hp = 0;
			adven[pc_num].max_health += add_hp;
			if (adven[pc_num].max_health > 250)
				adven[pc_num].max_health = 250;
			adven[pc_num].cur_health += add_hp;
			if (adven[pc_num].cur_health > 250)
				adven[pc_num].cur_health = 250;
			put_pc_screen();
				
		}
}

void drain_pc(short which_pc,short how_much)
{
	if (adven[which_pc].main_status == 1) {
		adven[which_pc].experience = max(adven[which_pc].experience - how_much,0);
		sprintf ((char *) c_line, "  %s drained.",(char *) adven[which_pc].name);
		add_string_to_buf((char *) c_line);
		}
}



void do_xp_keep(short pc_num,short mode)
{
					for (i = 0; i < 20; i++)
						adven[pc_num].skills[i] = store_skills[i];
					adven[pc_num].cur_health += store_h - adven[pc_num].max_health;
					adven[pc_num].max_health = store_h;
					adven[pc_num].cur_sp += store_sp - adven[pc_num].max_sp;
					adven[pc_num].max_sp = store_sp;
					if (mode == 1)
						party.gold = store_g;
					adven[pc_num].skill_pts = store_skp;

}

void draw_xp_skills()
{
	short i;
	for (i = 0; i < 19; i++) {
		if ((store_skp >= skill_cost[i]) && (store_g >= skill_g_cost[i]))
			cd_text_frame(1010,54 + i,11);
			else cd_text_frame(1010,54 + i,1);
		cd_set_item_num(1010,54 + i,store_skills[i]);
		}

		if ((store_skp >= 1) && (store_g >= 10))
			cd_text_frame(1010,52,11);
			else cd_text_frame(1010,52,1);
	cd_set_item_num(1010,52,store_h);
		if ((store_skp >= 1) && (store_g >= 15))
			cd_text_frame(1010,53,11);
			else cd_text_frame(1010,53,1);
	cd_set_item_num(1010,53,store_sp);
}


void do_xp_draw()

{

	short item_hit,what_talk_field;
	char get_text[256];
	short mode,pc_num;

	mode = store_train_mode;
	pc_num = store_train_pc;
	if (mode == 0) {
		if (adven[pc_num].main_status == 1)
			sprintf((char *) get_text, "%s",(char *) adven[pc_num].name);
			else sprintf((char *) get_text, "New PC");
		}
		else sprintf((char *) get_text, "%s",(char *) adven[pc_num].name);

	cd_set_item_text (1010, 51,get_text);

	for (i = 0; i < 20; i++)
		store_skills[i] = adven[pc_num].skills[i];
	store_h = adven[pc_num].max_health;
	store_sp = adven[pc_num].max_sp;
	store_g = (mode == 0) ? 20000 : party.gold;
	store_skp = adven[pc_num].skill_pts;

	draw_xp_skills();


	update_gold_skills();
}

Boolean spend_xp_event_filter (short item_hit)
{
	short what_talk_field,i,j,mode,pc_num;
	char get_text[256];
	Boolean talk_done = FALSE;

	mode = store_train_mode;
	pc_num = store_train_pc;

		switch (item_hit) {
			case 73:
				if ((mode == 0) && (adven[pc_num].main_status < 0))
					adven[pc_num].main_status = 0;
				dialog_answer = 0;
				talk_done = TRUE;
				break;
	
			case 82:
				party.help_received[10] = 0;
				give_help(210,11,1010);
				break;

			case 3: case 4:
					if (((store_h >= 250) && (item_hit == 4)) ||
						((store_h == adven[pc_num].max_health) && (item_hit == 3) && (mode == 1)) ||
						((store_h == 6) && (item_hit == 3) && (mode == 0)))
							SysBeep(2);
						else {
							if (item_hit == 3) {
								store_g += 10;
								store_h -= 2;
								store_skp += 1;
								}
								else {
									if ((store_g < 10) || (store_skp < 1)) {
										if (store_g < 10)
											give_help(24,0,1010);
											else give_help(25,0,1010);
										SysBeep(2);
										}
										else {
											store_g -= 10;
											store_h += 2;
											store_skp -= 1;
											}
								}

							update_gold_skills();
							cd_set_item_num(1010,52,store_h);
                     	draw_xp_skills();

						}
				break;

			case 5: case 6:
					if (((store_sp >= 150) && (item_hit == 6)) ||
						((store_sp == adven[pc_num].max_sp) && (item_hit == 5) && (mode == 1)) ||
						((store_sp == 0) && (item_hit == 5) && (mode == 0)))
							SysBeep(2);
						else {
							if (item_hit == 5) {
								store_g += 15;
								store_sp -= 1;
								store_skp += 1;
								}
								else {
									if ((store_g < 15) || (store_skp < 1)) {
										if (store_g < 15)
											give_help(24,0,1010);
											else give_help(25,0,1010);
										SysBeep(2);
										}
										else {
											store_sp += 1;
											store_g -= 15;
											store_skp -= 1;
											}
								}

							update_gold_skills();
							cd_set_item_num(1010,53,store_sp);
							draw_xp_skills();
						}
				break;

			case 48:
					do_xp_keep(pc_num,mode);
					dialog_answer = 1;
					talk_done = TRUE;
				break;

			case 49:
				if (mode == 0) {
					SysBeep(2);
					break;
					}
					else {
						do_xp_keep(pc_num,mode);
						do {
							pc_num = (pc_num == 0) ? 5 : pc_num - 1;
						} while (adven[pc_num].main_status != 1);
						store_train_pc = pc_num;
						do_xp_draw();
						}
				break;

			case 50:
				if (mode == 0) {
					SysBeep(2);
					break;
					}
					else {
						do_xp_keep(pc_num,mode);
						do {
							pc_num = (pc_num == 5) ? 0 : pc_num + 1;
						} while (adven[pc_num].main_status != 1);
						store_train_pc = pc_num;
						do_xp_draw();
						}
				break;

			case 100:
				break;

			default:
				if (item_hit >= 100) {
					item_hit -= 100;
					if ((item_hit == 3) || (item_hit == 4)) {
						display_strings_with_nums(10,63,0,0,"About Health",57,724,1010);
						}
						else if ((item_hit == 5) || (item_hit == 6)){
						display_strings_with_nums(10,64,0,0,"About Spell Points",57,724,1010);
						}
						else {
							which_skill = (item_hit - 7) / 2;
							display_skills(which_skill,1010);
							}
					}
				else {
				which_skill = (item_hit - 7) / 2;
				
				if (((store_skills[which_skill] >= skill_max[which_skill]) && ((item_hit - 7) % 2 == 1)) ||
					((store_skills[which_skill] == adven[pc_num].skills[which_skill]) && ((item_hit - 7) % 2 == 0) && (mode == 1)) ||
					((store_skills[which_skill] == 0) && ((item_hit - 7) % 2 == 0) && (mode == 0) && (which_skill > 2)) ||
					((store_skills[which_skill] == 1) && ((item_hit - 7) % 2 == 0) && (mode == 0) && (which_skill <= 2)))
						SysBeep(2);
					else {
						if ((item_hit - 7) % 2 == 0) {
							store_g += skill_g_cost[which_skill];
							store_skills[which_skill] -= 1;
							store_skp += skill_cost[which_skill];
							}
							else {
								if ((store_g < skill_g_cost[which_skill]) || (store_skp < skill_cost[which_skill])) {
									if (store_g < skill_g_cost[which_skill])
										give_help(24,0,1010);
										else give_help(25,0,1010);
									SysBeep(2);
									}
									else {
										store_skills[which_skill] += 1;
										store_g -= skill_g_cost[which_skill];
										store_skp -= skill_cost[which_skill];
										}
							}

							update_gold_skills();
							cd_set_item_num(1010,54 + which_skill,store_skills[which_skill]);
							draw_xp_skills();
						}
				}	
				break;
			}
			
	store_train_pc = pc_num;
	if (talk_done == TRUE) {
		dialog_not_toast = FALSE;
		}
	return FALSE;
}
void update_gold_skills()
{
	cd_set_item_num(1010,47,((store_train_mode == 0) ? 0 : (short) store_g));
	cd_set_item_num(1010,46,(short) store_skp);
}
Boolean spend_xp(short pc_num, short mode, short parent)
//short mode; // 0 - create  1 - train
// returns 1 if cancelled
{
	Str255 get_text,text2;
	short item_hit;

	store_train_pc = pc_num;
	store_train_mode = mode;

	make_cursor_sword();

	cd_create_dialog_parent_num(1010,parent);
	sprintf((char *) get_text,"Health (%d/%d)",1,10);
	cd_add_label(1010,52,(char *) get_text,1075);
	sprintf((char *) get_text,"Spell Pts. (%d/%d)",1,15);
	//cd_add_label(1010,5,get_text,1040);
	cd_add_label(1010,53,(char *) get_text,1075);
	for (i = 54; i < 73; i++) {
		get_str(text2,9,1 + 2 * (i - 54));
		sprintf((char *) get_text,"%s (%d/%d)",text2,skill_cost[i - 54],skill_g_cost[i - 54]);
		cd_add_label(1010,i,(char *) get_text,(i < 63) ? 1075 : 1069);
		}
	do_xp_draw();
	
	dialog_answer = 0;
	
	if (party.help_received[10] == 0) {
		cd_initial_draw(1010);
		give_help(10,11,1010);
		}

#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		

	cd_kill_dialog(1010,0);

	return dialog_answer;
}


short mage_lore_total()
{
	short total = 0,i;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			total += adven[i].skills[11];
	
	return total;
}


Boolean poison_weapon( short pc_num, short how_much,short safe)
//short safe; // 1 - always succeeds
{
	short i,weap = 24,p_level,r1;
	short p_chance[21] = {40,72,81,85,88,89,90,
							91,92,93,94,94,95,95,96,97,98,100,100,100,100};
	
	for (i = 0; i < 24; i++)
		if ((adven[pc_num].equip[i] == TRUE) && (is_weapon(pc_num,i) == TRUE)) {
			weap = i;
			i = 30;
			}
	if (weap == 24) {
		add_string_to_buf("  No weapon equipped.       ");
		return FALSE;
		}
		else {
				p_level = how_much;
				add_string_to_buf("  You poison your weapon.       ");
				r1 = get_ran(1,0,100);
				// Nimble?
				if (adven[pc_num].traits[3] == FALSE)
					r1 -= 6;
				if ((r1 > p_chance[adven[pc_num].skills[17]]) && (safe == 0)) {
					add_string_to_buf("  Poison put on badly.         ");
					p_level = p_level / 2;
					r1 = get_ran(1,0,100);
					if (r1 > p_chance[adven[pc_num].skills[17]] + 10) {
						add_string_to_buf("  You nick yourself.          ");
						adven[pc_num].status[2] += p_level;
						}
					}
				if (safe != 1)
					play_sound(55);
				adven[pc_num].weap_poisoned = weap;
				adven[pc_num].status[0] = max (adven[pc_num].status[0],
					p_level);

				return TRUE;
			}
}

Boolean is_weapon(short pc_num,short item)
{
	if ((adven[pc_num].items[item].variety  == 1) ||
		(adven[pc_num].items[item].variety  == 2) ||
		(adven[pc_num].items[item].variety  == 5) ||
		(adven[pc_num].items[item].variety  == 24))
			return TRUE;
			else return FALSE;

}

void cast_spell(short type,short situation)
//short type; // 0 - mage  1 - priest
//short situation; // 0 - out  1 - town
{
	short spell;
	location loc;
	
	if ((is_town()) && (is_antimagic(c_town.p_loc.x,c_town.p_loc.y))) {
		add_string_to_buf("  Not in antimagic field.");
		return;
		}
		
		if (spell_forced == FALSE)
			spell = pick_spell(6, type, situation);
			else {
				if (repeat_cast_ok(type) == FALSE)
					return;
				spell = (type == 0) ? store_mage : store_priest;
				}
		if (spell < 70) {
			print_spell_cast(spell,type);

			if (type == 0)
				do_mage_spell(pc_casting,spell);
				else do_priest_spell(pc_casting,spell);
			put_pc_screen();
		
			}
}

Boolean repeat_cast_ok(short type)
{
	short store_select,who_would_cast,what_spell;

	if (overall_mode == 10)
		who_would_cast = current_pc;
		else if (overall_mode < 2)
			who_would_cast = pc_casting;
			else return FALSE;

	if (is_combat())
		what_spell = pc_last_cast[type][who_would_cast];
		else what_spell = (type == 0) ? store_mage : store_priest;

				if (pc_can_cast_spell(who_would_cast,type,what_spell) == FALSE) {
					add_string_to_buf("Repeat cast: Can't cast.");
					return FALSE;
					}
				store_select = (type == 0) ? mage_need_select[what_spell] : 
					priest_need_select[what_spell];
				if ((store_select > 0) && (store_spell_target == 6)) {
					add_string_to_buf("Repeat cast: No target stored.");
					return FALSE;
					}
				if ((store_select == 2) &&
					 ((adven[store_spell_target].main_status == 0) ||
						(adven[store_spell_target].main_status > 4))) {
					add_string_to_buf("Repeat cast: No target stored.");
					return FALSE;
					}
				if ((store_select == 1) &&
					 (adven[store_spell_target].main_status != 1)) {
					add_string_to_buf("Repeat cast: No target stored.");
					return FALSE;
					}

	return TRUE;

}

void give_party_spell(short which) ////
//which; // 100 + x : priest spell x
{
	short i;
	Boolean sound_done = FALSE;
	char str[60];
	
	if ((which < 0) || (which > 161) || ((which > 61) && (which < 100))) {
		give_error("The scenario has tried to give you a non-existant spell.","",0);
		 return;}

	if (which < 100)
		for (i = 0; i < 6; i++)
			if (adven[i].mage_spells[which] == FALSE) {
				adven[i].mage_spells[which] = TRUE;
				if (adven[i].main_status == 1)
					sprintf((char *) str,"%s learns spell.",adven[i].name);
				give_help(41,0,0);
				if (sound_done == FALSE) {sound_done = TRUE; play_sound(62);};
				}
	if (which >= 100)
		for (i = 0; i < 6; i++)
			if (adven[i].priest_spells[which - 100] == FALSE) {
				adven[i].priest_spells[which - 100] = TRUE;
				if (adven[i].main_status == 1)
					sprintf((char *) str,"%s learns spell.",adven[i].name);
				give_help(41,0,0);
				if (sound_done == FALSE) {sound_done = TRUE; play_sound(62);};
				}
}

void do_mage_spell(short pc_num,short spell_num)
{
	short i,j,item,target,r1,adj,store;
	location where;
	
	where = c_town.p_loc;
	play_sound(25);
	current_spell_range = 8;
	
	adj = stat_adj(who_cast,2);
	
	switch (spell_num) {
		case 0:  // Light
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			increase_light(50);
			break;
			
		case 6: // Identify
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			ASB("All of your items are identified.");
			for (i = 0; i < 6; i++)
				for (j = 0; j < 24; j++)
					adven[i].items[j].item_properties = adven[i].items[j].item_properties | 1;
			break;
		
		case 9: // true sight
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			for (where.x = 0; where.x < 64; where.x++)
				for (where.y = 0; where.y < 64; where.y++) 
					if (dist(where,c_town.p_loc) <= 2)
						make_explored(where.x,where.y);
			clear_map();
			break;
			
		case 16: // summon beast ////
			r1 = get_summon_monster(1);
			if (r1 < 0) break;
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			store = get_ran(3,1,4) + adj;
			if (summon_monster(r1,where,store,2) == FALSE)
				add_string_to_buf("  Summon failed.");
			break;
		case 26: // summon 1
			store = adven[who_cast].level / 5 + stat_adj(who_cast,2) / 3 + get_ran(1,0,2);
			j = minmax(1,7,store);
			r1 = get_summon_monster(1); ////
			if (r1 < 0) break;
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			store = get_ran(4,1,4) + adj;
			for (i = 0; i < j; i++)
				if (summon_monster(r1,where,store,2) == FALSE)
					add_string_to_buf("  Summon failed.");
			break;
		case 43: // summon 2
			store = adven[who_cast].level / 7 + stat_adj(who_cast,2) / 3 + get_ran(1,0,1);
			j = minmax(1,6,store);
			r1 = get_summon_monster(2); ////
			if (r1 < 0) break;
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			store = get_ran(5,1,4) + adj;
			for (i = 0; i < j; i++)
				if (summon_monster(r1,where,store,2) == FALSE)
					add_string_to_buf("  Summon failed.");
			break;
		case 58: // summon 3
			store = adven[who_cast].level / 10 + stat_adj(who_cast,2) / 3 + get_ran(1,0,1);
			j = minmax(1,5,store);
			r1 = get_summon_monster(3); ////
			if (r1 < 0) break;
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			store = get_ran(7,1,4) + stat_adj(who_cast,2);
			for (i = 0; i < j; i++)
				if (summon_monster(r1,where,store,2) == FALSE)
					add_string_to_buf("  Summon failed.");
			break;
		case 50:
			store = get_ran(5,1,4) + 2 * stat_adj(who_cast,2);
			if (summon_monster(85,where,store,2) == FALSE)
				add_string_to_buf("  Summon failed.");		
				else adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			break;

		case 18: // dispel field
			add_string_to_buf("  Target spell.               ");
			current_pat = square;
			overall_mode = 3;
			set_town_spell(spell_num,pc_num);		
			break;
			
		case 23:  // Long light
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];		
			increase_light(200);
			break;

		case 33:  // Magic map
			item = pc_has_abil(pc_num,158);////
			if (item == 24)
				add_string_to_buf("  You need a sapphire.        ");
				else if (c_town.town.specials2 % 10 == 1)
					add_string_to_buf("  The spell fails.                ");
				else {
					remove_charge(pc_num,item);
					adven[pc_num].cur_sp -= spell_cost[0][spell_num];
					add_string_to_buf("  As the sapphire dissolves,       ");
					add_string_to_buf("  you have a vision.               ");
					for (i = 0; i < 64; i++)
						for (j = 0; j < 64; j++) 
							make_explored(i,j);
					clear_map();
					}
			break;
	
	
		case 38:  // Stealth
			adven[pc_num].cur_sp -= spell_cost[0][spell_num];
			party.stuff_done[305][0] += max(6,adven[pc_num].level * 2);
			break;	

			
		case 7: case 20: case 34: case 41:  //  Scry monster, Unlock, disp. barrier, Capture SOul
			add_string_to_buf("  Target spell.               ");
			current_pat = single;
			overall_mode = 3;
			set_town_spell(spell_num,pc_num);
			break;		

		case 42: case 59: case 60: // fire and force barriers, quickfire
			add_string_to_buf("  Target spell.               ");
			overall_mode = 3;
			current_pat = single;
			set_town_spell(spell_num,pc_num);
			break;
					
		case 51: // antimagic
			add_string_to_buf("  Target spell.               ");
			overall_mode = 3;
			current_pat = rad2;
			set_town_spell(spell_num,pc_num);
			break;		

		case 53: // fly
			if (party.stuff_done[305][1] > 0) {
				add_string_to_buf("  Not while already flying.          ");
				return;
				}
			if (party.in_boat >= 0)
				add_string_to_buf("  Leave boat first.             "); 
			else if (party.in_horse >= 0) ////
				add_string_to_buf("  Leave horse first.             "); 
			else {
				adven[pc_num].cur_sp -= spell_cost[0][spell_num];
				add_string_to_buf("  You start flying!               ");
				party.stuff_done[305][1] = 3;
				}
			break;		

		case 29: case 57: //  resist magic  protection
				target = store_spell_target;
				if (target < 6)
					adven[pc_num].cur_sp -= spell_cost[0][spell_num];
				if ((spell_num == 57) && (target < 6)) {
						adven[target].status[4] += 2 + stat_adj(pc_num,2) + get_ran(2,1,2);
						for (i = 0; i < 6; i++)
							if (adven[i].main_status == 1) {
								adven[i].status[5] += 4 + adven[pc_num].level / 3 + stat_adj(pc_num,2);										
								}
						sprintf ((char *) c_line, "  Party protected.                         ");
					}
				if ((spell_num == 29) && (target < 6)) {
						adven[target].status[5] += 2 + stat_adj(pc_num,2) + get_ran(2,1,2);
						sprintf ((char *) c_line, "  %s protected.",adven[target].name);
					}
				add_string_to_buf((char *) c_line);	
			break;
		}
}

void do_priest_spell(short pc_num,short spell_num) ////
{
	short r1,r2, target, i,j,item,store,adj,x,y;
	location loc;
	location where,dest = {27,1};
	
	short store_victim_health,store_caster_health,targ_damaged; // for symbiosis	

	where = c_town.p_loc;

	adj = stat_adj(pc_num,2);

	play_sound(24);
	current_spell_range = 8;
	
	switch (spell_num) {
		case 4: ////
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];

					if (is_town()) {
					loc = (overall_mode == 0) ? party.p_loc : c_town.p_loc;
						sprintf ((char *) c_line, "  You're at: x %d  y %d.",
						(short) loc.x, (short) loc.y);
						}
					if (is_out()) {
						loc = (overall_mode == 0) ? party.p_loc : c_town.p_loc;
						x = loc.x; y = loc.y;
							x += 48 * party.outdoor_corner.x; y += 48 * party.outdoor_corner.y;
							sprintf ((char *) c_line, "  You're outside at: x %d  y %d.",x,y);

						}
					add_string_to_buf((char *) c_line);
			break;			
	
		case 7: case 25: // manna spells
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			store = adven[pc_num].level / 3 + 2 * stat_adj(who_cast,2) + get_ran(2,1,4);
			r1 = max(0,store);
			if (spell_num == 7)
				r1 = r1 / 3;
			sprintf ((char *) c_line, "  You gain %d food.   ",r1);
			add_string_to_buf((char *) c_line);	
			give_food(r1,TRUE);
			break;
			
		case 8: // Ritual - Sanctify
			add_string_to_buf("  Sanctify which space?               ");
			current_pat = single;
			overall_mode = 3;
			set_town_spell(100 + spell_num,pc_num);		
			break;
			
		case 13:
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			party.light_level += 210;
			break;
			
		case 15:
			store = stat_adj(who_cast,2);
			if (summon_monster(125,where,get_ran(2,1,4) + store,2) == FALSE)
				add_string_to_buf("  Summon failed.");		
				else adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			break;
		case 34:
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			r1 = adven[who_cast].level / 6 + stat_adj(who_cast,2) / 3 + get_ran(1,0,1);
			for (i = 0; i < r1; i++) {
				r2 = get_ran(1,0,7);
				store = get_ran(2,1,5) + stat_adj(who_cast,2);
				if (summon_monster((r2 == 1) ? 100 : 99,where,store,2 ) == FALSE)
					add_string_to_buf("  Summon failed.");
				}	
			break;
		case 43:
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			store = get_ran(2,1,4) + stat_adj(who_cast,2);
			if (summon_monster(126,where,store,2) == FALSE)
				add_string_to_buf("  Summon failed.");	
			for (i = 0; i < 4; i++)	{
				store = get_ran(2,1,4) + stat_adj(who_cast,2);
				if (summon_monster(125,where,store,2) == FALSE)
					add_string_to_buf("  Summon failed.");		
				}
			break;
		case 50:
			store = get_ran(6,1,4) + stat_adj(who_cast,2);
			if (summon_monster(122,where,store,2) == FALSE)
				add_string_to_buf("  Summon failed.");		
				else adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			break;

		case 16:
			add_string_to_buf("  Destroy what?               ");
			current_pat = (spell_num == 16) ? single : square;
			overall_mode = 3;
			set_town_spell(100 + spell_num,pc_num);		
			break;

		case 45: // dispelling fields
			add_string_to_buf("  Target spell.               ");
			current_pat = (spell_num == 19) ? single : rad2;
			overall_mode = 3;
			set_town_spell(100 + spell_num,pc_num);		
			break;

		case 23: // Detect life
			add_string_to_buf("  Monsters now on map.                ");
			party.stuff_done[305][2] += 6 + get_ran(1,0,6);
			clear_map();
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			break;
		case 37: // firewalk
			add_string_to_buf("  You are now firewalking.                ");
			party.stuff_done[305][3] += adven[pc_num].level / 12 + 2;
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			break;
			
		case 44: // shatter
			add_string_to_buf("  You send out a burst of energy. ");
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			for (loc.x = where.x - 1;loc.x < where.x + 2; loc.x++)
				for (loc.y = where.y - 1;loc.y < where.y + 2; loc.y++)
					crumble_wall(loc);
			update_explored(c_town.p_loc);
			break;
			
		case 60:
			if (overall_mode > 0) {
				add_string_to_buf("  Can only cast outdoors. ");
				return;
				}
			if (party.in_boat >= 0) {
				add_string_to_buf("  Not while in boat. ");
				return;			
				}
			if (party.in_horse >= 0) {////
				add_string_to_buf("  Not while on horseback. ");
				return;			
				}
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];
			add_string_to_buf("  You are moved... ");
			force_town_enter(scenario.which_town_start,scenario.where_start);
			start_town_mode(scenario.which_town_start,9);
			position_party(scenario.out_sec_start.x,scenario.out_sec_start.y,
				scenario.out_start.x,scenario.out_start.y);
			center = c_town.p_loc = scenario.where_start;
//			overall_mode = 0;
//			center = party.p_loc;
//			update_explored(party.p_loc);
			redraw_screen(0);
			break;
			
		case 1: case 20: case 39:  case 2: case 11: case 27: case 28: case 36: case 19: case 24:
//			target = select_pc(11,0);
			target = store_spell_target;
			if (target < 6) {
				adven[pc_num].cur_sp -= spell_cost[1][spell_num];
				switch(spell_num) {
					case 1: case 20: case 39:		
						r1 = get_ran(2 + 2 * (spell_num / 6), 1, 4);
						sprintf ((char *) c_line, "  %s healed %d.   ",
						(char *) adven[target].name,r1);
						heal_pc(target,r1);
					one_sound(52);
					break;

					case 2: case 11:
						sprintf ((char *) c_line, "  %s cured.    "
							,(char *) adven[target].name);
						r1 = ((spell_num == 2) ? 1 : 3) + get_ran(1,0,2) + stat_adj(pc_num,2) / 2;
						cure_pc(target,r1);
					break;
					
					case 19: // awaken
						if (adven[target].status[11] <= 0) {
							sprintf ((char *) c_line, "  %s is already awake!    "
								,(char *) adven[target].name);
							break;
							}
						sprintf ((char *) c_line, "  %s wakes up.    "
							,(char *) adven[target].name);
						adven[target].status[11] = 0;
					break;
					case 24: // cure paralysis
						if (adven[target].status[12] <= 0) {
							sprintf ((char *) c_line, "  %s isn't paralyzed!    "
								,(char *) adven[target].name);
							break;
							}
						sprintf ((char *) c_line, "  %s can move now.    "
							,(char *) adven[target].name);
						adven[target].status[12] = 0;
					break;

					case 27:
						sprintf ((char *) c_line, "  %s recovers.      "
							,(char *) adven[target].name);
						r1 = 2 + get_ran(1,0,2) + stat_adj(pc_num,2) / 2;
						adven[target].status[7] = max(0,adven[target].status[7] - r1);
						break;

					case 28:
						sprintf ((char *) c_line, "  %s restored.      "
							,(char *) adven[target].name);
						r1 = 1 + get_ran(1,0,2) + stat_adj(pc_num,2) / 2;
						adven[target].status[9] = max(0,adven[target].status[9] - r1);
						break;
						
					case 36:
						sprintf ((char *) c_line, "  %s cleansed.      "
							,(char *) adven[target].name);
						adven[target].status[7] = 0;
						adven[target].status[6] = 0;					
						break;
					}
				}
				add_string_to_buf((char *) c_line);	
				put_pc_screen();					
			break;

		case 47: case 49: case 40: case 56: case 33: case 5: case 6: case 35:
			target = store_spell_target;

			if (target < 6) {
				if ((spell_num == 6) && (target == pc_num)) { // check symbiosis
					add_string_to_buf("  Can't cast on self.");
					return;
					}					
			
				adven[pc_num].cur_sp -= spell_cost[1][spell_num]; 
				if (spell_num == 35) { // martyr's shield
						sprintf ((char *) c_line, "  %s shielded.         ",
							(char *) adven[target].name);
						r1 = max(1,get_ran((adven[pc_num].level + 5) / 5,1,3) + adj);
						adven[target].status[10] += r1;
					}
				if (spell_num == 5) { // sanctuary
						sprintf ((char *) c_line, "  %s hidden.         ",
							(char *) adven[target].name);
						r1 = max(0,get_ran(0,1,3) + adven[pc_num].level / 4 + adj);
						adven[target].status[8] += r1;
					}
				if (spell_num == 6) { // symbiosis
					store_victim_health = adven[target].cur_health;
					store_caster_health = adven[pc_num].cur_health;
					targ_damaged = adven[target].max_health - adven[target].cur_health;
					while ((targ_damaged > 0) && (adven[pc_num].cur_health > 0)) {
						adven[target].cur_health++;
						r1 = get_ran(1,0,100) + adven[pc_num].level / 2 + 3 * adj;
						if (r1 < 100)
							adven[pc_num].cur_health--;
						if (r1 < 50)
							adven[pc_num].cur_health = move_to_zero(adven[pc_num].cur_health);
						targ_damaged = adven[target].max_health - adven[target].cur_health;
						}
					add_string_to_buf("  You absorb damage.");
					sprintf ((char *) c_line, "  %s healed %d.         ", (char *) adven[target].name,
						adven[target].cur_health - store_victim_health);
					add_string_to_buf ((char *) c_line);
					sprintf ((char *) c_line, "  %s takes %d.         ", (char *) adven[pc_num].name,
						store_caster_health - adven[pc_num].cur_health);
					}
				if (spell_num == 47) {
						sprintf ((char *) c_line, "  %s healed.         ",
							(char *) adven[target].name);
						heal_pc(target,250);
						adven[target].status[2] = 0;
						one_sound(-53); one_sound(52);
					}
				if (spell_num == 49) {
						if (adven[target].main_status == 4) {
							adven[target].main_status = 1;
							sprintf ((char *) c_line, "  %s destoned.                                  ",
									(char *) adven[target].name);							
							play_sound(53);
							}
							else sprintf ((char *) c_line,"  Wasn't stoned.              ");				
					}
				if (spell_num == 33) {
						for (i = 0; i < 24; i++) 
							if (is_cursed(adven[target].items[i]) == TRUE){
									r1 = get_ran(1,0,200) - 10 * stat_adj(pc_num,2);
									if (r1 < 60) {
										adven[target].items[i].item_properties = 
											adven[target].items[i].item_properties & 239;
										}
								}
						play_sound(52);
						sprintf ((char *) c_line,"  Your items glow.     ");				
					}

				if ((spell_num == 40) || (spell_num == 56))
					if ((item = pc_has_abil(pc_num,160)) == 24) {
						add_string_to_buf("  Need resurrection balm.        ");
						spell_num = 500;
						}
						else take_item(pc_num,item);
				if (spell_num == 40) {
						if (adven[target].main_status == 2) 
								if (get_ran(1,1,adven[pc_num].level / 2) == 1) {
										sprintf ((char *) c_line, "  %s now dust.                          ",
											(char *) adven[target].name);									
										play_sound(5);
										adven[target].main_status = 3;								
									}
									else {
									adven[target].main_status = 1;
									for (i = 0; i < 3; i++)
										if (get_ran(1,0,2) < 2)
											adven[target].skills[i] -= (adven[target].skills[i] > 1) ? 1 : 0;
									adven[target].cur_health = 1;
									sprintf ((char *) c_line, "  %s raised.                          ",
											(char *) adven[target].name);					
									play_sound(52);
									}						
							else sprintf ((char *) c_line,"  Didn't work.              ");					
				
					}
				if (spell_num == 56) {
						if (adven[target].main_status != 1) {
							adven[target].main_status = 1;
							for (i = 0; i < 3; i++)
								if (get_ran(1,0,2) < 1)
									adven[target].skills[i] -= (adven[target].skills[i] > 1) ? 1 : 0;
							adven[target].cur_health = 1;
							sprintf ((char *) c_line, "  %s raised.",
									(char *) adven[target].name);							
							play_sound(52);					
							}
							else sprintf ((char *) c_line,"  Was OK.              ");				
					}
				add_string_to_buf((char *) c_line);	
				put_pc_screen();					
			}
			break;	

		case 21: case 46: case 54:
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];			
			r1 = get_ran(spell_num / 7 + adj, 1, 4);
			if (spell_num < 54) {
				sprintf ((char *) c_line, "  Party healed %d.       ", r1);
				add_string_to_buf((char *) c_line);	
				heal_party(r1);	
				play_sound(52);
				}
				else if (spell_num == 54) {
				sprintf ((char *) c_line, "  Party revived.     ");
				add_string_to_buf((char *) c_line);	
				r1 = r1 * 2;
				heal_party(r1);
				play_sound(-53);	
				play_sound(-52);
				cure_party(3 + adj);
				}
			break;
			
		case 30:
				adven[pc_num].cur_sp -= spell_cost[1][spell_num];			
				sprintf ((char *) c_line, "  Party cured.  ");
				add_string_to_buf((char *) c_line);		
				cure_party(3 + stat_adj(pc_num,2));
			break;
			
		case 42: case 61: case 48:
			adven[pc_num].cur_sp -= spell_cost[1][spell_num];	
			switch (spell_num) {
				case 42: add_string_to_buf("  Party hidden.");break;
				case 61: add_string_to_buf("  Party cleansed.");break;
				case 48: add_string_to_buf("  Party is now really, REALLY awake.");break;
				}		

			for (i = 0; i < 6; i++)
				if (adven[i].main_status == 1) {
					if (spell_num == 42) {
						store = get_ran(0,1,3) + adven[pc_num].level / 6 + stat_adj(pc_num,2);
						r1 = max(0,store);
						adven[i].status[8] += r1;					
						}
					if (spell_num == 61) {
						adven[i].status[6] = 0;
						adven[i].status[7] = 0;	
						}
					if (spell_num == 48) { // Hyperactivity
						adven[i].status[11] -= 6 + 2 * stat_adj(pc_num,2);
						adven[i].status[3] = max(0,adven[i].status[3]);
						}
				}
			break;
	}
}

void cast_town_spell(location where) ////
{
	short adjust,r1,i,j,targ,store;
	location loc;
	unsigned char ter;
	
	if ((where.x <= c_town.town.in_town_rect.left) ||
		(where.x >= c_town.town.in_town_rect.right) ||
		(where.y <= c_town.town.in_town_rect.top) ||
		(where.y >= c_town.town.in_town_rect.bottom)) {
			add_string_to_buf("  Can't target outside town.");
			return;
			}
			
	adjust = can_see(c_town.p_loc,where,0);
	if (town_spell < 1000) 
		adven[who_cast].cur_sp -= spell_cost[town_spell / 100][town_spell % 100];
		else town_spell -= 1000;
	ter = t_d.terrain[where.x][where.y];
	
	if (adjust > 4)
		add_string_to_buf("  Can't see target.       ");
		else switch (town_spell) {
			case 7: case 34: // Scry, Capture Soul
				targ = monst_there(where);
				if (targ < T_M) {
					if (town_spell == 7) {
						party.m_seen[c_town.monst.dudes[targ].number] = TRUE;
						adjust_monst_menu();
						display_monst(0,&c_town.monst.dudes[targ],0);
						}
						else record_monst(&c_town.monst.dudes[targ]);
					}
					else add_string_to_buf("  No monster there.");
				break;
				
			case 119: case 145: case 18:
				add_string_to_buf("  You attempt to dispel.              ");
				place_spell_pattern(current_pat,where,11,FALSE,7);
				break;
			case 116: // Move M.
				add_string_to_buf("  You blast the area.              ");
				crumble_wall(where);
				update_explored(c_town.p_loc);
				break;
			case 42:
				if ((get_obscurity(where.x,where.y) == 5) || (monst_there(where) < 90)) {
					add_string_to_buf("  Target space obstructed.");
					break;
					}
				make_fire_barrier(where.x,where.y);
				if (is_fire_barrier(where.x,where.y))
					add_string_to_buf("  You create the barrier.              ");
					else add_string_to_buf("  Failed.");
				break;		
			case 59:
				if ((get_obscurity(where.x,where.y) == 5) || (monst_there(where) < 90)) {
					add_string_to_buf("  Target space obstructed.");
					break;
					}
				make_force_barrier(where.x,where.y);
				if (is_force_barrier(where.x,where.y))
					add_string_to_buf("  You create the barrier.              ");
					else add_string_to_buf("  Failed.");				
				break;		
			case 60:
				make_quickfire(where.x,where.y);
				if (is_quickfire(where.x,where.y))
					add_string_to_buf("  You create quickfire.              ");
					else add_string_to_buf("  Failed.");				
				break;

			case 51: // am cloud
				add_string_to_buf("  You create an antimagic cloud.              ");
				for (loc.x = 0; loc.x < town_size[town_type]; loc.x++)
					for (loc.y = 0; loc.y < town_size[town_type]; loc.y++) 
						if ((dist(where,loc) <= 2) && (can_see(where,loc,2) < 5) &&
						((a_v(loc.x - where.x) < 2) || (a_v(loc.y - where.y) < 2)))
							make_antimagic(loc.x,loc.y);
				break;

			case 108: // RItual - sanctify
				sanctify_space(where);
				break;
				
			case 20:
				switch (scenario.ter_types[ter].special) { ////
					case 9: case 10:
						r1 = get_ran(1,0,100) - 5 * stat_adj(who_cast,2) + 5 * c_town.difficulty;
						r1 += scenario.ter_types[ter].flag2 * 7;
						if (scenario.ter_types[ter].flag2 == 10)
							r1 = 10000;
						if (r1 < (135 - combat_percent[min(19,adven[who_cast].level)])) {
							add_string_to_buf("  Door unlocked.                 ");
							play_sound(9);
							t_d.terrain[where.x][where.y] = scenario.ter_types[ter].flag1;
							}
							else {
								play_sound(41);
								add_string_to_buf("  Didn't work.                  ");
								}
						break;

					default:
						add_string_to_buf("  Wrong terrain type.               ");
						break;
					}
				break;
				
			case 41:
				if ((is_fire_barrier(where.x,where.y)) || (is_force_barrier(where.x,where.y))) {
						r1 = get_ran(1,0,100) - 5 * stat_adj(who_cast,2) + 5 * (c_town.difficulty / 10);
						if (is_fire_barrier(where.x,where.y))
							r1 -= 8;
						if (r1 < (120 - combat_percent[min(19,adven[who_cast].level)])) {
							add_string_to_buf("  Barrier broken.                 ");
							take_fire_barrier(where.x,where.y);
							take_force_barrier(where.x,where.y);
								
							// Now, show party new things
							update_explored(c_town.p_loc);
							}
							else {
								store = get_ran(1,0,1);
								play_sound(41);
								add_string_to_buf("  Didn't work.                  ");
								}
					}

						else add_string_to_buf("  No barrier there.");

				break;
	
	}
}

void sanctify_space(location where)
{
	short r1,i,j,s1,s2,s3;
	location l = {25,13};

		for (i = 0; i < 50; i++)
			if ((same_point(where,c_town.town.special_locs[i]) == TRUE) &&
				(c_town.town.spec_id[i] >= 0)) {
				if (c_town.town.specials[c_town.town.spec_id[i]].type == 24) 
					run_special(16,2,c_town.town.spec_id[i],where,&s1,&s2,&s3);
				return;
				}
	add_string_to_buf("  Nothing happens.");
}

void crumble_wall(location where)
{
	unsigned char ter,blastable[9] = {111,112,113,128,129, 130,143,144,145};
	short i;
	
	if (loc_off_act_area(where) == TRUE)
		return;
	ter = t_d.terrain[where.x][where.y];
	if (scenario.ter_types[ter].special == 7) {
			play_sound(60);
				t_d.terrain[where.x][where.y] = scenario.ter_types[ter].flag1;
			add_string_to_buf("  Barrier crumbles.");	
		}

}

void do_mindduel(short pc_num,creature_data_type *monst)
{
	short i = 0,adjust,r1,r2,balance = 0;
	
	adjust = (adven[pc_num].level + adven[pc_num].skills[2]) / 2 - monst->m_d.level * 2;
	if ((i = get_prot_level(pc_num,53)) > 0)
		adjust += i * 5;
	if (monst->attitude % 2 != 1)
		make_town_hostile();
	monst->attitude = 1;
	
	add_string_to_buf("Mindduel!");
	while ((adven[pc_num].main_status == 1) && (monst->active > 0) && (i < 10)) {
		play_sound(1);
		r1 = get_ran(1,0,100) + adjust;
		r1 += 5 * (monst->m_d.status[9] - adven[pc_num].status[9]);
		r1 += 5 * balance;
		r2 = get_ran(1,1,6);
		if (r1 < 30) {
			sprintf((char *)c_line, "  %s is drained %d.",adven[pc_num].name,r2);
			add_string_to_buf((char *) c_line);
			monst->m_d.mp += r2;
			balance++;
			if (adven[pc_num].cur_sp == 0) {
				adven[pc_num].status[9] += 2;
				sprintf((char *) c_line,"  %s is dumbfounded.",adven[pc_num].name);
				add_string_to_buf((char *) c_line);
				if (adven[pc_num].status[9] > 7) {
					sprintf((char *) c_line,"  %s is killed!",adven[pc_num].name);
					add_string_to_buf((char *) c_line);
					kill_pc(pc_num,2);
					}
					
				}
				else {
					adven[pc_num].cur_sp = max(0,adven[pc_num].cur_sp - r2);
					}
			}
		if (r1 > 70) {
			sprintf((char *)c_line, "  %s drains %d.",adven[pc_num].name,r2);
			add_string_to_buf((char *) c_line);
			adven[pc_num].cur_sp += r2;
			balance--;
			if (monst->m_d.mp == 0) {
				monst->m_d.status[9] += 2;
				monst_spell_note(monst->number,22);
				if (monst->m_d.status[9] > 7) {
					kill_monst(monst,pc_num);
					}
					
				}
				else {
					monst->m_d.mp = max(0,monst->m_d.mp - r2);
					}
			
		
			}
		print_buf();
		i++;
		}
}

// mode 0 - dispel spell, 1 - always take  2 - always take and take fire and force too
void dispel_fields(short i,short j,short mode)
{
	short r1;
	
	if (mode == 2) {
		take_fire_barrier(i,j);
		take_force_barrier(i,j);
		take_barrel(i,j);
		take_crate(i,j);
		take_web(i,j);
		}
	if (mode >= 1)
		mode = -10;
	take_fire_wall(i,j);
	take_force_wall(i,j);
	take_scloud(i,j);
	r1 = get_ran(1,1,6) + mode;
	if (r1 <= 1)
		take_web(i,j);
	r1 = get_ran(1,1,6) + mode;
	if (r1 < 6)
		take_ice_wall(i,j);
	r1 = get_ran(1,1,6) + mode;
	if (r1 < 5)
		take_sleep_cloud(i,j);
	r1 = get_ran(1,1,8) + mode;
	if (r1 <= 1)
		take_quickfire(i,j);
	r1 = get_ran(1,1,7) + mode;
	if (r1 < 5)
		take_blade_wall(i,j);
}

Boolean pc_can_cast_spell(short pc_num,short type,short spell_num)
//short type;  // 0 - mage  1 - priest
{
	short level,store_w_cast;
	
	level = spell_level[spell_num];
	
	if ((spell_num < 0) || (spell_num > 61))
		return FALSE;
	if (adven[pc_num].skills[9 + type] < level)
		return FALSE;
	if (adven[pc_num].main_status != 1)
		return FALSE;
	if (adven[pc_num].cur_sp < spell_cost[type][spell_num])
		return FALSE;
	if ((type == 0) && (adven[pc_num].mage_spells[spell_num] == FALSE))
		return FALSE;
	if ((type == 1) && (adven[pc_num].priest_spells[spell_num] == FALSE))
		return FALSE;
	if (adven[pc_num].status[9] >= 8 - level)
		return FALSE;	
	if (adven[pc_num].status[12] != 0)
		return FALSE;	
	if (adven[pc_num].status[11] > 0)
		return FALSE;	
	
// 0 - everywhere 1 - combat only 2 - town only 3 - town & outdoor only 4 - town & combat only  5 - outdoor only
	store_w_cast = spell_w_cast[type][spell_num];
		if (is_out()) {
			if ((store_w_cast == 1) || (store_w_cast == 2) || (store_w_cast == 4)) {
						return FALSE;
						}
				}		
		if (is_town()) {
			if ((store_w_cast == 1) || (store_w_cast == 5)) {
						return FALSE;
						}
				}
		if (is_combat()) {
			if ((store_w_cast == 2) || (store_w_cast == 3) || (store_w_cast == 5)) {
						return FALSE;
						}
				}
	return TRUE;
}



void draw_caster_buttons()
{
	short i;
	
	if (can_choose_caster == FALSE) {
		for (i = 0; i < 6; i++) {
			if (i == pc_casting) {
				cd_activate_item(1098,4 + i,1);
				}
				else {
					cd_activate_item(1098,4 + i,0);
					}
			}
		}
		else {
			for (i = 0; i < 6; i++) {
				if (pc_can_cast_spell(i,store_situation,store_situation) == TRUE) {
					cd_activate_item(1098,4 + i,1);
					}
					else {
						cd_activate_item(1098,4 + i,0);
						}
				}
			}


}

void draw_spell_info()
{

	char string[256];				

	if (((store_situation == 0) && (store_mage == 70)) ||
		((store_situation == 1) && (store_priest == 70))) {	 // No spell selected
			for (i = 0; i < 6; i++) {
				cd_activate_item(1098,10 + i,0);
				}
				
			}
			else { // Spell selected

				for (i = 0; i < 6; i++) {
					switch (((store_situation == 0) ?
						mage_need_select[store_mage] : priest_need_select[store_priest])) {
					case 0:
						cd_activate_item(1098,10 + i,0);
						break;
					case 1:
						if (adven[i].main_status != 1) {
							cd_activate_item(1098,10 + i,0);
							}
							else {
								cd_activate_item(1098,10 + i,1);
								}
						break;
					case 2:
						if (adven[i].main_status > 0) {
							cd_activate_item(1098,10 + i,1);
							}
							else {
								cd_activate_item(1098,10 + i,0);
								}
						break;
								 
					}
								 	
			 		}
				}
}

void draw_spell_pc_info()
{
	short i;
	char string[256];

	for (i = 0; i < 6; i++) {
		if (adven[i].main_status != 0) {
				cd_set_item_text(1098,18 + i,adven[i].name);
				//if (pc_casting == i)
				//	cd_text_frame(1098,24 + store_spell_target,11);
				//	else cd_text_frame(1098,24 + store_spell_target,1);

				if (adven[i].main_status == 1) {
					cd_set_item_num(1098,24 + i, adven[i].cur_health);
					cd_set_item_num(1098,30 + i, adven[i].cur_sp);			
					}
			}		
		}
}


void put_pc_caster_buttons()
{

	short item_hit,what_talk_field,i;

	for (i = 0; i < 6; i++) 
		if (cd_get_active(1098,i + 4) > 0) {
		if (i == pc_casting)
			cd_text_frame(1098,i + 18,11);
			else cd_text_frame(1098,i + 18,1);
		}
}
void put_pc_target_buttons()
{
	short item_hit,what_talk_field,i;

	if (store_spell_target < 6) {
		cd_text_frame(1098,24 + store_spell_target,11);
		cd_text_frame(1098,30 + store_spell_target,11);
		}
	if ((store_last_target_darkened < 6) && (store_last_target_darkened != store_spell_target)) {
		cd_text_frame(1098,24 + store_last_target_darkened,1);
		cd_text_frame(1098,30 + store_last_target_darkened,1);
		}
	store_last_target_darkened = store_spell_target;
}

void put_spell_led_buttons()
{
	short item_hit,what_talk_field,i,spell_for_this_button;

	for (i = 0; i < 38; i++) {
		spell_for_this_button = (on_which_spell_page == 0) ? i : spell_index[i];
	
		if (spell_for_this_button < 90) {
			if (((store_situation == 0) && (store_mage == spell_for_this_button)) ||
			 ((store_situation == 1) && (store_priest == spell_for_this_button))) {
				cd_set_led(1098,i + 37,2);
			 }
			else if (pc_can_cast_spell(pc_casting,store_situation,spell_for_this_button) == TRUE) {
				cd_set_led(1098,i + 37,1);
				}
				else {
					cd_set_led(1098,i + 37,0);
					}
			}
		}

}

void put_spell_list()
{

	short item_hit,what_talk_field,i,j;
	Str255 add_text;

	if (on_which_spell_page == 0) {
		csit(1098,80,"Level 1:");
		csit(1098,81,"Level 2:");
		csit(1098,82,"Level 3:");
		csit(1098,83,"Level 4:");
		for (i = 0; i < 38; i++) {
			if (store_situation == 0) {
				if (i == 35)
					sprintf((char *) add_text,"%s %c ?",mage_s_name[i],
					(char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i));
					else sprintf((char *) add_text,"%s %c %d",mage_s_name[i],
					(char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i),spell_cost[0][i]);
				}
				else sprintf((char *) add_text,"%s %c %d",priest_s_name[i],
				(char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i),spell_cost[1][i]);
			//for (j = 0; j < 30; i++)
			//	if (add_text[j] == '&')
			//		add_text[j] = (char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i);
			cd_add_label(1098,37 + i,(char *) add_text,53);
			if (spell_index[i] == 90)
				cd_activate_item(1098,37 + i,1);
			}
		}
		else {
			csit(1098,80,"Level 5:");
			csit(1098,81,"Level 6:");
			csit(1098,82,"Level 7:");
			csit(1098,83,"");
			for (i = 0; i < 38; i++) 
				if (spell_index[i] < 90) {
					if (store_situation == 0)
						sprintf((char *) add_text,"%s %c %d",mage_s_name[spell_index[i]],
						(char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i),spell_cost[0][spell_index[i]]);
						else sprintf((char *) add_text,"%s %c %d",priest_s_name[spell_index[i]],
						(char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i),spell_cost[1][spell_index[i]]);
					cd_add_label(1098,37 + i,(char *) add_text,53);
					}
					else cd_activate_item(1098,37 + i,0);
			}
}


void put_pick_spell_graphics()
{
	short i;


	put_spell_list();
	put_pc_caster_buttons();
	put_pc_target_buttons();
	put_spell_led_buttons();
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			draw_pc_effects(10 + i);
}

void pick_spell_event_filter (short item_hit)
{
	char *choose_target = " Now pick a target.";
	char *no_target = " No target needed.";
	char *bad_target = " Can't cast on him/her.";
	char *got_target = " Target selected.";
	char *bad_spell = " Spell not available.";
	Boolean spell_toast = FALSE,dialog_done = FALSE;
	
		switch (item_hit) {
			case 4: case 5: case 6: case 7: case 8: case 9: // pick caster
			if (cd_get_active(1098,item_hit) == 1) {
				pc_casting = item_hit - 4;
				if (pc_can_cast_spell(pc_casting,store_situation,
					((store_situation == 0) ? store_mage : store_priest)) == FALSE) {
						if (store_situation == 0)
							store_mage = 70;
							else store_priest = 70;
						store_spell_target = 6;
					}
					draw_spell_info();
               draw_spell_pc_info();
					put_spell_led_buttons();
					put_pc_caster_buttons();
					put_pc_target_buttons();
					}
				break;
			case 10: case 11: case 12: case 13: case 14: case 15:  // pick target
				if (cd_get_active(1098,10 + pc_casting) == FALSE) {
					cd_set_item_text(1098,36,no_target);				
					}
					else if (cd_get_active(1098,item_hit) == FALSE) {
					cd_set_item_text(1098,36,bad_target);				
					}
					else {
				
						cd_set_item_text(1098,36,got_target);				
						store_spell_target = item_hit - 10;
						draw_spell_info();		
						put_pc_target_buttons();
						}		
				break;

			case 16: // cancel
				spell_toast = TRUE;
				dialog_done = TRUE;
				break;
			case 1: case 17:  // cast!
				dialog_done = TRUE;
				break;
				
			case 75: // other spells
				on_which_spell_page = 1 - on_which_spell_page;
				put_spell_list();
				put_spell_led_buttons();
				break;
			
			case 79: // help
				party.help_received[7] = 0;
				give_help(207,8,1098);
				break;
			
			case 100:
				break;

			default:
				if (item_hit >= 100) {
					item_hit -= 100;
					i = (on_which_spell_page == 0) ? item_hit - 37 : spell_index[item_hit - 37];
					display_spells(store_situation,i,1098);
					}
				else if (cd_get_led(1098,item_hit) == 0) {
							cd_set_item_text(1098,36,bad_spell);				
				}
					else {

						if (store_situation == 0)
							store_mage = (on_which_spell_page == 0) ? item_hit - 37 : spell_index[item_hit - 37];
							else store_priest = (on_which_spell_page == 0) ? item_hit - 37 : spell_index[item_hit - 37];				
							draw_spell_info();	
							put_spell_led_buttons();
	
							if (store_spell_target < 6) {
								if (cd_get_active(1098,10 + store_spell_target) == FALSE) {
									store_spell_target = 6;
									draw_spell_info();
									put_pc_target_buttons();
									}
								}
						// Cute trick now... if a target is needed, caster can always be picked
						if ((store_spell_target == 6) && (cd_get_active(1098,10 + pc_casting) == 1)) {
							cd_set_item_text(1098,36,choose_target);				
							draw_spell_info();
							force_play_sound(45);
							}
							else if (cd_get_active(1098,10 + pc_casting) == 0) {
								store_spell_target = 6;
								put_pc_target_buttons();
								}

						}			
				break;
			}
	if (dialog_done == TRUE) {
	if (spell_toast == TRUE) {
			store_mage = store_mage_store;
			store_priest = store_priest_store;
			store_spell_target = store_store_target ;
			if (store_situation == 0)
				store_last_cast_mage = pc_casting;
				else store_last_cast_priest = pc_casting;
			dialog_not_toast = FALSE;
			dialog_answer = 70;
			return;
		}

	if (((store_situation == 0) && (store_mage == 70)) ||
		((store_situation == 1) && (store_priest == 70))) {
			add_string_to_buf("Cast: No spell selected.");
			store_mage = store_mage_store;
			store_priest = store_priest_store;
			store_spell_target = store_store_target ;
			dialog_not_toast = FALSE;
			dialog_answer = 70;
			return;
			}
	if ((store_situation == 0) && (mage_need_select[store_mage] == 0)) {
			store_last_cast_mage = pc_casting;
			pc_last_cast[store_situation][pc_casting] = store_mage;
			dialog_not_toast = FALSE;
			dialog_answer = store_mage;
			return;
		}
	if ((store_situation == 1) && (priest_need_select[store_priest] == 0)) {
			store_last_cast_priest = pc_casting;
			pc_last_cast[store_situation][pc_casting] = store_priest;
			dialog_not_toast = FALSE;
			dialog_answer = store_priest;
			return;
		}
	if (store_spell_target == 6) {
		add_string_to_buf("Cast: Need to select target.");
		store_mage = store_mage_store;
		store_priest = store_priest_store;
		store_spell_target = store_store_target ;
		dialog_not_toast = FALSE;
		give_help(39,0,1098);
		dialog_answer = 70;
			return;
		}
	item_hit = ((store_situation == 0) ? store_mage : store_priest);
	if (store_situation == 0)
		store_last_cast_mage = pc_casting;
		else store_last_cast_priest = pc_casting;
	pc_last_cast[store_situation][pc_casting] = ((store_situation == 0) ? store_mage : store_priest);
	dialog_not_toast = FALSE;
	dialog_answer = item_hit;
   }
	
}

short pick_spell(short pc_num,short type,short situation)  // 70 - no spell OW spell num
//short pc_num; // if 6, anyone
//short type; // 0 - mage   1 - priest
//short situation; // 0 - out  1 - town  2 - combat
{
	short item_hit;
	
	store_mage_store = store_mage;
	store_priest_store = store_priest;
	store_store_target = store_spell_target;
	store_situation = type;
	store_last_target_darkened = 6;
	can_choose_caster = (pc_num < 6) ? FALSE : TRUE;

	pc_casting = (type == 0) ? store_last_cast_mage : store_last_cast_priest;
	if (pc_casting == 6)
		pc_casting = current_pc;
	
	if (pc_num == 6) { // See if can keep same caster
		can_choose_caster = TRUE;
		if (pc_can_cast_spell(pc_casting,type,type) == FALSE) {
			for (i = 0; i < 6; i++)
				if (pc_can_cast_spell(i,type,type)) {
					pc_casting = i;
					i = 500;
					}
				if (i == 6) {
					add_string_to_buf("Cast: Nobody can.");
					return 70;
					}
				}
		}
		else {
			can_choose_caster = FALSE;
			pc_casting = pc_num;
			}

	if (can_choose_caster == FALSE) {
		if ((type == 0) && (adven[pc_num].skills[9] == 0)) {
			add_string_to_buf("Cast: No mage skill.");
			return 70;
			}
		if ((type == 1) && (adven[pc_num].skills[10] == 0)) {
			add_string_to_buf("Cast: No priest skill.");
			return 70;
			}
		if (adven[pc_casting].cur_sp == 0) {
			add_string_to_buf("Cast: No spell points.");
			return 70;
			}
				
		}

	// If in combat, make the spell being cast this PCs most recent spell
	if (is_combat()) {
		if (type == 0)
			store_mage = pc_last_cast[0][pc_casting];
			else store_priest = pc_last_cast[1][pc_casting];
		}

	// Keep the stored spell, if it's still castable
	if (pc_can_cast_spell(pc_casting,type,((type == 0) ? store_mage : store_priest)) == FALSE) {
		if (type == 0) {
				store_mage = 0;
				store_mage_lev = 1;
			}
			else {
				store_priest = 1;
				store_priest_lev = 1;
			}
		}

	// If a target is needed, keep the same target if that PC still targetable
	if (((type == 0) && (mage_need_select[store_mage] > 0)) ||
		((type == 1) && (priest_need_select[store_priest] > 0))) {
		if (adven[store_spell_target].main_status != 1)
			store_spell_target = 6;
		}
		else store_spell_target = 6;

	// Set the spell page, based on starting spell
	if (((type == 0) && (store_mage >= 38)) || ((type == 1) && (store_priest >= 38)))
		on_which_spell_page = 1;
		else on_which_spell_page = 0;


	make_cursor_sword();

	cd_create_dialog(1098,mainPtr);
	cd_set_pict(1098,2,714 + type);
	for (i = 37; i < 75; i++) {
		cd_add_label(1098,i,"",55);
		if (i > 62)
			cd_attach_key(1098,i,(char ) (65 + i - 63));
			else cd_attach_key(1098,i,(char) (97 + i - 37));
		cd_set_led(1098,i,( pc_can_cast_spell(pc_casting,type,
		 (on_which_spell_page == 0) ? i - 37 : spell_index[i - 37]) == TRUE) ? 1 : 0);
		}
	cd_attach_key(1098,10,'!');
	cd_attach_key(1098,11,'@');
	cd_attach_key(1098,12,'#');
	cd_attach_key(1098,13,'$');
	cd_attach_key(1098,14,'%');
	cd_attach_key(1098,15,'^');
	for (i = 0; i < 6; i++)
   		cd_key_label(1098,10 + i,0);
   		
  	cd_set_flag(1098,78,0);

	put_spell_list();
	draw_spell_info();
   put_pc_caster_buttons();
	draw_spell_pc_info();
	draw_caster_buttons();
   put_spell_led_buttons();
   
	if (party.help_received[7] == 0) {
		cd_initial_draw(1098);
		give_help(7,8,1098);
		}
   
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		

	final_process_dialog(1098);

	return dialog_answer;
}


void print_spell_cast(short spell_num,short which)
//short which; // 0 - mage  1 - priest
{
	sprintf ((char *) c_line, "Spell: %s                  ",
		(which == 0) ? (char *) mage_s_name[spell_num] : (char *) priest_s_name[spell_num]);
	add_string_to_buf((char *) c_line);		
}

short stat_adj(short pc_num,short which)
{
	short tr;
	
	tr = skill_bonus[adven[pc_num].skills[which]];
	if (which == 2) {
		if (adven[pc_num].traits[1] == TRUE)
			tr++;
		if (pc_has_abil_equip(pc_num,40) < 24) 
			tr++;
		}
	if (which == 0) {
		if (adven[pc_num].traits[8] == TRUE)
			tr++;
		if (pc_has_abil_equip(pc_num,38) < 24) 
			tr++;
		}
	if (which == 1) {
		if (pc_has_abil_equip(pc_num,39) < 24) 
			tr++;
		}
	return tr;
}

void set_town_spell(short s_num,short who_c)
{
	town_spell = s_num;
	who_cast = who_c;
}

void do_alchemy() ////
{
	short abil1_needed[20] = {150,151,150,151,153, 152,152,153,156,153,
								156,154,156,157,155, 157,157,152,156,157};
	short abil2_needed[20] = {0,0,0,153,0, 0,0,152,0,154,
							150,0,151,0,0, 154,155,155,154,155};
	short difficulty[20] = {1,1,1,3,3, 4,5,5,7,9, 9,10,12,12,9, 14,19,10,16,20};
	short fail_chance[20] = {50,40,30,20,10,8,6,4,2,0,0,0,0,0,0,0,0,0,0,0};
	short which_p,which_item,which_item2,r1;
	short pc_num;
	item_record_type store_i = {7,0, 0,0,0,1,0,0, 50,0,0,0,0, 0, 8,0, {0,0},"Potion","Potion",0,5,0,0};

	//	{7,0,0,0,0,1,1,30,59,0,0,250,1,0,1,{0,0},"Graymold Salve","Potion"},
	//	{7,0,0,0,0,1,1,30,13,0,0,250,1,0,1,{0,0},"Resurrection Balm","Potion"},

	short potion_abils[20] = {72,87,70,73,70, 87,72,73,77,88,
							79,70,87,70,160, 88,86,71,84,88};
	short potion_strs[20] = {2,2,2,2,4, 5,8,5,4,2,
							8,6,8,8,0, 5,2,8,5,8};
	short potion_val[20] = {40,60,15,50,50, 180,200,100,150,100,
						200,150,300,400,100, 300,500,175,250,500};
	
	pc_num = select_pc(1,0);
	if (pc_num == 6)
		return;
		
	which_p = alch_choice(pc_num);
	if (which_p < 20) {
			if (pc_has_space(pc_num) == 24) {
				add_string_to_buf("Alchemy: Can't carry another item.");
				return;
				}
			if (((which_item = pc_has_abil(pc_num,abil1_needed[which_p])) == 24) ||
				((abil2_needed[which_p] > 0) && ((which_item2 = pc_has_abil(pc_num,abil2_needed[which_p])) == 24))) {
				add_string_to_buf("Alchemy: Don't have ingredients.");
				return;
				}
			play_sound(8);
			remove_charge(pc_num,which_item);
			if (abil2_needed[which_p] > 0)
				remove_charge(pc_num,which_item2);

			r1 = get_ran(1,0,100);
			if (r1 < fail_chance[adven[pc_num].skills[12] - difficulty[which_p]]) {
				add_string_to_buf("Alchemy: Failed.               ");
				r1 = get_ran(1,0,1);
				play_sound(41 );
				}
				else {
					store_i.value = potion_val[which_p];
					store_i.ability_strength = potion_strs[which_p];
					store_i.ability = potion_abils[which_p];
					if (which_p == 8)
						store_i.magic_use_type = 2;
					strcpy(store_i.full_name,alch_names_short[which_p]);
					if (adven[pc_num].skills[12] - difficulty[which_p] >= 5)
						store_i.charges++;
					if (adven[pc_num].skills[12] - difficulty[which_p] >= 11)
						store_i.charges++;
					if (store_i.variety == 7)
						store_i.graphic_num += get_ran(1,0,2);
					if (give_to_pc(pc_num,store_i,0) == FALSE) {
						ASB("No room in inventory.");
						ASB("  Potion placed on floor.");
						place_item(store_i,c_town.p_loc,TRUE);
						}
						else add_string_to_buf("Alchemy: Successful.             ");
					}
			put_item_screen(stat_window,0);
		}

}

void alch_choice_event_filter (short item_hit)
{
	if (item_hit == 49) {
		party.help_received[20] = 0;
		give_help(20,21,1047);
		return;
		}
	if (item_hit == 1)
		dialog_answer = 20;
		else {
		item_hit = (item_hit - 9) / 2;
		dialog_answer = item_hit;
		}
	dialog_not_toast = FALSE;

}

short alch_choice(short pc_num)
{
	short difficulty[20] = {1,1,1,3,3, 4,5,5,7,9, 9,10,12,12,9, 14,19,10,16,20};
	short i,item_hit,store_alchemy_pc;
	char get_text[256];

	make_cursor_sword();

	store_alchemy_pc = pc_num;

	cd_create_dialog(1047,mainPtr);
	for (i = 0; i < 20; i++) {
		cd_set_item_text(1047,10 + i * 2,alch_names[i]);
		if ((adven[pc_num].skills[12] < difficulty[i]) || (party.alchemy[i] == 0))
			cd_activate_item(1047,9 + i * 2,0);
		}
	sprintf((char *) get_text, "%s (skill %d)", 
		adven[pc_num].name,adven[pc_num].skills[12]);
	cd_set_item_text(1047,4,get_text);
	if (party.help_received[20] == 0) {
		cd_initial_draw(1047);
		give_help(20,21,1047);
		}
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		

	final_process_dialog(1047);
	return dialog_answer;
}

void pc_graphic_event_filter (short item_hit)
{
	switch (item_hit) {
		case 1:
			adven[store_graphic_pc_num].which_graphic = store_pc_graphic;
			update_pc_graphics();
			if (store_graphic_mode == 0)
					dialog_not_toast = FALSE;
				else {
					dialog_not_toast = FALSE;
					dialog_answer = TRUE;
					}
			break;

		case 4:
			update_pc_graphics();
			if (store_graphic_mode == 0) {
				if (adven[store_graphic_pc_num].main_status < 0)
				   adven[store_graphic_pc_num].main_status = 0;
					dialog_not_toast = FALSE;
				}
				else {
					dialog_not_toast = FALSE;
					dialog_answer =  TRUE;
					}
			break;

		default:
			cd_set_led(1050,store_pc_graphic + 5,0);
			store_pc_graphic = item_hit - 5;
			cd_set_led(1050,item_hit,1);
			break;
		}

	dialog_answer =  FALSE;
}

Boolean pick_pc_graphic(short pc_num,short mode,short parent_num)
// mode ... 0 - create  1 - created
{
	short i,item_hit;
	Boolean munch_pc_graphic = FALSE;
	
	store_graphic_pc_num = pc_num;
	store_graphic_mode = mode;
	store_pc_graphic = adven[pc_num].which_graphic;

	make_cursor_sword();

	if (pcs_gworld == NULL) {
		munch_pc_graphic = TRUE;
		pcs_gworld = load_pict(902);
		}
	cd_create_dialog_parent_num(1050,parent_num);

	for (i = 41; i < 77; i++) 
		csp(1050,i,800 + i - 41);
	for (i = 5; i < 41; i++) {
		if (store_pc_graphic + 5 == i)
			cd_set_led(1050,i,1);
			else cd_set_led(1050,i,0);
		}
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		
	cd_kill_dialog(1050,0);

	if (munch_pc_graphic == TRUE) {
		DisposeGWorld(pcs_gworld);
		pcs_gworld = NULL;
		}
	return dialog_answer;
}

void pc_name_event_filter (short item_hit)
{
	Str255 get_text;
	
	cd_retrieve_text_edit_str(1051,(char *) get_text);
	if ((get_text[0] < 33) || (get_text[0] > 126)) {
		csit(1051,6,"Must begin with a letter.");
		}
		else {
			sprintf((char *) adven[store_train_pc].name,"%.18s",(char *) get_text);
			dialog_not_toast = FALSE;
			}

}

Boolean pick_pc_name(short pc_num,short parent_num)  
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{

	short item_hit;
	Str255 sign_text;
	location view_loc;

	store_train_pc = pc_num;

	make_cursor_sword();
	
	cd_create_dialog_parent_num(1051,parent_num);
		
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		
	
	cd_kill_dialog(1051,0);
	
	return 1;
}

void pick_trapped_monst_event_filter (short item_hit)
{
	dialog_not_toast = FALSE;
	dialog_answer = item_hit;
}

unsigned char pick_trapped_monst()  
// ignore parent in Mac version
{
	short item_hit,i;
	Str255 sp;
	monster_record_type get_monst;
	
	make_cursor_sword();
	
	cd_create_dialog_parent_num(988,0);
	
	for (i = 0; i < 4; i++) 
		if (party.imprisoned_monst[i] == 0) {
			cd_activate_item(988, 2 + 3 * i, 0);
			}
			else {
				get_m_name((char *) sp,(unsigned char)(party.imprisoned_monst[i]));
				csit(988,3 + 3 * i,(char *) sp);
				get_monst = return_monster_template((unsigned char)(party.imprisoned_monst[i]));
				cdsin(988,4 + 3 * i,get_monst.level);
				}
				
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast)
		ModalDialog(main_dialog_UPP, &item_hit);
#endif		
	cd_kill_dialog(988,0);

	if (dialog_answer == 1)
		return 0;
		else return ((unsigned char)(party.imprisoned_monst[(dialog_answer - 2) / 3]));	
}


Boolean flying() 
{
	if (party.stuff_done[305][1] == 0)
		return FALSE;
		else return TRUE;
}

void poison_pc(short which_pc,short how_much)
{
	short level;
	
	if (adven[which_pc].main_status == 1) {
		if ((level = get_prot_level(which_pc,34)) > 0)////
			how_much -= level / 2;
		if ((level = get_prot_level(which_pc,31)) > 0)////
			how_much -= level / 3;

			if ((adven[which_pc].traits[12] == TRUE) && 
				(how_much > 1))
					how_much++;
			if ((adven[which_pc].traits[12] == TRUE) && 
				(how_much == 1) && (get_ran(1,0,1) == 0))
					how_much++;
			
			if (how_much > 0) {
				adven[which_pc].status[2] = min(adven[which_pc].status[2] + how_much,8);
				sprintf ((char *) c_line, "  %s poisoned.",
					(char *) adven[which_pc].name);
				add_string_to_buf((char *) c_line);
				one_sound(17);
				give_help(33,0,0);
				}
		}
	put_pc_screen(); 
}

void poison_party(short how_much)
{
	short i;

	for (i = 0; i < 6; i++)
		poison_pc(i,how_much);
}
void affect_pc(short which_pc,short type,short how_much)////
//type; // which status to affect
{

		if (adven[which_pc].main_status != 1)
			return;
			adven[which_pc].status[type] = minmax (-8,8,adven[which_pc].status[type] + how_much);
	if (((type >= 4) && (type <= 10)) || (type == 12) || (type == 13))
		adven[which_pc].status[type] = max(adven[which_pc].status[type],0);
	put_pc_screen(); 
}
void affect_party(short type,short how_much)
//type; // which status to affect
{
	short i;

	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			adven[i].status[type] = minmax (-8,8,adven[i].status[type] + how_much);
	put_pc_screen(); 
}

void void_sanctuary(short pc_num)
{
	if (adven[pc_num].status[8] > 0) {
		add_string_to_buf("You become visible!");
		adven[pc_num].status[8] = 0;
		}
}

void hit_party(short how_much,short damage_type)
{
	short i;
	Boolean dummy;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			dummy = damage_pc(i,how_much,damage_type,-1);
//			dummy = damage_pc(i,how_much,damage_type + 30);
	put_pc_screen(); 
}

void slay_party(short mode)
{ 
	short i;
	
		boom_anim_active = FALSE;
					for (i = 0; i < 6; i++)
						if (adven[i].main_status == 1)
							adven[i].main_status = mode;
	put_pc_screen(); 
}

Boolean damage_pc(short which_pc,short how_much,short damage_type,short type_of_attacker)
//short damage_type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable
					// 5 - cold  6 - undead attack  7 - demon attack
					// 10 - marked damage, from during anim mode ... no boom, and totally unblockable
					// 30 + *   same as *, but no print
					// 100s digit - sound data
{
	short i, r1,sound_type,level;
	Boolean do_print = TRUE;
	
	if (adven[which_pc].main_status != 1)
		return FALSE;
	
	sound_type = damage_type / 100;
	damage_type = damage_type % 100;

	if (damage_type >= 30) {
		do_print = FALSE;
		damage_type -= 30;
		}

	if (sound_type == 0) {
		if ((damage_type == 1) || (damage_type == 4) )
			sound_type = 5;
		if 	(damage_type == 3)
			sound_type = 12;	
		if 	(damage_type == 5)
			sound_type = 7;	
		if 	(damage_type == 2)
			sound_type = 11;	
		}
		
	// armor	
	if ((damage_type == 0) || (damage_type == 6) ||(damage_type == 7)) {
		how_much -= minmax(-5,5,adven[which_pc].status[1]);
		for (i = 0; i < 24; i++)
			if ((adven[which_pc].items[i].variety != 0) && (adven[which_pc].equip[i] == TRUE)) {
				if ((adven[which_pc].items[i].variety >= 12) && (adven[which_pc].items[i].variety <= 17)) {
						r1 = get_ran(1,1,adven[which_pc].items[i].item_level);
						how_much -= r1;
						
						// bonus for magical items
						if (adven[which_pc].items[i].bonus > 0) {
							r1 = get_ran(1,1,adven[which_pc].items[i].bonus);
							how_much -= r1;
							how_much -= adven[which_pc].items[i].bonus / 2;
							}
						if (adven[which_pc].items[i].bonus < 0) {
							how_much = how_much - adven[which_pc].items[i].bonus;
							}
						r1 = get_ran(1,0,100);
						if (r1 < hit_chance[adven[which_pc].skills[8]] - 20)
							how_much -= 1;
					}
				if (adven[which_pc].items[i].protection > 0) {
						r1 = get_ran(1,1,adven[which_pc].items[i].protection);
						how_much -= r1;
					}
				if (adven[which_pc].items[i].protection < 0) {
						r1 = get_ran(1,1,-1 * adven[which_pc].items[i].protection);
						how_much += r1;
					}
				}	
		}
	
	// parry
	if ((damage_type < 2) && (pc_parry[which_pc] < 100))
		how_much -= pc_parry[which_pc] / 4;


	if (damage_type != 10) {
		if (PSD[306][7] > 0)
			how_much -= 3;
		// toughness
		if (adven[which_pc].traits[0] == TRUE)
			how_much--;
		// luck
		if (get_ran(1,0,100) < 2 * (hit_chance[adven[which_pc].skills[18]] - 20))
			how_much -= 1;
		}

	if ((damage_type == 0) && ((level = get_prot_level(which_pc,30)) > 0))
		how_much = how_much - level;
	if ((damage_type == 6) && ((level = get_prot_level(which_pc,57)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if ((damage_type == 7) && ((level = get_prot_level(which_pc,58)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if ((type_of_attacker == 6) && ((level = get_prot_level(which_pc,59)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if ((type_of_attacker == 1) && ((level = get_prot_level(which_pc,60)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if ((type_of_attacker == 9) && ((level = get_prot_level(which_pc,61)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	

	// invuln
	if (adven[which_pc].status[4] > 0)
		how_much = 0;
		
	// magic resistance
	if ((damage_type == 3) && ((level = get_prot_level(which_pc,35)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	// Mag. res helps w. fire and cold
	if (((damage_type == 1) || (damage_type == 5)) && 
		(adven[which_pc].status[5] > 0))
			how_much = how_much / 2;
			
	// fire res.
	if ((damage_type == 1) && ((level = get_prot_level(which_pc,32)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
		
	// cold res.
	if ((damage_type == 5) && ((level = get_prot_level(which_pc,33)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	// major resistance
	if (((damage_type == 1) || (damage_type == 2) || (damage_type == 3) || (damage_type == 5))
	 && ((level = get_prot_level(which_pc,31)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	if (boom_anim_active == TRUE) {
		if (how_much < 0)
			how_much = 0;
		pc_marked_damage[which_pc] += how_much;
		if (is_town())
			add_explosion(c_town.p_loc,how_much,0,(damage_type > 2) ? 2 : 0,0,0);
			else add_explosion(pc_pos[which_pc],how_much,0,(damage_type > 2) ? 2 : 0,0,0);
	//	sprintf ((char *) c_line, "  %s takes %d. ",(char *) adven[which_pc].name, how_much);
	//	if (do_print == TRUE)
	//		add_string_to_buf((char *) c_line);
		if (how_much == 0)
			return FALSE;
			else return TRUE;
		}
	
	if (how_much <= 0) {
		if ((damage_type == 0) || (damage_type == 6) || (damage_type == 7))
			play_sound(2);
		add_string_to_buf ("  No damage.");
		return FALSE;
		}
		else {
			// if asleep, get bonus
			if (adven[which_pc].status[11] > 0)
				adven[which_pc].status[11]--;
			
			sprintf ((char *) c_line, "  %s takes %d. ",(char *) adven[which_pc].name, how_much);
			if (do_print == TRUE)
				add_string_to_buf((char *) c_line);
			if (damage_type != 10) {
				if (is_combat())
					boom_space(pc_pos[which_pc],overall_mode,boom_gr[damage_type],how_much,sound_type);
					else if (is_town()) 
						boom_space(c_town.p_loc,overall_mode,boom_gr[damage_type],how_much,sound_type);
						else boom_space(c_town.p_loc,100,boom_gr[damage_type],how_much,sound_type);
				}
				if (overall_mode != 0)
					FlushEvents(mDownMask,0);				
				FlushEvents(keyDownMask,0);				
		}

	party.total_dam_taken += how_much;
	
	if (adven[which_pc].cur_health >= how_much)
		adven[which_pc].cur_health = adven[which_pc].cur_health - how_much;
		else if (adven[which_pc].cur_health > 0)
			adven[which_pc].cur_health = 0;
			else // Check if PC can die
			 if (how_much > 25) {
				sprintf ((char *) c_line, "  %s is obliterated.  ",(char *) adven[which_pc].name);
				add_string_to_buf((char *) c_line);					
				kill_pc(which_pc, 3);
				}
				else {
				sprintf ((char *) c_line, "  %s is killed.",(char *) adven[which_pc].name);
				add_string_to_buf((char *) c_line);					
				kill_pc(which_pc,2);
				}
	if ((adven[which_pc].cur_health == 0) && (adven[which_pc].main_status == 1))
		play_sound(3);
	put_pc_screen();
	return TRUE;
}

void kill_pc(short which_pc,short type)
{
	short i = 24;
	Boolean dummy,no_save = FALSE;
	location item_loc;
	
	if (type >= 10) {
		type -= 10;
		no_save = TRUE;
		}
	
	if (type != 4)
		i = pc_has_abil_equip(which_pc,48);

	if ((no_save == FALSE) && (type != 0) && (adven[which_pc].skills[18] > 0) && 
		(get_ran(1,0,100) < hit_chance[adven[which_pc].skills[18]])) {
			add_string_to_buf("  But you luck out!          ");
			adven[which_pc].cur_health = 0;
			}
		else if ((i == 24) || (type == 0)) {
			if (combat_active_pc == which_pc)
				combat_active_pc = 6;
	
			for (i = 0; i < 24; i++)
				adven[which_pc].equip[i] = FALSE;

			item_loc = (overall_mode >= 10) ? pc_pos[which_pc] : c_town.p_loc;
	
			if (type == 2)
				make_sfx(item_loc.x,item_loc.y,3);
				else if (type == 3)
					make_sfx(item_loc.x,item_loc.y,6);
		
			if (overall_mode != 0)	
				for (i = 0; i < 24; i++)
					if (adven[which_pc].items[i].variety != 0) {
						dummy = place_item(adven[which_pc].items[i],item_loc,TRUE);
						adven[which_pc].items[i].variety = 0;
						}
				if ((type == 2) || (type == 3))
					play_sound(21);
				adven[which_pc].main_status = type;
				pc_moves[which_pc] = 0;
			}
			else {
				add_string_to_buf("  Life saved!              ");
				take_item(which_pc,i);
				heal_pc(which_pc,200);
			}
	if (adven[current_pc].main_status != 1)
		current_pc = first_active_pc();
	put_pc_screen();
	set_stat_window(current_pc);
}

void set_pc_moves()
{
	short i,r,i_level;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status != 1)
			pc_moves[i] = 0;
			else {
				pc_moves[i] = (adven[i].traits[10] == TRUE) ? 3 : 4;
				r = get_encumberance(i);
				pc_moves[i] = minmax(1,8,pc_moves[i] - (r / 3));
				
				if ((i_level = get_prot_level(i,55)) > 0)
					pc_moves[i] += i_level / 7 + 1;
				if ((i_level = get_prot_level(i,56)) > 0)
					pc_moves[i] -= i_level / 5;

				if ((adven[i].status[3] < 0) && (party.age % 2 == 1)) // slowed?
					pc_moves[i] = 0;
					else { // do webs
						pc_moves[i] = max(0,pc_moves[i] - adven[i].status[6] / 2);
						if (pc_moves[i] == 0) {
							sprintf((char *) c_line,"%s must clean webs.",adven[i].name);
							add_string_to_buf((char *) c_line);
							adven[i].status[6] = max(0,adven[i].status[6] - 3);
							}
						}
				if (adven[i].status[3] > 7)
					pc_moves[i] = pc_moves[i] * 3;
					else if (adven[i].status[3] > 0)
						pc_moves[i] = pc_moves[i] * 2;
				if ((adven[i].status[11] > 0) || (adven[i].status[12] > 0)) 	
					pc_moves[i] = 0;

				}

}

void take_ap(short num)
{
	pc_moves[current_pc] = max(0,pc_moves[current_pc] - num);
}

short cave_lore_present()
{
	short i,ret = 0;
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (adven[i].traits[4] > 0))
			ret += 1;
	return ret;
}

short woodsman_present()
{
	short i,ret = 0;
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (adven[i].traits[5] > 0))
			ret += 1;
	return ret;
}

void init_spell_menus()
{
	short i,j;
	
	for (i = 0; i < 2; i++)
		for (j = 0; j < 62; j++)
			on_spell_menu[i][j] = -1;

}



void adjust_spell_menus()
{
	short i,j,spell_pos = 0;
	MenuHandle spell_menu;
	Str255 spell_name;
	short old_on_spell_menu[2][62];
	Boolean need_menu_change = FALSE;
	
	if (in_startup_mode == TRUE)
		return;
		
	for (i = 0; i < 2; i++)
		for (j = 0; j < 62; j++)
			old_on_spell_menu[i][j] = on_spell_menu[i][j];
			
	spell_menu = GetMHandle(900);
	
	for (i = 0; i < 62; i++) {
		on_spell_menu[0][i] = -1; 
		}
	for (i = 0; i < 62; i++) 
		if (pc_can_cast_spell(current_pc,0,i)) {
			on_spell_menu[0][spell_pos] = i;
			spell_pos++;
			}
	for (i = 0; i < 62; i++) 
		if (on_spell_menu[0][i] != old_on_spell_menu[0][i])
			need_menu_change = TRUE;
	if (need_menu_change) {
		for (i = 0; i < 62; i++) {
			DeleteMenuItem(spell_menu,3); 
			}
		for (i = 0; i < 62; i++) 
			if (on_spell_menu[0][i] >= 0) {
				if (spell_cost[0][on_spell_menu[0][i]] > 0)
					sprintf((char *)spell_name," L%d - %s, C %d",spell_level[on_spell_menu[0][i]],
						(char *) mage_s_name[on_spell_menu[0][i]],spell_cost[0][on_spell_menu[0][i]]);
					else sprintf((char *)spell_name," L%d - %s, C ?",spell_level[on_spell_menu[0][i]],
						(char *) mage_s_name[on_spell_menu[0][i]]);
				spell_name[0] = strlen((char *) spell_name);
				//strcpy((char *) (spell_name + 1),mage_s_name[on_spell_menu[0][i]]);
				AppendMenu(spell_menu,spell_name);
				}
		}
		
	need_menu_change = FALSE;
	spell_pos = 0;
		
		spell_menu = GetMHandle(950);
	
	for (i = 0; i < 62; i++) {
		on_spell_menu[1][i] = -1; 
		}
	for (i = 0; i < 62; i++) 
		if (pc_can_cast_spell(current_pc,1,i)) {
			on_spell_menu[1][spell_pos] = i;
			spell_pos++;
			}
	for (i = 0; i < 62; i++) 
		if (on_spell_menu[1][i] != old_on_spell_menu[1][i])
			need_menu_change = TRUE;
	if (need_menu_change) {
		for (i = 0; i < 62; i++) {
			DeleteMenuItem(spell_menu,3); 
			}
		for (i = 0; i < 62; i++) 
			if (on_spell_menu[1][i] >= 0) {
				//spell_name[0] = strlen((char *) priest_s_name[on_spell_menu[1][i]]);
				//strcpy((char *) (spell_name + 1),priest_s_name[on_spell_menu[1][i]]);
				if (spell_cost[1][on_spell_menu[1][i]] > 0)
					sprintf((char *)spell_name," L%d - %s, C %d",spell_level[on_spell_menu[1][i]],
						(char *) priest_s_name[on_spell_menu[1][i]],spell_cost[1][on_spell_menu[1][i]]);
					else sprintf((char *)spell_name," L%d - %s, C ?",spell_level[i],
						(char *) mage_s_name[on_spell_menu[1][i]]);
				spell_name[0] = strlen((char *) spell_name);
				AppendMenu(spell_menu,spell_name);
				}
		}
	

}