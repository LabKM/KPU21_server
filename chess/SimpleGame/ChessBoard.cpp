#include "stdafx.h"
#include "GSEGlobals.h"
#include "ChessBoard.h"

void ChessBoard::GetA1Point(Vector2D& point)
{
	point = m_chessBoardPosition - m_chessBoardSize / 2 + m_chessBoardSize / m_chessBoardSpace / 2; // a1 위치 
}

ChessBoard::ChessBoard(int tid)
{
	m_chessBoardPosition = { 0, 0 };
	getGlobalWindowSize(m_chessBoardSize);
	m_chessBoardSpace = 8; // default 8칸 보드
	m_board_texture_id = tid;
}

void ChessBoard::Render(Renderer* renderer)
{
	renderer->DrawTextureRect(0, 0, 1, m_chessBoardSize.x, m_chessBoardSize.y, 1.f, 1.f, 1.f, 1.f, 1.f, m_board_texture_id);
}

void ChessBoard::GetPointToDraw(int row, int column, float& x, float& y) // a1 = (0, 0)
{
	Vector2D StartPoint;
	GetA1Point(StartPoint);
	Vector2D point = StartPoint + Vector2D(m_chessBoardSize.x / m_chessBoardSpace * row, m_chessBoardSize.y / m_chessBoardSpace * column);
	x = point.x;
	y = point.y;
}

void ChessBoard::GetSizeToDraw(float& sx, float& sy)
{
	sx = m_chessBoardSize.x / m_chessBoardSpace;
	sy = m_chessBoardSize.y / m_chessBoardSpace;
}

void ChessBoard::GetPointFormWorld(const Vector2D& point, float& x, float& y)
{
	int sx, sy;
	GetSpaceFormWorld(point, sx, sy); // 월드 위치 기준으로 어디 칸인지 찾아냄
	GetPointToDraw(sx, sy, x, y); // 해당 칸의 중심점 위치 
}

void ChessBoard::GetSpaceFormWorld(const Vector2D& point, int& x, int& y)
{
	Vector2D a1leftbottom = m_chessBoardPosition - m_chessBoardSize / 2; // a1칸 좌측하단;
	Vector2D vector2FormA1 = point - a1leftbottom; // a1 좌측 하단으로부터의 벡터
	Vector2D OneSpaceSize = (m_chessBoardSize / m_chessBoardSpace); // 한 칸의 크기
	x = int(vector2FormA1.x / OneSpaceSize.x);
	y = int(vector2FormA1.y / OneSpaceSize.y);
}