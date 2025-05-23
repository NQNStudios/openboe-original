#include <QDOffscreen.h>
#include "stdio.h"

#include "global.h"
#include "loc_utils.h"
#include "fields.h"
#include "monster.h"
#include "text.h"
#include "specials.h"
#include "items.h"
#include "Exile.sound.h"
#include "blxgraphics.h"
#include "newgraph.h"

extern current_town_type c_town;
extern party_record_type party;
extern short overall_mode;
extern outdoor_record_type outdoors[2][2];
extern unsigned char combat_terrain[64][64],out[96][96];
extern short which_combat_type;
extern pc_record_type adven[6];
extern big_tr_type t_d;
extern short monst_target[T_M]; // 0-5 target that pc   6 - no target  100 + x - target monster x
extern short spell_caster, missile_firer,current_monst_tactic;
extern short hit_chance[21];
extern unsigned char misc_i[64][64];
extern location monster_targs[T_M];

extern location pc_pos[6],center;
extern short boom_gr[8],futzing;
extern Boolean processing_fields,monsters_going;
extern town_item_list	t_i;
extern scenario_data_type scenario;



short charm_odds[20] = {90,90,85,80,78, 75,73,60,40,30, 20,10,4,1,0, 0,0,0,0,0};	

creature_start_type null_start_type = {0,0,{80,80},1,0,0,0,0,0,0,0, 0,-1,-1,-1};
	
////				
monster_record_type return_monster_template(unsigned char store)
{
	monster_record_type monst;
	short m_num,i;
	
	m_num = store;
	monst = scenario.scen_monsters[store];
	if (monst.spec_skill == 11)
		monst.picture_num = 0;
		
	monst.m_num = m_num;
	monst.health = (PSD[306][7] == 0) ? monst.health : monst.health / 2;
	
	// now adjust for difficulty
	monst.health = monst.health * difficulty_adjust();
	
	monst.m_health = monst.health; // in scenario file, health is stored in health field
	monst.max_mp = 0;
	monst.mp = monst.max_mp;

	monst.ap = 0;

	if ((monst.mu > 0) || (monst.cl > 0))
		monst.max_mp = monst.mp = 12 * monst.level;
	
	monst.m_morale = 10 * monst.level;
	if (monst.level >= 20)
		monst.m_morale += 10 * (monst.level - 20);
	
	monst.morale = monst.m_morale;
	monst.direction = 0;
	for (i = 0; i < 15; i++)
		monst.status[i] = 0;
		
	return monst;
}

short difficulty_adjust()
{
	short i, j = 0;
	short to_return = 1;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			j += adven[i].level;
	
	if ((scenario.difficulty <= 0) && (j >= 60))
		to_return++;
	if ((scenario.difficulty <= 1) && (j >= 130))
		to_return++;
	if ((scenario.difficulty <= 2) && (j >= 210))
		to_return++;
	return to_return;
}	

short out_enc_lev_tot(short which)
{
	short count = 0,i;
	monster_record_type store_m;
	short num[7] = {22,8,4,4,3,2,1};
	
	if (party.out_c[which].what_monst.cant_flee == TRUE)
		return 10000;
		
	for (i = 0; i < 7; i++)
		if (party.out_c[which].what_monst.monst[i] != 0) {
			store_m = return_monster_template(party.out_c[which].what_monst.monst[i]);
			count += store_m.level * num[i];
			}
	return count;
}

short count_monst()
{
	short to_ret = 0,i;

	for (i = 0; i < T_M; i++)
		if (c_town.monst.dudes[i].active > 0)
			to_ret++;
	return to_ret;
}

void create_wand_monst()
{
	short r1,r2,i = 0,num_tries = 0;
	location p_loc;

	r1 = get_ran(1,0,3);
	if (overall_mode == 0)
		if (is_null_out_wand_entry(outdoors[party.i_w_c.x][party.i_w_c.y].wandering[r1]) == 0) {
			r2 = get_ran(1,0,3);
			while ((point_onscreen(outdoors[party.i_w_c.x][party.i_w_c.y].wandering_locs[r2],global_to_local(party.p_loc)) == TRUE)
				&& (num_tries++ < 100))
				r2 = get_ran(1,0,3);
			if (is_blocked(outdoors[party.i_w_c.x][party.i_w_c.y].wandering_locs[r2]) == FALSE)
				place_outd_wand_monst(outdoors[party.i_w_c.x][party.i_w_c.y].wandering_locs[r2],
					outdoors[party.i_w_c.x][party.i_w_c.y].wandering[r1],0);
			}
	
////
	
	if (overall_mode != 0) // won't place wandering is more than 50 monsters
		if ((is_null_wand_entry(c_town.town.wandering[r1]) == 0) && (count_monst() <= 50)
			&& (party.m_killed[c_town.town_num] < c_town.town.max_num_monst)) {
			r2 = get_ran(1,0,3);
			while ((point_onscreen(c_town.town.wandering_locs[r2],c_town.p_loc) == TRUE) && 
			(loc_off_act_area(c_town.town.wandering_locs[r2]) == FALSE) && (num_tries++ < 100))
				r2 = get_ran(1,0,3);
			for (i = 0; i < 4; i++) {
				if (c_town.town.wandering[r1].monst[i] != 0) { // place a monster
					p_loc = c_town.town.wandering_locs[r2];
					p_loc.x += get_ran(1,0,4) - 2;
					p_loc.y += get_ran(1,0,4) - 2;
					if (is_blocked(p_loc) == FALSE)
						place_monster(c_town.town.wandering[r1].monst[i],p_loc);
					p_loc = c_town.town.wandering_locs[r2];
					p_loc.x += get_ran(1,0,4) - 2;
					p_loc.y += get_ran(1,0,4) - 2;
					if ((r1 >= 2) && (i == 0) && (is_blocked(p_loc) == FALSE)) // place extra monsters?
						place_monster(c_town.town.wandering[r1].monst[i],p_loc);
					p_loc = c_town.town.wandering_locs[r2];
					p_loc.x += get_ran(1,0,4) - 2;
					p_loc.y += get_ran(1,0,4) - 2;
					if ((r1 == 3) && (i == 1) && (is_blocked(p_loc) == FALSE)) 
						place_monster(c_town.town.wandering[r1].monst[i],p_loc);						
					}
				}			
		}
}

