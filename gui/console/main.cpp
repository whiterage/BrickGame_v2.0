#include "consoleView.h"
#include "snakeController.h"
#include "snakeModel.h"

int main() {
  SnakeModel model;
  SnakeController controller(&model);
  ConsoleView view(&controller);
  view.startEventLoop();
  return 0;
}
