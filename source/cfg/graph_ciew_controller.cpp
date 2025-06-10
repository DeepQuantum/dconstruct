#include "gaph_view_controller.h"

GraphicsViewController::GraphicsViewController(QGraphicsView *view, QGraphicsScene *scene, const FunctionDisassembly *function) {
    this->m_graphicsView = view;
    this->m_graphicsScene = scene;
    this->m_currentFunction = function;
}

void GraphicsViewController::create_cfg() noexcept {
    this->m_head = std::make_unique<CFGElement>("head_node", QRectF(450, 450, 500, 500));
    std::string next_node_text = "";
    int i = 0;
    for (const auto &line : this->m_currentFunction->m_lines) {
        if (line.m_label == -1) {
            next_node_text += line.m_comment;
            next_node_text += "\n";
        } else {
            break;
        }
        this->m_head->m_text = QString::fromStdString(next_node_text);
    }
    this->m_head->update_rect(QRectF(450, 450, 500, 500));
    this->m_graphicsScene->addItem(this->m_head.get());
}