void place_outd_wand_monst(location where,out_wandering_type group,short forced)
{
	short i = 0,j = 0;
	location l;
				
	
			while (i < 10) {
			////
				if ((party.out_c[i].exists == FALSE) || ((i == 9) && (forced > 0))) {
					if ((sd_legit(group.end_spec1,group.end_spec2) == TRUE) && (PSD[group.end_spec1][group.end_spec2] > 0))
						return;
					party.out_c[i].exists = TRUE;
					party.out_c[i].direction = 0;
					party.out_c[i].what_monst = group;
					party.out_c[i].which_sector = party.i_w_c;
					party.out_c[i].m_loc = where;
				if (party.out_c[i].which_sector.x == 1)
					party.out_c[i].m_loc.x += 48;
				if (party.out_c[i].which_sector.y == 1)
					party.out_c[i].m_loc.y += 48;
				l = party.out_c[i].m_loc;
				while ((forced == TRUE) && (is_blocked(l)) && (j < 50)) {
					l = party.out_c[i].m_loc;
					l.x += get_ran(1,0,2) - 1;
					l.y += get_ran(1,0,2) - 1;
					j++;
					}
				party.out_c[i].m_loc = l;
				
				i = 50;
				}
				i++;
				}

}

short is_null_wand_entry(wandering_type wand_entry)
{
	short i = 0;
	
	while (i < 4) {
		if (wand_entry.monst[i] != 0)
			return 0;
		i++;	
		}
	return 1;
}

short is_null_out_wand_entry(out_wandering_type wand_entry)
{
	short i = 0;
	
	while (i < 7) {
		if (wand_entry.monst[i] != 0)
			return 0;
		i++;	
		}
	return 1;
}

location get_monst_head(short m_num)
{
	location l;
	
	l = c_town.monst.dudes[m_num].m_loc;
	if ((c_town.monst.dudes[m_num].m_d.direction < 4) &&
		(c_town.monst.dudes[m_num].m_d.x_width > 1))
			l.x++;

	return l;
}

short get_monst_picnum(unsigned char monst)
{
	return scenario.scen_monsters[monst].picture_num;
}

void get_monst_dims(unsigned char monst,short *width, short *height)
{

	*width = scenario.scen_monsters[monst].x_width;
	*height = scenario.scen_monsters[monst].y_width;
}

// Used to set up monsters for outdoor wandering encounters.
void set_up_monst(short mode,unsigned char m_num)
//mode; // 0 - unfriendly  1 - friendly & fightin'
{
	short which;
	
	for (which = 0; which < T_M; which++) 
		if (c_town.monst.dudes[which].active == 0) {
			c_town.monst.dudes[which].active = 2;
			c_town.monst.dudes[which].summoned = 0;
			c_town.monst.dudes[which].attitude = mode + 1;
			c_town.monst.dudes[which].number = m_num;
			c_town.monst.dudes[which].m_d = return_monster_template(m_num);
			c_town.monst.dudes[which].mobile = TRUE;
			c_town.monst.dudes[which].monst_start = null_start_type;
			which = T_M;
			}
}

void do_monsters()
{
	short i,j,r1,target;
	location dest,l1,l2, dummy_loc = {0,0},store_loc;
	Boolean acted_yet = FALSE;
	char debug[60];
	
	if (overall_mode == 1) 
		for (i = 0; i < T_M; i++) 
		if ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].m_d.status[11] <= 0)
			&& (c_town.monst.dudes[i].m_d.status[12] <= 0)) {
			// have to pick targets
			if (c_town.monst.dudes[i].active == 1)
				target = 6;
				else {
					target = monst_pick_target(i); // will return 0 if target party
					target = switch_target_to_adjacent(i,target);
					if (target == 0) {
						if (dist(c_town.monst.dudes[i].m_loc,c_town.p_loc) > 8) 
							target = 6;
							else target = select_active_pc();
					}
					if ((c_town.monst.dudes[i].attitude % 2 != 1) && (target < 6))
						target = 6;	
					}
			monst_target[i] = target;
//			sprintf((char *)debug,"  t: %d targets %d.",i,monst_target[i]);
//			add_string_to_buf((char *) debug);			

			if ((c_town.monst.dudes[i].active == 2)
				 || ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].attitude % 2 != 1))) {
				acted_yet = FALSE;
				if (((c_town.monst.dudes[i].attitude == 0) || (monst_target[i] == 6)) && (c_town.hostile == 0)) {
					if (c_town.monst.dudes[i].mobile == TRUE) { // OK, it doesn't see the party or
					    // isn't nasty, and the town isn't totally hostile. 
					    if ((c_town.monst.dudes[i].attitude % 2 != 1) || (get_ran(1,0,1) == 0)) {
							acted_yet = rand_move(i);
							}
							else acted_yet = seek_party(i,c_town.monst.dudes[i].m_loc,c_town.p_loc);
						}
					}
				if ((c_town.monst.dudes[i].attitude > 0) || (c_town.hostile == 1)) {
					if ((c_town.monst.dudes[i].mobile == TRUE) && (monst_target[i] != 6)) {
					l1 = c_town.monst.dudes[i].m_loc;
					l2 = (monst_target[i] <= 6) ? c_town.p_loc : c_town.monst.dudes[target - 100].m_loc;

					if ((c_town.monst.dudes[i].m_d.morale < 0) && (c_town.monst.dudes[i].m_d.spec_skill != 13)
						&&  (c_town.monst.dudes[i].m_d.m_type != 8))  {
						acted_yet = flee_party(i,l1,l2);
						if (get_ran(1,0,10) < 6)
							c_town.monst.dudes[i].m_d.morale++;
						}							
						else if (monst_hate_spot(i,&l2) == TRUE)
							acted_yet = seek_party(i,l1,l2);
						else if (((c_town.monst.dudes[i].m_d.mu == 0) && (c_town.monst.dudes[i].m_d.mu == 0))
							|| (can_see(l1,l2,0) > 3))
							acted_yet = seek_party(i,l1,l2);
					}
				}
			}


			// Make hostile monsters active
			if ((c_town.monst.dudes[i].active == 1) && (c_town.monst.dudes[i].attitude % 2 == 1)
				&& (dist(c_town.monst.dudes[i].m_loc,c_town.p_loc) <= 8)) {
				r1 = get_ran(1,1,100);
				r1 += (party.stuff_done[305][0] > 0) ? 46 : 0;
				r1 += can_see(c_town.monst.dudes[i].m_loc,c_town.p_loc,0) * 10;
				if (r1 < 50) {
					c_town.monst.dudes[i].active = 2;
					add_string_to_buf("Monster saw you!");
					// play go active sound
					switch (c_town.monst.dudes[i].m_d.m_type) {
						case 0: case 3: case 4: case 5: case 6: case 9:  
							play_sound(18); break;
						default: play_sound(46); break;
						}
					}
				for (j = 0; j < T_M; j++)
					if ((c_town.monst.dudes[j].active == 2)
						 && ((dist(c_town.monst.dudes[i].m_loc,c_town.monst.dudes[j].m_loc) <= 5) == TRUE))
						c_town.monst.dudes[i].active = 2;		
				}
		
		}
		if (overall_mode == 0) {
			for (i = 0; i < 10; i++)
				if (party.out_c[i].exists == TRUE) {
						acted_yet = FALSE;
						l1 = party.out_c[i].m_loc;
						l2 = party.p_loc;

						r1 = get_ran(1,1,6);
						if (r1 == 3)
							acted_yet = rand_move(i);	
							else acted_yet = seek_party(i,l1,l2);
					}
			}
}

	////
