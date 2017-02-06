#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDLauxiliary.h"
#include "TestModel.h"
#include <limits>
#include <math.h>

using namespace std;
using glm::vec3;
using glm::mat3;

struct Intersection
{
	vec3 position;
	float distance;
	int triangleIndex;
};

/* ----------------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                            */

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
float f = SCREEN_WIDTH/2;
vec3 cameraPos( 0.0f, 0.0f, -2.0f);
vec3 black( 0.0, 0.0, 0.0);
vec3 lightPos( 0, -0.5, -0.7 );
vec3 lightColor = 14.f * vec3( 1, 1, 1 );

/* ----------------------------------------------------------------------------*/
/* FUNCTIONS                                                                   */

void Update();
void Draw();
void Interpolate( float a, float b, vector<float>& result );
void Interpolate( vec3 a, vec3 b, vector<vec3>& result );
bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection );
vec3 DirectLight( const Intersection& i );
vec3 SurfaceColor(const Intersection& i);


int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	t = SDL_GetTicks();	// Set start value for timer.

	LoadTestModel( triangles );
	cout << triangles.size() <<endl;

	

	while( NoQuitMessageSDL() )
	{
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

}

void Draw()
{
	//SDL_FillRect( screen, 0, 0 );

	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for (int y=-0;y<SCREEN_HEIGHT;y++)
	{
		for(int x=0;x<SCREEN_WIDTH;x++)
		{
			vec3 d(x- SCREEN_WIDTH/2, y- SCREEN_HEIGHT/2, f);
			Intersection intersection;
			bool isInterection = ClosestIntersection(cameraPos, d, triangles, intersection);
			if(isInterection)
			{
				vec3 color = SurfaceColor(intersection);
				PutPixelSDL(screen, x, y, color);
			}
			else
			{
				//cout << " NO intersection at (" << x <<"," << y << ")" << endl;
				PutPixelSDL(screen, x, y, black);
			}
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}

// vec3 Project(vec3 object, int f)
// {
// 	vec3 pos = vec3(f* object.x/object.z + SCREEN_WIDTH/2, f*stars[s].y/stars[s].z + SCREEN_HEIGHT/2, 0);
// 	return pos;
// }

bool ClosestIntersection( vec3 start, vec3 dir, const vector<Triangle>& triangles, Intersection& closestIntersection )
{
	closestIntersection.distance = std::numeric_limits<float>::max();
	for(int i = 0; i < triangles.size(); i++)
	{
		Triangle triangle = triangles[i];
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;
		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;

		if( (x.x>=0) && (x.y>=0) && (x.z>=0) && (x.y + x.z <= 1)) //Intersection!
		{
			//vec3 r = start + dir * x.x;
			//float r_dis = glm::length(r);
			if(closestIntersection.distance > x.x)
			{
				closestIntersection.position = start + x.x*dir;
				closestIntersection.distance = x.x;
				closestIntersection.triangleIndex = i;
			}
			
		}
			
	}
	return closestIntersection.distance != std::numeric_limits<float>::max();
}

vec3 SurfaceColor(const Intersection& i)
{
	return triangles[i.triangleIndex].color*DirectLight(i);
}

vec3 DirectLight( const Intersection& i )
{
	vec3 lightdir = lightPos - i.position;
	float r = glm::length(lightdir);
	vec3 B = (lightColor / (float)(4*r*r*M_PI));
	float dp = glm::dot(glm::normalize(triangles[i.triangleIndex].normal), glm::normalize(lightdir));
	return B * glm::max(dp, 0.0f);
}


void Interpolate( float a, float b, vector<float>& result )
{
	if(result.size() == 0) return;
	if(result.size() == 1)
	{
		result[0] = (a+b)/2;
		return;
	}
	else
	{
		float diff = b-a;
		float step = diff/(result.size()-1);
		result[0] = a;
		for(int i=1; i < result.size(); i++){
	    	result[i] = result[i-1] + step;
		}
	}
}

void Interpolate( vec3 a, vec3 b, vector<vec3>& result )
{
	vector<float> result_x( result.size() );
	Interpolate(a.x, b.x, result_x);
	vector<float> result_y( result.size() );
	Interpolate(a.y, b.y, result_y);
	vector<float> result_z( result.size() );
	Interpolate(a.z, b.z, result_z);
	for(int i = 0; i < result.size(); i++)
	{
		result[i] = vec3(result_x[i], result_y[i], result_z[i]);
	}
}
