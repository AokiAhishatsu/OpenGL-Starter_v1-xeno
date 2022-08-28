//------------------------------------------------------------------------------
//--------------------------Code By: 3DSage-------------------------------------
//----------------Video tutorial on YouTube-3DSage------------------------------
//------------------------------------------------------------------------------

#define _USE_MATH_DEFINES	// MVSC fix

#include <math.h>
#include <stdio.h>
#include <GL/glut.h> 

#define res        1                        //0=160x120 1=360x240 4=640x480
#define SW         160*res                  //screen width
#define SH         120*res                  //screen height
#define SW2        (SW/2)                   //half of screen width
#define SH2        (SH/2)                   //half of screen height
#define pixelScale 4/res                    //OpenGL pixel scale
#define GLSW       (SW*pixelScale)          //OpenGL window width
#define GLSH       (SH*pixelScale)          //OpenGL window height
#define numSect    4
#define numWall    16


// Structs

typedef struct
{
	// frame 1 frame 2, to create constant frame rate
	int fr1;
	int fr2;
}time;


typedef struct
{
	int w, s, a, d; // move up, down, left, right
	int sl, sr;     // strafe left, right 
	int m;          // move up, down, look up, down
} keys;


typedef struct
{
	int x, y, z; // posision
	int a;       // roataion angle l/r
	int l;       // look up/down
} player;


typedef struct
{
	int x1, y1;		// Bottom line point 1
	int x2, y2;		// Bottom line point 2
	int c;			// Wall color
} walls;


typedef struct
{
	int ws, we;		// Wall number start and end
	int z1, z2;		// Height of bottom and top
	//int x, y;		// Center position of sector
	int d;			// Add y distance to sort drawing oder
	int c1, c2;		// Bottom and top color
	int surf[SW];	// to hold points for surface
	int surface;	// is there a surface to draw
} sectors;