Boolean monst_hate_spot(short which_m,location *good_loc)
{
	location prospect,loc;
	
	loc = c_town.monst.dudes[which_m].m_loc;
	if ((misc_i[loc.x][loc.y] & 224) 
	|| (c_town.explored[loc.x][loc.y] & 64) // hate regular fields
	|| ((c_town.explored[loc.x][loc.y] & 32) && (c_town.monst.dudes[which_m].m_d.radiate_1 != 2)
		&& (c_town.monst.dudes[which_m].m_d.immunities & 32 == 0)) // hate ice wall?
	|| ((c_town.explored[loc.x][loc.y] & 4) && (c_town.monst.dudes[which_m].m_d.radiate_1 != 1)
		&& (c_town.monst.dudes[which_m].m_d.immunities & 8 == 0)) // hate fire wall?
	|| ((c_town.explored[loc.x][loc.y] & 16) && (c_town.monst.dudes[which_m].m_d.radiate_1 != 6)
		&& (c_town.monst.dudes[which_m].m_d.immunities & 3 == 0)) // hate stink cloud?
	|| ((c_town.explored[loc.x][loc.y] & 128) && (c_town.monst.dudes[which_m].m_d.radiate_1 != 5)
		&& (c_town.monst.dudes[which_m].m_d.immunities & 3 == 0)) // hate sleep cloud?
	|| ((c_town.explored[loc.x][loc.y] & 2) && (c_town.monst.dudes[which_m].m_d.radiate_1 != 3)
		&& (c_town.monst.dudes[which_m].m_d.immunities & 3 == 0)) // hate shock cloud?
	|| (((c_town.monst.dudes[which_m].m_d.mu > 0) || (c_town.monst.dudes[which_m].m_d.cl > 0))
		 && (c_town.explored[loc.x][loc.y] & 8))) // hate antimagic
		 {
		 	prospect = find_clear_spot(loc,1);
		 	if (prospect.x > 0) {
		 		*good_loc = prospect;
		 		return TRUE;
		 		}
		 	return FALSE;
		 	}
		 else return FALSE;
}

short monst_pick_target(short which_m)
{
	creature_data_type *cur_monst;
	short r1,targ_pc,targ_m,a,b;

	cur_monst = &c_town.monst.dudes[which_m];

	// First, any chance target is screwed?
	if (monst_target[which_m] >= 100) {
	if (((cur_monst->attitude % 2 == 1) && 
		(c_town.monst.dudes[monst_target[which_m] - 100].attitude == cur_monst->attitude)) ||
		((cur_monst->attitude % 2 == 0) && (c_town.monst.dudes[monst_target[which_m] - 100].attitude % 2 == 0)))
			monst_target[which_m] = 6;
			else if (c_town.monst.dudes[monst_target[which_m] - 100].active == 0)
				monst_target[which_m] = 6;
			}
	if (monst_target[which_m] < 6) 
		if ((adven[monst_target[which_m]].main_status != 1) || (get_ran(1,0,3) == 1))
			monst_target[which_m] = 6;		
	
	if ((is_combat()) && (cur_monst->attitude % 2 == 1)) {
		if (spell_caster < 6)
			if ((get_ran(1,1,5) < 5) && (monst_can_see(which_m,pc_pos[spell_caster]) == TRUE)
					&& (adven[spell_caster].main_status == 1)) 
						return spell_caster;
		if (missile_firer < 6)
			if ((get_ran(1,1,5) < 3) && (monst_can_see(which_m,pc_pos[missile_firer]) == TRUE)
				&& (adven[missile_firer].main_status == 1)) 
					return missile_firer;
		if (monst_target[which_m] < 6)
			if ((monst_can_see(which_m,pc_pos[monst_target[which_m]]) == TRUE)
				&& (adven[monst_target[which_m]].main_status == 1)) 
					return monst_target[which_m];
		}

//	if (monst_target[which_m] >= 100) {
//		if ((can_see(cur_monst->m_loc,c_town.monst.dudes[monst_target[which_m] - 100].m_loc,0) < 4)
//			&& (c_town.monst.dudes[monst_target[which_m] - 100].active > 0)) 
//				return monst_target[which_m];
//		}
		
	// Now pick a target pc and a target monst and see which is more attractive
	targ_pc = monst_pick_target_pc(which_m,cur_monst);
	targ_m = monst_pick_target_monst(cur_monst);
	
	if ((targ_pc != 6) && (targ_m == 6))
		return targ_pc;
	if ((targ_pc == 6) && (targ_m != 6))
		return targ_m;
	if ((targ_pc == 6) && (targ_m == 6))
		return 6;
		
	if (is_town()) {
		if (cur_monst->attitude % 2 == 0) {
			return targ_m;
			}
		if ((targ_m == 6) && (cur_monst->attitude % 2 == 1))
			return 0;
		if (dist(cur_monst->m_loc,c_town.monst.dudes[targ_m - 100].m_loc) <
			dist(cur_monst->m_loc,c_town.p_loc))
				return targ_m;
				else return 0;
		}
	// Otherwise we're in combat
	if ((dist(cur_monst->m_loc,c_town.monst.dudes[targ_m - 100].m_loc) ==
		dist(cur_monst->m_loc,pc_pos[targ_pc])) && (get_ran(1,0,6) < 3))
			return targ_m;
			else return targ_pc;
	if (dist(cur_monst->m_loc,c_town.monst.dudes[targ_m - 100].m_loc) <
		dist(cur_monst->m_loc,pc_pos[targ_pc]))
			return targ_m;
			else return targ_pc;

}

short monst_pick_target_monst(creature_data_type *which_m)
{
	short min_dist = 1000,i,cur_targ = 6;

	for (i = 0; i < T_M; i++) {
		if ((c_town.monst.dudes[i].active > 0) && // alive
			 (((which_m->attitude % 2 == 1) && (c_town.monst.dudes[i].attitude % 2 == 0)) ||
			 ((which_m->attitude % 2 == 0) && (c_town.monst.dudes[i].attitude % 2 == 1)) ||
			 ((which_m->attitude % 2 == 1) && (c_town.monst.dudes[i].attitude != which_m->attitude))) && // they hate each other
			 ((dist(which_m->m_loc,c_town.monst.dudes[i].m_loc) < min_dist) ||
			 ((dist(which_m->m_loc,c_town.monst.dudes[i].m_loc) == min_dist) && (get_ran(1,0,7) < 4))) &&
			 (monst_can_see(i,c_town.monst.dudes[i].m_loc) == TRUE) ) {
			 	min_dist = dist(which_m->m_loc,c_town.monst.dudes[i].m_loc);
			 	cur_targ = i + 100;	 

			 }
		}
	return cur_targ;
}

