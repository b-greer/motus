#version 330 core

#define SPEED 3.0
#define OFFSET 40.0

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

uniform float time;
uniform float offset;
uniform mat4 modelViewProjectionMatrix;

uniform vec4 lightPosition;

in Vertex {
  vec4 pos; 
  vec4 normal;
  vec4 color;
  vec2 texCoord;
} vertex[];

out vec2 vTexCoord;
out vec3 vNormal;

out vec3 off;

//######################### hash functions are sort of like mapping but handle any size of data ? ##################################

vec2 hash2D( vec2 p )
{
    float r = 523.0 * sin( dot( p, vec2( 53.3158, 43.6143 ) ) );
    return vec2( fract( 15.32354 * r ), fract( 17.25865 * r ) );
}

vec3 hash3D( vec3 p )
{
    float r = 523.0 * sin( dot( p, vec3( 53.3158, 43.6143, 12.1687 ) ) );
    return vec3( fract( 15.32354 * r ), fract( 17.25865 * r ), fract( 11.1021 * r ) );
}

//#################################################################################################################################

// Copyright © 2013 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// algorithm from https://www.shadertoy.com/view/ldl3Dl  

//needs editing for cells/resolution
//voronoi is a descendent of worley, this could be more worley-y. 
// returns closest, second closest, and cell id

vec3 voronoi( in vec3 x )
{
//tile the space according to the vector input
    vec3 p = floor( x );
    vec3 f = fract( x );

//base for a return val
	float id = 0.0;
	//minimum distance i think
    vec2 res = vec2( 1 );

//cycle thru 3 closest pixels in the x,y,z
//so like the 3*3 grid with our pixel in the middle? 
    for( int k=-1; k<=1; k++ ){
		for( int j=-1; j<=1; j++ ){
			for( int i=-1; i<=1; i++ ){

			//let b == the pixel we r comparing to this cycle 
				vec3 b = vec3( float(i), float(j), float(k) );
			//r is b, adjusted to tile and hashed (restricted) 
				vec3 r = vec3( b ) - f + hash3D( p + b );
			//and d is the direction of that 
				float d = dot( r, r );

				//this needs to come into 3d, including a Z value 
				//basically sorts the distance between the pixel and the boundary 
					if( d < res.x )
					{
						id = dot( p+b, vec3(1.0,57.0,113.0 ) );
						res = vec2( d, res.x );			
					}
					else if( d < res.y )
					{
						res.y = d;
					}
			}
		}

    }


//return the closest dist of our pixels
    return vec3( res , abs(id) );
	}

// algorithm from 
// ----------------------  Raymarching - Jose Angel Canabal --------------------------
// ---------------------  based on inigo quilez work ---------------------
//----------------------   https://www.shadertoy.com/view/Mll3Wl --------------------

vec3 voronoi3D( in vec3 x )
{
// copy p if necessary
    vec3 q = x;
    vec3 cp = vec3(2.);

	//cell size definition 
    x *= 1;
    
    // get the pixel's cell
    vec3 c = floor( x );
    
    // the three closest distances
    vec3 f = vec3( 1e06 );
    
    // look for the closest point
    for( int i=-1; i<=1; i++ )
    for( int j=-1; j<=1; j++ )
    for( int k=-1; k<=1; k++ )
    {
        // get the point at this grid cell
        vec3 g = c + vec3( float( i ), float( j ), float( k ) );
        vec3 o = g + hash3D( g);
        // compute the distance between the current pixel and the grid point
        vec3 r = x - o;
        
        // euclidean^2 distance
        float d = dot( r, r );
        // euclidean
       // d = sqrt( d );
        
        //float d = length(r);
        
        // manhatan distance
        //float d = abs( r.x ) + abs( r.y ) + abs( r.z );
        
        // check if it's the closest point
        if( d < f.x )
        {
            f.z = f.y;
            f.y = f.x;
            f.x = d;
            cp.x = o.x + o.y + o.z;
        }
        else if( d < f.y )
        {
            f.z = f.y;
            f.y = d;
            cp.y = o.x + o.y + o.z;
        }
        else if( d < f.z )
        {
            f.z = d;
            cp.z = o.x + o.y + o.z;
        }
    }
    
    return f;
   }

//############################################# lighting ##########################################################################

bool facesLight ( vec3 a, vec3 b, vec3 c)
{
//takes the avg normal of 3 points 
	vec3 norm = cross(b-a, c-a);
//essentially the direction between each point & light 
	vec3 da = lightPosition.xyz-a;
	vec3 db = lightPosition.xyz-b;
	vec3 dc = lightPosition.xyz-c;

//if any are positive then triangle is facing the light 
//and this returns true 
	return dot(norm,da) > 0 || dot(norm,db) > 0 || dot(norm, dc) >0; 
}

//#################################################################################################################################

void main() {    

//-------------- PASSTHRU ---------------------
    //gl_Position = gl_in[0].gl_Position; 
    //EmitVertex();
    //EndPrimitive();

//------------ in progress  -------------------------------------
//define some points with relevance to the first
  vec3 a = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
//middle of triangle
  vec3 center = vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;

//create a normal from a cross product 
  vec3 normal = normalize(cross(b, a));

//cycle through the geom
  for(int j = 0; j < gl_in.length(); j++) {

    // For modifying geometry a bit, use these three lines

	//create a new vertex 
    vec3 newvertex;

//expand along the normal by multiplying it 
    newvertex = vertex[j].normal.xyz *
//this is the animating factor 

	//vec3(-1 * voronoi(gl_in[j].gl_Position.xyz * sin(time) * 0.01).z)
	vec3(1 - voronoi(gl_in[j].gl_Position.xyz * time).z)
//LOWER THE OFFSET FOR SMOOTHER WAVES
	 * offset
//use the original point to keep it in the same original format
//as the offset is ADDED to the original position 
	+ vec3(gl_in[j].gl_Position.xyz);

	 
	 //white with stripes 
		//off = newvertex * gl_in[j].gl_Position.xyz;

	 //segments
		//off = newvertex;

	//pretty blue/purple waves with weird green patches
		//off = voronoi(gl_in[j].gl_Position.xyz * sin(time) * 0.01);

	off = voronoi3D(gl_in[j].gl_Position.xyz * time);


	//replace the position with the new vertex
    gl_Position = modelViewProjectionMatrix * vec4(newvertex, 1.0);

	vNormal = vertex[j].normal.xyz;
    vTexCoord = vertex[j].texCoord.xy;

    EmitVertex();
  }
  EndPrimitive(); 
}  
