/*
 *  menubox.c -- implements the menu box
 *
 *  ORIGINAL AUTHOR: Savio Lam (lam836@cs.cuhk.hk)
 *  MODIFIED FOR LINUX KERNEL CONFIG BY: William Roadcap (roadcapw@cfw.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "dialog.h"

static int menu_width, item_x;

/*
 * Print menu item
 */
static void
print_item (WINDOW * win, const char *item, int choice, int selected, int hotkey)
{
    int i, j;
    char menu_item[menu_width+1];

    strncpy(menu_item, item, menu_width);
    menu_item[menu_width] = 0;
    j = first_alpha(menu_item);

    /* Clear 'residue' of last item */
    wattrset (win, menubox_attr);
    wmove (win, choice, 0);
    for (i = 0; i < menu_width; i++)
	waddch (win, ' ');
    wattrset (win, selected ? item_selected_attr : item_attr);
    mvwaddstr (win, choice, item_x, menu_item);
    if (hotkey) {
    	wattrset (win, selected ? tag_key_selected_attr : tag_key_attr);
    	mvwaddch(win, choice, item_x+j, menu_item[j]);
    }
}

/*
 * Print the scroll indicators.
 */
static void
print_arrows (WINDOW * win, int choice, int item_no, int scroll,
		int y, int x, int height)
{
    int cur_y, cur_x;

    getyx(win, cur_y, cur_x);

    wmove(win, y, x);

    if (scroll > 0) {
	wattrset (win, uarrow_attr);
	waddch (win, ACS_UARROW);
	waddstr (win, "(-)");
    }
    else {
	wattrset (win, menubox_attr);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
    }

   y = y + height + 1;
   wmove(win, y, x);

   if ((height < item_no) && (scroll + choice < item_no - 1)) {
	wattrset (win, darrow_attr);
	waddch (win, ACS_DARROW);
	waddstr (win, "(+)");
    }
    else {
	wattrset (win, menubox_border_attr);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
	waddch (win, ACS_HLINE);
   }

   wmove(win, cur_y, cur_x);
}

/*
 * Display the termination buttons.
 */
static void
print_buttons (WINDOW *win, int height, int width, int selected)
{
    int x = width / 2 - 16;
    int y = height - 2;

    print_button (win, "Select", y, x, selected == 0);
    print_button (win, " Exit ", y, x + 12, selected == 1);
    print_button (win, " Help ", y, x + 24, selected == 2);

    wmove(win, y, x+1+12*selected);
    wrefresh (win);
}

/*
 * Display a menu for choosing among a number of options
 */
int
dialog_menu (const char *title, const char *prompt, int height, int width,
		int menu_height, const char *current, int item_no,
		const char * const * items)