short monst_pick_target_pc(short m_num,creature_data_type *which_m)////
{
	short num_tries = 0,r1,store_targ = 6;

	if (which_m->attitude % 2 == 0)
		return 6;
	if (is_town())
		return 0;
	
	// First pick any visible, nearby PC
	r1 = get_ran(1,0,5);
	while ((num_tries < 6) && ((adven[r1].main_status != 1) ||
		(monst_can_see(m_num,pc_pos[r1]) == FALSE))) {
			r1 = get_ran(1,0,5);
			num_tries++;
			}
	if (num_tries < 6)
		store_targ = r1;
	
	// Then, see if target can be replaced with someone nice and close
	r1 = get_ran(1,0,5);
	while ((num_tries < 6) && ((adven[r1].main_status != 1) ||
		(dist(which_m->m_loc,pc_pos[r1]) > 4) ||
		(monst_can_see(m_num,pc_pos[r1]) == FALSE))) {
			r1 = get_ran(1,0,5);
			num_tries++;
			}

	if (num_tries < 6)
		return r1;
		else return store_targ;
}

// returns 6 if no
short select_active_pc()
{
	short r1, num_tries = 0;
	
	r1 = get_ran(1,0,5);
	while ((adven[r1].main_status != 1) && (num_tries++ < 50))
		r1 = get_ran(1,0,5);
		
	return r1;
}

short closest_pc(location where)
{
	short how_close = 200,i,store = 6;

	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (dist(where,pc_pos[i]) < how_close)) {
			store = i;
			how_close = dist(where,pc_pos[i]);
			}
	return store;
}

short closest_monst(location where,short mode)
//mode;  // 1 - closest hostile to PCs  2 - closest friendly to PCs
{
	short how_close = 200,i,store = 6;

	for (i = 0; i < T_M; i++)
		if ((((c_town.monst.dudes[i].attitude % 2 == 1) && (mode == 1)) || 
		   ((c_town.monst.dudes[i].attitude % 2 == 0) && (mode == 2)))
		  && (dist(where,c_town.monst.dudes[i].m_loc) < how_close)) {
			store = i;
			how_close = dist(where,c_town.monst.dudes[i].m_loc);
			}
	return store;
}

short switch_target_to_adjacent(short which_m,short orig_target)
{
	location monst_loc;
	short i,num_adj = 0,targ_pc = 6, targ_m = 6;
	
	monst_loc = c_town.monst.dudes[which_m].m_loc;

	// First, take care of friendly monsters.
	if (c_town.monst.dudes[which_m].attitude % 2 == 0) {
		if (orig_target >= 100)
			if ((c_town.monst.dudes[orig_target - 100].active > 0) &&
			 (monst_adjacent(c_town.monst.dudes[orig_target - 100].m_loc,which_m) == TRUE))
				return orig_target;
		for (i = 0; i < T_M; i++)
			if ((c_town.monst.dudes[i].active > 0) &&
			 (c_town.monst.dudes[i].attitude % 2 == 1) &&
			 (monst_adjacent(c_town.monst.dudes[i].m_loc,which_m) == TRUE))
				return i + 100;
		return orig_target;
		}

	// If we get here while in town, just need to check if switch to pc
	if ((is_town()) && (monst_adjacent(c_town.p_loc,which_m) == TRUE))
		return 0;
	if (is_town())
		return orig_target;
		
	// If target is already adjacent, we're done here.
	if ((is_combat()) && (orig_target < 6))
		if ((adven[orig_target].main_status == 1) && (monst_adjacent(pc_pos[orig_target],which_m) == TRUE))
			return orig_target;
	if (orig_target >= 100)
		if ((c_town.monst.dudes[orig_target - 100].active > 0) &&
		 (monst_adjacent(c_town.monst.dudes[orig_target - 100].m_loc,which_m) == TRUE))
			return orig_target;
	
	// Anyone unarmored? Heh heh heh...
	if (is_combat())
		for (i = 0; i < 6; i++)
			if ((adven[i].main_status == 1) && (monst_adjacent(pc_pos[i],which_m) == TRUE) && 
			 (get_encumberance(i) < 2))
		 		return i; 

	// Check for a nice, adjacent, friendly monster and maybe attack
	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active > 0) &&
		 (c_town.monst.dudes[i].attitude % 2 == 0) &&
		 (monst_adjacent(c_town.monst.dudes[i].m_loc,which_m) == TRUE) &&
		 (get_ran(1,0,2) < 2))
			return i + 100;

	// OK. Now if this monster has PCs adjacent, pick one at randomn and hack. Otherwise,
	// stick with orig. target.
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (monst_adjacent(pc_pos[i],which_m) == TRUE)) 
			num_adj++;

	if (num_adj == 0)	
		return orig_target;
	
	i = 0;
	num_adj = get_ran(1,1,num_adj);
	while ((num_adj > 1) || (adven[i].main_status != 1) || (monst_adjacent(pc_pos[i],which_m) == FALSE)) {
		if ((adven[i].main_status == 1) && (monst_adjacent(pc_pos[i],which_m) == TRUE)) 
			num_adj--;
		i++;
		}
	return i;
}


Boolean rand_move(char i)
{
	Boolean acted_yet = FALSE;
	short j;
	location store_loc;
	
	// first, if outdoor, just roam. 
	if (is_out()) {
		store_loc = random_shift(party.out_c[i].m_loc);
		return outdoor_move_monster(i,store_loc);
		}
		
	if (same_point(monster_targs[i],c_town.monst.dudes[i].m_loc) == TRUE)
		monster_targs[i].x = 0;
			
	// FIrst, try to move to monst_targs. If it don't work, then we'll shift.
	if (monster_targs[i].x > 0)
		acted_yet = seek_party(i,c_town.monst.dudes[i].m_loc,monster_targs[i]);

	if (acted_yet == FALSE) {
		monster_targs[i].x = 0;
		for (j = 0; j < 3; j++) {
			store_loc = c_town.monst.dudes[i].m_loc;
			store_loc.x += get_ran(1,0,24) - 12;
			store_loc.y += get_ran(1,0,24) - 12;
			if ((loc_off_act_area(store_loc) == FALSE) && (can_see(c_town.monst.dudes[i].m_loc,store_loc,0) < 5)) {
				monster_targs[i] = store_loc; j = 3;
				}
			}

		if (monster_targs[i].x == 0) {
			// maybe pick a wand loc, else juist pick a loc
			j = get_ran(1,0,3);
			store_loc = c_town.town.wandering_locs[j];
			
			if ((loc_off_act_area(store_loc) == FALSE) && (get_ran(1,0,1) == 1))
				monster_targs[i] = store_loc;
				else {
				store_loc = c_town.monst.dudes[i].m_loc;
				store_loc.x += get_ran(1,0,20) - 10;
				store_loc.y += get_ran(1,0,20) - 10;
				if (loc_off_act_area(store_loc) == FALSE)
					monster_targs[i] = store_loc;
				}
			}
		if (monster_targs[i].x > 0)
			acted_yet = seek_party(i,c_town.monst.dudes[i].m_loc,monster_targs[i]);
		}

	return acted_yet;
}





