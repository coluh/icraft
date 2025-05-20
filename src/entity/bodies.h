#ifndef _ICRAFT_entity_bodies_h
#define _ICRAFT_entity_bodies_h

#include "../util/types.h"

#define BODY(x, y, z, w, h, t) (&(Body){x,y,z,w,h,t})
#define BODYP(p, w, h, t) BODY(p.x-(w)/2,p.y-(h)/2,p.z-(t)/2,w,h,t)

#define PLAYER_WIDTH	0.6f
#define PLAYER_HEIGHT	1.8f
#define PLAYER_BODY(p) BODY(p.x-PLAYER_WIDTH/2,p.y-PLAYER_HEIGHT/2,p.z-PLAYER_WIDTH/2, \
		PLAYER_WIDTH,PLAYER_HEIGHT,PLAYER_WIDTH)
#define PLAYER_EYE_OFFSET_Y 0.6f

#define DROPS_WIDTH	0.2f
#define DROPS_BODY(p) BODY(p.x-DROPS_WIDTH/2,p.y-DROPS_WIDTH/2,p.z-DROPS_WIDTH/2,DROPS_WIDTH,DROPS_WIDTH,DROPS_WIDTH)

#define EMPTY_BODY(p) BODY(p.x,p.y,p.z,0,0,0)

#endif
