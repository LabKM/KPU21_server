#include "ChessBoard.h"
#include "ChessPiece.h"

ChessPiece::ChessPiece()
{
	m_texture_id = -1;
	m_Board = NULL;
}

ChessPiece::~ChessPiece()
{
}

void ChessPiece::SetPiece(int id, ChessPieceType type, bool white)
{
	m_texture_id = id;
	m_type = type;
	m_bWhite = white;
}

void ChessPiece::SetOnBoard(ChessBoard* board, int row, int column)
{
	m_Board = board;
	m_pointOnBoard = { row, column };
}



