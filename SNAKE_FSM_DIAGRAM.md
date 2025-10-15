# Snake Game Finite State Machine (FSM) Diagram

## States (Состояния)

1. **kInit** - Начальное состояние
2. **kReady** - Готовность к запуску
3. **kRunning** - Игра активна
4. **kPaused** - Игра на паузе
5. **kGameOver** - Игра завершена

## Events (События)

1. **kStart** - Запуск игры
2. **kTick** - Игровой тик (движение змейки)
3. **kPauseToggle** - Переключение паузы
4. **kMoveUp/Down/Left/Right** - Движение змейки
5. **kReset** - Сброс игры
6. **kQuit** - Выход из игры

## State Transitions (Переходы между состояниями)

```
┌─────────┐    kStart     ┌─────────┐    kTick     ┌─────────┐
│  kInit  │ ────────────► │ kReady  │ ──────────► │kRunning │
└─────────┘               └─────────┘             └─────────┘
     ▲                           ▲                       │
     │                           │                       │
     │ kReset                    │ kStart                │ kPauseToggle
     │                           │                       ▼
┌─────────┐               ┌─────────┐             ┌─────────┐
│kGameOver│               │ kInit   │             │kPaused  │
└─────────┘               └─────────┘             └─────────┘
     ▲                           ▲                       │
     │                           │                       │
     │ (Collision with wall/     │ kReset                │ kPauseToggle
     │  snake body or snake      │                       │
     │  reaches max length)      │                       │
     │                           │                       ▼
     └───────────────────────────┴─────────────────► ┌─────────┐
                                                     │kRunning │
                                                     └─────────┘
```