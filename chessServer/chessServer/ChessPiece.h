#pragma once
#include "Vector2D.h"

enum class ChessPieceType : int {
	King, Queen, Bishop, Knight, Rook, Pawn
};

class ChessBoard;

class ChessPiece
{
	int m_texture_id;
	ChessBoard *m_Board;
	ChessPieceType m_type;
	bool m_bWhite;
	Vector2D m_pointOnBoard;

public:
	ChessPiece();
	~ChessPiece();
	void SetPiece(int textureId, ChessPieceType type, bool white); // 어떤 피스인지 
	void SetOnBoard(ChessBoard* board, int row, int column); // 보드 위에 어디에 있는지 정보
};

