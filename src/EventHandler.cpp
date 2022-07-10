#include <windows.h>
#include "EventHandler.h"
#include "Input.h"
#include "Application.h"
#include "Camera.h"

void EventHandler::HandleEvent(EventType type, unsigned long p1, unsigned long p2) {
  switch (type) {
    case KEYDOWN_EVENT: {
      Input::HandleKeyPressEvent((int) p1);
      if (p1 == VK_ESCAPE) {
        // Lose focus on escape
        SetFocus(nullptr);
      }
      break;
    }
    case KEYUP_EVENT:
      Input::HandleKeyReleaseEvent((int) p1);
      break;
    case MOUSEMOVE_EVENT:
//      LPPOINT mousePos;
//      GetCursorPos(mousePos);
//      if (mousePos)
//        Input::HandleMouseMoveEvent(mousePos->x, mousePos->y);
      Camera::Displace((int) p1, (int) p2);
      break;
    case RESIZE_EVENT: {
      int width = LOWORD(p2);
      int height = HIWORD(p2);
      if (Application::window) {
        Application::window->width = width;
        Application::window->height = height;
        Application::frameBuf->Resize(width, height);
        Application::renderer->SetProjection(width, height);
      }
      break;
    }
    case QUIT_EVENT:
      Application::running = false;
      break;
    default:
      return;
  }
}
