#include <iostream>
#include <vector>
#include <cmath>
#include "LaserMirror.h"

using namespace std;

// Helper to keep objects within the window
bool check_mouse_in_bounds(float pos_x, float pos_y, float width, float height) {
    return (pos_x >= 0 && pos_x <= width && pos_y >= 0 && pos_y <= height);
}

int main(int argc, char **argv) {
    // Allegro Setup
    if (!al_init()) return -1;
    al_install_mouse();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    al_set_window_title(display, "Laser Simulation");
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    vector<Mirror*> objects;
    objects.push_back(new Mirror(400, 300, 100, 0.45));
    objects.push_back(new Mirror(200, 400, 150, -0.3));
    objects.push_back(new Lens(600, 200, 100, 0.0, 1));
    objects.push_back(new Lens(500, 500, 120, 1.0, -0.5));

    vector<Laser> lasers;
    lasers.push_back(Laser(100, 100, 0.3));
    lasers.push_back(Laser(100, 500, -0.3));

    bool running = true;
    bool redraw = true;
    bool dragging = false;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;
        if (ev.type == ALLEGRO_EVENT_TIMER) redraw = true;

        // Interaction Logic
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            bool found = false;

            for (auto &l : lasers) {
                float dist = sqrt(pow(ev.mouse.x - l.pos.x, 2) + pow(ev.mouse.y - l.pos.y, 2));
                if (dist < 20) {
                    l.is_selected = true;
                    dragging = true;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                for (auto &obj : objects) {
                    float dist = sqrt(pow(ev.mouse.x - obj->pos.x, 2) + pow(ev.mouse.y - obj->pos.y, 2));
                    if (dist < 40) {
                        obj->is_selected = true;
                        dragging = true;
                        break;
                    }
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            dragging = false;
            for (auto &l : lasers) l.is_selected = false;
            for (auto &obj : objects) obj->is_selected = false;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES && dragging) {
            if (check_mouse_in_bounds(ev.mouse.x, ev.mouse.y, 800, 600)) {
                for (auto &l : lasers) {
                    if (l.is_selected) { l.pos.x = ev.mouse.x; l.pos.y = ev.mouse.y; }
                }
                for (auto &obj : objects) {
                    if (obj->is_selected) { obj->pos.x = ev.mouse.x; obj->pos.y = ev.mouse.y; }
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            for (auto &obj : objects) {
                if (obj->is_selected) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) obj->angle -= 0.05;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) obj->angle += 0.05;
                }
            }
            for (auto &l : lasers) {
                if (l.is_selected) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) l.angle -= 0.05;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) l.angle += 0.05;
                }
            }
        }

        // Drawing Logic
        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(10, 10, 20));

            for (auto &laser : lasers) {
                Point current_p = laser.pos;
                float current_a = laser.angle;

                for (int i = 0; i < 10; i++) {
                    HitResult closest_hit = {false, 0, 0, 0};
                    Mirror* hit_object = nullptr;
                    float min_dist = 10000;

                    for (Mirror* obj : objects) {
                        HitResult hr = check_collision(current_p, current_a, *obj);
                        if (hr.hit) {
                            float dist = sqrt(pow(hr.x - current_p.x, 2) + pow(hr.y - current_p.y, 2));
                            if (dist < min_dist) {
                                min_dist = dist;
                                closest_hit = hr;
                                hit_object = obj;
                            }
                        }
                    }

                    if (closest_hit.hit && hit_object != nullptr) {
                        al_draw_line(current_p.x, current_p.y, closest_hit.x, closest_hit.y, al_map_rgb(255, 0, 0), 2);
                        al_draw_filled_circle(closest_hit.x, closest_hit.y, 3, al_map_rgb(255, 255, 255));
                        
                        current_a = hit_object->get_next_angle(current_a);
                        current_p = {closest_hit.x, closest_hit.y};
                    } else {
                        float end_x = current_p.x + cos(current_a) * 2000;
                        float end_y = current_p.y + sin(current_a) * 2000;
                        al_draw_line(current_p.x, current_p.y, end_x, end_y, al_map_rgb(255, 0, 0), 2);
                        break; 
                    }
                }
            }

            // Draw all objects and lasers
            for (auto &obj : objects) obj->draw();
            for (auto &l : lasers) l.draw();
            
            al_flip_display();
            redraw = false;
        }
    }

    // Cleanup: Delete the pointers
    for (Mirror* obj : objects) delete obj;
    
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;

}
