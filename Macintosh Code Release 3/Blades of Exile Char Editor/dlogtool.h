void cd_init_dialogs();
short cd_create_dialog_parent_num(short dlog_num,short parent);
short cd_create_dialog(short dlog_num,WindowPtr parent);
void process_new_window (WindowPtr hDlg);
void cd_set_edit_focus();
short cd_kill_dialog(short dlog_num,short parent_message);
short cd_process_click(WindowPtr window,Point the_point, short mods,short *item);
short cd_process_keystroke(WindowPtr window,char char_hit,short *item);
void cd_init_button(short dlog_num,short item_num, short button_num, short status);
void cd_attach_key(short dlog_num,short item_num,char key);
void csp(short dlog_num, short item_num, short pict_num);
void cd_set_pict(short dlog_num, short item_num, short pict_num);
void cd_activate_item(short dlog_num, short item_num, short status);
short cd_get_active(short dlog_num, short item_num);
void cd_get_item_text(short dlog_num, short item_num, char *str);
void csit(short dlog_num, short item_num, char *str);
void cd_retrieve_text_edit_str(short dlog_num, char *str);
void cd_set_text_edit_str(short dlog_num, char *str);
void cd_set_item_text(short dlog_num, short item_num, char *str);
void cdsin(short dlog_num, short item_num, short num) ;
void cd_set_item_num(short dlog_num, short item_num, short num);
void cd_set_led(short dlog_num,short item_num,short state);
void cd_set_flag(short dlog_num,short item_num,short flag);
short cd_get_led(short dlog_num,short item_num);
void cd_text_frame(short dlog_num,short item_num,short frame);
void cd_add_label(short dlog_num, short item_num, char *label, short label_flag);
void cd_take_label(short dlog_num, short item_num);
void cd_key_label(short dlog_num, short item_num,short loc);
void cd_draw_item(short dlog_num,short item_num);
void cd_initial_draw(short dlog_num);
void cd_draw(short dlog_num);
void cd_redraw(WindowPtr window);
void cd_frame_item(short dlog_num, short item_num, short width);
void cd_erase_item(short dlog_num, short item_num);
void cd_erase_rect(short dlog_num,Rect to_fry);
void cd_press_button(short dlog_num, short item_num);
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index);
short cd_get_dlg_index(short dlog_num);
short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key);
short cd_get_item_id(short dlg_num, short item_num);
void center_window(WindowPtr window);
Rect get_item_rect(WindowPtr hDlg, short item_num);
void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val);
void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, Boolean do_frame,short win_or_gworld);
Rect calc_rect(short i, short j);
void beep() ;

