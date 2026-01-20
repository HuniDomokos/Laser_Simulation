#ifndef LASER_MIRROR_H
#define LASER_MIRROR_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <vector>

struct Point { float x, y; };

struct HitResult {
    bool hit;
    float x, y;
    float new_angle;
};

class Mirror {
public:
    Point pos;
    float length;
    float angle; 
    bool is_selected;

    Mirror(float x, float y, float len, float ang) 
        : pos({x, y}), length(len), angle(ang), is_selected(false) {}

    virtual void draw() {
        float dx = cos(angle) * (length / 2);
        float dy = sin(angle) * (length / 2);
        ALLEGRO_COLOR color = is_selected ? al_map_rgb(255, 255, 0) : al_map_rgb(200, 200, 200);
        al_draw_line(pos.x - dx, pos.y - dy, pos.x + dx, pos.y + dy, color, 4.0);
    }

    virtual float get_next_angle(float incoming_angle) {
        return (2.0 * angle) - incoming_angle;
    }
};

class Lens : public Mirror {
public:
    float refraction_index;

    Lens(float x, float y, float len, float ang, float refr) 
        : Mirror(x, y, len, ang), refraction_index(refr) {}

    void draw() override {
        float dx = cos(angle) * (length / 2.0);
        float dy = sin(angle) * (length / 2.0);
        
        ALLEGRO_COLOR color = is_selected ? al_map_rgb(255, 255, 0) : al_map_rgb(100, 200, 255);
        al_draw_line(pos.x - dx, pos.y - dy, pos.x + dx, pos.y + dy, color, 6.0);
    }
    float get_next_angle(float incoming_angle) override{
        return incoming_angle + refraction_index;
    }
};


class Laser {
public:
    Point pos;
    float angle;
    bool is_selected;

    Laser(float x, float y, float ang) : pos({x, y}), angle(ang), is_selected(false) {}

    void draw() {
        ALLEGRO_COLOR color = is_selected ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 0, 0);
        al_draw_filled_circle(pos.x, pos.y, 10, color);
    }
};

HitResult check_collision(Point rayP, float rayA, const Mirror& m) {
    HitResult res = {false, 0, 0, 0};

    // Calculate Mirror Endpoints
    float m_dx = cos(m.angle) * (m.length / 2.0);
    float m_dy = sin(m.angle) * (m.length / 2.0);
    float x1 = m.pos.x - m_dx;
    float y1 = m.pos.y - m_dy;
    float x2 = m.pos.x + m_dx;
    float y2 = m.pos.y + m_dy;

    // Ray Direction Vectors
    float x3 = rayP.x;
    float y3 = rayP.y;
    float x4 = rayP.x + cos(rayA); 
    float y4 = rayP.y + sin(rayA);

    // Line-Line Intersection Formula
    float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::abs(den) < 0.00001) return res; // Parallel lines never hit

    // t is the hit point location on the mirror (0.0 to 1.0)
    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    // u is the distance along the laser ray
    float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

    // Check Valid Hit:
    // t must be between 0 and 1 (meaning the hit is ON the mirror segment)
    // u must be > 0 (meaning the hit is in FRONT of the laser)
    if (t >= 0.0 && t <= 1.0 && u > 0.1) {
        res.hit = true;
        res.x = x1 + t * (x2 - x1);
        res.y = y1 + t * (y2 - y1);
        
        // Reflection angle relative to the mirror's normal
        // Formula: Reflected = 2 * MirrorAngle - IncidentAngle
        res.new_angle = (2.0 * m.angle) - rayA;
    }

    return res;
}
#endif