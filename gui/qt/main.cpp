/**
 * @file main.cpp
 * @brief Главный файл Qt приложения BrickGame v2.0
 * @author BrickGame Team
 * @version 2.0
 * @date 2024
 *
 * @details
 * Этот файл содержит основное окно приложения с меню выбора игр
 * и интеграцией игр Snake и Tetris. Реализует паттерн MVC для
 * управления игровыми сессиями.
 */

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPauseAnimation>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QtCore/QEasingCurve>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QFont>
#include <QtGui/QKeyEvent>
#include <QtGui/QPalette>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "snake/GameController.h"
#include "snake/SidebarWidget.h"
#include "snake/SnakeWidget.h"
#include "tetris/TetrisController.h"
#include "tetris/TetrisWidget.h"

/**
 * @class MainMenuWidget
 * @brief Главное меню приложения с кнопками выбора игр
 *
 * @details
 * Виджет главного меню, содержащий кнопки для запуска игр Snake и Tetris.
 * Реализует анимированные переходы между экранами и современный дизайн.
 */
class MainMenuWidget : public QWidget {
 public:
  /**
   * @brief Конструктор главного меню
   * @param stack Указатель на стек виджетов для переключения экранов
   * @param parent Родительский виджет
   */
  explicit MainMenuWidget(QStackedWidget* stack, QWidget* parent = nullptr)
      : QWidget(parent), stack_(stack) {
    setupUI();
    setupAnimations();
  }

 private:
  void startSnake() { stack_->setCurrentIndex(1); }

  void exitApp() { QApplication::quit(); }

 private:
  void setupUI() {
    setStyleSheet(R"(
    QWidget {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #0d1117, stop:1 #161b22);
      color: #c9d1d9;
      font-family: 'SF Pro Text','Segoe UI','Inter','Arial',sans-serif;
    }
    QLabel { background: transparent; }
   
    .Title {
      color: #58a6ff;
      letter-spacing: 2px;
    }
   
    QPushButton {
      background: #21262d;
      border: 1px solid #30363d;
      border-radius: 10px;
      padding: 12px 28px;
      font-size: 16px;
      font-weight: 600;
      color: #c9d1d9;
      min-width: 240px;
      min-height: 48px;
    }
    QPushButton:hover {
      background: #30363d;
      border-color: #3b4451;
      color: #e6edf3;
    }
    QPushButton:pressed {
      background: #161b22;
      border-color: #58a6ff;
      color: #58a6ff;
    }
   
    .Footer {
      color: #8b949e;
    }
  )");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* titleLabel = new QLabel("BrickGame v2.0");
    titleLabel->setAlignment(Qt::AlignCenter);
    {
      QFont f;
      f.setPointSize(30);
      f.setBold(true);
      titleLabel->setFont(f);
    }
    titleLabel->setObjectName("Title");
    layout->addSpacing(10);
    layout->addWidget(titleLabel);
    layout->addSpacing(10);

    auto* snakeBtn = new QPushButton("Snake");
    auto* tetrisBtn = new QPushButton("Tetris");
    auto* exitBtn = new QPushButton("Exit");

    auto animateHover = [this](QPushButton* btn) {
      btn->installEventFilter(this);
    };

    animateHover(snakeBtn);
    animateHover(exitBtn);

    layout->addSpacing(10);
    layout->addWidget(snakeBtn, 0, Qt::AlignHCenter);
    layout->addWidget(tetrisBtn, 0, Qt::AlignHCenter);
    layout->addWidget(exitBtn, 0, Qt::AlignHCenter);

    layout->addSpacing(24);

    auto* footer = new QLabel("by Whiterage");
    footer->setAlignment(Qt::AlignCenter);
    {
      QFont f;
      f.setPointSize(12);
      f.setItalic(false);
      footer->setFont(f);
    }
    footer->setObjectName("Footer");
    layout->addWidget(footer, 0, Qt::AlignHCenter);

    connect(snakeBtn, &QPushButton::clicked, this, &MainMenuWidget::startSnake);
    connect(tetrisBtn, &QPushButton::clicked, this,
            [this] { stack_->setCurrentIndex(2); });
    connect(exitBtn, &QPushButton::clicked, this, &MainMenuWidget::exitApp);
  }

  void setupAnimations() {
    const auto widgets = findChildren<QWidget*>();
    auto* group = new QParallelAnimationGroup(this);

    int delay = 0;
    for (auto* w : widgets) {
      if (!qobject_cast<QPushButton*>(w) && !qobject_cast<QLabel*>(w)) continue;

      auto* fx = new QGraphicsOpacityEffect(w);
      w->setGraphicsEffect(fx);
      fx->setOpacity(0.0);

      auto* seq = new QSequentialAnimationGroup(group);
      auto* pause = new QPauseAnimation(delay, seq);

      auto* fade = new QPropertyAnimation(fx, "opacity", seq);
      fade->setStartValue(0.0);
      fade->setEndValue(1.0);
      fade->setDuration(700);
      fade->setEasingCurve(QEasingCurve::InOutCubic);

      seq->addAnimation(pause);
      seq->addAnimation(fade);

      group->addAnimation(seq);
      delay += 100;
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);
  }

