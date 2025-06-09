#include "gaph_view_controller.h"

GraphicsViewController::GraphicsViewController(QGraphicsView *view, QGraphicsScene *scene, const FunctionDisassembly *function) {
    this->m_graphicsView = view;
    this->m_graphicsScene = scene;
    this->m_currentFunction = function;
}

void GraphicsViewController::start_draw() noexcept {
    QString test = QString::fromStdString(this->m_currentFunction->m_lines[0].m_comment);
    this->m_head = std::make_unique<CFGElement>(test, QRectF(0, 0, 100, 50), nullptr);
    this->m_graphicsScene->addItem(this->m_head.get());
}