// Use saved values instead of generating them on init
// Map degree to radian?
const float cos_map [] = { 1.000000,0.999848,0.999391,0.998630,0.997564,0.996195,0.994522,0.992546,0.990268,0.987688,0.984808,0.981627,0.978148,0.974370,0.970296,0.965926,0.961262,0.956305,0.951057,0.945519,0.939693,0.933580,0.927184,0.920505,0.913545,0.906308,0.898794,0.891007,0.882948,0.874620,0.866025,0.857167,0.848048,0.838671,0.829038,0.819152,0.809017,0.798636,0.788011,0.777146,0.766044,0.754710,0.743145,0.731354,0.719340,0.707107,0.694658,0.681998,0.669131,0.656059,0.642788,0.629320,0.615661,0.601815,0.587785,0.573576,0.559193,0.544639,0.529919,0.515038,0.500000,0.484810,0.469472,0.453990,0.438371,0.422618,0.406737,0.390731,0.374607,0.358368,0.342020,0.325568,0.309017,0.292372,0.275637,0.258819,0.241922,0.224951,0.207912,0.190809,0.173648,0.156434,0.139173,0.121869,0.104528,0.087156,0.069756,0.052336,0.034899,0.017452,0.000000,-0.017452,-0.034899,-0.052336,-0.069756,-0.087156,-0.104528,-0.121869,-0.139173,-0.156434,-0.173648,-0.190809,-0.207912,-0.224951,-0.241922,-0.258819,-0.275637,-0.292372,-0.309017,-0.325568,-0.342020,-0.358368,-0.374607,-0.390731,-0.406737,-0.422618,-0.438371,-0.453990,-0.469472,-0.484810,-0.500000,-0.515038,-0.529919,-0.544639,-0.559193,-0.573576,-0.587785,-0.601815,-0.615661,-0.629320,-0.642788,-0.656059,-0.669131,-0.681998,-0.694658,-0.707107,-0.719340,-0.731354,-0.743145,-0.754710,-0.766044,-0.777146,-0.788011,-0.798636,-0.809017,-0.819152,-0.829038,-0.838671,-0.848048,-0.857167,-0.866025,-0.874620,-0.882948,-0.891007,-0.898794,-0.906308,-0.913545,-0.920505,-0.927184,-0.933580,-0.939693,-0.945519,-0.951057,-0.956305,-0.961262,-0.965926,-0.970296,-0.974370,-0.978148,-0.981627,-0.984808,-0.987688,-0.990268,-0.992546,-0.994522,-0.996195,-0.997564,-0.998630,-0.999391,-0.999848,-1.000000,-0.999848,-0.999391,-0.998630,-0.997564,-0.996195,-0.994522,-0.992546,-0.990268,-0.987688,-0.984808,-0.981627,-0.978148,-0.974370,-0.970296,-0.965926,-0.961262,-0.956305,-0.951057,-0.945519,-0.939693,-0.933580,-0.927184,-0.920505,-0.913545,-0.906308,-0.898794,-0.891007,-0.882948,-0.874620,-0.866025,-0.857167,-0.848048,-0.838671,-0.829038,-0.819152,-0.809017,-0.798636,-0.788011,-0.777146,-0.766044,-0.754710,-0.743145,-0.731354,-0.719340,-0.707107,-0.694658,-0.681998,-0.669131,-0.656059,-0.642788,-0.629320,-0.615661,-0.601815,-0.587785,-0.573576,-0.559193,-0.544639,-0.529919,-0.515038,-0.500000,-0.484810,-0.469472,-0.453990,-0.438371,-0.422618,-0.406737,-0.390731,-0.374607,-0.358368,-0.342020,-0.325568,-0.309017,-0.292372,-0.275637,-0.258819,-0.241922,-0.224951,-0.207912,-0.190809,-0.173648,-0.156434,-0.139173,-0.121869,-0.104528,-0.087156,-0.069756,-0.052336,-0.034899,-0.017452,-0.000000,0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,0.156434,0.173648,0.190809,0.207912,0.224951,0.241922,0.258819,0.275637,0.292372,0.309017,0.325568,0.342020,0.358368,0.374607,0.390731,0.406737,0.422618,0.438371,0.453990,0.469472,0.484810,0.500000,0.515038,0.529919,0.544639,0.559193,0.573576,0.587785,0.601815,0.615661,0.629320,0.642788,0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,0.754710,0.766044,0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,0.838671,0.848048,0.857167,0.866025,0.874620,0.882948,0.891007,0.898794,0.906308,0.913545,0.920505,0.927184,0.933580,0.939693,0.945519,0.951057,0.956305,0.961262,0.965926,0.970296,0.974370,0.978148,0.981627,0.984808,0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,0.999848 };
const float sin_map [] = { 0.000000,0.017452,0.034899,0.052336,0.069756,0.087156,0.104528,0.121869,0.139173,0.156434,0.173648,0.190809,0.207912,0.224951,0.241922,0.258819,0.275637,0.292372,0.309017,0.325568,0.342020,0.358368,0.374607,0.390731,0.406737,0.422618,0.438371,0.453990,0.469472,0.484810,0.500000,0.515038,0.529919,0.544639,0.559193,0.573576,0.587785,0.601815,0.615661,0.629320,0.642788,0.656059,0.669131,0.681998,0.694658,0.707107,0.719340,0.731354,0.743145,0.754710,0.766044,0.777146,0.788011,0.798636,0.809017,0.819152,0.829038,0.838671,0.848048,0.857167,0.866025,0.874620,0.882948,0.891007,0.898794,0.906308,0.913545,0.920505,0.927184,0.933580,0.939693,0.945519,0.951057,0.956305,0.961262,0.965926,0.970296,0.974370,0.978148,0.981627,0.984808,0.987688,0.990268,0.992546,0.994522,0.996195,0.997564,0.998630,0.999391,0.999848,1.000000,0.999848,0.999391,0.998630,0.997564,0.996195,0.994522,0.992546,0.990268,0.987688,0.984808,0.981627,0.978148,0.974370,0.970296,0.965926,0.961262,0.956305,0.951057,0.945519,0.939693,0.933580,0.927184,0.920505,0.913545,0.906308,0.898794,0.891007,0.882948,0.874620,0.866025,0.857167,0.848048,0.838671,0.829038,0.819152,0.809017,0.798636,0.788011,0.777146,0.766044,0.754710,0.743145,0.731354,0.719340,0.707107,0.694658,0.681998,0.669131,0.656059,0.642788,0.629320,0.615661,0.601815,0.587785,0.573576,0.559193,0.544639,0.529919,0.515038,0.500000,0.484810,0.469472,0.453990,0.438371,0.422618,0.406737,0.390731,0.374607,0.358368,0.342020,0.325568,0.309017,0.292372,0.275637,0.258819,0.241922,0.224951,0.207912,0.190809,0.173648,0.156434,0.139173,0.121869,0.104528,0.087156,0.069756,0.052336,0.034899,0.017452,0.000000,-0.017452,-0.034899,-0.052336,-0.069756,-0.087156,-0.104528,-0.121869,-0.139173,-0.156434,-0.173648,-0.190809,-0.207912,-0.224951,-0.241922,-0.258819,-0.275637,-0.292372,-0.309017,-0.325568,-0.342020,-0.358368,-0.374607,-0.390731,-0.406737,-0.422618,-0.438371,-0.453990,-0.469472,-0.484810,-0.500000,-0.515038,-0.529919,-0.544639,-0.559193,-0.573576,-0.587785,-0.601815,-0.615661,-0.629320,-0.642788,-0.656059,-0.669131,-0.681998,-0.694658,-0.707107,-0.719340,-0.731354,-0.743145,-0.754710,-0.766044,-0.777146,-0.788011,-0.798636,-0.809017,-0.819152,-0.829038,-0.838671,-0.848048,-0.857167,-0.866025,-0.874620,-0.882948,-0.891007,-0.898794,-0.906308,-0.913545,-0.920505,-0.927184,-0.933580,-0.939693,-0.945519,-0.951057,-0.956305,-0.961262,-0.965926,-0.970296,-0.974370,-0.978148,-0.981627,-0.984808,-0.987688,-0.990268,-0.992546,-0.994522,-0.996195,-0.997564,-0.998630,-0.999391,-0.999848,-1.000000,-0.999848,-0.999391,-0.998630,-0.997564,-0.996195,-0.994522,-0.992546,-0.990268,-0.987688,-0.984808,-0.981627,-0.978148,-0.974370,-0.970296,-0.965926,-0.961262,-0.956305,-0.951057,-0.945519,-0.939693,-0.933580,-0.927184,-0.920505,-0.913545,-0.906308,-0.898794,-0.891007,-0.882948,-0.874620,-0.866025,-0.857167,-0.848048,-0.838671,-0.829038,-0.819152,-0.809017,-0.798636,-0.788011,-0.777146,-0.766044,-0.754710,-0.743145,-0.731354,-0.719340,-0.707107,-0.694658,-0.681998,-0.669131,-0.656059,-0.642788,-0.629320,-0.615661,-0.601815,-0.587785,-0.573576,-0.559193,-0.544639,-0.529919,-0.515038,-0.500000,-0.484810,-0.469472,-0.453990,-0.438371,-0.422618,-0.406737,-0.390731,-0.374607,-0.358368,-0.342020,-0.325568,-0.309017,-0.292372,-0.275637,-0.258819,-0.241922,-0.224951,-0.207912,-0.190809,-0.173648,-0.156434,-0.139173,-0.121869,-0.104528,-0.087156,-0.069756,-0.052336,-0.034899,-0.017452 };