Boolean seek_party(short i,location l1,location l2)
{
	Boolean acted_yet = FALSE;
	short m,n;
				if ((l1.x > l2.x) && (l1.y > l2.y)) 
					acted_yet = try_move(i,l1,-1,-1);
				if ((l1.x < l2.x) & (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,1,1);											
				if ((l1.x > l2.x) & (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,-1,1);								
				if ((l1.x < l2.x) & (l1.y > l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,1,-1);			
				if ((l1.x > l2.x) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,-1,0);
				if ((l1.x < l2.x) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,1,0);											
				if ( (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,0,1);								
				if ( (l1.y > l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,0,-1);	
				if (acted_yet == FALSE) {
					futzing++;
					m = get_ran(1,0,2) - 1;
					n = get_ran(1,0,2) - 1;
					acted_yet = try_move(i,l1,m,n);	
					}
	return acted_yet;
}

Boolean flee_party(short i,location l1,location l2)
{
	Boolean acted_yet = FALSE;

				if ((l1.x > l2.x) & (l1.y > l2.y)) 
					acted_yet = try_move(i,l1,1,1);
				if ((l1.x < l2.x) & (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,-1,-1);											
				if ((l1.x > l2.x) & (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,1,-1);								
				if ((l1.x < l2.x) & (l1.y > l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,-1,+1);			
				if ((l1.x > l2.x) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,1,0);
				if ((l1.x < l2.x) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,-1,0);											
				if ( (l1.y < l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,0,-1);								
				if ( (l1.y > l2.y) & (acted_yet == FALSE)) 
					acted_yet = try_move(i,l1,0,1);			
				if (acted_yet == FALSE) {
					futzing++;
					acted_yet = rand_move(i);	
					}				
	return acted_yet;
}

Boolean try_move(short i,location start,short x,short y)
{
	location dest;

	dest = start;
	dest.x = dest.x + x;
	dest.y = dest.y + y;
	
	
	if (overall_mode == 1)
		return town_move_monster(i,dest);
	if (overall_mode == 0)
		return outdoor_move_monster(i,dest);
	if (overall_mode == 10)
		return combat_move_monster((short) i,dest);
	return 0;
}

Boolean combat_move_monster(short which,location destination)
{	
	short r1;


	if (monst_can_be_there(destination,which) == FALSE)
		return FALSE;
		else if (monst_check_special_terrain(destination,2,which) == FALSE)
		return FALSE;
			else {
				c_town.monst.dudes[which].m_d.direction = 
				  set_direction(c_town.monst.dudes[which].m_loc, destination);
				c_town.monst.dudes[which].m_loc = destination;
				monst_inflict_fields(which);

				if (point_onscreen(destination,center) == TRUE) {
					if (is_combat())
						 move_sound(combat_terrain[destination.x][destination.y],
						  	(short) c_town.monst.dudes[which].m_d.ap);
						else move_sound(t_d.terrain[destination.x][destination.y],
					  	(short) c_town.monst.dudes[which].m_d.ap);
					}  
					
				return TRUE;
				}
	return FALSE;
}

// Looks at all spaces within 2, looking for a spot which is clear of nastiness and beings
// returns {0,0} if none found
// THIS MAKES NO ADJUSTMENTS FOR BIG MONSTERS!!!
location find_clear_spot(location from_where,short mode)
//mode; // 0 - normal  1 - prefer adjacent space
{
	location loc,store_loc = {0,0};
	short num_tries = 0,r1;
	
	while (num_tries < 75) {
		num_tries++;
		loc = from_where;
		r1 = get_ran(1,-2,2);
		loc.x = loc.x + r1;
		r1 = get_ran(1,-2,2);
		loc.y = loc.y + r1;
		if ((loc_off_act_area(loc) == FALSE) && (is_blocked(loc) == FALSE)
			&& (can_see(from_where,loc,1) == 0)
			&& (!(is_combat()) || (pc_there(loc) == 6))
			&& (!(is_town()) || (same_point(loc,c_town.p_loc) == FALSE))
			 && (!(misc_i[loc.x][loc.y] & 248)) &&
			(!(c_town.explored[loc.x][loc.y] & 254))) {
				if ((mode == 0) || ((mode == 1) && (adjacent(from_where,loc) == TRUE)))
					return loc;
					else store_loc = loc;			
				}
		}
	return store_loc;
}

short pc_there(location where)
{
	short i;

	for (i = 0; i < 6; i++)
			if ((same_point(where,pc_pos[i]) == TRUE) && (adven[i].main_status == 1))
				return i;
	return 6;	
}

location random_shift(location start)
{
	location store;

	store = start;
	store.x = store.x + get_ran(1,0,2) - 1;
	store.y = store.y + get_ran(1,0,2) - 1;

	return store;
}

Boolean outdoor_move_monster(short num,location dest)
{

	if ((outd_is_blocked(dest) == FALSE) && (outd_is_special(dest) == FALSE) && 
		(same_point(dest, party.p_loc) != TRUE) && 
		((out[dest.x][dest.y] > 21) || (out[dest.x][dest.y] < 5))) {
		party.out_c[num].direction = 
				set_direction(party.out_c[num].m_loc, dest);
		party.out_c[num].m_loc = dest;
		return TRUE;
	}
	else return FALSE;
}

Boolean town_move_monster(short num,location dest)
{
	if (monst_check_special_terrain(dest,1,num) == FALSE)
		return FALSE;

		if (monst_can_be_there(dest,num) == TRUE) {
			c_town.monst.dudes[num].m_d.direction = 
				set_direction(c_town.monst.dudes[num].m_loc, dest);
			c_town.monst.dudes[num].m_loc = dest;
			monst_inflict_fields(num);
			return TRUE;
		}
		else return FALSE;
}

Boolean monster_placid(short m_num)
{
	if ((c_town.monst.dudes[m_num].attitude == 0) || 
		((c_town.monst.dudes[m_num].attitude == 2) && (party.stuff_done[305][9] == 0)))
		{ return TRUE;}
		else { return FALSE;}
}

// This damages a monster by any fields it's in, and destroys any barrels or crates
// it's stiing on.
void monst_inflict_fields(short which_monst)
{
	short i,j,r1,k;
	location where_check;
	creature_data_type *which_m;
		
	if (c_town.monst.dudes[which_monst].active == 0)
		return;
		
	which_m = &c_town.monst.dudes[which_monst];
	for (i = 0; i < c_town.monst.dudes[which_monst].m_d.x_width; i++)
		for (j = 0; j < c_town.monst.dudes[which_monst].m_d.y_width; j++) 
		if (c_town.monst.dudes[which_monst].active > 0) {
			where_check.x = c_town.monst.dudes[which_monst].m_loc.x + i; 
			where_check.y = c_town.monst.dudes[which_monst].m_loc.y + j;
			if (is_quickfire(where_check.x,where_check.y)) {
				r1 = get_ran(2,1,8);
				damage_monst(which_monst,7,r1,0,1);
				break;
				}
			if (is_blade_wall(where_check.x,where_check.y)) {
				r1 = get_ran(6,1,8);
				damage_monst(which_monst,7,r1,0,0);
				break;
				}
			if (is_force_wall(where_check.x,where_check.y)) {
				r1 = get_ran(3,1,6);
				damage_monst(which_monst,7,r1,0,3);
				break;
				}
			if (is_sleep_cloud(where_check.x,where_check.y)) {
				charm_monst(which_m,0,11,3);
				break;
				}
			if (is_ice_wall(where_check.x,where_check.y)) {
				r1 = get_ran(3,1,6);
				if (c_town.monst.dudes[which_monst].m_d.spec_skill != 23)
					damage_monst(which_monst,7,r1,0,5);
				break;
				}
			if (is_scloud(where_check.x,where_check.y)) {
				r1 = get_ran(1,2,3);
				curse_monst(which_m,r1);
				break;
				}
			if ((is_web(where_check.x,where_check.y)) && (which_m->m_d.m_type != 12)) {
				monst_spell_note(which_m->number,19);
				r1 = get_ran(1,2,3);
				web_monst(which_m,r1);
				take_web(where_check.x,where_check.y);
				break;
				}
			if (is_fire_wall(where_check.x,where_check.y)) {
				r1 = get_ran(2,1,6);
				if (c_town.monst.dudes[which_monst].m_d.spec_skill != 22)
					damage_monst(which_monst,7,r1,0,1);	
				break;
				}
			}
	if (c_town.monst.dudes[which_monst].active > 0)
	for (i = 0; i < c_town.monst.dudes[which_monst].m_d.x_width; i++)
		for (j = 0; j < c_town.monst.dudes[which_monst].m_d.y_width; j++) {
			where_check.x = c_town.monst.dudes[which_monst].m_loc.x + i; 
			where_check.y = c_town.monst.dudes[which_monst].m_loc.y + j;
			if ((is_crate(where_check.x,where_check.y)) ||
				(is_barrel(where_check.x,where_check.y)) )
				for (k = 0; k < NUM_TOWN_ITEMS; k++)
					if ((t_i.items[k].variety > 0) && (is_contained(t_i.items[k]) == TRUE)
					&& (same_point(t_i.items[k].item_loc,where_check) == TRUE))
						t_i.items[k].item_properties = t_i.items[k].item_properties & 247;
			take_crate(where_check.x,where_check.y);
			take_barrel(where_check.x,where_check.y);
			if (is_fire_barrier(where_check.x,where_check.y)) {
				r1 = get_ran(2,1,10);
				damage_monst(which_monst,7,r1,0,1);
				}
			}


	
}

Boolean monst_check_special_terrain(location where_check,short mode,short which_monst)
//mode; // 1 - town 2 - combat
{
	char debug[60];
	unsigned char ter;
	short r1,i,j,choice,door_pc,guts = 0;
	Boolean can_enter = TRUE,mage = FALSE;
	location out_where,from_loc,to_loc;
	Boolean do_look = FALSE; // If becomes true, terrain changed, so need to update what party sees
	creature_data_type *which_m;
	short ter_abil;
	
	from_loc = c_town.monst.dudes[which_monst].m_loc;
	switch (mode) {	
		case 1:
			ter = t_d.terrain[where_check.x][where_check.y];
			break;	
		case 2:
			ter = combat_terrain[where_check.x][where_check.y];
			break;	
		}
	////
	which_m = &c_town.monst.dudes[which_monst];
	ter_abil = scenario.ter_types[ter].special;
	
		if ((mode > 0) && (ter_abil >= 16) && 
			(ter_abil <= 19)) {
			if (
				((ter_abil == 16) && (where_check.y > from_loc.y)) ||
				((ter_abil == 17) && (where_check.x < from_loc.x)) ||
				((ter_abil == 18) && (where_check.y < from_loc.y)) ||
				((ter_abil == 19) && (where_check.x > from_loc.x)) ) {
					return FALSE;
					}
			}

	// begin determining guts, which determines how enthused the monst is about entering
	// nasty barriers
	if ((which_m->m_d.mu > 0) || (which_m->m_d.cl > 0))
		mage = TRUE;
	if (which_m->m_d.spec_skill == 13)
		guts = 20;
		else guts = get_ran(1,1,(which_m->m_d.level / 2));
	guts += which_m->m_d.health / 20;
	if (mage == TRUE)
		guts = guts / 2;
	if (which_m->attitude == 0)
		guts = guts / 2;
		
	if ((is_antimagic(where_check.x,where_check.y)) && (mage == TRUE))
		return FALSE;
	if ((is_fire_wall(where_check.x,where_check.y)) && (which_m->m_d.spec_skill != 22)) {
			if (guts < 3) return FALSE;
		}
	if (is_force_wall(where_check.x,where_check.y)) {
			if (guts < 4) return FALSE;
		}
	if ((is_ice_wall(where_check.x,where_check.y)) && (which_m->m_d.spec_skill != 23)) {
			if (guts < 5) return FALSE;
		}
	if (is_sleep_cloud(where_check.x,where_check.y)) {
			if (guts < 8) return FALSE;
		}
	if (is_blade_wall(where_check.x,where_check.y)) {
			if (guts < 8) return FALSE;
		}
	if (is_quickfire(where_check.x,where_check.y)) {
			if (guts < 8) return FALSE;
		}
	if (is_scloud(where_check.x,where_check.y)) {
		if (guts < 4) return FALSE;
		}
	if ((is_web(where_check.x,where_check.y)) && (which_m->m_d.m_type != 12)) {
		if (guts < 3) return FALSE;
		}
	if (is_fire_barrier(where_check.x,where_check.y)) {
		if ((which_m->attitude % 2 == 1) && (get_ran(1,0,100) < (which_m->m_d.mu * 10 + which_m->m_d.cl * 4))) {
			play_sound(60);
			add_string_to_buf("Monster breaks barrier.");
			take_fire_barrier(where_check.x,where_check.y);
			}
			else {
				if (guts < 6) return FALSE;
				r1 = get_ran(1,0,10);
				if ((r1 < 8) || (monster_placid(which_monst)))
					can_enter = FALSE;
				}
		}
	if (is_force_barrier(where_check.x,where_check.y)) { /// Not in big towns
		if ((which_m->attitude % 2 == 1) && (get_ran(1,0,100) < (which_m->m_d.mu * 10 + which_m->m_d.cl * 4))
			&& (c_town.town_num >= 20)) {
			play_sound(60);
			add_string_to_buf("Monster breaks barrier.");
			take_force_barrier(where_check.x,where_check.y);
			}
			else can_enter = FALSE;
		}
	if (is_crate(where_check.x,where_check.y)) {
		if (monster_placid(which_monst))
			can_enter = FALSE;
			else {
				to_loc = push_loc(from_loc,where_check);
				take_crate((short) where_check.x,(short) where_check.y);
				if (to_loc.x > 0)
					make_crate((short) to_loc.x,(short) to_loc.y);
				for (i = 0; i < NUM_TOWN_ITEMS; i++)
					if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where_check))
					 && (is_contained(t_i.items[i]) == TRUE))
			 			t_i.items[i].item_loc = to_loc;
						}
		}
	if (is_barrel(where_check.x,where_check.y)) {
		if (monster_placid(which_monst))
			can_enter = FALSE;
			else {
				to_loc = push_loc(from_loc,where_check);
				take_barrel((short) where_check.x,(short) where_check.y);
				if (to_loc.x > 0)
				    	make_barrel((short) to_loc.x,(short) to_loc.y);
				for (i = 0; i < NUM_TOWN_ITEMS; i++)
					if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where_check))
					 && (is_contained(t_i.items[i]) == TRUE))
			 			t_i.items[i].item_loc = to_loc;
				
				}
		}
	if (monster_placid(which_monst) && // monstyers don't hop into bed when things are calm
		(scenario.ter_types[ter].picture == 143))
				can_enter = FALSE;
	if ((scenario.ter_types[ter].picture <= 212) && (scenario.ter_types[ter].picture >= 207))
		can_enter = FALSE;
	if (ter == 90) {
			if ((is_combat()) && (which_combat_type == 0)) {
				c_town.monst.dudes[which_monst].active = 0;
				add_string_to_buf("Monster escaped! ");
				}
			return FALSE;
		}
				
	switch (ter_abil) {
		// changing ter
		case 1:
			can_enter = FALSE;
			if (!(monster_placid(which_monst))) {
				t_d.terrain[where_check.x][where_check.y] = scenario.ter_types[ter].flag1;
				combat_terrain[where_check.x][where_check.y] = scenario.ter_types[ter].flag1;
				do_look = TRUE;
				if (point_onscreen(center,where_check))
					play_sound(scenario.ter_types[ter].flag2);
				}
			break;

		case 20: case 21: case 15: 
			can_enter = FALSE;
			break;
						
		case 2:
			if (c_town.monst.dudes[which_monst].m_d.immunities & 8)
				return TRUE;
				else return FALSE;
			break;
		}

	// Action may change terrain, so update what's been seen
	if (do_look == TRUE) {
		if (is_town())
			update_explored(c_town.p_loc);
		if (is_combat())
			for (i = 0; i < 6; i++)
				if (adven[i].main_status == 1)
					update_explored(pc_pos[i]);
		}
		
	return can_enter;
}

void forced_place_monster(unsigned char which,location where)
{
	Boolean free_spot = FALSE;
	short i = 0,r1;

	while ((free_spot == FALSE) && (i < T_M)) {
		if (c_town.monst.dudes[i].active == 0)
			free_spot = TRUE;
		i++;
		}
	do
		r1 = get_ran(1,0,59);
		while ((c_town.monst.dudes[r1].monst_start.spec1 != 0) || (c_town.monst.dudes[r1].monst_start.spec2 != 0));
	if (free_spot == FALSE)
		c_town.monst.dudes[r1].active = 0;
	place_monster(which,where);
}

void magic_adjust(creature_data_type *which_m,short *how_much)
{
	if (which_m->m_d.spec_skill == 26) {
		*how_much = 0;
		which_m->m_d.health += 3;
		}
	if (which_m->m_d.immunities & 1)
		*how_much = *how_much / 2;
	if (which_m->m_d.immunities & 2)
		*how_much = 0;
}

void poison_monst(creature_data_type *which_m,short how_much)
{
	if (which_m->m_d.immunities & 64)
		how_much = how_much / 2;
	if (which_m->m_d.immunities & 128) {
		monst_spell_note(which_m->number,10);
		return;
		}
	which_m->m_d.status[2] = min(8, which_m->m_d.status[2] + how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 4);

}
void acid_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[13] = minmax(-8,8, which_m->m_d.status[13] + how_much);
	monst_spell_note(which_m->number,31);

}

void slow_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[3] = minmax(-8,8, which_m->m_d.status[3] - how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 2);

}
void curse_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[1] = minmax(-8,8, which_m->m_d.status[1] - how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 5);

}
void web_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[6] = minmax(-8,8, which_m->m_d.status[6] + how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 19);

}
void scare_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.morale = which_m->m_d.morale - how_much;
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 1);

}
void disease_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[7] = minmax(-8,8, which_m->m_d.status[7] + how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 25);

}

