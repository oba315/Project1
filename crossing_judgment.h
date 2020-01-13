#pragma once

#include "image.h"
#include "vector3.h"
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include <iostream>
#include <vector>
#include <igl/readOBJ.h>
#include "MyScene.h"

bool crossing_judgement(Image& image, RTCScene& scene, MyScene myscene);