// Globals
time T;
keys K;
//math M;
player P;
walls W[30];
sectors S[30];


// draw a pixel at x/y with rgb
void pixel(int x, int y, int c)
{
	int rgb[] = {0, 0 , 0};
	if (c == 0) { rgb[0] = 255; rgb[1] = 255; rgb[2] = 0; } //Yellow	
	if (c == 1) { rgb[0] = 160; rgb[1] = 160; rgb[2] = 0; } //Yellow darker	
	if (c == 2) { rgb[0] = 0; rgb[1] = 255; rgb[2] = 0; }   //Green	
	if (c == 3) { rgb[0] = 0; rgb[1] = 160; rgb[2] = 0; }   //Green darker	
	if (c == 4) { rgb[0] = 0; rgb[1] = 255; rgb[2] = 255; } //Cyan	
	if (c == 5) { rgb[0] = 0; rgb[1] = 160; rgb[2] = 160; } //Cyan darker
	if (c == 6) { rgb[0] = 160; rgb[1] = 100; rgb[2] = 0; } //brown	
	if (c == 7) { rgb[0] = 110; rgb[1] = 50; rgb[2] = 0; }  //brown darker
	if (c == 8) { rgb[0] = 0; rgb[1] = 60; rgb[2] = 130; }  //background 
	glColor3ub(rgb[0], rgb[1], rgb[2]);
	glBegin(GL_POINTS);
	glVertex2i(x * pixelScale + 2, y * pixelScale + 2);
	glEnd();
}

