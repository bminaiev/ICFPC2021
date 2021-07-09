#pragma once

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QApplication>
#include <QWindow>
#include <QKeyEvent>
#include <QScreen>

#include <iostream>

namespace {

const int SCALE = 5;

QPen whitePen(Qt::white);
QPen darkWhitePen(QColor(222, 222, 222));
QPen orangePen(QColor(255, 127, 0));
QPen yellowPen(Qt::yellow);
QPen darkOrangePen(QColor(127, 55, 0));
QPen darkBluePen(QColor(0, 0, 100));
QPen redPen(Qt::red);
QPen thickRedPen(QBrush(Qt::red), 2.0);
QPen thickBlackPen(QBrush(Qt::black), 3.0);
QPen greenPen(Qt::green);
QPen darkGreenPen(QColor(0, 90, 0));
QPen bluePen(Qt::blue);
QPen thickBluePen(QBrush(Qt::blue), 2.0);
QPen blackPen(Qt::black);
QPen transparentPen(Qt::transparent);
QPen magentaPen(Qt::magenta);
QPen palePen(QColor(0xbc, 0xbd, 0xba));
QPen grayPen(QColor(96, 96, 96));
QPen lightBluePen(QColor(166, 202, 240));
QPen darkYellowPen(QColor(100, 100, 0));
QPen darkRedPen(QColor(100, 0, 0));
QPen lightGreenPen(QColor(139, 250, 117));
QPen balloonPen(QColor(0xbc, 0xbd, 0xba, 222));

QBrush whiteBrush(Qt::white);
QBrush darkWhiteBrush(QColor(222, 222, 222));
QBrush orangeBrush(QColor(255, 127, 0));
QBrush darkYellowBrush(QColor(100, 100, 0));
QBrush yellowBrush(Qt::yellow);
QBrush redBrush(Qt::red);
QBrush darkOrangeBrush(QColor(127, 55, 0));
QBrush darkBlueBrush(QColor(0, 0, 100));
QBrush darkRedBrush(QColor(100, 0, 0));
QBrush greenBrush(Qt::green);
QBrush blueBrush(Qt::blue);
QBrush blackBrush(Qt::black);
QBrush darkGrayBrush(QColor(47,44,45));
QBrush paleBrush(QColor(0xbc, 0xbd, 0xba));
QBrush grayBrush(QColor(87,84,85));
QBrush transparentBrush(Qt::transparent);
QBrush magentaBrush(Qt::magenta);
QBrush lightBlueBrush(QColor(159, 194, 229));
QBrush balloonBrush(QColor(0xbc, 0xbd, 0xba, 222));

std::vector<QColor> playerColors = {
    QColor(0, 55, 127),
    QColor(0, 127, 55),
    QColor(127, 0, 55),
    QColor(215, 110, 0)
};

std::vector<QColor> playerColorsAlpha = {
    QColor(0, 55, 127, 64),
    QColor(0, 127, 55, 64),
    QColor(127, 0, 55, 64),
    QColor(215, 110, 0, 64)
};

std::vector<QBrush> brushesPerPlayer = {
    QBrush(playerColors[0]),
    QBrush(playerColors[1]),
    QBrush(playerColors[2]),
    QBrush(playerColors[3])
};

std::vector<QBrush> brushesPerPlayerAlpha = {
    QBrush(playerColorsAlpha[0]),
    QBrush(playerColorsAlpha[1]),
    QBrush(playerColorsAlpha[2]),
    QBrush(playerColorsAlpha[3])
};

std::vector<QPen> pensPerPlayer = {
    QPen(playerColors[0]),
    QPen(playerColors[1]),
    QPen(playerColors[2]),
    QPen(playerColors[3])
};

}

class Widget : public QWidget {
protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.drawPixmap(0, 0, pixmap_);
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Space) {
            paused_ = !paused_;
        } else if (event->key() == Qt::Key_Equal) {
            scalex *= SCALE_COEFF;
            scaley *= SCALE_COEFF;
        } else if (event->key() == Qt::Key_Minus) {
            scalex /= SCALE_COEFF;
            scaley /= SCALE_COEFF;
        } else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_H) {
            movex -= MOVE_COEFF / scalex;
        } else if (event->key() == Qt::Key_Right || event->key() == Qt::Key_L) {
            movex += MOVE_COEFF / scalex;
        } else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_K) {
            movey -= MOVE_COEFF / scaley;
        } else if (event->key() == Qt::Key_Down || event->key() == Qt::Key_J) {
            movey += MOVE_COEFF / scaley;
        } else if (event->key() == Qt::Key_0) {
            scalex = 1;
            scaley = 1;
            movex = 0;
            movey = 0;
        }

        // std::cerr << movex << " " << movey << " " << scalex << " " << scaley << std::endl;

        if (onKeyPress_) {
            onKeyPress_(*event);
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (onMouseClick_) {
            onMouseClick_(*event, event->x(), event->y());
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (onMouseMove_) {
            onMouseMove_(*event, event->x(), event->y());
        }
    }

    void setOnMousePress(std::function<void(const QMouseEvent&, double, double)> onMouseClick) {
        onMouseClick_ = std::move(onMouseClick);
    }

    void setOnMouseMove(std::function<void(const QMouseEvent&, double, double)> onMouseMove) {
        onMouseMove_ = std::move(onMouseMove);
    }

    void setOnKeyPress(std::function<void(const QKeyEvent&)> onKeyPress) {
        onKeyPress_ = std::move(onKeyPress);
    }

private:
    friend class Visualizer;
    friend class RenderCycle;
    QPixmap pixmap_;

    static constexpr double SCALE_COEFF = 1.08;
    static constexpr double MOVE_COEFF = 10;
    double scalex = 1;
    double scaley = 1;
    double movex = 0;
    double movey = 0;
    bool paused_ = false;

    std::function<void(const QMouseEvent&, double, double)> onMouseClick_;
    std::function<void(const QMouseEvent&, double, double)> onMouseMove_;
    std::function<void(const QKeyEvent&)> onKeyPress_;
};


