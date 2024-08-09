#ifndef LIGHT_H
#define LIGHT_H

#include "palette.h"
#include "../global.h"
#include "../math/vector.h"
#include "../polygon.h"
#include "../camera.h"

// attributes of polygons and polygon faces
#define POLY4DV1_ATTR_2SIDED        0x0001
#define POLY4DV1_ATTR_TRANSPARENT   0x0002
#define POLY4DV1_ATTR_8BITCOLOR     0x0004
#define POLY4DV1_ATTR_RGB16         0x0008
#define POLY4DV1_ATTR_RGB24         0x0010

#define POLY4DV1_ATTR_SHADE_MODE_PURE       0x0020
#define POLY4DV1_ATTR_SHADE_MODE_CONSTANT   0x0020 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_FLAT       0x0040
#define POLY4DV1_ATTR_SHADE_MODE_GOURAUD    0x0080
#define POLY4DV1_ATTR_SHADE_MODE_PHONG      0x0100
#define POLY4DV1_ATTR_SHADE_MODE_FASTPHONG  0x0100 // (alias)
#define POLY4DV1_ATTR_SHADE_MODE_TEXTURE    0x0200

// states of polygons and faces
#define POLY4DV1_STATE_ACTIVE   0x0001
#define POLY4DV1_STATE_CLIPPED  0x0002
#define POLY4DV1_STATE_BACKFACE 0x0004

// defines for materials, follow our polygon attributes as much as possible
#define MATV1_ATTR_2SIDED       0x0001
#define MATV1_ATTR_TRANSPARENT  0x0002
#define MATV1_ATTR_8BITCOLOR    0x0004
#define MATV1_ATTR_RGB16        0x0008
#define MATV1_ATTR_RGB24        0x0010

#define MATV1_ATTR_SHADE_MODE_CONSTANT  0x0020
#define MATV1_ATTR_SHADE_MODE_EMMISIVE  0x0020 // alias
#define MATV1_ATTR_SHADE_MODE_FLAT      0x0040
#define MATV1_ATTR_SHADE_MODE_GOURAUD   0x0080
#define MATV1_ATTR_SHADE_MODE_FASTPHONG 0x0100
#define MATV1_ATTR_SHADE_MODE_TEXTURE   0x0200

// defines for light types
#define LIGHTV1_ATTR_AMBIENT    0x0001 // basic ambient light
#define LIGHTV1_ATTR_INFINITE   0x0002 // infinite light source
#define LIGHTV1_ATTR_POINT      0x0004 // point light source
#define LIGHTV1_ATTR_SPOTLIGHT1 0x0008 // spotlight type 1 (simple)
#define LIGHTV1_ATTR_SPOTLIGHT2 0x0010 // spotlight type 2 (complex)
#define LIGHTV1_STATE_ON 1 // light on
#define LIGHTV1_STATE_OFF 0 // light off
#define MAX_LIGHTS 8 // good luck with 1!

// states of materials 
#define MATV1_STATE_ACTIVE 0x0001

// defines for material system
#define MAX_MATERIAL 256


// a first version of a "material"
typedef struct {

    int state;          // state of material
    int id;             // id of this material, index into material array
    char name[64];      // name of material
    int attr;           // attributes, the modes for shading, constant, flat...

    RGB color;               // color of material
    float ka, kd, ks, power; // ambient, diffuse, specular, coefficients
    // note they are separate and scalars since many modelers use this
    // format along with specular power.

    RGB ra, rd, rs;         // the reflectivities/colors pre-multiplied
    // to more match our definitions, each is basically computed by 
    // multiplying the color by the k's, eg:  rd = color * kd.

    char texture_file[80];  // file location of texture
    //BITMAP_IMAGE texture; 

}Material;

typedef struct {
    int state;      // state of light
    int id;         // id of light
    int attr;       // type of light, and extra qualifiers

    RGB c_ambient;  // ambient light intensity
    RGB c_diffuse;  // diffuse light intensity
    RGB c_specular; // specular light intensity

    Vector pos;     // position of light
    Vector dir;     // idrection of light

    float kc, kl, kq; // attenuation factors
    float spot_inner; // inner angle for spot light
    float spot_outer; // oter angle for spot light

    float pf;   // power factor/falloff for spot lights
}Light;


Material materials[MAX_MATERIAL];   // materials in system
int num_materials;                  // current number of materials

Light lights[MAX_LIGHTS];   // lights in system
int num_lights;             // current number of lights



int Light_Object_world8(Object* object, Camera* camera, Light* lights, int max_lights, int shading_mode, unsigned char* rgblookup);

int Reset_Lights(void);

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
    float _pf);



#endif