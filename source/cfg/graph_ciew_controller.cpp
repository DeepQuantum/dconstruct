#include "gaph_view_controller.h"

GraphicsViewController::GraphicsViewController(QGraphicsView *view, QGraphicsScene *scene, const FunctionDisassembly *function) {
    this->m_graphicsView = view;
    this->m_graphicsScene = scene;
    this->m_currentFunction = function;
}

void GraphicsViewController::create_cfg() noexcept {
    std::string next_node_text = "";
    u32 i = 0;
    const u32 x_start = 500;
    const u32 y_start = 20;
    const u32 line_height = 20;
    const u32 node_width = 500;
    const u32 node_margin = 50;
    const u32 node_padding = 50;
    const u32 node_x_middle = x_start + node_width / 2;
    u32 node_height, y_node_offset = 0;
    const QPen pen{Qt::red};
    for (const auto &line : this->m_currentFunction->m_lines) {
        if (line.m_label == -1) {
            next_node_text += line.m_text;
            next_node_text += "\n";
            ++i;
        } else {
            next_node_text += line.m_text;
            next_node_text += "\n";
            y_node_offset += y_start + node_height + node_padding; 
            std::unique_ptr<CFGElement> next_node = std::make_unique<CFGElement>("PLACEHOLDER", QRectF(x_start, y_node_offset, node_width, node_height));
            next_node->m_text = QString::fromStdString(next_node_text);
            if (!this->m_nodes.empty()) {
                next_node->connect(this->m_nodes.back().get());
                this->m_graphicsScene->addLine(node_x_middle, this->m_nodes.back()->boundingRect().bottom(), node_x_middle, y_node_offset, pen);
            }
            this->m_graphicsScene->addItem(next_node.get());
            this->m_nodes.push_back(std::move(next_node));
            next_node_text = "";
            i = 0;
        }
    }
    this->m_graphicsScene->setSceneRect(this->m_graphicsScene->itemsBoundingRect());
}