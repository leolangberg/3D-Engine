#include "light.h"
#include <string.h>
#include <stdlib.h>


int Reset_Materials(void) {
    // this function resets all the materials
    static int first_time = 1;

    // if this is the first time the zero EVERYTHING out
    if(first_time)
    {
        memset(materials, 0, MAX_MATERIAL*sizeof(Material));
        first_time = 0;
    }

    // scan through materials and release all textures, if any?
    for(int curr_mat = 0; curr_mat < MAX_MATERIAL; curr_mat++)
    {
        // ergardless if the material is active check to see if there is a
        // dangling texture map
        // destory bitmap()
        //free(materials[curr_mat].texture.buffer);
        //free(&materials[curr_mat].texture);
        memset(&materials[curr_mat], 0, sizeof(Material));

    }

    return 1;
}

int Reset_Lights(void) {
    // this function simply resets all lights in the system
    static int first_time = 1;

    memset(lights, 0, MAX_LIGHTS * sizeof(Light));
    // reset number of lights
    num_lights = 0;

    // reset first time
    first_time = 0;

    return 1;
}

// returns index which means that you can use return value 
// as both name and direct link to find construct in array.
int light_construct(
    int     index,      // index of light to create (0..MAX_LIGHTS-1)
    int    _state,      // state of light
    int    _attr,       // tpye of light, and extra qualifiers
    RGB    _c_ambient,  // ambient light intensity
    RGB    _c_diffuse,  // diffuse light intensity
    RGB    _c_specular, // specular light intensity
    Vector* _pos,       // position of light
    Vector* _dir,       // direction of light
    float _kc,           // attenuation factors
    float _kl,
    float _kq,
    float  _spot_inner, // inner angle for spot light
    float  _spot_outer, // outer angle for spot light
    float _pf)
{
    // this function initializes a light based on the flags sent in _attr, values that
    // arent need are set to 0 by caller
    // make sure light is in range
    if(index < 0 || index >= MAX_LIGHTS) {
        return 0;
    }

    // all good, initalize the light (many fields may be dead)
    lights[index].state = _state;
    lights[index].id    = index;
    lights[index].attr  = _attr;    // tpye of light, extra qualifiers
    lights[index].c_ambient  = _c_ambient;
    lights[index].c_diffuse  = _c_diffuse;
    lights[index].c_specular = _c_specular;

    lights[index].kc = _kc; // constant attenuation factor
    lights[index].kl = _kl; // linear attenuation factor
    lights[index].kq = _kq; // quadratic attenuation factor

    if(_pos != NULL) {
        lights[index].pos = *_pos;
    }
    if(_dir != NULL) {
        lights[index].dir = *vector_normalize(_dir);
    }

    lights[index].spot_inner = _spot_inner; // inner angle for spot light
    lights[index].spot_outer = _spot_outer; // outer angle for spot light
    lights[index].pf         = _pf;     // power factor/falloff for spot lights

    num_lights++;

    return index;

}

