#include <xinu.h>
#include <gui.h>
#include <renderer.h>
#include "atlas.inl"

#define BUFFER_SIZE 16384

static int width  = 800;
static int height = 600;
static int buf_idx;
struct mouse_state prev_m_state;
struct mouse_state m_state;

uint32 mu_color_to_rgb32(mu_Color color)
{
	/* Construct 32-bit format, we get 0x00RRGGBB by shifting bits */
	uint32 color32 = 0x00ffffff & ((color.r << 16) | (color.g << 8) | color.b);
	return color32;
}


void r_init() {
	/* init mouse states */
	prev_m_state.pos_x = 0;
	prev_m_state.pos_y = 0;
	prev_m_state.left_mouse = 0;
	prev_m_state.left_mouse = 0;
	m_state.pos_x = 0;
	m_state.pos_y = 0;
	m_state.left_mouse = 0;
	m_state.left_mouse = 0;
}


void r_draw_rect(mu_Rect rect, mu_Color color) {
  gui_draw_rect(rect.x, rect.y, rect.w, rect.h, mu_color_to_rgb32(color));
}


void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
  gui_print_text(pos.x, pos.y, text, mu_color_to_rgb32(color), 0);
}


void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
  int c, w, h;
	mu_Vec2 pos;
	char buf[2];
	switch (id) {
	  case MU_ICON_CLOSE:		c = 'x'; break;
	  case MU_ICON_CHECK:		c = 'X'; break;
	  case MU_ICON_COLLAPSED:	c = '>'; break;
	  case MU_ICON_EXPANDED:	c = 'v'; break;
  	}
	buf[0] = c;
	buf[1] = 0;
	w = r_get_text_width(buf, 1);
	h = r_get_text_height();
	pos.x = rect.x + (rect.w - w) / 2;
	pos.y = rect.y + (rect.h - h) / 2;
	r_draw_text(buf, pos, color);
}


void r_draw_image(void * addr, mu_Rect rect) {
  gui_draw_image(rect.x, rect.y, rect.w, rect.h, (uint32 *) addr);
}


int r_get_text_width(const char *text, int len) {
  return len * 6;
}


int r_get_text_height(void) {
  return 8;
}


void r_set_clip_rect(mu_Rect rect) {
}


void r_clear(mu_Color clr) {
	gui_paint_screen(mu_color_to_rgb32(clr));
}

void r_present(void) {
	gui_flush();
}


extern void write_log(char * );

int mouse_buf[3];
void r_draw_mouse() {
    read(MOUSE, mouse_buf, 3);
	/* check left mouse being pressed */
	m_state.left_mouse = (mouse_buf[0] & 0x01) ? 1 : 0;
	/* update mouse position */
	m_state.pos_x = mouse_buf[1];
	m_state.pos_y = mouse_buf[2];
	/* render */
    gui_draw_image(m_state.pos_x,m_state.pos_y,18,11,pointer);
}

char cc;
void r_handle_input(mu_Context *ctx)
{
	char buf[2];
	buf[0] = cc++;
	buf[1] = 0;
	write_log(buf);
	/* keep mouse position updated in microui */
	mu_input_mousemove(ctx, m_state.pos_x, m_state.pos_y);
	if(prev_m_state.left_mouse != m_state.left_mouse) {
		if(m_state.left_mouse) {
			mu_input_mousedown(ctx, m_state.pos_x, m_state.pos_y, 0x01);
		} else {
			mu_input_mouseup(ctx, m_state.pos_x, m_state.pos_y, 0x01);
		}
		/* update previous mouse state once used*/
		prev_m_state.left_mouse = m_state.left_mouse;
	}
}
