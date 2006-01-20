#include <io.h>
#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
#include "WT/wtdef.h"
#include "WT/wt_facilities.h"

static char text[133];

static int Fac_wt_wakeup = WT_FACILITY_WAKEUP;
static unsigned int Fac_wt_keyboard = WT_FACILITY_KEYBOARD;
static int Fac_wt_scr = WT_FACILITY_SCR;
static int Fac_wt_broadcast = WT_FACILITY_BROADCAST;

static Pasteboard *Kbd = 0;

static char Last_char = 0;
static char Armed = 0;
static int  Insignal;
#define KEY_BUF_SIZE 80
static char Key_buffer[KEY_BUF_SIZE+2] = "";
static int  Key_ptr = 0;
static int  Last_key_stroke = -1;

static int (* User_ast) () = 0;

static int Exit_status;

static int  (*User_mouse_handler)(int,int) = 0;
static int  (*User_configure_handler)(int,int) = 0;

extern int scr_ignore_input;

/*----------------------------------------------------------------------------*/
static int scrc_exit_handler (int* status, Pasteboard *pb)    {
  scrc_cursor_on (pb);
  scrc_end_pasteboard_update (pb);
  wtc_insert_head (Fac_wt_wakeup);
  scrc_delete_pasteboard (pb);
  return 1;
}
/*----------------------------------------------------------------------------*/
int scrc_rearm_keyboard (unsigned int /* fac */, void* /* par */)   {
  if (Armed) return 0;
  Armed = 1;  
#if 0
  if (!Insignal)  {
    Insignal = sig_book_signal();
    sig_declare_signal(Insignal, scrc_ast );
  }
  _ss_ssig(0,Insignal);
#endif
  return (1);
}

/*----------------------------------------------------------------------------*/
int scrc_ast_keyboard (unsigned int /* fac */)   {
  if (scr_ignore_input == 0)  {
    wtc_insert (Fac_wt_keyboard);
    if (User_ast) (* User_ast) ();
  }
  return 1;
}

/*----------------------------------------------------------------------------*/
int scrc_handler_keyboard (unsigned int fac, void* /* par */)  {
  int status = 0;
  do  {
    //status = _gs_rdy(0);
    if( status > 0 )    {  
      read(0,&Last_char,1);
      if (Key_ptr >= KEY_BUF_SIZE) status = 0;
      else if (Last_char)      {
        Key_buffer[Key_ptr] = Last_char;
        Key_ptr++;
        Key_buffer[Key_ptr] = 0;
        Last_key_stroke = scrc_check_key_buffer (Key_buffer);
        if (Last_key_stroke > 0)        {
          Key_ptr = 0;
          Key_buffer[Key_ptr] = 0;
  
          if (Kbd->moving)          {
            if (scrc_action_moving_display (Kbd, Last_key_stroke))
              wtc_insert_head (Fac_wt_scr);
            else
              Last_key_stroke = -1;
          }
          else if (Kbd->resizing)          {
            if (scrc_action_resizing_display (Kbd, Last_key_stroke))
              wtc_insert_head (Fac_wt_scr);
            else
              Last_key_stroke = -1;
          }
          else
            wtc_insert_head (Fac_wt_scr);
        }
      }
    }
  } while( status > 0);
  Armed = 0;
  return (0);
}

/*----------------------------------------------------------------------------*/
static int scrc_rearm (unsigned int /* fac */, void* /* param */)  {
  return scrc_fflush(Kbd);
}

/*----------------------------------------------------------------------------*/
int scrc_init_screen (Pasteboard *pb, int rows, int cols)   {
  if (cols) pb->cols = cols;
  if (rows) pb->rows = rows;
  scrc_setANSI();
  if (!Kbd)  {
    Kbd = pb;
    wtc_init();
    wtc_subscribe (Fac_wt_keyboard, scrc_rearm_keyboard, scrc_handler_keyboard);
    wtc_subscribe (Fac_wt_scr, scrc_rearm, (wt_callback_t)0);
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
int scrc_get_last_key ()    {
  int key = Last_key_stroke;
  Last_key_stroke = -1;
  if (key < 0) key = 0;
  return (key);
}


int scrc_save_screen_rearm()  {
  return scrc_rearm_keyboard(Fac_wt_keyboard, 0);
}


/*----------------------------------------------------------------------------*/
int scrc_fputs (Pasteboard *pb)   {
  return write(1,pb->bufout, strlen(pb->bufout));
}

/*---------------------------------------------------------------------------*/
int scrc_wait (Display *disp)   {
  return scrc_read_keyboard(disp, 1);
}

/*---------------------------------------------------------------------------*/
int scrc_read_keyboard (Display *disp, int wait)  {
  int status, sub_status;
  unsigned int event;
  void* dummy;  
  if (wait) status = wtc_wait (&event,&dummy,&sub_status);
  return scrc_get_last_key ();
}

/*----------------------------------------------------------------------------*/
int scrc_read (Display *disp, unsigned char *buffer, int wait)      {
  int sub_status;
  static int count = 0;
  unsigned int c;
  void* dummy;
  
  if (!Last_char)  {
    scrc_fflush(disp->pb);
    if (wait) wtc_wait (&c,&dummy,&sub_status);
  }
  *buffer = Last_char;
  if (Last_char)  {
    Armed = 0;
    Last_char = 0;
  }
  return 1;
}

/*---------------------------------------------------------------------------*/
int scrc_test_input ()    {
  scrc_rearm (0,0);
  return (Last_char);
}

/*----------------------------------------------------------------------------*/
int scrc_get_smgid (Pasteboard *pb)   {
  return pb->smgid;
}

/*----------------------------------------------------------------------------*/
int scrc_enable_unsolicited_input (Pasteboard *pb, int (* ast)())   {
  User_ast = ast;
  wtc_subscribe (Fac_wt_keyboard, scrc_rearm_keyboard, scrc_handler_keyboard);
  scrc_rearm_keyboard (Fac_wt_keyboard,0);
  return 1;
}

/*----------------------------------------------------------------------------*/
int scrc_disable_unsolicited_input (Pasteboard *pb)   {
  wtc_remove (Fac_wt_keyboard);
  return 1;
}

/*----------------------------------------------------------------------------*/
int scrc_change_pbd_characteristics (Pasteboard *pb)  {
  return 1;
}

/*----------------------------------------------------------------------------*/
int scrc_delete_physical_pasteboard (Pasteboard *pb)    {
  return 1;
}

/*---------------------------------------------------------------------------*/
void scrc_set_mouse_handler (Pasteboard* pb, int (*handler)(int,int))  {
  User_mouse_handler = handler;
}

/*---------------------------------------------------------------------------*/
void scrc_set_configure_handler (Pasteboard* pb, int (*handler)(int,int))  {
  User_configure_handler = handler;
}

/*---------------------------------------------------------------------------*/
void scrc_iconify_display (Display* d)    {
}

/*---------------------------------------------------------------------------*/
void scrc_uniconify_display (Display* d)    {
}
