// Temp draw cursor
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "CursorNode.h"

#include <Math/Vector.h>

using OpenEngine::Math::Vector;

void CursorNode::Apply(IRenderingView* r) {
    Vector<3,float> col(1.0,0.0,0.0); // red
    r->GetRenderer()->DrawPoint(Vector<3,float>(0.0,0.0,0.0), col, 5);
}