void movePlayer()
{
	//move up, down, left, right
	if (K.sr == 1 && K.m == 0) { P.a -= 4; if (P.a < 0  ) { P.a += 360; } }
	if (K.sl == 1 && K.m == 0) { P.a += 4; if (P.a > 359) { P.a -= 360; } }

	int dx = sin_map[P.a] * 10.0;
	int dy = cos_map[P.a] * 10.0;

	if (K.w == 1 && K.m == 0) { P.x += dx; P.y += dy; }
	if (K.s == 1 && K.m == 0) { P.x -= dx; P.y -= dy; }
	
	//strafe left, right
	if (K.d == 1) { P.x += dy; P.y -= dx; }
	if (K.a == 1) { P.x -= dy; P.y += dx; }

	//move up, down, look up, look down
	//if (K.a == 1 && K.m == 1) { P.l -= 1; }
	//if (K.d == 1 && K.m == 1) { P.l += 1; }
	if (K.w == 1 && K.m == 1) { P.z += 4; }
	if (K.s == 1 && K.m == 1) { P.z -= 4; }
}

void clearBackground()
{
	int x, y;
	for (y = 0; y < SH; y++)
	{
		for (x = 0; x < SW; x++) { pixel(x, y, 8); } //clear background color
	}
}

void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2) // clip line
{
	float da = *y1;					// distance plane -> point a
	float db = y2;					// distance plane -> point b
	float d = da - db;
	if (d == 0) d = 1;
	float s = da / (da - db);		// intersection factor (beween 0 and 1)
	*x1 = *x1 + s * (x2 - (*x1));
	*y1 = *y1 + s * (y2 - (*y1));
	if (*y1 == 0) *y1 = 1;			// prevent divide by zero
	*z1 = *z1 + s * (z2 - (*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int c, int s)
{
	printf("surface:%d\n", S[s].surface);
	int x, y;

	// hold difference in distance
	int dyb = b2 - b1; // y distance of botton line
	int dyt = t2 - t1; // y distance of top line
	int dx = x2 - x1;  // x distance 
	if (dx == 0) { dx = 1; }
	int xs = x1; // hold initial x1 starting position

	// Clip x
	if (x1 < 1) x1 = 1;				// clip left
	if (x2 < 1) x2 = 1;				// clip left
	if (x1 > SW - 1) x1 = SW - 1;	// clip right
	if (x2 > SW - 1) x2 = SW - 1;	// clip right

	// Draw x verticle lines
	for (x = x1; x < x2; x++)
	{
		// y start and end point
		int y1 = dyb * (x - xs + 0.5) / dx + b1; // y bottom point
		int y2 = dyt * (x - xs + 0.5) / dx + t1; // y top point

		// Clip y
		if (y1 < 1) y1 = 1;
		if (y2 < 1) y2 = 1;
		if (y1 > SH - 1) y1 = SH - 1;
		if (y2 > SH - 1) y2 = SH - 1;


		// Surface
		if (S[s].surface == 1) { S[s].surf[x] = y1; continue; }		// Save bottom points
		if (S[s].surface == 2) { S[s].surf[x] = y2; continue; }		// Save top points
		if (S[s].surface == -1) for (y = S[s].surf[x]; y < y1; y++) pixel(x, y, S[s].c1);	// Bottom
		if (S[s].surface == -2) for (y = y2; y < S[s].surf[x]; y++) pixel(x, y, S[s].c2);	// Top

		for (y = y1; y < y2; y++) pixel(x, y, c);	// Normal wall
	}
}

int distance(int x1, int y1, int x2, int y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); // use pow() here?
}

void draw3D()
{
	int wx[4], wy[4], wz[4];
	float cos = cos_map[P.a];
	float sin = sin_map[P.a];
	int s, w, loop;

	// Order sectors by distance
	for (s = 0; s < numSect - 1; s++)
		for (w = 0; w < numSect - s - 1; w++)
			if (S[w].d < S[w + 1].d) { sectors st = S[w]; S[w] = S[w + 1]; S[w + 1] = st; }

	// Draw sectors
	for (s = 0; s < numSect; s++)
	{
		S[s].d = 0;		// Clear distance

		if (P.z < S[s].z1) S[s].surface = 1;		// Bottom surface
		else if (P.z > S[s].z2) S[s].surface = 2;	// Top surface
		else S[s].surface = 0;						// No surface

		for (loop = 0; loop < 2; loop++)
		{
			for (w = S[s].ws; w < S[s].we; w++)
			{
				// Offset bottom 2 points by player
				int x1 = W[w].x1 - P.x, y1 = W[w].y1 - P.y;
				int x2 = W[w].x2 - P.x, y2 = W[w].y2 - P.y;

				// Swap for surface
				if (loop == 0) { int swp = x1; x1 = x2; x2 = swp; swp = y1; y1 = y2; y2 = swp; }

				// world X positon
				wx[0] = x1 * cos - y1 * sin;
				wx[1] = x2 * cos - y2 * sin;
				wx[2] = wx[0];                // top line has the same x
				wx[3] = wx[1];

				// world Y positon (depth)
				wy[0] = y1 * cos + x1 * sin;
				wy[1] = y2 * cos + x2 * sin;
				wy[2] = wy[0];                // top line has the same y
				wy[3] = wy[1];

				S[s].d += distance(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2); // Store this wall distance

				// world z position (height)
				wz[0] = S[s].z1 - P.z + ((P.l * wy[0]) / 32.0);
				wz[1] = S[s].z1 - P.z + ((P.l * wy[1]) / 32.0);
				wz[2] = wz[0] + S[s].z2;          // top line has new z
				wz[3] = wz[1] + S[s].z2;

				// Don't draw behind player
				if (wy[0] < 1 && wy[1] < 1) continue;

				//printf("wx0:%d wx1:%d wy0:%d wy1:%d wy2:%d wy3:%d\n", wx[0], wx[1], wy[0], wy[1], wy[2], wy[3]);

				// Clip point 1 behind player
				if (wy[0] < 1)
				{
					clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]);	// bottom line
					clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]);	// top line
				}

				// Clip point 2 behind player
				if (wy[1] < 1)
				{
					clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]);	// bottom line
					clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]);	// top line
				}

				// sceen x/y positon
				wx[0] = wx[0] * 200 / wy[0] + SW2; wy[0] = wz[0] * 200 / wy[0] + SH2;
				wx[1] = wx[1] * 200 / wy[1] + SW2; wy[1] = wz[1] * 200 / wy[1] + SH2;
				wx[2] = wx[2] * 200 / wy[2] + SW2; wy[2] = wz[2] * 200 / wy[2] + SH2;
				wx[3] = wx[3] * 200 / wy[3] + SW2; wy[3] = wz[3] * 200 / wy[3] + SH2;

				// draw points - need to be differnt color
				// if (wx[0] > 0 && wx[0] < SW && wy[0] >  0 && wy[0] < SH) { pixel(wx[0], wy[0], 0); }
				// if (wx[1] > 0 && wx[1] < SW && wy[1] >  0 && wy[1] < SH) { pixel(wx[1], wy[1], 0); }

				drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3], W[w].c, s);
			}

			S[s].d /= S[s].we - S[s].ws;	// Find average sector distance
			S[s].surface *= -1;				// Filp to negative to draw surface
		}
	}
}