void dumbfound_monst(creature_data_type *which_m,short how_much)
{
	magic_adjust(which_m,&how_much);
	which_m->m_d.status[9] = minmax(-8,8, which_m->m_d.status[9] + how_much);
	monst_spell_note(which_m->number,(how_much == 0) ? 10 : 22);

}

void charm_monst(creature_data_type *which_m,short penalty,short which_status,short amount)
// Also used for sleep and paralyze, which_statys is 0 means charm
{
	short r1;
	

	if ((which_status == 11) && 
		((which_m->m_d.m_type == 8) || (which_m->m_d.m_type == 10) || (which_m->m_d.m_type == 11)))
		return;
	r1 = get_ran(1,0,100);
	if (which_m->m_d.immunities & 1)
		r1 = r1 * 2;
	if (which_m->m_d.immunities & 2)
		r1 = 200;
	r1 += penalty;
	if (which_status == 11)
		r1 -= 25;
	if (which_status == 12)
		r1 -= 15;
	if ((which_status == 11) && (which_m->m_d.spec_skill == 32))
		return;
	
	if (r1 > charm_odds[which_m->m_d.level / 2]) {
		//one_sound(68);
		monst_spell_note(which_m->number,10);
		}
		else {
			if (which_status == 0) {
				which_m->attitude = 2;
				monst_spell_note(which_m->number,23);
				}
				else {
					which_m->m_d.status[which_status] = amount;
					if (which_status == 11)
						monst_spell_note(which_m->number,28);
					if (which_status == 12)
						monst_spell_note(which_m->number,30);					
					}
			//one_sound(53);
			}
}
void record_monst(creature_data_type *which_m)
{
	short r1;
	char str[60];
	
	r1 = get_ran(1,0,100);
	r1 = (r1 * 7) / 10;
	
	if ((which_m->m_d.x_width > 1) || (which_m->m_d.y_width > 1)) {
		ASB("Capture Soul: Monster is too big.");
		}
	else if ((r1 > charm_odds[which_m->m_d.level / 2]) || (which_m->m_d.spec_skill == 12)
		|| (which_m->m_d.m_type == 3)) {
		monst_spell_note(which_m->number,10);
		play_sound(68);
		}
		else {
			monst_spell_note(which_m->number,24);
			r1 = get_ran(1,0,3);
			if (party.imprisoned_monst[r1] == 0)
				party.imprisoned_monst[r1] = which_m->number;
				else {
					r1 = get_ran(1,0,3);
					party.imprisoned_monst[r1] = which_m->number;
					}
			ASB("Capture Soul: Success!");
			sprintf((char *)str,"  Caught in slot %d.",r1 + 1);
			add_string_to_buf((char *)str);
			play_sound(53);
			}
}
// returns 90 is no placement, OW returns # of spot
////
short place_monster(unsigned char which,location where)
{
	short i = 0,x,y,j;
	
	while ((i < T_M) && ((c_town.monst.dudes[i].active != 0) ||
			(c_town.monst.dudes[i].monst_start.spec_enc_code > 0))) {
		i++;
		}
	
	if (i < T_M) {
		c_town.monst.dudes[i].m_d = return_monster_template((unsigned char) which);
		c_town.monst.dudes[i].attitude = scenario.scen_monsters[which].default_attitude;
		if (c_town.monst.dudes[i].attitude % 2 == 0)
			c_town.monst.dudes[i].attitude = 1;
		c_town.monst.dudes[i].mobile = TRUE;
		c_town.monst.dudes[i].active = 2;
		c_town.monst.dudes[i].number = which;
		c_town.monst.dudes[i].m_loc = where;	
		c_town.monst.dudes[i].summoned = 0;
		c_town.monst.dudes[i].monst_start = null_start_type;	
		monst_target[i] = 6;
	
		if (c_town.monst.dudes[i].m_d.picture_num < 1000) {
			add_monst_graphic(which,1);
			}

		take_crate(where.x,where.y);
		take_barrel(where.x,where.y);
		
		return i;
	}
	return 90;
}

