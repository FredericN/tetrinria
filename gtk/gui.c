#include "gui.h"

#include <malloc.h>


void fill_cell(cairo_t *cr, TrnColor color, int i, int j, bool border_shade)
{
  const int line_width = 2;
  double x = j * NPIXELS + line_width;
  double y = i * NPIXELS + line_width;
  double width = NPIXELS - line_width;
  double height = NPIXELS - line_width;

  cairo_rectangle(cr, x, y, width, height);

  cairo_set_source_rgb(cr, color.red, color.green, color.blue);
  cairo_fill_preserve(cr);
  cairo_set_line_width(cr, line_width);
  if (border_shade) {
    cairo_set_source_rgb(cr, color.red * 0.5, color.green * 0.5, color.blue * 0.5);
  }
  cairo_stroke(cr);
}

gint on_timeout_event(gpointer data)
{
  TrnGUI* self = (TrnGUI*)data;
  trn_game_try_to_move_down(self->game);
  trn_gui_update_view(self);
  g_timeout_add(trn_game_delay(self->game),on_timeout_event,(gpointer)self);
  return 0;
}

gboolean on_preview_expose_event(GtkWidget* preview,
                                 GdkEventExpose* UNUSED(event),
                                 TrnGUI* gui)
{
  cairo_t* cr = gdk_cairo_create(preview->window);

  int rowIndex, columnIndex;

  for ( rowIndex = 0; rowIndex < TRN_TETROMINO_NUMBER_OF_SQUARES; ++rowIndex )
  {
    for ( columnIndex = 0; columnIndex < TRN_TETROMINO_NUMBER_OF_SQUARES; ++columnIndex)
    {
      fill_cell(cr,TRN_WHITE,rowIndex,columnIndex,false);
    }
  }

  int squareIndex;

  TrnPiece* piece = gui->game->next_piece;

  TrnTetrominoRotation tetromino_rotation = 
      TRN_ALL_TETROMINO_FOUR_ROTATIONS[piece->type][TRN_ANGLE_0];

  TrnColor color = TRN_ALL_TETROMINO_COLORS[piece->type];

  for (squareIndex=0;squareIndex<TRN_TETROMINO_NUMBER_OF_SQUARES;++squareIndex)
  {
    rowIndex = tetromino_rotation[squareIndex].rowIndex;
    columnIndex = tetromino_rotation[squareIndex].columnIndex;
    fill_cell(cr,color,rowIndex,columnIndex,true);
  }

  return TRUE;
}

gboolean on_matrix_expose_event(GtkWidget *matrix,GdkEventExpose* UNUSED(event), TrnGUI* gui)
{
  cairo_t* cr = gdk_cairo_create(matrix->window);

  TrnGrid* grid = gui->game->grid;
  TrnColor color;

  int irow, icol;
  for (irow = 0; irow < grid->numberOfRows; irow++) {
    for (icol = 0; icol < grid->numberOfColumns; icol++) {
      TrnPositionInGrid pos;
      pos.rowIndex = irow;
      pos.columnIndex = icol;
      TrnTetrominoType type = trn_grid_get_cell(grid,pos);
      if (type == TRN_TETROMINO_VOID)
          color = TRN_BLACK;
      else
          color = TRN_ALL_TETROMINO_COLORS[type];
      fill_cell(cr, color, irow, icol, true);
    }
  }
  cairo_destroy(cr);
  return TRUE;
}

gboolean on_key_press_event(GtkWidget* UNUSED(widget),
                            GdkEventKey* event,
                            TrnGUI* gui)
{

  switch (event->keyval) {
  case GDK_Left:
    trn_game_try_to_move_left(gui->game);
    break;
  case GDK_Right:
    trn_game_try_to_move_right(gui->game);
    break;
  case GDK_Up:
    trn_game_try_to_rotate_clockwise(gui->game);
    break;
  case GDK_Down:
    trn_game_try_to_move_down(gui->game);
    break;
  case GDK_KEY_space:
    trn_game_move_to_bottom(gui->game);
    break;

    break;
  }

  trn_gui_update_view(gui);
  
  return TRUE;
}

gboolean button_newgame_clicked(GtkWidget* UNUSED(widget), TrnGUI* gui) {
  int numberOfRows = gui->game->grid->numberOfRows;
  int numberOfColumns = gui->game->grid->numberOfColumns;
  int delay = gui->game->initial_delay;
  trn_game_destroy(gui->game);
  gui->game = trn_game_new(numberOfRows, numberOfColumns, delay);
  return TRUE;
}

gboolean button_pause_clicked(GtkWidget* UNUSED(widget), TrnGUI* gui) {
  if (gui->game->status == TRN_GAME_ON)
    gui->game->status = TRN_GAME_PAUSED;
  else if (gui->game->status == TRN_GAME_PAUSED)
    gui->game->status = TRN_GAME_ON;
  return TRUE;
}

TrnGUI* trn_gui_new(int numberOfRows, int numberOfColumns, int delay)
{

  TrnGUI* gui = (TrnGUI*)malloc(sizeof(TrnGUI));

  gui->game = trn_game_new(numberOfRows, numberOfColumns, delay);

  gui->window = trn_window_new(numberOfRows,numberOfColumns);
  
  g_signal_connect(gui->window->newGameButton, "clicked", G_CALLBACK(button_newgame_clicked), gui);
  g_signal_connect(gui->window->pauseButton, "clicked", G_CALLBACK(button_pause_clicked), gui);

  trn_window_show(gui->window);
  g_timeout_add(trn_game_delay(gui->game),on_timeout_event,(gpointer)gui);

  return gui;
}

void trn_gui_destroy(TrnGUI* gui)
{
  trn_game_destroy(gui->game);
  trn_window_destroy(gui->window);
  free(gui);
}

void trn_gui_update_view(TrnGUI* gui)
{
  trn_window_refresh(gui->window);
  trn_gui_update_labels(gui);
}

void trn_gui_update_labels(TrnGUI* gui)
{
  trn_window_update_level(gui->window,gui->game->level);
  trn_window_update_lines(gui->window, gui->game->lines_count);
  trn_window_update_score(gui->window, gui->game->score);
}

