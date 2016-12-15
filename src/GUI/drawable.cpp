#include "drawable.h"

Drawable::Drawable(unsigned int layer, bool ghost = false) {
    this->layer = layer;
    this->ghost = ghost;
}

Drawable::~Drawable() {

}

bool Drawable::isGhost() {
    return ghost;
}

int Drawable::getLayer() {
    return layer;
}