{
    int i, j, x, y, box_x, box_y;
    int key = 0, button = 0, scroll = 0, choice = 0, first_item = 0, max_choice;
    WINDOW *dialog, *menu;

    max_choice = MIN (menu_height, item_no);

    /* center dialog box on screen */
    x = (COLS - width) / 2;
    y = (LINES - height) / 2;

    draw_shadow (stdscr, y, x, height, width);

    dialog = newwin (height, width, y, x);
    keypad (dialog, TRUE);

    draw_box (dialog, 0, 0, height, width, dialog_attr, border_attr);
    wattrset (dialog, border_attr);
    mvwaddch (dialog, height - 3, 0, ACS_LTEE);
    for (i = 0; i < width - 2; i++)
	waddch (dialog, ACS_HLINE);
    wattrset (dialog, dialog_attr);
    waddch (dialog, ACS_RTEE);

    if (title != NULL) {
	wattrset (dialog, title_attr);
	mvwaddch (dialog, 0, (width - strlen(title))/2 - 1, ' ');
	waddstr (dialog, (char *)title);
	waddch (dialog, ' ');
    }

    wattrset (dialog, dialog_attr);
    print_autowrap (dialog, prompt, width - 2, 1, 3);

    menu_width = width - 6;
    box_y = height - menu_height - 5;
    box_x = (width - menu_width) / 2 - 1;

    /* create new window for the menu */
    menu = subwin (dialog, menu_height, menu_width,
		y + box_y + 1, x + box_x + 1);
    keypad (menu, TRUE);

    /* draw a box around the menu items */
    draw_box (dialog, box_y, box_x, menu_height + 2, menu_width + 2,
	      menubox_border_attr, menubox_attr);

    /*
     * Find length of longest item in order to center menu.
     * Set 'choice' to default item. 
     */
    item_x = 0;
    for (i = 0; i < item_no; i++) {
	item_x = MAX (item_x, MIN(menu_width, strlen (items[i * 2 + 1]) + 2));
	if (strcmp(current, items[i*2]) == 0) choice = i;
    }

    item_x = (menu_width - item_x) / 2;

    if (choice >= max_choice){
	scroll = first_item = choice - max_choice + 1;
	choice = max_choice-1;
    }

    /* Print the menu */
    for (i=0; i < max_choice; i++) {
	print_item (menu, items[(first_item + i) * 2 + 1], i, i == choice,
                    (items[(first_item + i)*2][0] != ':'));
    }

    wnoutrefresh (menu);

    print_arrows(dialog, choice, item_no, scroll,
		 box_y, box_x+item_x+1, menu_height);

    print_buttons (dialog, height, width, 0);

    while (key != ESC) {
	key = wgetch (dialog);

        for (i = choice+1; i < max_choice; i++) {
		j = first_alpha(items[(scroll+i)*2+1]);
		if (toupper(key) == toupper(items[(scroll+i)*2+1][j]))
                	break;
	}
	if (i == max_choice)
       		for (i = 0; i < max_choice; i++) {
			j = first_alpha(items[(scroll+i)*2+1]);
			if (toupper(key) == toupper(items[(scroll+i)*2+1][j]))
                		break;
		}

	if (i < max_choice || key == KEY_UP || key == KEY_DOWN || key == '-' || key == '+') {
	    if (key == KEY_UP || key == '-') {
		if (choice == 0) {
		    if (scroll) {

			/* Scroll menu down */
			if (menu_height > 1) {
			    /* De-highlight current first item */
			    print_item (menu, items[scroll*2+1], 0, FALSE,
                                        (items[scroll*2][0] != ':'));
			    scrollok (menu, TRUE);
			    wscrl (menu, -1);
			    scrollok (menu, FALSE);
			}
			scroll--;
			print_item (menu, items[scroll * 2 + 1], 0, TRUE,
                                   (items[scroll*2][0] != ':'));
			wnoutrefresh (menu);

    			print_arrows(dialog, choice, item_no, scroll,
		 			box_y, box_x+item_x+1, menu_height);

		    }
		    continue;	/* wait for another key press */
		} else
		    i = choice - 1;
	    } else if (key == KEY_DOWN || key == '+')
		if (choice == max_choice - 1) {
		    if (scroll + choice < item_no - 1) {
			/* Scroll menu up */
			if (menu_height > 1) {
			    /* De-highlight current last item */
			    print_item (menu, items[(scroll + max_choice - 1)
					   * 2 + 1], max_choice - 1, FALSE,
                                           (items[(scroll+max_choice-1)*2][0] != ':'));
			    scrollok (menu, TRUE);
			    scroll (menu);
			    scrollok (menu, FALSE);
			}
			scroll++;
			print_item (menu, items[(scroll+max_choice-1)*2+1],
				    max_choice-1, TRUE,
                                    (items[(scroll+max_choice-1)*2][0] != ':'));

			wnoutrefresh (menu);

    			print_arrows(dialog, choice, item_no, scroll,
		 			box_y, box_x+item_x+1, menu_height);

			wrefresh (dialog);
		    }
		    continue;	/* wait for another key press */
		} else
		    i = choice + 1;

	    if (i != choice) {
		/* De-highlight current item */
		print_item (menu, items[(scroll+choice)*2+1], choice, FALSE,
                                (items[(scroll+choice)*2][0] != ':'));

		/* Highlight new item */
		choice = i;
		print_item (menu, items[(scroll+choice)*2+1], choice, TRUE,
                                (items[(scroll+choice)*2][0] != ':'));
		wnoutrefresh (menu);
		wrefresh (dialog);
	    }
	    continue;		/* wait for another key press */
	}

	switch (key) {
	case KEY_LEFT:
	case TAB:
	case KEY_RIGHT:
	    button = ((key == KEY_LEFT ? --button : ++button) < 0)
			? 2 : (button > 2 ? 0 : button);

	    print_buttons(dialog, height, width, button);
	    wrefresh (dialog);
	    break;
	case ' ':
	case 'S':
	case 's':
	    delwin (dialog);
            fprintf(stderr, items[(scroll + choice) * 2]);
	    return 0;
	case 'H':
	case 'h':
	case '?':
	    button = 2;
	case '\n':
	    delwin (dialog);
	    if (button == 2) 
            	fprintf(stderr, "%s \"%s\"", 
			items[(scroll + choice) * 2],
			items[(scroll + choice) * 2 + 1] +
			first_alpha(items[(scroll + choice) * 2 + 1]));
	    else
            	fprintf(stderr, items[(scroll + choice) * 2]);

	    return button;
	case 'E':
	case 'e':
	case 'X':
	case 'x':
	    key = ESC;
	case ESC:
	    break;
	}
    }

    delwin (dialog);
    return -1;			/* ESC pressed */
}
