#pragma once
#include "Vector2D.h"

class ChessBoard {
	Vector2D m_chessBoardPosition; // 체스 보드를 그리는 위치
	Vector2D m_chessBoardSize; // 체스 보드를 그리는 크기
	int m_chessBoardSpace; // 몇 칸 대 몇 칸짜리 체스판인지
	int m_board_texture_id; // 체스 보드 텍스쳐 아이디

	void GetA1Point(Vector2D& point);

public:
	ChessBoard(int texture_id);
	~ChessBoard() {}

	void GetPointToDraw(int row, int column, float& x, float& y); // row column 기반으로 위치 찾기 a1이 0, 0이라 생각 a1은 좌측최하단
	void GetSizeToDraw(float& sx, float& sy); // 한 칸당 사이즈 계산
	void GetPointFormWorld(const Vector2D& point, float& x, float& y); // 월드 위치 기반으로 칸 중심 위치 얻기	
	void GetSpaceFormWorld(const Vector2D& point, int& x, int& y); // 월드 위치 기반으로 몇 번째 칸인지 얻기
};