// returns TRUE if placement was successful
Boolean summon_monster(unsigned char which,location where,short duration,short given_attitude)
//which; // if in town, this is caster loc., if in combat, this is where to try
					// to put monster
{
	location loc;
	short which_m,spot;

	if ((is_town()) || (monsters_going)) {
		// Ooooh ... mondo kludge. Need to find caster's attitude to give it to monst.
		which_m = monst_there(where);
//		if (pc_there(where) < 6) 
//			which_att = 2;
//			else if (which_m == 90)
//				which_att = 1;
//				else which_att = c_town.monst.dudes[which_m].attitude;
		loc = find_clear_spot(where,0);
		if (loc.x == 0)
			return FALSE;
		}
		else {
			// pc may be summoning using item, in which case where will be pc's space, so fix
			if (pc_there(where) < 6) {
				where = find_clear_spot(where,0);
				if (where.x == 0)
					return FALSE;
				}
			if ((is_barrel(where.x,where.y)) || (is_crate(where.x,where.y)))
				return FALSE;
			loc = where;
			}
			
	spot = place_monster(which,loc);
	if (spot >= T_M) {
		if (duration < 100)
			add_string_to_buf("  Too many monsters.");
			//ASB("  Monster fails to summon monster.");
		return FALSE;
		}
	//play_sound(61);
		
//	if (duration < 100)
		c_town.monst.dudes[spot].attitude = given_attitude;
//		else c_town.monst.dudes[spot].attitude = which_att;
	
	if (which > 0) {//monster here for good
		c_town.monst.dudes[spot].summoned = duration;
		monst_spell_note(which,21);
		}
		else c_town.monst.dudes[spot].summoned = 0;
		
	return TRUE;
}