void display(void)
{
	if (T.fr1 - T.fr2 >= 50)                        //only draw 20 frames/second
	{
		clearBackground();
		movePlayer();
		draw3D();

		T.fr2 = T.fr1;
		glutSwapBuffers();
		glutReshapeWindow(GLSW, GLSH);             //prevent window scaling
	}

	T.fr1 = glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
	glutPostRedisplay();
}

void KeysDown(unsigned char key, int x, int y)
{
	printf("%c\n", key);
	if (key == 'w' == 1) { K.w = 1; }
	if (key == 's' == 1) { K.s = 1; }
	if (key == 'a' == 1) { K.a = 1; }
	if (key == 'd' == 1) { K.d = 1; }
	if (key == 'm' == 1) { K.m = 1; }
	if (key == ',' == 1) { K.sr = 1; }
	if (key == '.' == 1) { K.sl = 1; }
}

void KeysUp(unsigned char key, int x, int y)
{
	if (key == 'w' == 1) { K.w = 0; }
	if (key == 's' == 1) { K.s = 0; }
	if (key == 'a' == 1) { K.a = 0; }
	if (key == 'd' == 1) { K.d = 0; }
	if (key == 'm' == 1) { K.m = 0; }
	if (key == ',' == 1) { K.sr = 0; }
	if (key == '.' == 1) { K.sl = 0; }
}

