#include "grid.h"
#include <stdlib.h>

TrnGrid* trn_grid_new(unsigned int numberOfRows, unsigned int numberOfColumns)
{
    /* Allocate grid */
    TrnGrid* grid = (TrnGrid*) malloc(sizeof(TrnGrid));

    /* Set number of rows and columns */
    grid->numberOfRows = numberOfRows;
    grid->numberOfColumns = numberOfColumns;

    /* Allocate a classical C-style 2D array: creates an array of numberOfRows
     * pointers, each one pointing to contiguous memory of the column data. */
    grid->tetrominoTypes = (TrnTetrominoType**)
        malloc(sizeof(TrnTetrominoType*) * numberOfRows);
    unsigned int rowIndex;
    for (rowIndex = 0 ; rowIndex < numberOfRows ; rowIndex++)
    {
        grid->tetrominoTypes[rowIndex] = (TrnTetrominoType*)
            malloc(sizeof(TrnTetrominoType) * numberOfColumns);
    }

    /* clear grid, ie intialize it to TRN_TETROMINO_VOID */
    trn_grid_clear(grid);

    return grid;
}

void trn_grid_destroy(TrnGrid* grid)
{
    /* Deallocate C-style 2D array */
    unsigned int rowIndex;
    for (rowIndex = 0 ; rowIndex < grid->numberOfRows ; rowIndex++)
    {
        free(grid->tetrominoTypes[rowIndex]);
    }
    free(grid->tetrominoTypes);

    /* Deallocate grid */
    free(grid);
}

void trn_grid_clear(TrnGrid* grid)
{
    unsigned int rowIndex;
    unsigned int columnIndex;
    for (rowIndex = 0 ; rowIndex < grid->numberOfRows; rowIndex++) {
        for (columnIndex = 0 ; columnIndex < grid->numberOfColumns ; columnIndex++) {
            grid->tetrominoTypes[rowIndex][columnIndex] = TRN_TETROMINO_VOID;
        }
    }
}

void trn_grid_set_cell(TrnGrid* grid, TrnPositionInGrid pos, TrnTetrominoType type)
{
    grid->tetrominoTypes[pos.rowIndex][pos.columnIndex] = type;
}

TrnTetrominoType trn_grid_get_cell(TrnGrid* grid, TrnPositionInGrid pos)
{
    return grid->tetrominoTypes[pos.rowIndex][pos.columnIndex];
}

void trn_grid_set_cells_with_piece(TrnGrid* grid, TrnPiece* piece, TrnTetrominoType type)
{
    unsigned squareIndex;
    TrnPositionInGrid pos;

    for (squareIndex = 0 ; 
         squareIndex < TRN_TETROMINO_NUMBER_OF_SQUARES ;
         squareIndex++)
    {
        pos = trn_piece_position_in_grid(piece, squareIndex);
        trn_grid_set_cell(grid, pos, type);
    }
}

bool trn_grid_cell_is_in_grid(TrnGrid* grid, TrnPositionInGrid pos)
{
    return 0 <= pos.rowIndex && pos.rowIndex < grid->numberOfRows &&
           0 <= pos.columnIndex && pos.columnIndex < grid->numberOfColumns;
}

bool trn_grid_cell_is_in_grid_and_is_void(TrnGrid* grid, TrnPositionInGrid pos)
{
    return trn_grid_cell_is_in_grid(grid,pos) && trn_grid_get_cell(grid, pos) == TRN_TETROMINO_VOID;
}

bool trn_grid_can_set_cells_with_piece(TrnGrid* grid, TrnPiece* piece)
{
    unsigned squareIndex;
    TrnPositionInGrid pos;

    for (squareIndex = 0 ; 
         squareIndex < TRN_TETROMINO_NUMBER_OF_SQUARES ;
         squareIndex++)
    {
        pos = trn_piece_position_in_grid(piece, squareIndex);
        if (! trn_grid_cell_is_in_grid_and_is_void(grid,pos) ) return false;
    }

    return true;
}