int Light_Object_world8(Object* object, Camera* camera, Light* lights, int max_lights, int shading_mode, unsigned char* rgblookup) {
    // 8-bit version of function
    // function lights an boject based on the sent lights and camera.
    // the function supports constnat/pure shading (emissive), flat shading with ambient,
    // infinite, point lights, and spot lights
    // Note that this lighting function is rather brute force and simply follows the math
    // however there are some clever integer operations taht are used in scale 256 rather
    // than going to floating point, but why? floating point and ints are the same speed.
    // HOWEVER, the conversion to and from floating point can be cycle intensive, so if 
    // you can keep your calc in ints then you can gain some speed.
    // Also note, typ 1 spot lights are simply point lights with direction,
    // the "cone" is more of a function of the fallof due to attenuation, but they still
    // look like spot lights.
    // Type 2 spot ligths are implemented with the intensity having a dot product relationship
    // with the angle from the surface point to the light direction just like in the optimized
    // model, but the pf term that is used for a concentration control must be 1,2,3...
    // integran and non-fractional.

    unsigned int r_base, g_base, b_base,        // base color being lit
                 r_sum,  g_sum,  b_sum,         // sum of lighting procees over all lights
                 shaded_color;                  // final color
    
    float dp,       // dot product
          dist,     // distance from light to surface
          i,        // general intensities
          nl,       // length of normal
          atten;    // attenuation computations

    // test if the object is culled
    //wont be

    // process each pooly in mesh
    for(int curr_poly = 0; curr_poly < object->num_polys; curr_poly++)
    {
        // is this polygon valid?
        // test this polygon if and only if its not clipped, not culled,
        // active and visible. Note we test for backface in the event that
        // a previous call might have already determined this, so why work 
        // harder.
        if(object->polys[curr_poly].clipped || (!object->polys[curr_poly].active) ||
         (!object->polys[curr_poly].visible))
        {
            continue;
        }

        // extract vertex indices into master list, remember the polygons are 
        // NOT self contained, but based on the vertex list stored in the object
        // itself.
        int vertex_0 = object->polys[curr_poly].vertex_list[0];
        int vertex_1 = object->polys[curr_poly].vertex_list[1];
        int vertex_2 = object->polys[curr_poly].vertex_list[2];

        // we will use the transformed polygon
        // vertex list since the backface removal only makes sense
        // at the world coordinate stage further of the pipeline

        // test the lighting mode of the polygon (flat, gouraud, etc)
        if(shading_mode == FLAT_SHADING)
        {
            // step 1: extract the base color out in RGB mode.
            _RGB565FROM888(object->polys[curr_poly].color, &r_base, &g_base, &b_base);

            // scale to 8 bit
            r_base <<= 3;
            g_base <<= 2;
            b_base <<= 3;

            // initialize color sum
            r_sum = 0;
            g_sum = 0;
            b_sum = 0;

            // loop through lights
            for(int curr_light = 0; curr_light < max_lights; curr_light++)
            {
                // is this light active
                if(!lights[curr_light].state) {
                    printf("lights state 0\n");
                    continue;
                }

                // what kind of lights are we dealing with
                if(lights[curr_light].attr & LIGHTV1_ATTR_AMBIENT)
                {
                    // simply multiply each channel against the color of the
                    // polygon then divide by 256 to scale back to 0..255
                    // use a shift in real life!!! >> 8
                    r_sum += ((lights[curr_light].c_ambient.r * r_base) / 256);
                    g_sum += ((lights[curr_light].c_ambient.g * g_base) / 256);
                    b_sum += ((lights[curr_light].c_ambient.b * b_base) / 256);
                    // there better only be one ambient light!
                }
                else
                if(lights[curr_light].attr & LIGHTV1_ATTR_INFINITE)
                {
                    // infinite lighting, we need the surface normal,
                    // and the direciton of the light source.

                    // we need to compute the normal of this polygon face, and recall
                    // that the vertices should be in counter-clockwise order:
                    // u = p0->p1, v = p0->p2, n = u x v;
                    Vector u, v, n;

                    u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
                    v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);
                    n = *vector_cross_product(&u, &v);

                    // at this point, we are almost ready,
                    // but we have to normalize the normal vector!
                    // this is a key optimiazation we can make later.
                    // we can pre-compute the lenght of all polygon normals,
                    // so this step can be optimized.

                    // compute length of normal
                    nl = vector_length(&n);

                    // ok, recalling the lighting model for infinite lights
                    // I(d)dir = I0dir * Cldir
                    // and for the diffuse model:
                    // Itotald = Rsdiffuse * Idiffuse * (n . 1)
                    // so we basically need to multiply it all together.
                    // Notice the scaling by 128 done to avoid floating point
                    // calculations, not because they are slower, but the conversion
                    // to and from cost cycles.
                    dp = vector_dot_product(&n, &lights[curr_light].dir);

                    // only add light if dp > 0
                    if(dp > 0)
                    {
                        i = 128 * dp / nl;
                        
                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
                        g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
                        b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

                    }
                } // ned if infinite light
                else
                if(lights[curr_light].attr & LIGHTV1_ATTR_POINT)
                {
                    // perform point light computations
                    // light model for point light is once again:
                    //               I0point * Clpoint
                    //  I(d)point = ___________________
                    //               kc + kl*d + kq*d2
                    //
                    // where d = |p - s|
                    // thus its almost identical to the infinite light,
                    // but attenuates as a function of distance from the 
                    // point source to the surface point being lit
                    // we need to compute the normal of this polygon face, and 
                    // recall that the vertices are in counter-clockwise order.

                    Vector u, v, n, l;

                    u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
                    v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);
                    n = *vector_cross_product(&u, &v);

                    // compute length of normal
                    nl = vector_length(&n);

                    // compute vector from surface to light
                    l = *vector_sub(&object->vertices_world[vertex_0], &lights[curr_light].pos);

                    // compute distance and attenuation
                    dist = vector_length(&l);

                    dp = vector_dot_product(&n, &l);

                    // only add light if dp > 0
                    if(dp > 0)
                    {
                        atten = (lights[curr_light].kc + (lights[curr_light].kl * dist) + (lights[curr_light].kq * dist * dist));
                        i = 128 * dp / (nl * dist * atten);

                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
                        g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
                        b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

                    }
                } // end if point light
                else 
                if(lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
                {
                    // perform spotlight/point computations simplified model that uses
                    // point light WITH a direction to simulate a spotlight
                    // ONLY difference here from point light is that normal is -n.
                    Vector u, v, n, l;
                    
                    u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
                    v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);

                    // we need -n so v x u
                    n = *vector_cross_product(&v, &u);

                    // compute length of normal
                    nl = vector_length(&n);

                    // compute vector from surface to light
                    l = *vector_sub(&object->vertices_world[vertex_0], &lights[curr_light].pos);

                    // compute distance and attenuation
                    dist = vector_length(&l);

                    dp = vector_dot_product(&n, &lights[curr_light].dir);

                    if(dp > 0)
                    {
                        atten = (lights[curr_light].kc + (lights[curr_light].kl * dist) + (lights[curr_light].kq * dist * dist));
                        i = 128 * dp / (nl * dist * atten);

                        r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
                        g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
                        b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

                    }
                } // end if spotlight 1
                else 
                if(lights[curr_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
                {
                    // simple/comoplex version 
                    // light model for spot light simple version is once again;
                    //                  I0spotlight * Clspotlight * MAX((1 . s), 0)^pf
                    //  I(d)spotlight = ______________________________________________
                    //                  kc + kl*d + kq*d2
                    //
                    // where d = |p - s|, and pf = power factor
                    // Thus it is almost identicalt to the point equation, but has the extra
                    // term in the numerator realting the angle between the light source and
                    // the point on the surface.
                    Vector u, v, n, d, s;
                    u = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_1]);
                    v = *vector_sub(&object->vertices_world[vertex_0], &object->vertices_world[vertex_2]);

                    // compute normal, but inversed (-n)
                    n = *vector_cross_product(&v, &u);

                    nl = vector_length(&n);
                    dp = vector_dot_product(&n, &lights[curr_light].dir);

                    if(dp > 0)
                    {
                        // comput evector from light to surface
                        // (different from l which IS the light dir)
                        s = *vector_sub(&lights[curr_light].pos, &object->vertices_world[vertex_0]);

                        // compute length of s (distance to light source)
                        // to normalize s for lighting calculation
                        dist = vector_length(&s);

                        // compute spot light term (s . 1)
                        float dpsl = vector_dot_product(&s, &lights[curr_light].dir) / dist;

                        // proceed only if term is positive
                        if(dpsl > 0)
                        {
                            // compute attenuation
                            atten = (lights[curr_light].kc + lights[curr_light].kl*dist + lights[curr_light].kq*dist*dist);

                            // for speed reasons, pf exponents that are
                            // less than 1.0 are out of the question and exponents
                            // must be integral
                            float dpsl_exp = dpsl;

                            // exponentiate for positive integral powers
                            for(int e_index = 1; e_index < (int) lights[curr_light].pf; e_index++)
                            {
                                dpsl_exp *= dpsl;
                            }

                            // now dpsl_exp holds (dspl)^pf power
                            // which is of course (s . 1)^pf
                            i = 128*dp * dpsl_exp / (nl * atten);

                            r_sum += (lights[curr_light].c_diffuse.r * r_base * i) / (256 * 128);
                            g_sum += (lights[curr_light].c_diffuse.g * g_base * i) / (256 * 128);
                            b_sum += (lights[curr_light].c_diffuse.b * b_base * i) / (256 * 128);

                        } // end if 

                    } // end if 

                } // end if spot light
                else {
                    printf("no light match\n");
                    continue;
                }
            } // end for light

            // make sure colors arent out of range
            if(r_sum > 255) { r_sum = 255; }
            if(g_sum > 255) { g_sum = 255; }
            if(b_sum > 255) { b_sum = 255; }

            // write the color
            int rgb_index = _RGB16BIT565(r_sum, g_sum, b_sum);
            shaded_color  = rgblookup[rgb_index]; 
            object->polys[curr_poly].shade = (int)((shaded_color << 16) | object->polys[curr_poly].color);

        } // end if FLAT_SHADING
        else 
        {
            // CONSTANT SHADING
            // emmisive shading only, copy base color into upper 16-bits without any change
            object->polys[curr_poly].shade = (int)((object->polys[curr_poly].color << 16) | object->polys[curr_poly].color);
        }
    } // end for poly

    return 1;
}