int loadSectors[] =
{ // wall start, wall end, z1, height, z2 height, bottom color, top color
	 0,  4, 0, 40, 2, 3,	// Sector 1
	 4,  8, 0, 40, 4, 5,	// Sector 2
	 8, 12, 0, 40, 6, 7,	// Sector 3
	12, 16, 0, 40, 0, 1,	// Sector 4
};

int loadWalls[] =
{ // x1, y1, x2, y2, color
	 0,  0, 32,  0, 0,
	32,  0, 32, 32, 1,
	32, 32,  0, 32, 0,
	 0, 32,  0,  0, 1,

	64,  0, 96,  0, 2,
	96,  0, 96, 32, 3,
	96, 32, 64, 32, 2,
	64 ,32, 64,  0, 3,

	64, 64, 96, 64, 4,
	96, 64, 96, 96, 5,
	96, 96, 64, 96, 4,
	64, 96, 64, 64, 5,

	 0, 64, 32, 64, 6,
	32, 64, 32, 96, 7,
	32, 96,  0, 96, 6,
	 0, 96,  0, 64, 7,
};

void init()
{
	// init player position
	P.x = 70;
	P.y = -110;
	P.z = 20;
	P.a = 0;
	P.l = 0;

	int s, w, v1 = 0, v2 = 0;
	for (s = 0; s < numSect; s++)
	{
		S[s].ws = loadSectors[v1 + 0];							// Wall start number
		S[s].we = loadSectors[v1 + 1];							// Wall end number
		S[s].z1 = loadSectors[v1 + 2];							// Sector bottom height
		S[s].z2 = loadSectors[v1 + 3] - loadSectors[v1 + 2];	// Sector top height
		S[s].c1 = loadSectors[v1 + 4];							// Sector top color
		S[s].c2 = loadSectors[v1 + 5];							// Sector bottom color
		v1 += 6;

		for (w = S[s].ws; w < S[s].we; w++)
		{
			W[w].x1 = loadWalls[v2 + 0];	// Bottom x1
			W[w].y1 = loadWalls[v2 + 1];	// Bottom y1
			W[w].x2 = loadWalls[v2 + 2];	// Top x1
			W[w].y2 = loadWalls[v2 + 3];	// Top y2
			W[w].c = loadWalls[v2 + 4];		// Wall color
			v2 += 5;
		}
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(GLSW / 2, GLSH / 2);
	glutInitWindowSize(GLSW, GLSH);
	glutCreateWindow("Let's Program Doom - Part 1");
	glPointSize(pixelScale);							//pixel size
	gluOrtho2D(0, GLSW, 0, GLSH);						//origin bottom left
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(KeysDown);
	glutKeyboardUpFunc(KeysUp);
	// add arrow keys with glutSpecialFunc here
	glutMainLoop();
	return 0;
}
