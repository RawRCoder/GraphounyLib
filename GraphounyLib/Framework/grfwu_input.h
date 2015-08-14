#pragma once
#include "grfw_shared.h"
#include "../gr_vector2.h"

struct mouse_data_s
{
	bool m_bIsInClient;
	POINT m_pntCursorLocation;
	POINT m_pntPrevCursorLocation;
	v2f m_vCursorPosClient;
	v2f m_vCursorPosPercents;
	v2f m_vCursorPrevPosPercents;
	v2f m_vCursorDelta;
};