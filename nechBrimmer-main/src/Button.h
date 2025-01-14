#ifndef BUTTON_H
#define BUTTON_H

#include <vector>
#include <functional>
#include "vex.h"

vex::brain Brainn;

class Button {
    private:
        int x, y, width, height;
        vex::color color;
        vex::color textColor;
        std::string label;
        std::function<void()> callback;
    
    public:
        Button(int x, int y, int width, int height, vex::color color, const std::string &label, std::function<void()> callback, vex::color textColor = vex::color::white): x(x), y(y), width(width), height(height), color(color), label(label), callback(callback), textColor(textColor) {};

        void draw() const {
            Brainn.Screen.setFillColor(color);
            Brainn.Screen.setPenColor(vex::color::black);
            Brainn.Screen.drawRectangle(x, y, width, height);

            Brainn.Screen.setPenColor(textColor);
            int textX = x + (width / 2) - (label.size() * 6 / 2);
            int textY = y + (height / 2) - 8;
            Brainn.Screen.printAt(textX, textY, false, label.c_str());
        }

        bool isPressed(int touchX, int touchY) const {
            return touchX >= x && touchX <= x + width && touchY >= y && touchY <= y + height;
        }

        void press() const {
            if (callback) {
                callback();
            }
        }
};

class ButtonManager {
    private:
        std::vector<Button> buttons;

    public:
        void addButton(const Button &button) {
            buttons.push_back(button);
        }

        void drawAllButtons() const {
            for (const auto &button : buttons) {
                button.draw();
            }
        }

        void handleTouch(int touchX, int touchY) {
            for (const auto &button : buttons) {
                if (button.isPressed(touchX, touchY)) {
                    button.press();
                    break;
                }
            }
        }
};

#endif