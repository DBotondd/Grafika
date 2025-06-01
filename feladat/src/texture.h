// src/texture.h
#pragma once

// Beolvassa az adott útvonalon lévő képet (PNG, JPG) és
// OpenGL textúrává alakítja. Visszaadja a glGenTextures ID-ját.
unsigned Texture_FromFile(const char *path);