bool trn_grid_equal(TrnGrid const * const left, TrnGrid const * const right)
{
    // Compare grid dimensions.
    if (left->numberOfRows != right->numberOfRows)
        return false;
    if (left->numberOfColumns != right->numberOfColumns)
        return false;

    // Compare grid values.
    TrnPositionInGrid pos;
    unsigned int rowIndex;
    unsigned int columnIndex;

    for (rowIndex = 0 ; rowIndex < left->numberOfRows ; rowIndex++) {
        pos.rowIndex = rowIndex;
        for (columnIndex = 0 ; columnIndex < left->numberOfColumns ; columnIndex++) {
            pos.columnIndex = columnIndex;
            if (trn_grid_get_cell(left, pos) != trn_grid_get_cell(right,pos)) {
               printf("sameGrid: (%u,%u): %u VS %u\n",pos.rowIndex, pos.columnIndex, trn_grid_get_cell(left, pos), trn_grid_get_cell(right,pos));
               return false; 
            }
        }
    }
    return true;
}

void trn_grid_print(TrnGrid const * const grid)
{
    TrnPositionInGrid pos;
    unsigned int rowIndex;
    unsigned int columnIndex;
    TrnTetrominoType type;

    char* symbols[] = {"I","O","T","S","Z","J","L"};

    // Print top matrix border.
    printf("+");
    for (columnIndex = 1 ; columnIndex < grid->numberOfColumns+1 ; columnIndex++) {
        printf("-");
    }
    printf("+\n");

    // Print left matrix border, matrix type values, matrix right border.
    for (rowIndex = 0 ; rowIndex < grid->numberOfRows ; rowIndex++) {
        pos.rowIndex = rowIndex;
        printf("|");
        for (columnIndex = 0 ; columnIndex < grid->numberOfColumns ; columnIndex++) {
            pos.columnIndex = columnIndex;
            type = trn_grid_get_cell(grid, pos);
            if (type == TRN_TETROMINO_VOID) {
                printf(" ");
            } else {
                printf(symbols[type]);
            }
        }
        printf("|\n");
    }

    // Print bottom matrix border.
    printf("+");
    for (columnIndex = 1 ; columnIndex < grid->numberOfColumns+1 ; columnIndex++) {
        printf("-");
    }
    printf("+\n");
}

bool trn_grid_is_row_complete(TrnGrid const * const grid, int rowIndex)
{
    unsigned int columnIndex;

    TrnPositionInGrid pos;
    pos.rowIndex = rowIndex;

    TrnTetrominoType type;

    for (columnIndex = 0 ; columnIndex < grid->numberOfColumns ; columnIndex++) {
        pos.columnIndex = columnIndex;
        type = trn_grid_get_cell(grid, pos);
        if (type == TRN_TETROMINO_VOID)
            return false;
    }

    return true;
}

void trn_grid_copy_row_bellow(TrnGrid* grid, int rowIndex)
{
  TrnPositionInGrid top_pos;
  TrnPositionInGrid bottom_pos;

  top_pos.rowIndex = rowIndex;
  bottom_pos.rowIndex = rowIndex+1;

  int columnIndex;
  TrnTetrominoType top_type;

  for (columnIndex = 0 ; columnIndex < grid->numberOfColumns ; columnIndex++) {
      top_pos.columnIndex = columnIndex;
      bottom_pos.columnIndex = columnIndex;
      top_type = trn_grid_get_cell(grid, top_pos);
      trn_grid_set_cell(grid, bottom_pos, top_type);
  }
}

void trn_grid_pop_row_and_make_above_fall(TrnGrid* grid, int rowIndexToPop)
{
  int rowIndex;
  for (rowIndex = rowIndexToPop-1 ; rowIndex >= 0 ; rowIndex-- ) {
    trn_grid_copy_row_bellow(grid, rowIndex);
  }

  int firstRowIndex = 0;
  trn_grid_clear_row(grid,firstRowIndex);
}

size_t trn_grid_pop_first_complete_rows_block()
{
    size_t number_of_poped_rows = 0;
    /* ... pop row blocs ... */
    return number_of_poped_rows;
}

/* Return -1 if no complete row */
int tnr_grid_find_last_complete_row_index(TrnGrid* grid)
{
  int rowIndex;
  for (rowIndex = grid->numberOfRows-1 ; rowIndex >= 0  ; rowIndex--) {
      if (trn_grid_is_row_complete(grid, rowIndex))
              return rowIndex;
  }
  return -1;
}

void trn_grid_clear_row(TrnGrid* grid, int rowIndex)
{
  TrnPositionInGrid pos;
  pos.rowIndex = rowIndex;
  int columnIndex;
  for (columnIndex = 0 ; columnIndex < grid->numberOfColumns ; columnIndex++) {
    pos.columnIndex = columnIndex;
    trn_grid_set_cell(grid, pos, TRN_TETROMINO_VOID);
  }
}