void activate_monsters(short code,short attitude)
{
	short i,pict; 
	unsigned char which;
	
	if (code == 0)
		return;
	for (i = 0; i < T_M; i++) 
		if (c_town.monst.dudes[i].monst_start.spec_enc_code == code)
			{
				c_town.monst.dudes[i].monst_start.spec_enc_code = 0;
				c_town.monst.dudes[i].active = 2;
				which = c_town.monst.dudes[i].number;
				c_town.monst.dudes[i].attitude = t_d.creatures[i].start_attitude;

				c_town.monst.dudes[i].summoned = 0;
				c_town.monst.dudes[i].m_loc = t_d.creatures[i].start_loc;
				c_town.monst.dudes[i].m_d = return_monster_template(c_town.monst.dudes[i].number);
				monst_target[i] = 6;
				
				add_monst_graphic(c_town.monst.dudes[i].number,1);
				take_crate(c_town.monst.dudes[i].m_loc.x,c_town.monst.dudes[i].m_loc.y);
				take_barrel(c_town.monst.dudes[i].m_loc.x,c_town.monst.dudes[i].m_loc.y);
			}
}

short get_encumberance(short pc_num)
{
	short store = 0,i,what_val;
	
	for (i = 0; i < 24; i++)
		if (adven[pc_num].equip[i] == TRUE) {
			what_val = adven[pc_num].items[i].awkward;
			if ((what_val == 1) && (get_ran(1,0,130) < hit_chance[adven[pc_num].skills[8]]))
				what_val--;
			if ((what_val > 1) && (get_ran(1,0,70) < hit_chance[adven[pc_num].skills[8]]))
				what_val--;
			store += what_val;
			}
	return store;
}

short get_summon_monster(short summon_class)
{
	short i,j;
	
	for (i = 0; i < 200; i++) {
		j = get_ran(1,0,255);
		if (scenario.scen_monsters[j].summon_type == summon_class) {
			return j;
			}
		}
	ASB("  Summon failed.");
	return -1;
}