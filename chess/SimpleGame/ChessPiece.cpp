#include "stdafx.h"
#include "GSEGlobals.h"
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

void ChessPiece::Render(Renderer* renderer)
{
	float x, y, sx, sy;
	m_Board->GetPointToDraw(m_pointOnBoard.x, m_pointOnBoard.y, x, y);
	m_Board->GetSizeToDraw(sx, sy);
	renderer->DrawTextureRectAnim(x, y, 0, sx, sy, 1.f, 1.f, 1.f, 1.f, 1.f, m_texture_id, 6, 2, (int)m_type, m_bWhite ? 0 : 1, false);
}



