# Raytracer
This is a C++ raytracer I developed for CPE 473 at Cal Poly.

## Features
* POV-Ray file parsing
* Shading
	* Blinn-Phong
	* Cook-Torrance
* Simple Geometry
	* Box
	* Sphere
	* Triangle
	* Plane
* Reflections
* Refractions
* Spatial Data Structures
* Super Sample Anti-Aliasing
* Ambient Occlusion

## Sample Images
### Ambient Occlusion Maps
Bunny:

![Image of BunnyAO](/images/bunny_ao.png)

Valentine:

![Image of ValentineAOMap](/images/valentine_ao_map.png)

Simple Cam 2:

![Image of SimpCam2AO](/images/simp_cam2_map.png)

### Images with Ambient Occlusion
Valentine with no Ambient Occlusion:

![Image of ValentineNoAO](/images/valentine_no_ao.png)

Valentine with Ambient Occlusion:

![Image of ValentineAO](/images/valentine_ao.png)

Simple Cam 2 with no Ambient Occlusion:

![Image of SimpleCam2NoAO](/images/simp_cam2.png)

Simple Cam 2 with Ambient Occlusion:

![Image of SimpleCam2AO](/images/simp_cam2_ao.png)

## Resources
[Hammersley Hemisphere Distribution](http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html#subsec-pointquality)

[Ambient Occlusion General Function](http://joomla.renderwiki.com/joomla/index.php?option=com_content&view=article&id=140&Itemid=157)

[Ambient Occlusion Shader](http://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html)

## Dependencies
[Eigen 3.0+](http://eigen.tuxfamily.org/index.php?title=Main_Page)

[CMake 3.5+](https://cmake.org)

[C++11 Compiler](https://en.wikipedia.org/wiki/List_of_compilers#C.2B.2B_compilers)

## Installation
1. Clone or Fork this repo
2. Run cmake
3. Run make