  bool eventFilter(QObject* obj, QEvent* event) {
    auto* btn = qobject_cast<QPushButton*>(obj);
    if (btn) {
      if (event->type() == QEvent::Enter) {
        auto* anim = new QPropertyAnimation(btn, "geometry");
        QRect r = btn->geometry();
        anim->setDuration(150);
        anim->setStartValue(r);
        anim->setEndValue(
            QRect(r.x() - 2, r.y() - 2, r.width() + 4, r.height() + 4));
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
      } else if (event->type() == QEvent::Leave) {
        auto* anim = new QPropertyAnimation(btn, "geometry");
        QRect r = btn->geometry();
        anim->setDuration(150);
        anim->setStartValue(r);
        anim->setEndValue(
            QRect(r.x() + 2, r.y() + 2, r.width() - 4, r.height() - 4));
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
      }
    }
    return QWidget::eventFilter(obj, event);
  }

 private:
  QStackedWidget* stack_;
};

/**
 * @class GameWindow
 * @brief Главное окно приложения с управлением играми
 *
 * @details
 * Основное окно приложения, содержащее стек виджетов для переключения
 * между меню и играми. Управляет контроллерами игр и обрабатывает
 * глобальные события клавиатуры.
 */
class GameWindow : public QMainWindow {
 private:
  QStackedWidget* stack_;            ///< Стек виджетов для переключения экранов
  GameController* snakeController_;  ///< Контроллер игры Snake
  TetrisController* tetrisController_{nullptr};  ///< Контроллер игры Tetris

 public:
  /**
   * @brief Конструктор главного окна приложения
   * @param parent Родительский виджет
   */
  explicit GameWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
    setWindowTitle("BrickGame v2.0 — by whiterage");
    setMinimumSize(1200, 800);
    resize(1200, 800);

    stack_ = new QStackedWidget(this);
    setCentralWidget(stack_);

    setupMainMenu();
    setupSnakeGame();
    setupTetrisGame();
    stack_->setCurrentIndex(0);

    stack_->setCurrentIndex(0);
  }

 protected:
  /**
   * @brief Обработка нажатий клавиш
   * @param e Событие нажатия клавиши
   *
   * @details
   * Обрабатывает глобальные клавиши приложения:
   * - Escape: возврат в главное меню
   * - Остальные клавиши передаются активному контроллеру игры
   */
  void keyPressEvent(QKeyEvent* e) override {
    if (e->key() == Qt::Key_Escape) {
      stack_->setCurrentIndex(0);
      return;
    }

    int currentIndex = stack_->currentIndex();
    if (currentIndex == 1 && snakeController_) {
      snakeController_->onKeyPressed(e->key());
    }

    if (currentIndex == 2 && tetrisController_) {
      tetrisController_->onKeyPressed(e->key());
    }

    QMainWindow::keyPressEvent(e);
  }

 private:
  void setupMainMenu() {
    auto* menuWidget = new MainMenuWidget(stack_);
    stack_->addWidget(menuWidget);
  }

  void setupSnakeGame() {
    auto* sidebar = new SidebarWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet("background:#111;color:#eee;");

    auto* snakeView = new SnakeWidget();
    snakeController_ = new GameController(snakeView);
    snakeController_->setSidebar(sidebar);

    auto* snakePage = new QWidget();
    auto* row = new QHBoxLayout(snakePage);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(0);
    row->addWidget(snakeView, 1);
    row->addWidget(sidebar, 0);

    QObject::connect(sidebar->startButton(), &QPushButton::clicked, [this] {
      snakeController_->onKeyPressed(Qt::Key_Return);
    });
    QObject::connect(sidebar->pauseButton(), &QPushButton::clicked,
                     [this] { snakeController_->onKeyPressed(Qt::Key_Space); });
    QObject::connect(sidebar->resetButton(), &QPushButton::clicked,
                     [this] { snakeController_->onKeyPressed(Qt::Key_R); });
    QObject::connect(sidebar->quitButton(), &QPushButton::clicked,
                     [this] { stack_->setCurrentIndex(0); });

    stack_->addWidget(snakePage);

    snakeController_->start();
  }

  void setupTetrisGame() {
    auto* sidebar = new SidebarWidget();
    sidebar->setFixedWidth(280);
    sidebar->setStyleSheet("background:#111;color:#eee;");

    auto* view = new TetrisWidget();
    tetrisController_ = new TetrisController(view);
    tetrisController_->setSidebar(sidebar);

    auto* page = new QWidget();
    auto* row = new QHBoxLayout(page);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(0);
    row->addWidget(view, 1);
    row->addWidget(sidebar, 0);

    QObject::connect(sidebar->startButton(), &QPushButton::clicked, [this] {
      tetrisController_->onKeyPressed(Qt::Key_Return);
    });
    QObject::connect(sidebar->pauseButton(), &QPushButton::clicked, [this] {
      tetrisController_->onKeyPressed(Qt::Key_Space);
    });
    QObject::connect(sidebar->resetButton(), &QPushButton::clicked,
                     [this] { tetrisController_->onKeyPressed(Qt::Key_R); });
    QObject::connect(sidebar->quitButton(), &QPushButton::clicked,
                     [this] { stack_->setCurrentIndex(0); });

    stack_->addWidget(page);
    tetrisController_->start();
  }
};

/**
 * @brief Точка входа в приложение
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения приложения
 *
 * @details
 * Инициализирует Qt приложение, создает главное окно и запускает
 * основной цикл обработки событий.
 */
int main(int argc, char** argv) {
  QApplication app(argc, argv);

  GameWindow window;
  window.show();

  return app.exec();
}