class Visualizer {
public:
    Visualizer(int width = 0, int height = 0) {
        static char arg0[] = "Visualizer";
        static char* argv[] = {&arg0[0], nullptr};
        static int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;
        app_ = new QApplication{argc, argv};

        widget_ = new Widget();
        widget_->show();

        setSize(width, height);

        app_->processEvents();
    }

    void setSize(int width, int height) {
        width_ = width;
        height_ = height;
        widget_->resize(width, height);
    }

    void process() {
        app_->processEvents();
    }

    bool paused() const {
        return widget_->paused_;
    }

    void setOnKeyPress(std::function<void(const QKeyEvent&)> onKeyPress) {
        onKeyPress_ = std::move(onKeyPress);
        widget_->setOnKeyPress([this] (const QKeyEvent& event) {
            onKeyPress_(event);
        });
    }

    void setOnMouseClick(std::function<void(const QMouseEvent&, double, double, double, double)> onMouseClick) {
        onMouseClick_ = onMouseClick;

        widget_->setOnMousePress([this] (const QMouseEvent& event, double screenX, double screenY) {
            const double worldX = (screenX - width_ * 0.5) / widget_->scalex + width_ * 0.5 + widget_->movex;
            const double worldY = (screenY - height_ * 0.5) / widget_->scaley + height_ * 0.5 + widget_->movey;
            onMouseClick_(event, screenX, screenY, worldX / SCALE, worldY / SCALE);
        });
    }

    void setOnMouseMove(std::function<void(const QMouseEvent&, double, double, double, double)> onMouseMove) {
        onMouseMove_ = onMouseMove;

        widget_->setOnMouseMove([this] (const QMouseEvent& event, double screenX, double screenY) {
            const double worldX = (screenX - width_ * 0.5) / widget_->scalex + width_ * 0.5 + widget_->movex;
            const double worldY = (screenY - height_ * 0.5) / widget_->scaley + height_ * 0.5 + widget_->movey;
            onMouseMove_(event, screenX, screenY, worldX / SCALE, worldY / SCALE);
        });
    }

    void switchToWindow() {
        p.end();
        p.begin(&widget_->pixmap_);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    }

    void switchToWorld() {
        p.end();
        adjust();
    }

    QPainter p; // public painter

    void adjust() {
        p.begin(&widget_->pixmap_);
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
        p.translate((-widget_->movex - width_ * 0.5) * widget_->scalex + width_ * 0.5,
                    (-widget_->movey - height_ * 0.5) * widget_->scaley + height_ * 0.5);
        p.scale(widget_->scalex, widget_->scaley);
    }

    ~Visualizer() {
        app_->quit();
    }

    QApplication* app_;
private:
    friend class RenderCycle;

    int height_, width_;
    Widget* widget_;

    std::function<void(const QMouseEvent&, double, double, double, double)> onMouseClick_;
    std::function<void(const QMouseEvent&, double, double, double, double)> onMouseMove_;
    std::function<void(const QKeyEvent&)> onKeyPress_;
};

class RenderCycle {
public:
    RenderCycle(Visualizer& visualizer) : visualizer_(&visualizer) {
        while (visualizer.paused())
            visualizer.process();

        Widget* widget = visualizer_->widget_;
        const auto devicePixelRatio = widget->devicePixelRatioF();
        const auto desiredWidth = static_cast<int>(visualizer.width_ * devicePixelRatio);
        const auto desiredHeight = static_cast<int>(visualizer.height_ * devicePixelRatio);

        if (widget->pixmap_.size() != QSize(desiredWidth, desiredHeight)) {
            widget->pixmap_ = QPixmap(desiredWidth, desiredHeight);
            widget->pixmap_.setDevicePixelRatio(devicePixelRatio);
        }

        widget->pixmap_.fill(Qt::black);
        visualizer.adjust();
    }

    ~RenderCycle() {
        visualizer_->p.end();
        visualizer_->widget_->repaint();
        visualizer_->process();
    }

private:
    Visualizer* visualizer_;
};
